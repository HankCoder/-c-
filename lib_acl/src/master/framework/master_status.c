/* System libraries. */
#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#endif

#ifdef ACL_UNIX

#include <unistd.h>
#include <errno.h>
#include <string.h>

/* Utility library. */

#include "stdlib/acl_msg.h"
#include "stdlib/acl_vstream.h"
#include "stdlib/acl_binhash.h"
#include "stdlib/acl_iostuff.h"
#include "event/acl_events.h"

/* Application-specific. */

#include "../master_proto.h"
#include "../master_params.h"
#include "master.h"

/* master_status_event - status read event handler */

static void master_status_event(int type, ACL_EVENT *event acl_unused,
	ACL_VSTREAM *stream acl_unused, void *context)
{
	const char *myname = "master_status_event";
	ACL_MASTER_SERV *serv = (ACL_MASTER_SERV *) context;
	ACL_MASTER_STATUS stat_buf;
	ACL_MASTER_PROC *proc;
	ACL_MASTER_PID pid;
	int     n;

	if (type == 0)  /* XXX Can this happen?  */
		return;

	/*
	 * We always keep the child end of the status pipe open, so an EOF read
	 * condition means that we're seriously confused. We use non-blocking
	 * reads so that we don't get stuck when someone sends a partial message.
	 * Messages are short, so a partial read means someone wrote less than a
	 * whole status message. Hopefully the next read will be in sync again...
	 * We use a global child process status table because when a child dies
	 * only its pid is known - we do not know what service it came from.
	 */

#if 1
	n = read(ACL_VSTREAM_FILE(serv->status_read_stream),
		(char *) &stat_buf, sizeof(stat_buf));
#else
	n = acl_vstream_readn(serv->status_read_stream,
		(char *) &stat_buf, sizeof(stat_buf));
#endif

	switch (n) {
	case -1:
		acl_msg_warn("%s(%d)->%s: fd = %d, read: %s",
			__FILE__, __LINE__, myname,
			serv->status_fd[0], strerror(errno));
		return;

	case 0:
		acl_msg_panic("%s(%d)->%s: fd = %d, read EOF status",
			__FILE__, __LINE__, myname, serv->status_fd[0]);
		/* NOTREACHED */

	default:
		acl_msg_warn("%s(%d)->%s: service %s: child (pid %d) "
			"sent partial, fd = %d, status update (%d bytes)", 
			__FILE__, __LINE__, myname, serv->name, stat_buf.pid,
			serv->status_fd[0], n);
		return;

	case sizeof(stat_buf):
		pid = stat_buf.pid;
		if (acl_msg_verbose)
			acl_msg_info("%s: pid = %d, gen = %u, avail = %d, "
				"fd = %d", myname, stat_buf.pid, stat_buf.gen,
				stat_buf.avail, serv->status_fd[0]);
	} /* end switch */

	/*
	 * Sanity checks. Do not freak out when the child sends garbage because
	 * it is confused or for other reasons. However, be sure to freak out
	 * when our own data structures are inconsistent. A process not found
	 * condition can happen when we reap a process before receiving its
	 * status update, so this is not an error.
	 */

	if (acl_var_master_child_table == 0)
		acl_msg_fatal("%s(%d): acl_var_master_child_table null",
			myname, __LINE__);

	if ((proc = (ACL_MASTER_PROC *) acl_binhash_find(
		acl_var_master_child_table, (char *) &pid, sizeof(pid))) == 0)
	{
		acl_msg_warn("%s(%d)->%s: process id not found: pid = %d,"
			 " status = %d, gen = %u", __FILE__, __LINE__,
			 myname, stat_buf.pid, stat_buf.avail, stat_buf.gen);
		return;
	}
	if (proc->gen != stat_buf.gen) {
		acl_msg_warn("%s(%d)->%s: ignoring status update from child "
			"pid %d generation %u", __FILE__, __LINE__,
			myname, pid, stat_buf.gen);
		return;
	}
	if (proc->serv != serv)
		acl_msg_panic("%s(%d)->%s: pointer corruption: %p != %p",
			__FILE__, __LINE__, myname, (void *) proc->serv,
			(void *) serv);

	/*
	 * Update our idea of the child process status. Allow redundant status
	 * updates, because different types of events may be processed out of
	 * order. Otherwise, warn about weird status updates but do not take
	 * action. It's all gossip after all.
	 */
	if (proc->avail == stat_buf.avail)
		return;

	switch (stat_buf.avail) {
	case ACL_MASTER_STAT_AVAIL:
		proc->use_count++;
		acl_master_avail_more(serv, proc);
		break;
	case ACL_MASTER_STAT_TAKEN:
		acl_master_avail_less(serv, proc);
		break;
	default:
		acl_msg_warn("%s(%d)->%s: ignoring unknown status: %d "
			"allegedly from pid: %d", __FILE__, __LINE__,
			myname, stat_buf.pid, stat_buf.avail);
		break;
	}
}

/* acl_master_status_init - start status event processing for this service */

void    acl_master_status_init(ACL_MASTER_SERV *serv)
{
	const char *myname = "acl_master_status_init";

	/*
	 * Sanity checks.
	 */
	if (serv->status_fd[0] >= 0 || serv->status_fd[1] >= 0)
		acl_msg_panic("%s: status events already enabled", myname);
	if (acl_msg_verbose)
		acl_msg_info("%s: %s", myname, serv->name);

	/*
	 * Make the read end of this service's status pipe non-blocking so that
	 * we can detect partial writes on the child side. We use a duplex pipe
	 * so that the child side becomes readable when the master goes away.
	 */
	if (acl_duplex_pipe(serv->status_fd) < 0)
		acl_msg_fatal("pipe: %s", strerror(errno));
	acl_non_blocking(serv->status_fd[0], ACL_BLOCKING);
	acl_close_on_exec(serv->status_fd[0], ACL_CLOSE_ON_EXEC);
	acl_close_on_exec(serv->status_fd[1], ACL_CLOSE_ON_EXEC);
	serv->status_read_stream = acl_vstream_fdopen(serv->status_fd[0],
		O_RDWR, acl_var_master_buf_size,
		acl_var_master_rw_timeout, ACL_VSTREAM_TYPE_SOCK);

	if (acl_msg_verbose)
		acl_msg_info("%s(%d)->%s: call acl_event_enable_read, "
			"status_fd = %d", __FILE__, __LINE__,
			myname, serv->status_fd[0]);

	acl_event_enable_read(acl_var_master_global_event,
		serv->status_read_stream, 0, master_status_event,
		(void *) serv);
}

/* acl_master_status_cleanup - stop status event processing for this service */

void    acl_master_status_cleanup(ACL_MASTER_SERV *serv)
{
	const char *myname = "acl_master_status_cleanup";

	/*
	 * Sanity checks.
	 */
	if (serv->status_fd[0] < 0 || serv->status_fd[1] < 0)
		acl_msg_panic("%s: status events not enabled", myname);
	if (acl_msg_verbose)
		acl_msg_info("%s: %s", myname, serv->name);

	/*
	 * Dispose of this service's status pipe after disabling read events.
	 */

	acl_event_disable_readwrite(acl_var_master_global_event,
		serv->status_read_stream);

	if (close(serv->status_fd[0]) != 0)
		acl_msg_warn("%s: close status descriptor (read side): %s",
			myname, strerror(errno));
	if (close(serv->status_fd[1]) != 0)
		acl_msg_warn("%s: close status descriptor (write side): %s",
			myname, strerror(errno));
	serv->status_fd[0] = serv->status_fd[1] = -1;
	if (serv->status_read_stream)
		acl_vstream_free(serv->status_read_stream);
	serv->status_read_stream = NULL;
}

#endif /* ACL_UNIX */
