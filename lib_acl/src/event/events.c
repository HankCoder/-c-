#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_msg.h"

#endif

#include "events.h"

void event_check_fds(ACL_EVENT *ev)
{
	ACL_EVENT_FDTABLE *fdp;
	int   i;

	for (i = 0; i < ev->fdcnt; i++) {
		fdp = ev->fdtabs[i];
		if ((fdp->stream->flag & ACL_VSTREAM_FLAG_BAD) != 0) {
			fdp->stream->flag &= ~ACL_VSTREAM_FLAG_BAD;
			fdp->event_type |= ACL_EVENT_XCPT;
			fdp->fdidx_ready = ev->fdcnt_ready;
			ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
		} else if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
			if (ACL_VSTREAM_BFRD_CNT(fdp->stream) > 0) {
				fdp->stream->sys_read_ready = 0;
				fdp->event_type |= ACL_EVENT_READ;
				fdp->fdidx_ready = ev->fdcnt_ready;
				ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
			} else if (fdp->r_ttl > 0 && ev->present > fdp->r_ttl) {
				fdp->event_type |= ACL_EVENT_RW_TIMEOUT;
				fdp->fdidx_ready = ev->fdcnt_ready;
				ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
			}
		} else if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
			if (fdp->w_ttl > 0 && ev->present > fdp->w_ttl) {
				fdp->event_type |= ACL_EVENT_RW_TIMEOUT;
				fdp->fdidx_ready = ev->fdcnt_ready;
				ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
			}
		}
	}
}

int event_prepare(ACL_EVENT *ev)
{
	ACL_EVENT_FDTABLE *fdp;
	ACL_SOCKET sockfd;
	int   i, nwait = 0;

	ev->fdcnt_ready = 0;

	for (i = 0; i < ev->fdcnt; i++) {
		fdp = ev->fdtabs[i];
		sockfd = ACL_VSTREAM_SOCK(fdp->stream);
		fdp->event_type = 0;
		if (ev->maxfd < sockfd)
			ev->maxfd = sockfd;
		if ((fdp->stream->flag & ACL_VSTREAM_FLAG_BAD) != 0) {
			fdp->stream->flag &= ~ACL_VSTREAM_FLAG_BAD;
			fdp->event_type |= ACL_EVENT_XCPT;
			fdp->fdidx_ready = ev->fdcnt_ready;
			ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
		} else if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
			if (ACL_VSTREAM_BFRD_CNT(fdp->stream) > 0) {
				fdp->stream->sys_read_ready = 0;
				fdp->event_type |= ACL_EVENT_READ;
				fdp->fdidx_ready = ev->fdcnt_ready;
				ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
			} else if (fdp->r_ttl > 0 && ev->present > fdp->r_ttl) {
				fdp->event_type |= ACL_EVENT_RW_TIMEOUT;
				fdp->fdidx_ready = ev->fdcnt_ready;
				ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
			} else
				nwait++;
		} else if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
			if (fdp->w_ttl > 0 && ev->present > fdp->w_ttl) {
				fdp->event_type |= ACL_EVENT_RW_TIMEOUT;
				fdp->fdidx_ready = ev->fdcnt_ready;
				ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
			} else
				nwait++;
		} else {
			nwait++;
		}
	}
	return nwait;
}

void event_fire(ACL_EVENT *ev)
{
	ACL_EVENT_FDTABLE *fdp;
	int   i, type;
	acl_int64   r_timeout, w_timeout;
	ACL_EVENT_NOTIFY_RDWR r_callback, w_callback;

	if (ev->fire_begin)
		ev->fire_begin(ev, ev->fire_ctx);

	for (i = 0; i < ev->fdcnt_ready; i++) {
		fdp = ev->fdtabs_ready[i];

		/* ev->fdtabs_ready[i] maybe be set NULL in timer callback */
		if (fdp == NULL || fdp->stream == NULL) 
			continue;

		type = fdp->event_type;

		if ((type & ACL_EVENT_XCPT) != 0) {
			fdp->event_type &= ~ACL_EVENT_XCPT;
			r_callback = fdp->r_callback;
			w_callback = fdp->w_callback;

			if (r_callback)
				r_callback(ACL_EVENT_XCPT, ev,
					fdp->stream, fdp->r_context);

			/* ev->fdtabs_ready[i] maybe be set NULL in r_callback */
			if (w_callback && ev->fdtabs_ready[i])
				w_callback(ACL_EVENT_XCPT, ev,
					fdp->stream, fdp->w_context);
			continue;
		}

		if ((type & ACL_EVENT_RW_TIMEOUT) != 0) {
			fdp->event_type &= ~ACL_EVENT_RW_TIMEOUT;
			r_timeout = fdp->r_timeout;
			w_timeout = fdp->w_timeout;
			r_callback = fdp->r_callback;
			w_callback = fdp->w_callback;

			if (r_timeout > 0 && r_callback) {
				fdp->r_ttl = ev->present + fdp->r_timeout;
				fdp->r_callback(ACL_EVENT_RW_TIMEOUT, ev,
					fdp->stream, fdp->r_context);
			}

			/* ev->fdtabs_ready[i] maybe be set NULL in r_callback */
			if (w_timeout > 0 && w_callback && ev->fdtabs_ready[i]) {
				fdp->w_ttl = ev->present + fdp->w_timeout;
				fdp->w_callback(ACL_EVENT_RW_TIMEOUT, ev,
					fdp->stream, fdp->w_context);
			}
			continue;
		}

		if ((type & (ACL_EVENT_READ | ACL_EVENT_ACCEPT))) {
			fdp->event_type &= ~(ACL_EVENT_READ | ACL_EVENT_ACCEPT);
			if (fdp->r_timeout > 0)
				fdp->r_ttl = ev->present + fdp->r_timeout;
			fdp->r_callback(type, ev, fdp->stream, fdp->r_context);
		}

		/* ev->fdtabs_ready[i] maybe be set NULL in fdp->r_callback() */
		if (ev->fdtabs_ready[i] == NULL)
			continue;

		if ((type & (ACL_EVENT_WRITE | ACL_EVENT_CONNECT))) {
			if (fdp->w_timeout > 0)
				fdp->w_ttl = ev->present + fdp->w_timeout;
			fdp->event_type &= ~(ACL_EVENT_WRITE | ACL_EVENT_CONNECT);
			fdp->w_callback(type, ev, fdp->stream, fdp->w_context);
		}
	}

	if (ev->fire_end)
		ev->fire_end(ev, ev->fire_ctx);
}

int event_thr_prepare(ACL_EVENT *ev)
{
	ACL_SOCKET sockfd;
	ACL_EVENT_FDTABLE *fdp;
	int   i, nwait = 0;

	for (i = 0; i < ev->fdcnt; i++) {
		fdp = ev->fdtabs[i];
		sockfd = ACL_VSTREAM_SOCK(fdp->stream);
		fdp->event_type = 0;
		if (ev->maxfd < sockfd)
			ev->maxfd = sockfd;

		if (fdp->listener) {
			nwait++;
			continue;
		}

		if (fdp->stream->flag & ACL_VSTREAM_FLAG_BAD) {
			fdp->stream->flag &= ~ACL_VSTREAM_FLAG_BAD;
			fdp->event_type |= ACL_EVENT_XCPT;
			fdp->fdidx_ready = ev->fdcnt_ready;
			ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
		} else if ((fdp->flag & EVENT_FDTABLE_FLAG_READ)) {
			if (ACL_VSTREAM_BFRD_CNT(fdp->stream) > 0) {
				fdp->stream->sys_read_ready = 0;
				fdp->event_type = ACL_EVENT_READ;
				fdp->fdidx_ready = ev->fdcnt_ready;
				ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
			} else if (fdp->r_ttl > 0 && ev->present > fdp->r_ttl) {
				fdp->event_type = ACL_EVENT_RW_TIMEOUT;
				fdp->fdidx_ready = ev->fdcnt_ready;
				ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
			} else
				nwait++;
		} else if ((fdp->flag & EVENT_FDTABLE_FLAG_WRITE)) {
			if (fdp->w_ttl > 0 && ev->present > fdp->w_ttl) {
				fdp->event_type = ACL_EVENT_RW_TIMEOUT;
				fdp->fdidx_ready = ev->fdcnt_ready;
				ev->fdtabs_ready[ev->fdcnt_ready++] = fdp;
			} else
				nwait++;
		} else
			nwait++;
	}

	return nwait;
}

void event_thr_fire(ACL_EVENT *ev)
{
	ACL_EVENT_FDTABLE *fdp;
	ACL_EVENT_NOTIFY_RDWR callback;
	ACL_VSTREAM *stream;
	void *context;
	int   type;
	int   i;

	if (ev->fire_begin)
		ev->fire_begin(ev, ev->fire_ctx);

	for (i = 0; i < ev->fdcnt_ready; i++) {
		fdp = ev->fdtabs_ready[i];

		/* ev->fdtabs_ready[i] maybe be set NULL by timer callback */
		if (fdp == NULL || fdp->stream == NULL)
			continue;

		stream = fdp->stream;
		type = fdp->event_type;

		if ((type & (ACL_EVENT_READ | ACL_EVENT_ACCEPT))) {
			fdp->event_type &= ~(ACL_EVENT_READ | ACL_EVENT_ACCEPT);
			callback = fdp->r_callback;
			context = fdp->r_context;
			if (!fdp->listener)
				ev->disable_readwrite_fn(ev, stream);
			callback(ACL_EVENT_READ, ev, stream, context);
		} else if ((type & (ACL_EVENT_WRITE | ACL_EVENT_CONNECT))) {
			fdp->event_type &= ~(ACL_EVENT_WRITE | ACL_EVENT_CONNECT);
			callback = fdp->w_callback;
			context = fdp->w_context;
			ev->disable_readwrite_fn(ev, stream);
			callback(ACL_EVENT_WRITE, ev, stream, context);
		} else if ((type & ACL_EVENT_RW_TIMEOUT)) {
			fdp->event_type &= ~ACL_EVENT_RW_TIMEOUT;
			if (fdp->r_callback) {
				callback = fdp->r_callback;
				context = fdp->r_context;
			} else if (fdp->w_callback) {
				callback = fdp->w_callback;
				context = fdp->w_context;
			} else {
				callback = NULL;
				context = NULL;
			}
			if (!fdp->listener)
				ev->disable_readwrite_fn(ev, stream);
			if (callback)
				callback(ACL_EVENT_RW_TIMEOUT, ev,
					stream, context);
		} else if ((type & ACL_EVENT_XCPT)) {
			fdp->event_type &= ~ACL_EVENT_XCPT;
			if (fdp->r_callback) {
				callback = fdp->r_callback;
				context = fdp->r_context;
			} else if (fdp->w_callback) {
				callback = fdp->w_callback;
				context = fdp->w_context;
			} else {
				callback = NULL;
				context = NULL;
			}
			if (!fdp->listener)
				ev->disable_readwrite_fn(ev, stream);
			if (callback)
				callback(ACL_EVENT_XCPT, ev, stream, context);
		}
	}

	if (ev->fire_end)
		ev->fire_end(ev, ev->fire_ctx);
}
