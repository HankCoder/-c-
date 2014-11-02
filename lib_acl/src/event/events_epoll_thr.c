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
#include "stdlib/acl_msg.h"
#include "stdlib/acl_ring.h"
#include "stdlib/acl_vstream.h"
#include "event/acl_events.h"

#endif

#include "events_define.h"

#ifdef	ACL_EVENTS_KERNEL_STYLE
#if (ACL_EVENTS_KERNEL_STYLE == ACL_EVENTS_STYLE_EPOLL)

#include <sys/epoll.h>
#include "events_fdtable.h"
#include "events.h"

typedef struct EVENT_EPOLL_THR {
	EVENT_THR event;
	struct epoll_event *ebuf;
	int   fdslots;
	int   handle;
} EVENT_EPOLL_THR;

static void event_enable_read(ACL_EVENT *eventp, ACL_VSTREAM *stream,
	int timeout, ACL_EVENT_NOTIFY_RDWR callback, void *context)
{
	const char *myname = "event_enable_read";
	EVENT_EPOLL_THR *event_thr = (EVENT_EPOLL_THR *) eventp;
	ACL_EVENT_FDTABLE *fdp;
	ACL_SOCKET sockfd;
	struct epoll_event ev;
	int   fd_ready;

	if (ACL_VSTREAM_BFRD_CNT(stream) > 0
		|| (stream->flag & ACL_VSTREAM_FLAG_BAD))
	{
		fd_ready = 1;
	} else
		fd_ready = 0;

	sockfd = ACL_VSTREAM_SOCK(stream);
	fdp = (ACL_EVENT_FDTABLE*) stream->fdp;
	if (fdp == NULL) {
		fdp = event_fdtable_alloc();
		fdp->listener = 0;
		fdp->stream = stream;
		stream->fdp = (void *) fdp;
	} else if (fdp->flag & EVENT_FDTABLE_FLAG_WRITE)
		acl_msg_panic("%s(%d), %s: fd %d: multiple I/O request",
			__FILE__, __LINE__, myname, sockfd);
	else {
		fdp->listener = 0;
		fdp->stream = stream;
	}

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

	if ((fdp->flag & EVENT_FDTABLE_FLAG_READ) != 0)
	{
		acl_msg_info("has set read");
		return;
	}

	stream->nrefer++;
	fdp->flag = EVENT_FDTABLE_FLAG_READ | EVENT_FDTABLE_FLAG_EXPT;

#if 0
	ev.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLET;
#else
	ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
#endif
	ev.data.u64 = 0;  /* avoid valgrind warning */
	ev.data.ptr = fdp;

	THREAD_LOCK(&event_thr->event.tb_mutex);

	fdp->fdidx = eventp->fdcnt;
	eventp->fdtabs[eventp->fdcnt++] = fdp;

	if (fd_ready) {
		if (epoll_ctl(event_thr->handle, EPOLL_CTL_ADD,
			sockfd, &ev) < 0)
		{
			if (errno == EEXIST)
				acl_msg_warn("%s: epoll_ctl: %s, fd: %d",
					myname, acl_last_serror(), sockfd);
			else
				acl_msg_fatal("%s: epoll_ctl: %s, fd: %d",
					myname, acl_last_serror(), sockfd);
		}

		THREAD_UNLOCK(&event_thr->event.tb_mutex);

		if (event_thr->event.blocked && event_thr->event.evdog
			&& event_dog_client(event_thr->event.evdog) != stream)
		{
			event_dog_notify(event_thr->event.evdog);
		}
	} else {
		THREAD_UNLOCK(&event_thr->event.tb_mutex);

		if (epoll_ctl(event_thr->handle, EPOLL_CTL_ADD,
			sockfd, &ev) < 0)
		{
			if (errno == EEXIST)
				acl_msg_warn("%s: epoll_ctl: %s, fd: %d",
					myname, acl_last_serror(), sockfd);
			else
				acl_msg_fatal("%s: epoll_ctl: %s, fd: %d",
					myname, acl_last_serror(), sockfd);
		}
	}
}

static void event_enable_listen(ACL_EVENT *eventp, ACL_VSTREAM *stream,
	int timeout, ACL_EVENT_NOTIFY_RDWR callback, void *context)
{
	const char *myname = "event_enable_listen";
	EVENT_EPOLL_THR *event_thr = (EVENT_EPOLL_THR *) eventp;
	ACL_EVENT_FDTABLE *fdp;
	ACL_SOCKET sockfd;
	struct epoll_event ev;

	sockfd = ACL_VSTREAM_SOCK(stream);
	fdp = (ACL_EVENT_FDTABLE*) stream->fdp;
	if (fdp == NULL) {
		fdp = event_fdtable_alloc();
		fdp->stream = stream;
		fdp->listener = 1;
		stream->fdp = (void *) fdp;
	} else if (fdp->flag & EVENT_FDTABLE_FLAG_WRITE)
		acl_msg_panic("%s(%d)->%s: fd %d: multiple I/O request",
			__FILE__, __LINE__, myname, sockfd);
	else {
		fdp->stream = stream;
		fdp->listener = 1;
	}

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

	if ((fdp->flag & EVENT_FDTABLE_FLAG_READ) != 0)
		return;

	fdp->flag = EVENT_FDTABLE_FLAG_READ | EVENT_FDTABLE_FLAG_EXPT;
	stream->nrefer++;

	ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
	ev.data.u64 = 0;  /* avoid valgrind warning */
	ev.data.ptr = fdp;

	THREAD_LOCK(&event_thr->event.tb_mutex);

	fdp->fdidx = eventp->fdcnt;
	eventp->fdtabs[eventp->fdcnt++] = fdp;

	THREAD_UNLOCK(&event_thr->event.tb_mutex);

	if (epoll_ctl(event_thr->handle, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
		if (errno == EEXIST)
			acl_msg_warn("%s: epool_ctl: %s, fd: %d",
				myname, acl_last_serror(), sockfd);
		else
			acl_msg_fatal("%s: epool_ctl: %s, fd: %d",
				myname, acl_last_serror(), sockfd);
	}
}

static void event_enable_write(ACL_EVENT *eventp, ACL_VSTREAM *stream,
	int timeout, ACL_EVENT_NOTIFY_RDWR callback, void *context)
{
	const char *myname = "event_enable_write";
	EVENT_EPOLL_THR *event_thr = (EVENT_EPOLL_THR *) eventp;
	ACL_EVENT_FDTABLE *fdp;
	ACL_SOCKET sockfd;
	struct epoll_event ev;
	int   fd_ready;

	if ((stream->flag & ACL_VSTREAM_FLAG_BAD))
		fd_ready = 1;
	else
		fd_ready = 0;

	sockfd = ACL_VSTREAM_SOCK(stream);
	fdp = (ACL_EVENT_FDTABLE*) stream->fdp;
	if (fdp == NULL) {
		fdp = event_fdtable_alloc();
		fdp->listener = 0;
		fdp->stream = stream;
		stream->fdp = (void *) fdp;
	} else if (fdp->flag & EVENT_FDTABLE_FLAG_READ)
		acl_msg_panic("%s(%d)->%s: fd %d: multiple I/O request",
			__FILE__, __LINE__, myname, sockfd);
	else {
		fdp->listener = 0;
		fdp->stream = stream;
	}

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

	if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE) != 0)
		return;

	fdp->flag = EVENT_FDTABLE_FLAG_WRITE | EVENT_FDTABLE_FLAG_EXPT;
	stream->nrefer++;

	ev.events = EPOLLIN | EPOLLHUP | EPOLLERR;
	ev.data.u64 = 0;  /* avoid valgrind warning */
	ev.data.ptr = fdp;

	THREAD_LOCK(&event_thr->event.tb_mutex);

	fdp->fdidx = eventp->fdcnt;
	eventp->fdtabs[eventp->fdcnt++] = fdp;

	if (fd_ready) {
		if (epoll_ctl(event_thr->handle, EPOLL_CTL_ADD,
			sockfd, &ev) < 0)
		{
			if (errno == EEXIST)
				acl_msg_warn("%s: epoll_ctl: %s, fd: %d",
					myname, acl_last_serror(), sockfd);
			else
				acl_msg_fatal("%s: epoll_ctl: %s, fd: %d",
					myname, acl_last_serror(), sockfd);
		}

		THREAD_UNLOCK(&event_thr->event.tb_mutex);
	} else {
		THREAD_UNLOCK(&event_thr->event.tb_mutex);

		if (epoll_ctl(event_thr->handle, EPOLL_CTL_ADD,
			sockfd, &ev) < 0)
		{
			if (errno == EEXIST)
				acl_msg_warn("%s: epoll_ctl: %s, fd: %d",
					myname, acl_last_serror(), sockfd);
			else
				acl_msg_fatal("%s: epoll_ctl: %s, fd: %d",
					myname, acl_last_serror(), sockfd);
		}
	}
}

/* event_disable_readwrite - disable request for read or write events */

static void event_disable_readwrite(ACL_EVENT *eventp, ACL_VSTREAM *stream)
{
	const char *myname = "event_disable_readwrite";
	EVENT_EPOLL_THR *event_thr = (EVENT_EPOLL_THR *) eventp;
	ACL_EVENT_FDTABLE *fdp;
	ACL_SOCKET sockfd;
	struct epoll_event dummy;

	dummy.events = EPOLLHUP | EPOLLERR;
	dummy.data.u64 = 0;  /* avoid valgrind warning */
	dummy.data.ptr = NULL;

	sockfd = ACL_VSTREAM_SOCK(stream);

	fdp = (ACL_EVENT_FDTABLE *) stream->fdp;
	if (fdp == NULL) {
		acl_msg_error("%s(%d): fdp null", myname, __LINE__);
		return;
	}

	if ((fdp->flag & (EVENT_FDTABLE_FLAG_READ
		| EVENT_FDTABLE_FLAG_WRITE)) == 0)
	{
		acl_msg_error("%s(%d): sockfd(%d) not be set",
			myname, __LINE__, sockfd);
		return;
	}
	if (fdp->fdidx == -1)
		acl_msg_fatal("%s(%d): fdidx(%d) invalid",
			myname, __LINE__, fdp->fdidx);

	if (eventp->fdtabs[fdp->fdidx] != fdp)
		acl_msg_fatal("%s(%d): fdidx(%d)'s fdp invalid",
			myname, __LINE__, fdp->fdidx);

	THREAD_LOCK(&event_thr->event.tb_mutex);

	if (fdp->fdidx < --eventp->fdcnt) {
		eventp->fdtabs[fdp->fdidx] = eventp->fdtabs[eventp->fdcnt];
		eventp->fdtabs[fdp->fdidx]->fdidx = fdp->fdidx;
	}

	if (fdp->flag & EVENT_FDTABLE_FLAG_READ) {
		stream->nrefer--;
		dummy.events |= EPOLLIN;
	}

	if (fdp->flag & EVENT_FDTABLE_FLAG_WRITE) {
		stream->nrefer--;
		dummy.events |= EPOLLOUT;
	}

	THREAD_UNLOCK(&event_thr->event.tb_mutex);

	if (epoll_ctl(event_thr->handle, EPOLL_CTL_DEL, sockfd, &dummy) < 0) {
		if (errno == ENOENT)
			acl_msg_warn("%s: epoll_ctl: %s, fd: %d",
				myname, acl_last_serror(), sockfd);
		else
			acl_msg_fatal("%s: epoll_ctl: %s, fd: %d",
				myname, acl_last_serror(), sockfd);
	}

	event_fdtable_reset(fdp);
}

static int event_isrset(ACL_EVENT *eventp acl_unused, ACL_VSTREAM *stream)
{
	ACL_EVENT_FDTABLE *fdp;

	fdp = (ACL_EVENT_FDTABLE *) stream->fdp;
	if (fdp == NULL)
		return (0);

	return ((fdp->flag & EVENT_FDTABLE_FLAG_READ));
}

static int event_iswset(ACL_EVENT *eventp acl_unused, ACL_VSTREAM *stream)
{
	ACL_EVENT_FDTABLE *fdp;

	fdp = (ACL_EVENT_FDTABLE *) stream->fdp;
	if (fdp == NULL)
		return (0);

	return ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE));
}

static int event_isxset(ACL_EVENT *eventp acl_unused, ACL_VSTREAM *stream)
{
	ACL_EVENT_FDTABLE *fdp;

	fdp = (ACL_EVENT_FDTABLE *) stream->fdp;
	if (fdp == NULL)
		return (0);

	return ((fdp->flag & EVENT_FDTABLE_FLAG_EXPT));
}

static void event_loop(ACL_EVENT *eventp)
{
	const char *myname = "event_loop";
	EVENT_EPOLL_THR *event_thr = (EVENT_EPOLL_THR *) eventp;
	ACL_EVENT_NOTIFY_TIME timer_fn;
	void    *timer_arg;
	ACL_EVENT_TIMER *timer;
	int   delay, nready;
	ACL_EVENT_FDTABLE *fdp;
	ACL_RING timer_ring, *entry_ptr;
	EVENT_BUFFER *bp;

	acl_ring_init(&timer_ring);

	delay = eventp->delay_sec * 1000 + eventp->delay_usec / 1000;
	if (delay <= 0)
		delay = 100; /* 100 milliseconds at least */

	SET_TIME(eventp->present);

	THREAD_LOCK(&event_thr->event.tm_mutex);

	/*
	 * Find out when the next timer would go off. Timer requests are
	 * sorted. If any timer is scheduled, adjust the delay appropriately.
	 */
	if ((timer = ACL_FIRST_TIMER(&eventp->timer_head)) != 0) {
		acl_int64  n = (timer->when - eventp->present + 1000000 - 1)
			/ 1000000;
		if (n <= 0)
			delay = 0;
		else if (n < eventp->delay_sec) {
			delay = (int) n * 1000 + eventp->delay_usec / 1000;
			if (delay <= 0) {  /* xxx */
				acl_msg_warn("%s(%d): interger overflow, "
					"when: %lld, present: %lld, delay: %d",
					myname, __LINE__, timer->when,
					eventp->present, delay);
				delay = 100;
			}
		}
	}

	THREAD_UNLOCK(&event_thr->event.tm_mutex);

	eventp->fdcnt_ready = 0;

	if (eventp->present - eventp->last_check >= eventp->check_inter) {
		eventp->last_check = eventp->present;

		THREAD_LOCK(&event_thr->event.tb_mutex);

		if (event_thr_prepare(eventp) == 0) {
			THREAD_UNLOCK(&event_thr->event.tb_mutex);

			if (eventp->fdcnt_ready == 0)
				sleep(1);

			nready = 0;
			goto TAG_DONE;
		}

		THREAD_UNLOCK(&event_thr->event.tb_mutex);

		if (eventp->fdcnt_ready > 0)
			delay = 0;
	}

	event_thr->event.blocked = 1;
	nready = epoll_wait(event_thr->handle, event_thr->ebuf,
			event_thr->fdslots, delay);
	event_thr->event.blocked = 0;

	if (nready < 0) {
		if (acl_last_error() != ACL_EINTR)
			acl_msg_fatal("%s(%d), %s: event_loop: epoll: %s",
				__FILE__, __LINE__, myname, acl_last_serror());
		goto TAG_DONE;
	} else if (nready == 0)
		goto TAG_DONE;

	for (bp = event_thr->ebuf; bp < event_thr->ebuf + nready; bp++) {
		fdp = (ACL_EVENT_FDTABLE *) bp->data.ptr;
		if ((fdp->event_type & (ACL_EVENT_XCPT | ACL_EVENT_RW_TIMEOUT)))
			continue;

		if ((bp->events & EPOLLIN) != 0) {
			if ((fdp->event_type & ACL_EVENT_READ) == 0) {
				fdp->event_type |= ACL_EVENT_READ;
				fdp->fdidx_ready = eventp->fdcnt_ready;
				eventp->fdtabs_ready[eventp->fdcnt_ready] = fdp;
				eventp->fdcnt_ready++;
			}
			if (fdp->listener)
				fdp->event_type |= ACL_EVENT_ACCEPT;
			fdp->stream->sys_read_ready = 1;
		} else if ((bp->events & EPOLLOUT) != 0) {
			fdp->event_type |= ACL_EVENT_WRITE;
			fdp->fdidx_ready = eventp->fdcnt_ready;
			eventp->fdtabs_ready[eventp->fdcnt_ready++] = fdp;
		} else if ((bp->events & (EPOLLERR | EPOLLHUP)) != 0) {
			fdp->event_type |= ACL_EVENT_XCPT;
			fdp->fdidx_ready = eventp->fdcnt_ready;
			eventp->fdtabs_ready[eventp->fdcnt_ready++] = fdp;
		}
	}

TAG_DONE:

	/*
	 * Deliver timer events. Requests are sorted: we can stop when
	 * we reach the future or the list end. Allow the application
	 * to update the timer queue while it is being called back. To
	 * this end, we repeatedly pop the first request off the timer
	 * queue before delivering the event to the application.
	 */

	SET_TIME(eventp->present);

	THREAD_LOCK(&event_thr->event.tm_mutex);

	while ((timer = ACL_FIRST_TIMER(&eventp->timer_head)) != NULL) {
		if (timer->when > eventp->present)
			break;

		acl_ring_detach(&timer->ring);  /* first this */
		acl_ring_prepend(&timer_ring, &timer->ring);
	}

	THREAD_UNLOCK(&event_thr->event.tm_mutex);

	while ((entry_ptr = acl_ring_pop_head(&timer_ring)) != NULL) {
		timer     = ACL_RING_TO_TIMER(entry_ptr);
		timer_fn  = timer->callback;
		timer_arg = timer->context;
		acl_myfree(timer);

		timer_fn(ACL_EVENT_TIME, eventp, timer_arg);
	}

	if (eventp->fdcnt_ready > 0)
		event_thr_fire(eventp);
}

static void event_add_dog(ACL_EVENT *eventp)
{
	EVENT_EPOLL_THR *event_thr = (EVENT_EPOLL_THR*) eventp;

	event_thr->event.evdog = event_dog_create((ACL_EVENT*) event_thr, 1);
}

static void event_free(ACL_EVENT *eventp)
{
	const char *myname = "event_free";
	EVENT_EPOLL_THR *event_thr = (EVENT_EPOLL_THR *) eventp;

	if (eventp == NULL)
		acl_msg_fatal("%s, %s(%d): eventp null",
			__FILE__, myname, __LINE__);

	LOCK_DESTROY(&event_thr->event.tm_mutex);
	LOCK_DESTROY(&event_thr->event.tb_mutex);

	acl_myfree(event_thr->ebuf);
	close(event_thr->handle);
	acl_myfree(eventp);
}

ACL_EVENT *event_epoll_alloc_thr(int fdsize acl_unused)
{
	EVENT_EPOLL_THR *event_thr;
	static int __default_max_events = 100;

	event_thr = (EVENT_EPOLL_THR*) event_alloc(sizeof(EVENT_EPOLL_THR));

	snprintf(event_thr->event.event.name,
		sizeof(event_thr->event.event.name), "thread events - epoll");
	event_thr->event.event.event_mode           = ACL_EVENT_KERNEL;
	event_thr->event.event.use_thread           = 1;
	event_thr->event.event.loop_fn              = event_loop;
	event_thr->event.event.free_fn              = event_free;
	event_thr->event.event.add_dog_fn           = event_add_dog;
	event_thr->event.event.enable_read_fn       = event_enable_read;
	event_thr->event.event.enable_write_fn      = event_enable_write;
	event_thr->event.event.enable_listen_fn     = event_enable_listen;
	event_thr->event.event.disable_readwrite_fn = event_disable_readwrite;
	event_thr->event.event.isrset_fn            = event_isrset;
	event_thr->event.event.iswset_fn            = event_iswset;
	event_thr->event.event.isxset_fn            = event_isxset;
	event_thr->event.event.timer_request        = event_timer_request_thr;
	event_thr->event.event.timer_cancel         = event_timer_cancel_thr;
	event_thr->event.event.timer_keep           = event_timer_keep_thr;
	event_thr->event.event.timer_ifkeep         = event_timer_ifkeep_thr;

	LOCK_INIT(&event_thr->event.tm_mutex);
	LOCK_INIT(&event_thr->event.tb_mutex);

	event_thr->handle = epoll_create(fdsize);
	event_thr->fdslots = __default_max_events;
	event_thr->ebuf = (EVENT_BUFFER *) acl_mycalloc(event_thr->fdslots + 1,
			sizeof(EVENT_BUFFER));

	acl_msg_info("%s(%d), %s, use %s", __FILE__, __LINE__, __FUNCTION__,
		event_thr->event.event.name);

	return ((ACL_EVENT *) event_thr);
}

#endif	/* ACL_EVENTS_KERNEL_STYLE == ACL_EVENTS_STYLE_EPOLL */
#endif  /* ACL_EVENTS_KERNEL_STYLE */
