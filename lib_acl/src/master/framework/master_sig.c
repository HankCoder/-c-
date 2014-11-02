/* System libraries. */
#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#endif

#ifdef ACL_UNIX

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/* Utility library. */

#include "stdlib/acl_msg.h"

/* Application-specific. */

#include "master.h"
#include "../master_params.h"

#ifdef USE_SIG_RETURN
#include <sys/syscall.h>
#endif

#ifndef USE_SIG_RETURN
#define USE_SIG_PIPE
#endif

/* Local stuff. */

#ifdef USE_SIG_PIPE
#include <errno.h>
#include <fcntl.h>

#include "stdlib/acl_iostuff.h"
#include "stdlib/acl_vstream.h"
#include "event/acl_events.h"

int   acl_master_sig_pipe[2];

#define ACL_SIG_PIPE_WRITE_FD acl_master_sig_pipe[1]
#define ACL_SIG_PIPE_READ_FD acl_master_sig_pipe[0]
#endif

ACL_VSTREAM *ACL_SIG_PIPE_WRITE_STREAM = NULL;
ACL_VSTREAM *ACL_SIG_PIPE_READ_STREAM = NULL;

int   acl_var_master_gotsigchld = 0;
int   acl_var_master_gotsighup = 0;

/* master_sighup - register arrival of hangup signal */

static void master_sighup(int sig)
{
	/*
	 * WARNING WARNING WARNING.
	 * 
	 * This code runs at unpredictable moments, as a signal handler.
	 * Don't put any code here other than for setting a global flag.
	 */
	acl_var_master_gotsighup = sig;
}

/* master_sigchld - register arrival of child death signal */

#ifdef USE_SIG_RETURN

static void master_sigchld(int sig, int code, struct sigcontext * scp)
{
	code = code;
	if (acl_msg_verbose)
		acl_msg_info("%s(%d), USE_SIG_RETURN", __FILE__, __LINE__);

	/*
	 * WARNING WARNING WARNING.
	 * 
	 * This code runs at unpredictable moments, as a signal handler.
	 * Don't put any code here other than for setting a global flag,
	 * or code that is intended to be run within a signal handler.
	 */
	acl_var_master_gotsigchld = sig;
	if (scp != NULL && scp->sc_syscall == SYS_select)
		scp->sc_syscall_action = SIG_RETURN;
#ifndef SA_RESTART
	else if (scp != NULL)
		scp->sc_syscall_action = SIG_RESTART;
#endif
}

#else

#ifdef USE_SIG_PIPE

/* master_sigchld - force wakeup from select() */

static void master_sigchld(int unused_sig)
{
	int     saved_errno = errno;

	unused_sig = unused_sig;

	if (acl_msg_verbose)
		acl_msg_info("%s(%d), USE_SIG_PIPE", __FILE__, __LINE__);
	/*
	 * WARNING WARNING WARNING.
	 * 
	 * This code runs at unpredictable moments, as a signal handler.
	 * Don't put any code here other than for setting a global flag,
	 * or code that is intended to be run within a signal handler.
	 * Restore errno in case we are interrupting the epilog of a failed
	 * system call.
	 */
	if (write(ACL_SIG_PIPE_WRITE_FD, "", 1) != 1)
		acl_msg_warn("write to SIG_PIPE_WRITE_FD failed: %s",
			strerror(errno));
	errno = saved_errno;
}

/* master_sig_event - called upon return from select() */

static void master_sig_event(int type acl_unused, ACL_EVENT *event acl_unused,
	ACL_VSTREAM *stream acl_unused, void *context acl_unused)
{
	char    c[1];

	while (read(ACL_SIG_PIPE_READ_FD, c, 1) > 0)
		/* void */ ;
	acl_var_master_gotsigchld = 1;
}

#else

static void master_sigchld(int sig)
{
	if (acl_msg_verbose)
		acl_msg_info("%s(%d), use nothing", __FILE__, __LINE__);

	/*
	 * WARNING WARNING WARNING.
	 * 
	 * This code runs at unpredictable moments, as a signal handler.
	 * Don't put any code here other than for setting a global flag.
	 */
	acl_var_master_gotsigchld = sig;
}

#endif
#endif

/* master_sigdeath - die, women and children first */

static void master_sigdeath(int sig)
{
	const char *myname = "master_sigdeath";
	struct sigaction action;
	pid_t   pid = getpid();

	/*
	 * XXX We're running from a signal handler, and really should not call
	 * any msg() routines at all, but it would be even worse to silently
	 * terminate without informing the sysadmin.
	 */
	acl_msg_info("%s: terminating on signal %d", myname, sig);

	/* Terminate all processes in our process group, except ourselves. */
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	action.sa_handler = SIG_IGN;
	if (sigaction(SIGTERM, &action, (struct sigaction *) 0) < 0)
		acl_msg_fatal("%s: sigaction: %s", myname, strerror(errno));
	if (kill(-pid, SIGTERM) < 0) {
		acl_msg_error("%s: kill process group: %s",
			myname, strerror(errno));
		exit (1);
	}

	/*
	 * Deliver the signal to ourselves and clean up. XXX We're running as a
	 * signal handler and really should not be doing complicated things...
	 */
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	action.sa_handler = SIG_DFL;
	if (sigaction(sig, &action, (struct sigaction *) 0) < 0)
		acl_msg_fatal("%s: sigaction: %s", myname, strerror(errno));
	if (kill(pid, sig) < 0)
		acl_msg_fatal("%s: kill myself: %s", myname, strerror(errno));
}

/* acl_master_sigsetup - set up signal handlers */

void    acl_master_sigsetup(void)
{
	const char *myname = "acl_master_sigsetup";
	struct sigaction action;
	static int sigs[] = {
		SIGINT, SIGQUIT, SIGILL, SIGBUS,
		/* SIGSEGV, only for test */ SIGTERM,
	};
	unsigned i;

	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	/*
	 * Prepare to kill our children
	 * when we receive any of the above signals.
	 */
	action.sa_handler = master_sigdeath;
	for (i = 0; i < sizeof(sigs) / sizeof(sigs[0]); i++) {
		if (sigaction(sigs[i], &action, (struct sigaction *) 0) < 0)
			acl_msg_fatal("%s: sigaction(%d): %s",
				myname, sigs[i], strerror(errno));
	}


#ifdef USE_SIG_PIPE
	if (pipe(acl_master_sig_pipe))
		acl_msg_fatal("pipe: %s", strerror(errno));
	acl_non_blocking(ACL_SIG_PIPE_WRITE_FD, ACL_NON_BLOCKING);
	acl_non_blocking(ACL_SIG_PIPE_READ_FD, ACL_NON_BLOCKING);
	acl_close_on_exec(ACL_SIG_PIPE_WRITE_FD, ACL_CLOSE_ON_EXEC);
	acl_close_on_exec(ACL_SIG_PIPE_READ_FD, ACL_CLOSE_ON_EXEC);

	ACL_SIG_PIPE_READ_STREAM = acl_vstream_fdopen(ACL_SIG_PIPE_READ_FD,
			O_RDONLY, acl_var_master_buf_size,
			acl_var_master_rw_timeout, ACL_VSTREAM_TYPE_SOCK);
	if (ACL_SIG_PIPE_READ_STREAM == NULL)
		acl_msg_fatal("%s(%d)->%s: acl_vstream_fdopen error=%s",
			__FILE__, __LINE__, myname, strerror(errno));

	if (acl_msg_verbose)
		acl_msg_info("%s(%d)->%s: call acl_event_enable_read, "
			"SIG_PIPE_READ_FD = %d", __FILE__, __LINE__, myname,
			ACL_SIG_PIPE_READ_FD);
	acl_event_enable_read(acl_var_master_global_event,
		ACL_SIG_PIPE_READ_STREAM, 0, master_sig_event, (void *) 0);
#endif

	/* Intercept SIGHUP (re-read config file) and SIGCHLD (child exit). */
#ifdef SA_RESTART
	action.sa_flags |= SA_RESTART;
#endif
	action.sa_handler = master_sighup;
	if (sigaction(SIGHUP, &action, (struct sigaction *) 0) < 0)
		acl_msg_fatal("%s: sigaction(%d): %s",
			myname, SIGHUP, strerror(errno));

	action.sa_flags |= SA_NOCLDSTOP;
	action.sa_handler = master_sigchld;
	if (sigaction(SIGCHLD, &action, (struct sigaction *) 0) < 0)
		acl_msg_fatal("%s: sigaction(%d): %s",
			myname, SIGCHLD, strerror(errno));
}
#endif /* ACL_UNIX */
