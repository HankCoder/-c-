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

#ifdef	ACL_UNIX
#include <unistd.h>
#endif

#include "stdlib/acl_sys_patch.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_mystring.h"
#include "stdlib/acl_msg.h"
#include "stdlib/acl_debug.h"
#include "stdlib/acl_vstream.h"
#include "event/acl_events.h"

#endif

#include "events.h"
#include "events_fdtable.h"

typedef struct EVENT_SELECT {
	ACL_EVENT event;
	fd_set rmask;
	fd_set wmask;
	fd_set xmask;
} EVENT_SELECT;

static void stream_on_close(ACL_VSTREAM *stream, void *arg)
{
	EVENT_SELECT *ev = (EVENT_SELECT*) arg;
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE*) stream->fdp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);

	if (fdp == NULL)
		return;

	FD_CLR(sockfd, &ev->xmask);

	if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)
		&& (fdp->flag & EVENT_FDTABLE_FLAG_WRITE))
	{
		FD_CLR(sockfd, &ev->rmask);
		FD_CLR(sockfd, &ev->wmask);
	} else if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
		FD_CLR(sockfd, &ev->rmask);
	} else if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
		FD_CLR(sockfd, &ev->wmask);
	}

	if (ev->event.maxfd == ACL_VSTREAM_SOCK(fdp->stream))
		ev->event.maxfd = ACL_SOCKET_INVALID;
	if (fdp->fdidx >= 0 && fdp->fdidx < --ev->event.fdcnt) {
		ev->event.fdtabs[fdp->fdidx] = ev->event.fdtabs[ev->event.fdcnt];
		ev->event.fdtabs[fdp->fdidx]->fdidx = fdp->fdidx;
		fdp->fdidx = -1;
	}

	if (fdp->fdidx_ready >= 0
		&& fdp->fdidx_ready < ev->event.fdcnt_ready
		&& ev->event.fdtabs_ready[fdp->fdidx_ready] == fdp)
	{
		ev->event.fdtabs_ready[fdp->fdidx_ready] = NULL;
		fdp->fdidx_ready = -1;
	}
	event_fdtable_free(fdp);
	stream->fdp = NULL;
}

static void event_enable_read(ACL_EVENT *eventp, ACL_VSTREAM *stream,
	int timeout, ACL_EVENT_NOTIFY_RDWR callback, void *context)
{
	EVENT_SELECT *ev = (EVENT_SELECT *) eventp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	if (fdp == NULL) {
		fdp = event_fdtable_alloc();

		fdp->stream = stream;
		stream->fdp = (void *) fdp;
		acl_vstream_add_close_handle(stream, stream_on_close, eventp);
	}

	if (fdp->fdidx == -1) {
		fdp->fdidx = eventp->fdcnt;
		eventp->fdtabs[eventp->fdcnt++] = fdp;
	}

	if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
		fdp->flag |= EVENT_FDTABLE_FLAG_READ;
	} else {
		fdp->flag = EVENT_FDTABLE_FLAG_READ | EVENT_FDTABLE_FLAG_EXPT;
	}
	FD_SET(sockfd, &ev->rmask);
	FD_SET(sockfd, &ev->xmask);

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
	EVENT_SELECT *ev = (EVENT_SELECT *) eventp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	if (fdp == NULL) {
		fdp = event_fdtable_alloc();

		fdp->stream = stream;
		stream->fdp = (void *) fdp;
		acl_vstream_add_close_handle(stream, stream_on_close, eventp);
	}

	if (fdp->fdidx == -1) {
		fdp->fdidx = eventp->fdcnt;
		eventp->fdtabs[eventp->fdcnt++] = fdp;
	}

	if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
		fdp->flag |= EVENT_FDTABLE_FLAG_WRITE;
	} else {
		fdp->flag = EVENT_FDTABLE_FLAG_WRITE | EVENT_FDTABLE_FLAG_EXPT;
	}
	FD_SET(sockfd, &ev->wmask);
	FD_SET(sockfd, &ev->xmask);

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
	EVENT_SELECT *ev = (EVENT_SELECT *) eventp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	if (fdp == NULL) {
		acl_msg_warn("%s(%d): fdp null", myname, __LINE__);
		return;
	}

	if (fdp->fdidx < 0 || fdp->fdidx >= eventp->fdcnt) {
		acl_msg_warn("%s(%d): sockfd(%d)'s fdidx invalid",
			myname, __LINE__, sockfd);
		return;
	}

	if (!(fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
		acl_msg_warn("%s(%d): sockfd(%d) not in rmask",
			myname, __LINE__, sockfd);
		return;
	}

	fdp->r_ttl = 0;
	fdp->r_timeout = 0;
	fdp->r_callback = NULL;
	fdp->event_type &= ~(ACL_EVENT_READ | ACL_EVENT_ACCEPT);
	fdp->flag &= ~EVENT_FDTABLE_FLAG_READ;

	if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
		FD_CLR(sockfd, &ev->rmask);
		return;
	}

	if (eventp->maxfd == sockfd)
		eventp->maxfd = ACL_SOCKET_INVALID;

	if (fdp->fdidx < --eventp->fdcnt) {
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

	FD_CLR(sockfd, &ev->xmask);
	FD_CLR(sockfd, &ev->rmask);
#if 0
	event_fdtable_free(fdp);
	stream->fdp = NULL;
#endif
}

/* event_disable_write - disable request for write events */

static void event_disable_write(ACL_EVENT *eventp, ACL_VSTREAM *stream)
{
	const char *myname = "event_disable_write";
	EVENT_SELECT *ev = (EVENT_SELECT *) eventp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;

	if (fdp == NULL) {
		acl_msg_warn("%s(%d): fdp null", myname, __LINE__);
		return;
	}

	if (fdp->fdidx < 0 || fdp->fdidx >= eventp->fdcnt) {
		acl_msg_warn("%s(%d): sockfd(%d)'s fdidx invalid",
			myname, __LINE__, sockfd);
		return;
	}

	if (!(fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
		acl_msg_warn("%s(%d): sockfd(%d) not in wmask",
			myname, __LINE__, sockfd);
		return;
	}

	fdp->w_ttl = 0;
	fdp->w_timeout = 0;
	fdp->w_callback = NULL;
	fdp->event_type &= ~(ACL_EVENT_WRITE | ACL_EVENT_CONNECT);
	fdp->flag &= ~EVENT_FDTABLE_FLAG_WRITE;

	if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
		FD_CLR(sockfd, &ev->wmask);
		return;
	}

	if (eventp->maxfd == sockfd)
		eventp->maxfd = ACL_SOCKET_INVALID;

	if (fdp->fdidx < --eventp->fdcnt) {
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

	FD_CLR(sockfd, &ev->xmask);
	FD_CLR(sockfd, &ev->wmask);
#if 0
	event_fdtable_free(fdp);
	stream->fdp = NULL;
#endif
}

/* event_disable_readwrite - disable request for read or write events */

static void event_disable_readwrite(ACL_EVENT *eventp, ACL_VSTREAM *stream)
{
	const char *myname = "event_disable_readwrite";
	EVENT_SELECT *ev = (EVENT_SELECT *) eventp;
	ACL_EVENT_FDTABLE *fdp = (ACL_EVENT_FDTABLE *) stream->fdp;
	ACL_SOCKET sockfd = ACL_VSTREAM_SOCK(stream);

	if (fdp == NULL) {
		return;
	}

	if (fdp->flag == 0 || fdp->fdidx < 0 || fdp->fdidx >= eventp->fdcnt) {
		acl_msg_warn("%s(%d): sockfd(%d) no set, fdp no null",
			myname, __LINE__, sockfd);
		event_fdtable_free(fdp);
		stream->fdp = NULL;
		return;
	}

	if (!FD_ISSET(sockfd, &ev->rmask) && !FD_ISSET(sockfd, &ev->wmask)) {
		acl_msg_error("%s(%d): sockfd(%d) no set, fdp no null",
			myname, __LINE__, sockfd);
		event_fdtable_free(fdp);
		stream->fdp = NULL;
		return;
	}

	if (eventp->maxfd == sockfd)
		eventp->maxfd = ACL_SOCKET_INVALID;

	if (fdp->fdidx < --eventp->fdcnt) {
		eventp->fdtabs[fdp->fdidx] = eventp->fdtabs[eventp->fdcnt];
		eventp->fdtabs[fdp->fdidx]->fdidx = fdp->fdidx;
	}
	fdp->fdidx = -1;

	if (FD_ISSET(sockfd, &ev->rmask)) {
		FD_CLR(sockfd, &ev->rmask);
	}
	if (FD_ISSET(sockfd, &ev->wmask)) {
		FD_CLR(sockfd, &ev->wmask);
	}
	FD_CLR(sockfd, &ev->xmask);

	if (fdp->fdidx_ready >= 0
		&& fdp->fdidx_ready < eventp->fdcnt_ready
		&& eventp->fdtabs_ready[fdp->fdidx_ready] == fdp)
	{
		eventp->fdtabs_ready[fdp->fdidx_ready] = NULL;
	}
	fdp->fdidx_ready = -1;
	event_fdtable_free(fdp);
	stream->fdp = NULL;
}

static void event_loop(ACL_EVENT *eventp)
{
	const char *myname = "event_loop";
	EVENT_SELECT *ev = (EVENT_SELECT *) eventp;
	ACL_EVENT_NOTIFY_TIME timer_fn;
	void    *timer_arg;
	ACL_SOCKET sockfd;
	ACL_EVENT_TIMER *timer;
	int   nready, i;
	acl_int64 delay;
	ACL_EVENT_FDTABLE *fdp;
	struct timeval tv, *tvp;
	fd_set rmask;  /* enabled read events */
	fd_set wmask;  /* enabled write events */
	fd_set xmask;  /* for bad news mostly */

	delay = eventp->delay_sec * 1000000 + eventp->delay_usec;

	/* �����¼������ʱ��� */

	SET_TIME(eventp->present);

	/* ���ݶ�ʱ����������������� select �ļ�ⳬʱ���� */

	if ((timer = ACL_FIRST_TIMER(&eventp->timer_head)) != 0) {
		acl_int64 n = timer->when - eventp->present;

		if (n <= 0)
			delay = 0;
		else if (n < delay)
			delay = n;
	}

	/* ���� event_prepare ����ж��ٸ���������Ҫͨ�� select ���м�� */

	if (event_prepare(eventp) == 0) {
		if (eventp->fdcnt_ready == 0) {
			delay /= 1000000;
			if (delay <= 0)
				delay = 1;
			/* Ϊ����ѭ�����죬����һ�� */
			sleep((int) delay);
		}

		goto TAG_DONE;
	}

	if (eventp->fdcnt_ready > 0) {
		tv.tv_sec  = 0;
		tv.tv_usec = 0;
		tvp = &tv;
	} else if (delay >= 0) {
		tv.tv_sec  = (unsigned long) delay / 1000000;
		tv.tv_usec = (unsigned long) delay - tv.tv_sec * 1000000;
		tvp = &tv;
	} else
		tvp = NULL;

	rmask = ev->rmask;
	wmask = ev->wmask;
	xmask = ev->xmask;

	/* ���� select ϵͳ���ü����������� */

#ifdef WIN32
	nready = select(0, &rmask, &wmask, &xmask, tvp);
#else
	nready = select(eventp->maxfd + 1, &rmask, &wmask, &xmask, tvp);
#endif

	if (eventp->nested++ > 0)
		acl_msg_fatal("%s(%d): recursive call(%d)",
			myname, __LINE__, eventp->nested);
	if (nready < 0) {
		if (acl_last_error() != ACL_EINTR) {
			acl_msg_fatal("%s(%d), %s: select: %s",
				__FILE__, __LINE__, myname, acl_last_serror());
		}
		goto TAG_DONE;
	} else if (nready == 0)
		goto TAG_DONE;

	/* ��� select �ļ�������� */

	/* if some fdp was cleared from eventp->fdtabs in timer callback,
	 * which has no effection on the rest fdp in eventp->fdtabs
	 */

	for (i = 0; i < eventp->fdcnt; i++) {
		fdp = eventp->fdtabs[i];

		/* ����������ֶ����Ѿ��ڱ�����Ϊ�쳣��ʱ״̬����� */

		if ((fdp->event_type & (ACL_EVENT_XCPT | ACL_EVENT_RW_TIMEOUT)))
			continue;

		sockfd = ACL_VSTREAM_SOCK(fdp->stream);

		/* ����������Ƿ�����쳣 */

		if (FD_ISSET(sockfd, &xmask)) {
			fdp->event_type |= ACL_EVENT_XCPT;
			fdp->fdidx_ready = eventp->fdcnt_ready;
			eventp->fdtabs_ready[eventp->fdcnt_ready++] = fdp;
			continue;
		}

		/* ����������Ƿ�ɶ� */

		if (FD_ISSET(sockfd, &rmask)) {

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

		if (FD_ISSET(sockfd, &wmask)) {

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

static int event_isrset(ACL_EVENT *eventp, ACL_VSTREAM *stream)
{
	EVENT_SELECT *ev = (EVENT_SELECT *) eventp;

	return FD_ISSET(ACL_VSTREAM_SOCK(stream), &ev->rmask);
}

static int event_iswset(ACL_EVENT *eventp, ACL_VSTREAM *stream)
{
	EVENT_SELECT *ev = (EVENT_SELECT *) eventp;

	return FD_ISSET(ACL_VSTREAM_SOCK(stream), &ev->wmask);
}

static int event_isxset(ACL_EVENT *eventp, ACL_VSTREAM *stream)
{
	EVENT_SELECT *ev = (EVENT_SELECT *) eventp;

	return FD_ISSET(ACL_VSTREAM_SOCK(stream), &ev->xmask);
}

static void event_free(ACL_EVENT *eventp)
{
	EVENT_SELECT *ev = (EVENT_SELECT *) eventp;

	acl_myfree(ev);
}

ACL_EVENT *event_new_select(void)
{
	ACL_EVENT *eventp;
	EVENT_SELECT *ev;

	eventp = event_alloc(sizeof(EVENT_SELECT));

	snprintf(eventp->name, sizeof(eventp->name), "events - select");
	eventp->event_mode           = ACL_EVENT_SELECT;
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

	ev = (EVENT_SELECT*) eventp;
	FD_ZERO(&ev->rmask);
	FD_ZERO(&ev->wmask);
	FD_ZERO(&ev->xmask);
	return eventp;
}
