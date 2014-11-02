#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include "stdlib/acl_sys_patch.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_mystring.h"
#include "stdlib/acl_msg.h"
#include "stdlib/acl_debug.h"
#include "stdlib/acl_vstream.h"
#include "event/acl_events.h"

#endif

#include "events_define.h"

#ifdef	ACL_EVENTS_POLL_STYLE
#include <sys/poll.h>
#include <unistd.h>

#include "events_fdtable.h"
#include "events.h"

typedef struct EVENT_POLL {
	ACL_EVENT event;
	struct pollfd *fds;
	ACL_FD_MAP *fdmap;
} EVENT_POLL;

static void stream_on_close(ACL_VSTREAM *stream, void *arg)
{
	EVENT_POLL *ev = (EVENT_POLL*) arg;
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE*) stream->fdp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);
	int   ret = 0;

	if (fdp == NULL)
		return;

	if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)
		&& (fdp->flag & EVENT_FDTABLE_FLAG_WRITE))
	{
		ret = 1;
	} else if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
		ret = 2;
	} else if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
		ret = 3;
	}

	if (ret) {
		acl_fdmap_del(ev->fdmap, sockfd);
	}

	if (ev->event.maxfd == ACL_VSTREAM_SOCK(fdp->stream))
		ev->event.maxfd = ACL_SOCKET_INVALID;
	if (fdp->fdidx >= 0 && fdp->fdidx < --ev->event.fdcnt) {
		ev->fds[fdp->fdidx] = ev->fds[ev->event.fdcnt];
		ev->event.fdtabs[fdp->fdidx] = ev->event.fdtabs[ev->event.fdcnt];
		ev->event.fdtabs[fdp->fdidx]->fdidx = fdp->fdidx;
	}
	fdp->fdidx = -1;

	if (fdp->fdidx_ready >= 0
		&& fdp->fdidx_ready < ev->event.fdcnt_ready
		&& ev->event.fdtabs_ready[fdp->fdidx_ready] == fdp)
	{
		ev->event.fdtabs_ready[fdp->fdidx_ready] = NULL;
	}
	fdp->fdidx_ready = -1;
	event_fdtable_free(fdp);
	stream->fdp = NULL;
}

static void event_enable_read(ACL_EVENT *eventp, ACL_VSTREAM *stream,
	int timeout, ACL_EVENT_NOTIFY_RDWR callback, void *context)
{
	EVENT_POLL *ev = (EVENT_POLL *) eventp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	if (fdp == NULL) {
		fdp = event_fdtable_alloc();

		fdp->stream = stream;
		stream->fdp = (void *) fdp;
		acl_vstream_add_close_handle(stream, stream_on_close, eventp);
		acl_fdmap_add(ev->fdmap, sockfd, fdp);
	}

	if (fdp->fdidx == -1) {
		fdp->fdidx = eventp->fdcnt;
		eventp->fdtabs[eventp->fdcnt++] = fdp;
	}

	if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
		fdp->flag |= EVENT_FDTABLE_FLAG_READ;
		ev->fds[fdp->fdidx].events |= POLLIN | POLLHUP | POLLERR;
	} else {
		fdp->flag = EVENT_FDTABLE_FLAG_READ | EVENT_FDTABLE_FLAG_EXPT;
		ev->fds[fdp->fdidx].events = POLLIN | POLLHUP | POLLERR;
	}

	ev->fds[fdp->fdidx].fd = sockfd;

	if (eventp->maxfd != ACL_SOCKET_INVALID && eventp->maxfd < sockfd)
		eventp->maxfd = sockfd;

	if (fdp->r_callback != callback || fdp->r_context != context) {
		fdp->r_callback = callback;
		fdp->r_context = context;
	}

	if (timeout > 0) {
		fdp->r_timeout = timeout * 1000000;
		fdp->r_ttl = eventp->present + fdp->r_timeout;
	} else {
		fdp->r_ttl = 0;
		fdp->r_timeout = 0;
	}
}

static void event_enable_write(ACL_EVENT *eventp, ACL_VSTREAM *stream,
	int timeout, ACL_EVENT_NOTIFY_RDWR callback, void *context)
{
	EVENT_POLL *ev = (EVENT_POLL *) eventp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	if (fdp == NULL) {
		fdp = event_fdtable_alloc();

		fdp->stream = stream;
		stream->fdp = (void *) fdp;
		acl_vstream_add_close_handle(stream, stream_on_close, eventp);
		acl_fdmap_add(ev->fdmap, sockfd, fdp);
	}

	if (fdp->fdidx == -1) {
		fdp->fdidx = eventp->fdcnt;
		eventp->fdtabs[eventp->fdcnt++] = fdp;
	}

	if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
		fdp->flag |= EVENT_FDTABLE_FLAG_WRITE;
		ev->fds[fdp->fdidx].events |= POLLOUT | POLLHUP | POLLERR;
	} else {
		fdp->flag = EVENT_FDTABLE_FLAG_WRITE | EVENT_FDTABLE_FLAG_EXPT;
		ev->fds[fdp->fdidx].events = POLLOUT | POLLHUP | POLLERR;
	}

	ev->fds[fdp->fdidx].fd = sockfd;

	if (eventp->maxfd != ACL_SOCKET_INVALID && eventp->maxfd < sockfd)
		eventp->maxfd = sockfd;

	if (fdp->w_callback != callback || fdp->w_context != context) {
		fdp->w_callback = callback;
		fdp->w_context = context;
	}

	if (timeout > 0) {
		fdp->w_timeout = timeout * 1000000;
		fdp->w_ttl = eventp->present + fdp->w_timeout;
	} else {
		fdp->w_ttl = 0;
		fdp->w_timeout = 0;
	}
}

/* event_disable_read - disable request for read events */

static void event_disable_read(ACL_EVENT *eventp, ACL_VSTREAM *stream)
{
	const char *myname = "event_disable_read";
	EVENT_POLL *ev = (EVENT_POLL *) eventp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	if (fdp == NULL) {
		acl_msg_warn("%s(%d): fdp null", myname, __LINE__);
		return;
	}

	if (fdp->fdidx < 0 || fdp->fdidx >= eventp->fdcnt) {
		acl_msg_warn("%s(%d): sockfd(%d)'s fdidx(%d) invalid",
			myname, __LINE__, sockfd, fdp->fdidx);
		return;
	}

	if (!(fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
		acl_msg_warn("%s(%d): sockfd(%d) not be set",
			myname, __LINE__, sockfd);
		return;
	}

	fdp->r_ttl = 0;
	fdp->r_timeout = 0;
	fdp->r_callback = NULL;
	fdp->event_type &= ~(ACL_EVENT_READ | ACL_EVENT_ACCEPT);
	fdp->flag &= ~EVENT_FDTABLE_FLAG_READ;

	if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
		ev->fds[fdp->fdidx].events = POLLOUT | POLLHUP | POLLERR;
		return;
	}

	if (eventp->maxfd == sockfd)
		eventp->maxfd = ACL_SOCKET_INVALID;

	if (fdp->fdidx < --eventp->fdcnt) {
		ev->fds[fdp->fdidx] = ev->fds[eventp->fdcnt];
		eventp->fdtabs[fdp->fdidx] = eventp->fdtabs[eventp->fdcnt];
		eventp->fdtabs[fdp->fdidx]->fdidx = fdp->fdidx;
	}
	fdp->fdidx = -1;

	if (fdp->fdidx_ready >= 0
		&& fdp->fdidx_ready < eventp->fdcnt_ready
		&& eventp->fdtabs_ready[fdp->fdidx_ready] == fdp)
	{
		eventp->fdtabs_ready[fdp->fdidx_ready] = NULL;
	}
	fdp->fdidx_ready = -1;

	acl_fdmap_del(ev->fdmap, sockfd);
}

/* event_disable_write - disable request for write events */

static void event_disable_write(ACL_EVENT *eventp, ACL_VSTREAM *stream)
{
	const char *myname = "event_disable_write";
	EVENT_POLL *ev = (EVENT_POLL *) eventp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	if (fdp == NULL) {
		acl_msg_warn("%s(%d): fdp null", myname, __LINE__);
		return;
	}

	if (fdp->fdidx < 0 || fdp->fdidx >= eventp->fdcnt) {
		acl_msg_warn("%s(%d): sockfd(%d)'s fdidx(%d) invalid",
			myname, __LINE__, sockfd, fdp->fdidx);
		return;
	}

	if (!(fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
		acl_msg_warn("%s(%d): sockfd(%d) not be set",
			myname, __LINE__, sockfd);
		return;
	}

	fdp->w_ttl = 0;
	fdp->w_timeout = 0;
	fdp->w_callback = NULL;
	fdp->event_type &= ~(ACL_EVENT_WRITE | ACL_EVENT_CONNECT);
	fdp->flag &= ~EVENT_FDTABLE_FLAG_WRITE;

	if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
		ev->fds[fdp->fdidx].events = POLLIN | POLLHUP | POLLERR;
		return;
	}

	if (eventp->maxfd == sockfd)
		eventp->maxfd = ACL_SOCKET_INVALID;

	if (fdp->fdidx < --eventp->fdcnt) {
		ev->fds[fdp->fdidx] = ev->fds[eventp->fdcnt];
		eventp->fdtabs[fdp->fdidx] = eventp->fdtabs[eventp->fdcnt];
		eventp->fdtabs[fdp->fdidx]->fdidx = fdp->fdidx;
	}
	fdp->fdidx = -1;

	if (fdp->fdidx_ready >= 0
		&& fdp->fdidx_ready < eventp->fdcnt_ready
		&& eventp->fdtabs_ready[fdp->fdidx_ready] == fdp)
	{
		eventp->fdtabs_ready[fdp->fdidx_ready] = NULL;
	}
	fdp->fdidx_ready = -1;

	acl_fdmap_del(ev->fdmap, sockfd);
}

/* event_disable_readwrite - disable request for read or write events */

static void event_disable_readwrite(ACL_EVENT *eventp, ACL_VSTREAM *stream)
{
	const char *myname = "event_disable_readwrite";
	EVENT_POLL *ev = (EVENT_POLL *) eventp;
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;
	ACL_SOCKET sockfd  = ACL_VSTREAM_SOCK(stream);

	if (fdp == NULL) {
		return;
	}

	if (fdp->flag == 0 || fdp->fdidx < 0 || fdp->fdidx >= eventp->fdcnt) {
		acl_msg_warn("%s(%d): sockfd(%d) no set, fdp no null",
			myname, __LINE__, sockfd);
		acl_fdmap_del(ev->fdmap, sockfd);
		event_fdtable_free(fdp);
		stream->fdp = NULL;
		return;
	}

	if (eventp->maxfd == sockfd)
		eventp->maxfd = ACL_SOCKET_INVALID;

	if (fdp->fdidx < --eventp->fdcnt) {
		ev->fds[fdp->fdidx] = ev->fds[eventp->fdcnt];
		eventp->fdtabs[fdp->fdidx] = eventp->fdtabs[eventp->fdcnt];
		eventp->fdtabs[fdp->fdidx]->fdidx = fdp->fdidx;
	}
	fdp->fdidx = -1;

	if (fdp->fdidx_ready >= 0
		&& fdp->fdidx_ready < eventp->fdcnt_ready
		&& eventp->fdtabs_ready[fdp->fdidx_ready] == fdp)
	{
		eventp->fdtabs_ready[fdp->fdidx_ready] = NULL;
	}
	fdp->fdidx_ready = -1;

	acl_fdmap_del(ev->fdmap, sockfd);
	event_fdtable_free(fdp);
	stream->fdp = NULL;
}

static void event_loop(ACL_EVENT *eventp)
{
	const char *myname = "event_loop";
	EVENT_POLL *ev = (EVENT_POLL *) eventp;
	ACL_EVENT_NOTIFY_TIME timer_fn;
	void    *timer_arg;
	ACL_EVENT_TIMER *timer;
	int   delay, nready, i, revents;
	ACL_EVENT_FDTABLE *fdp;

	delay = eventp->delay_sec * 1000 + eventp->delay_usec / 1000;
	if (delay < 0)
		delay = 0; /* 0 milliseconds at least */

	/* �����¼������ʱ��� */

	SET_TIME(eventp->present);

	/* ���ݶ�ʱ����������������� poll �ļ�ⳬʱ���� */

	if ((timer = ACL_FIRST_TIMER(&eventp->timer_head)) != 0) {
		acl_int64 n = timer->when - eventp->present;
		if (n <= 0)
			delay = 0;
		else if ((int) n < delay) {
			delay = (int) n;
			if (delay <= 0)  /* xxx */
				delay = 100;
		}
	}

	/* ���� event_prepare ����ж��ٸ���������Ҫͨ�� poll ���м�� */

	if (event_prepare(eventp) == 0) {

		/* ˵������ poll ��� */

		if (eventp->fdcnt_ready == 0) {
			/* Ϊ����ѭ�����죬����һ�� */
			sleep(1);
		}
		goto TAG_DONE;
	}

	/* ����Ѿ���������׼������ poll ��ⳬʱʱ���� 0 */

	if (eventp->fdcnt_ready > 0)
		delay = 0;

	/* ���� poll ϵͳ���ü����������� */

	nready = poll(ev->fds, eventp->fdcnt, delay);

	if (eventp->nested++ > 0)
		acl_msg_fatal("%s(%d): recursive call", myname, __LINE__);
	if (nready < 0) {
		if (acl_last_error() != ACL_EINTR) {
			acl_msg_fatal("%s(%d), %s: select: %s",
				__FILE__, __LINE__, myname,
				acl_last_serror());
		}
		goto TAG_DONE;
	} else if (nready == 0)
		goto TAG_DONE;

	/* ��� poll �ļ�������� */

	for (i = 0; i < eventp->fdcnt; i++) {
		fdp = acl_fdmap_ctx(ev->fdmap, ev->fds[i].fd);
		if (fdp == NULL || fdp->stream == NULL)
			continue;

		/* ����������ֶ����Ѿ��ڱ�����Ϊ�쳣��ʱ״̬����� */

		if ((fdp->event_type & (ACL_EVENT_XCPT | ACL_EVENT_RW_TIMEOUT)))
			continue;

		revents = ev->fds[i].revents;

		/* ����������Ƿ�����쳣 */

		if ((revents & (POLLHUP | POLLERR)) != 0) {
			fdp->event_type |= ACL_EVENT_XCPT;
			fdp->fdidx_ready = eventp->fdcnt_ready;
			eventp->fdtabs_ready[eventp->fdcnt_ready++] = fdp;
			continue;
		}

		/* ����������Ƿ�ɶ� */

		if ((fdp->flag & EVENT_FDTABLE_FLAG_READ) && (revents & POLLIN) ) {

			/* �������ֿɶ������� ACL_VSTREAM ��ϵͳ�ɶ���־�Ӷ�����
			 * ACL_VSTREAM ���ڶ�ʱ����ϵͳ�� read ����
			 */

			fdp->stream->sys_read_ready = 1;

			/* ���������ֶ��󸽼ӿɶ����� */

			if ((fdp->event_type & (ACL_EVENT_READ | ACL_EVENT_WRITE)) == 0)
			{
				fdp->event_type |= ACL_EVENT_READ;
				if (fdp->listener)
					fdp->event_type |= ACL_EVENT_ACCEPT;
				fdp->fdidx_ready = eventp->fdcnt_ready;
				eventp->fdtabs_ready[eventp->fdcnt_ready++] = fdp;
			}
		}

		/* ����������Ƿ��д */

		if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE) && (revents & POLLOUT)) {

			/* ���������ֶ��󸽼ӿ�д���� */

			if ((fdp->event_type & (ACL_EVENT_READ | ACL_EVENT_WRITE)) == 0)
			{
				fdp->event_type |= ACL_EVENT_WRITE;
				fdp->fdidx_ready = eventp->fdcnt_ready;
				eventp->fdtabs_ready[eventp->fdcnt_ready++] = fdp;
			}
		}
	}

TAG_DONE:

	/* �����¼������ʱ��� */

	SET_TIME(eventp->present);

	/* ���ȴ���ʱ���е����� */

	while ((timer = ACL_FIRST_TIMER(&eventp->timer_head)) != 0) {
		if (timer->when > eventp->present)
			break;
		timer_fn  = timer->callback;
		timer_arg = timer->context;

		/* �����ʱ����ʱ���� > 0 ������ʱ����ѭ�����ã��������趨ʱ�� */
		if (timer->delay > 0 && timer->keep) {
			timer->ncount++;
			eventp->timer_request(eventp, timer->callback,
				timer->context, timer->delay, timer->keep);
		} else {
			acl_ring_detach(&timer->ring);		/* first this */
			timer->nrefer--;
			if (timer->nrefer != 0)
				acl_msg_fatal("%s(%d): nrefer(%d) != 0",
					myname, __LINE__, timer->nrefer);
			acl_myfree(timer);
		}
		timer_fn(ACL_EVENT_TIME, eventp, timer_arg);
	}

	/* ����׼���õ��������¼� */

	if (eventp->fdcnt_ready > 0)
		event_fire(eventp);

	eventp->nested--;
}

static int event_isrset(ACL_EVENT *eventp acl_unused, ACL_VSTREAM *stream)
{
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	return fdp == NULL ? 0 : (fdp->flag & EVENT_FDTABLE_FLAG_READ);
}

static int event_iswset(ACL_EVENT *eventp acl_unused, ACL_VSTREAM *stream)
{
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	return fdp == NULL ? 0 : (fdp->flag & EVENT_FDTABLE_FLAG_WRITE);

}

static int event_isxset(ACL_EVENT *eventp acl_unused, ACL_VSTREAM *stream)
{
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	return fdp == NULL ? 0 : (fdp->flag & EVENT_FDTABLE_FLAG_EXPT);
}

static void event_free(ACL_EVENT *eventp)
{
	EVENT_POLL *ev = (EVENT_POLL *) eventp;

	acl_fdmap_free(ev->fdmap);
	acl_myfree(ev->fds);
	acl_myfree(ev);
}

ACL_EVENT *event_new_poll(int fdsize)
{
	ACL_EVENT *eventp;
	EVENT_POLL *ev;

	eventp = event_alloc(sizeof(EVENT_POLL));

	snprintf(eventp->name, sizeof(eventp->name), "events - poll");
	eventp->event_mode           = ACL_EVENT_POLL;
	eventp->use_thread           = 0;
	eventp->loop_fn              = event_loop;
	eventp->free_fn              = event_free;
	eventp->enable_read_fn       = event_enable_read;
	eventp->enable_write_fn      = event_enable_write;
	eventp->enable_listen_fn     = event_enable_read;
	eventp->disable_read_fn      = event_disable_read;
	eventp->disable_write_fn     = event_disable_write;
	eventp->disable_readwrite_fn = event_disable_readwrite;
	eventp->isrset_fn            = event_isrset;
	eventp->iswset_fn            = event_iswset;
	eventp->isxset_fn            = event_isxset;
	eventp->timer_request        = event_timer_request;
	eventp->timer_cancel         = event_timer_cancel;
	eventp->timer_keep           = event_timer_keep;
	eventp->timer_ifkeep         = event_timer_ifkeep;

	ev = (EVENT_POLL*) eventp;
	ev->fds = (struct pollfd *)
		acl_mycalloc(fdsize + 1, sizeof(struct pollfd));
	ev->fdmap = acl_fdmap_create(fdsize);
	return eventp;
}
#endif	/* ACL_EVENTS_POLL_STYLE */
