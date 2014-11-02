/* System library. */
#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#endif

#ifdef ACL_UNIX

#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#ifdef STRCASECMP_IN_STRINGS_H
#include <strings.h>
#endif
#include <time.h>

/* Utility library. */

#include "stdlib/acl_msg.h"
#include "stdlib/unix/acl_chroot_uid.h"
#include "stdlib/unix/acl_core_limit.h"
#include "stdlib/acl_vstring.h"
#include "stdlib/acl_vstream.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_iostuff.h"
#include "stdlib/acl_stringops.h"
#include "stdlib/acl_myflock.h"
#include "stdlib/unix/acl_safe_open.h"
#include "stdlib/unix/acl_watchdog.h"
#include "stdlib/acl_split_at.h"
#include "net/acl_listen.h"
#include "event/acl_events.h"

/* Global library. */

#include "../master_flow.h"
#include "../master_params.h"
#include "../master_proto.h"

/* Application-specific */

#include "master/acl_trigger_params.h"
#include "master/acl_server_api.h"
#include "master_log.h"

int   acl_var_trigger_pid;
char *acl_var_trigger_procname;
char *acl_var_trigger_log_file;

int   acl_var_trigger_buf_size;
int   acl_var_trigger_rw_timeout;
int   acl_var_trigger_in_flow_delay;
int   acl_var_trigger_idle_limit;
int   acl_var_trigger_delay_sec;
int   acl_var_trigger_delay_usec;
int   acl_var_trigger_daemon_timeout;
int   acl_var_trigger_use_limit;
int   acl_var_trigger_enable_core;
int   acl_var_trigger_max_debug;

static ACL_CONFIG_INT_TABLE __conf_int_tab[] = {
	{ ACL_VAR_TRIGGER_BUF_SIZE, ACL_DEF_TRIGGER_BUF_SIZE, &acl_var_trigger_buf_size, 0, 0 },
	{ ACL_VAR_TRIGGER_RW_TIMEOUT, ACL_DEF_TRIGGER_RW_TIMEOUT, &acl_var_trigger_rw_timeout, 0, 0 },
	{ ACL_VAR_TRIGGER_IN_FLOW_DELAY, ACL_DEF_TRIGGER_IN_FLOW_DELAY, &acl_var_trigger_in_flow_delay, 0, 0 },
	{ ACL_VAR_TRIGGER_IDLE_LIMIT, ACL_DEF_TRIGGER_IDLE_LIMIT, &acl_var_trigger_idle_limit, 0, 0 },
	{ ACL_VAR_TRIGGER_DELAY_SEC, ACL_DEF_TRIGGER_DELAY_SEC, &acl_var_trigger_delay_sec, 0, 0 },
	{ ACL_VAR_TRIGGER_DELAY_USEC, ACL_DEF_TRIGGER_DELAY_USEC, &acl_var_trigger_delay_usec, 0, 0 },
	{ ACL_VAR_TRIGGER_DAEMON_TIMEOUT, ACL_DEF_TRIGGER_DAEMON_TIMEOUT, &acl_var_trigger_daemon_timeout, 0, 0 },
	{ ACL_VAR_TRIGGER_USE_LIMIT, ACL_DEF_TRIGGER_USE_LIMIT, &acl_var_trigger_use_limit, 0, 0 },
	{ ACL_VAR_TRIGGER_ENABLE_CORE, ACL_DEF_TRIGGER_ENABLE_CORE, &acl_var_trigger_enable_core, 0, 0 },
	{ ACL_VAR_TRIGGER_MAX_DEBUG, ACL_DEF_TRIGGER_MAX_DEBUG, &acl_var_trigger_max_debug, 0, 0 },

	{ 0, 0, 0, 0, 0 },
};

char *acl_var_trigger_queue_dir;
char *acl_var_trigger_owner;
char *acl_var_trigger_pid_dir;
char *acl_var_trigger_log_debug;

static ACL_CONFIG_STR_TABLE __conf_str_tab[] = {
	{ ACL_VAR_TRIGGER_QUEUE_DIR, ACL_DEF_TRIGGER_QUEUE_DIR, &acl_var_trigger_queue_dir },
	{ ACL_VAR_TRIGGER_OWNER, ACL_DEF_TRIGGER_OWNER, &acl_var_trigger_owner },
	{ ACL_VAR_TRIGGER_PID_DIR, ACL_DEF_TRIGGER_PID_DIR, &acl_var_trigger_pid_dir },
	{ ACL_VAR_TRIGGER_LOG_DEBUG, ACL_DEF_TRIGGER_LOG_DEBUG, &acl_var_trigger_log_debug },

	{ 0, 0, 0 },
};

 /*
  * Global state.
  */
static int use_count;
static ACL_EVENT *__eventp = NULL;
static ACL_VSTREAM **__stream_array;

static ACL_TRIGGER_SERVER_FN __service_main;
static char *__service_name;
static char **__service_argv;
static void *__service_ctx;
static void (*__service_accept) (int, ACL_EVENT *, ACL_VSTREAM *, void *);
static ACL_MASTER_SERVER_EXIT_FN __service_onexit;
static ACL_VSTREAM *__service_lock;
static int trigger_server_in_flow_delay;
static unsigned trigger_server_generation;

ACL_EVENT *acl_trigger_server_event()
{
	return (__eventp);
}

/* trigger_server_exit - normal termination */

static void trigger_server_exit(void)
{
	if (__service_onexit)
		__service_onexit(__service_ctx);
	exit(0);
}

/* trigger_server_abort - terminate after abnormal master exit */

static void trigger_server_abort(int type acl_unused, ACL_EVENT *event acl_unused,
	ACL_VSTREAM *stream acl_unused, void *context acl_unused)
{
	if (acl_msg_verbose)
		acl_msg_info("master disconnect -- exiting");
	trigger_server_exit();
}

/* trigger_server_timeout - idle time exceeded */

static void trigger_server_timeout(int type acl_unused,
	ACL_EVENT *event acl_unused, void *context acl_unused)
{
	if (acl_msg_verbose)
		acl_msg_info("idle timeout -- exiting");
	trigger_server_exit();
}

/* trigger_server_wakeup - wake up application */

static void trigger_server_wakeup(ACL_EVENT *event, int fd)
{
	char    buf[ACL_TRIGGER_BUF_SIZE];
	int     len;

	/*
	 * Commit suicide when the master process disconnected from us.
	 */
	if (acl_master_notify(acl_var_trigger_pid, trigger_server_generation,
		ACL_MASTER_STAT_TAKEN) < 0)
	{
		trigger_server_abort(ACL_EVENT_NULL_TYPE, event,
			NULL, ACL_EVENT_NULL_CONTEXT);
	}
	if (trigger_server_in_flow_delay && acl_master_flow_get(1) < 0)
		acl_doze(acl_var_trigger_in_flow_delay * 1000);
	if ((len = read(fd, buf, sizeof(buf))) >= 0)
		__service_main(buf, len, __service_ctx, __service_argv);

	if (acl_master_notify(acl_var_trigger_pid, trigger_server_generation,
		ACL_MASTER_STAT_AVAIL) < 0)
	{
		trigger_server_abort(ACL_EVENT_NULL_TYPE, event,
			NULL, ACL_EVENT_NULL_CONTEXT);
	}
	if (acl_var_trigger_idle_limit > 0)
		acl_event_request_timer(event, trigger_server_timeout, NULL,
			(acl_int64) acl_var_trigger_idle_limit * 1000000, 0);
	use_count++;
}

/* __service_accept_fifo - accept fifo client request */

static void __service_accept_fifo(int type acl_unused, ACL_EVENT *event,
	ACL_VSTREAM *stream, void *context acl_unused)
{
	const char *myname = "__service_accept_fifo";
	int     listen_fd = ACL_VSTREAM_SOCK(stream);

	if (__service_lock != 0
	    && acl_myflock(ACL_VSTREAM_FILE(__service_lock),
		    	ACL_INTERNAL_LOCK, ACL_FLOCK_OP_NONE) < 0)
	{
		acl_msg_fatal("select unlock: %s", acl_last_serror());
	}

	if (acl_msg_verbose)
		acl_msg_info("%s: trigger arrived", myname);

	/*
	 * Read whatever the other side wrote into the FIFO. The FIFO read end is
	 * non-blocking so we won't get stuck when multiple processes wake up.
	 */
	trigger_server_wakeup(event, listen_fd);
}

/* __service_accept_local - accept socket client request */

static void __service_accept_local(int type acl_unused, ACL_EVENT *event,
	ACL_VSTREAM *stream, void *context acl_unused)
{
	const char *myname = "__service_accept_local";
	int listen_fd = ACL_VSTREAM_SOCK(stream), time_left = 0, fd;

	if (acl_msg_verbose)
		acl_msg_info("%s: trigger arrived", myname);

	/*
	 * Read a message from a socket. Be prepared for accept() to fail because
	 * some other process already got the connection. The socket is
	 * non-blocking so we won't get stuck when multiple processes wake up.
	 * Don't get stuck when the client connects but sends no data. Restart
	 * the idle timer if this was a false alarm.
	 */
	if (acl_var_trigger_idle_limit > 0)
		time_left = (int) ((acl_event_cancel_timer(event,
			trigger_server_timeout, NULL) + 999999) / 1000000);

	fd = acl_unix_accept(listen_fd);
	if (__service_lock != 0
	    && acl_myflock(ACL_VSTREAM_FILE(__service_lock),
	    	ACL_INTERNAL_LOCK, ACL_FLOCK_OP_NONE) < 0)
	{
		acl_msg_fatal("select unlock: %s", acl_last_serror());
	}
	if (fd < 0) {
		if (errno != EAGAIN)
			acl_msg_fatal("accept connection: %s", acl_last_serror());
		if (time_left >= 0)
			acl_event_request_timer(event, trigger_server_timeout,
				NULL, (acl_int64) time_left * 1000000, 0);
		return;
	}
	acl_close_on_exec(fd, ACL_CLOSE_ON_EXEC);
	if (acl_read_wait(fd, 10) == 0)
		trigger_server_wakeup(event, fd);
	else if (time_left >= 0)
		acl_event_request_timer(event, trigger_server_timeout,
			NULL, (acl_int64) time_left * 1000000, 0);
	close(fd);
}

#ifdef ACL_MASTER_XPORT_NAME_PASS

/* __service_accept_pass - accept descriptor */

static void __service_accept_pass(int type acl_unused, ACL_EVENT *event,
	ACL_VSTREAM *stream, void *context acl_unused)
{
	const char *myname = "__service_accept_pass";
	int listen_fd = ACL_VSTREAM_SOCK(stream), time_left = 0, fd;

	if (acl_msg_verbose)
		acl_msg_info("%s: trigger arrived", myname);

	/*
	 * Read a message from a socket. Be prepared for accept() to fail because
	 * some other process already got the connection. The socket is
	 * non-blocking so we won't get stuck when multiple processes wake up.
	 * Don't get stuck when the client connects but sends no data. Restart
	 * the idle timer if this was a false alarm.
	 */
	if (acl_var_trigger_idle_limit > 0)
		time_left = (int) ((acl_event_cancel_timer(event,
			trigger_server_timeout, NULL) + 999999) / 1000000);

	fd = PASS_ACCEPT(listen_fd);
	if (__service_lock != 0
	    && acl_myflock(ACL_VSTREAM_FILE(__service_lock),
		    	ACL_INTERNAL_LOCK,
			ACL_FLOCK_OP_NONE) < 0)
		acl_msg_fatal("select unlock: %s", acl_last_serror());
	if (fd < 0) {
		if (errno != EAGAIN)
			acl_msg_fatal("accept connection: %s", acl_last_serror());
		if (time_left >= 0)
			acl_event_request_timer(event, trigger_server_timeout,
				NULL, (acl_int64) time_left * 1000000, 0);
		return;
	}
	acl_close_on_exec(fd, ACL_CLOSE_ON_EXEC);
	if (acl_read_wait(fd, 10) == 0)
		trigger_server_wakeup(event, fd);
	else if (time_left >= 0)
		acl_event_request_timer(event, trigger_server_timeout,
			NULL, (acl_int64) time_left * 1000000, 0);
	close(fd);
}

#endif

static void trigger_server_init(const char *procname)
{
	const char *myname = "trigger_server_init";
	static int  inited = 0;

	if (inited)
		return;

	inited = 1;

	if (procname == NULL || *procname == 0)
		acl_msg_fatal("%s(%d); procname null", myname, __LINE__);

	/*
	 * Don't die when a process goes away unexpectedly.
	 */
	signal(SIGPIPE, SIG_IGN);

	/*
	 * Don't die for frivolous reasons.
	 */
#ifdef SIGXFSZ
	signal(SIGXFSZ, SIG_IGN);
#endif

	/*
	 * May need this every now and then.
	 */

	acl_var_trigger_pid = getpid();
	acl_var_trigger_procname = acl_mystrdup(acl_safe_basename(procname));

	acl_var_trigger_log_file = getenv("SERVICE_LOG");
	if (acl_var_trigger_log_file == NULL) {
		acl_var_trigger_log_file = acl_mystrdup("acl_master.log");
		acl_msg_fatal("%s(%d)->%s: can't get MASTER_LOG's env value,"
			" use %s log", __FILE__, __LINE__, myname,
			acl_var_trigger_log_file);
	}

	acl_get_app_conf_int_table(__conf_int_tab);
	acl_get_app_conf_str_table(__conf_str_tab);
        
	acl_master_vars_init(acl_var_trigger_buf_size, acl_var_trigger_rw_timeout);
}

static void trigger_server_open_log(void)
{
	/* first, close the master's log */
	master_log_close();

	/* second, open the service's log */
	acl_msg_open(acl_var_trigger_log_file, acl_var_trigger_procname);

	if (acl_var_trigger_log_debug && *acl_var_trigger_log_debug
		&& acl_var_trigger_max_debug >= 100)
	{
		acl_debug_init2(acl_var_trigger_log_debug,
			acl_var_trigger_max_debug);
	}
}

static void usage(int argc, char *argv[])
{
	int   i;
	char *service_name;

	if (argc <= 0)
		acl_msg_fatal("%s(%d): argc(%d) invalid",
			__FILE__, __LINE__, argc);

	service_name = acl_mystrdup(acl_safe_basename(argv[0]));

	for (i = 0; i < argc; i++)
		acl_msg_info("argv[%d]: %s", i, argv[i]);

	acl_msg_info("usage: %s -h[help]"
		" -c [use chroot]"
		" -d [debug]"
		" -l [run alone]"
		" -n service_name"
		" -s socket_count"
		" -i [use stdin]"
		" -t transport"
		" -u [use setgid initgroups setuid]"
		" -v [on acl_msg_verbose]"
		" -z [unlimit process count]"
		" -f conf_file",
		service_name);
}

/* trigger_server_main - the real main program */

void acl_trigger_server_main(int argc, char **argv, ACL_TRIGGER_SERVER_FN service,...)
{
	const char *myname = "trigger_server_main";
	char   *root_dir = 0;
	char   *user_name = 0;
	char   *service_name = acl_mystrdup(acl_safe_basename(argv[0]));
	ACL_VSTREAM *stream = 0;
	int     delay;
	int     c;
	int     socket_count = 1;
	va_list ap;
	ACL_MASTER_SERVER_INIT_FN pre_init = 0;
	ACL_MASTER_SERVER_INIT_FN post_init = 0;
	ACL_MASTER_SERVER_LOOP_FN loop = 0;
	int     key;
	char    buf[ACL_TRIGGER_BUF_SIZE];
	int     len;
	char   *transport = 0;
	char   *lock_path;
	ACL_VSTRING *why;
	int     alone = 0;
	int     zerolimit = 0;
	ACL_WATCHDOG *watchdog;
	char   *generation;

	int     fd, fdtype = 0;
	int     f_flag = 0;
	char   *conf_file_ptr = 0;

	/* ��ǰ����ģ���ʼ������ʹ��־����ش�, ��ʼ��ʹ�� acl_master ����־�ļ� */
	master_log_open(argv[0]);

	if (acl_msg_verbose)
		acl_msg_info("daemon started");

	/*
	 * Pick up policy settings from master process. Shut up error messages to
	 * stderr, because no-one is going to see them.
	 */
	opterr = 0;
	while ((c = getopt(argc, argv, "hcDlm:n:s:it:uvzf:")) > 0) {
		switch (c) {
		case 'h':
			usage(argc, argv);
			exit (0);
		case 'f':
			acl_app_conf_load(optarg);
			f_flag = 1;
			conf_file_ptr = optarg;
			break;
		case 'c':
			root_dir = "setme";
			break;
		case 'l':
			alone = 1;
			break;
		case 'n':
			service_name = optarg;
			break;
		case 's':
			if ((socket_count = atoi(optarg)) <= 0)
				acl_msg_fatal("invalid socket_count: %s", optarg);
			break;
		case 't':
			transport = optarg;
			break;
		case 'u':
			user_name = "setme";
			break;
		case 'v':
			acl_msg_verbose++;
			break;
		case 'z':
			zerolimit = 1;
			break;
		default:
			break;
		}
	}

	if (stream == NULL)
		trigger_server_init(argv[0]);

	if (f_flag == 0)
		acl_msg_fatal("%s(%d), %s: need \"-f pathname\"",
			__FILE__, __LINE__, myname);
	else if (acl_msg_verbose)
		acl_msg_info("%s(%d), %s: configure file = %s",
			__FILE__, __LINE__, myname, conf_file_ptr);

	/*
	 * Application-specific initialization.
	 */
	va_start(ap, service);
	while ((key = va_arg(ap, int)) != 0) {
		switch (key) {
		case ACL_MASTER_SERVER_INT_TABLE:
			acl_get_app_conf_int_table(va_arg(ap, ACL_CONFIG_INT_TABLE *));
			break;
		case ACL_MASTER_SERVER_INT64_TABLE:
			acl_get_app_conf_int64_table(va_arg(ap, ACL_CONFIG_INT64_TABLE *));
			break;
		case ACL_MASTER_SERVER_STR_TABLE:
			acl_get_app_conf_str_table(va_arg(ap, ACL_CONFIG_STR_TABLE *));
			break;
		case ACL_MASTER_SERVER_BOOL_TABLE:
			acl_get_app_conf_bool_table(va_arg(ap, ACL_CONFIG_BOOL_TABLE *));
			break;

		case ACL_MASTER_SERVER_CTX:
			__service_ctx = va_arg(ap, void *);
			break;
		case ACL_MASTER_SERVER_PRE_INIT:
			pre_init = va_arg(ap, ACL_MASTER_SERVER_INIT_FN);
			break;
		case ACL_MASTER_SERVER_POST_INIT:
			post_init = va_arg(ap, ACL_MASTER_SERVER_INIT_FN);
			break;
		case ACL_MASTER_SERVER_LOOP:
			loop = va_arg(ap, ACL_MASTER_SERVER_LOOP_FN);
			break;
		case ACL_MASTER_SERVER_EXIT:
			__service_onexit = va_arg(ap, ACL_MASTER_SERVER_EXIT_FN);
			break;
		case ACL_MASTER_SERVER_IN_FLOW_DELAY:
			trigger_server_in_flow_delay = 1;
			break;
		case ACL_MASTER_SERVER_SOLITARY:
			if (!alone)
				acl_msg_fatal("service %s requires a process"
					" limit of 1", service_name);
			break;
		case ACL_MASTER_SERVER_UNLIMITED:
			if (!zerolimit)
				acl_msg_fatal("service %s requires a process"
					" limit of 0", service_name);
			break;
		default:
			acl_msg_panic("%s: unknown argument type: %d", myname, key);
		}
	}

	va_end(ap);

	if (root_dir)
		root_dir = acl_var_trigger_queue_dir;
	if (user_name)
		user_name = acl_var_trigger_owner;

	/*
	 * If not connected to stdin, stdin must not be a terminal.
	 */
	if (stream == 0 && isatty(STDIN_FILENO))
		acl_msg_fatal("do not run this command by hand");

	/*
	 * Can options be required?
	 * 
	 * XXX Initially this code was implemented with UNIX-domain sockets, but
	 * Solaris <= 2.5 UNIX-domain sockets misbehave hopelessly when the
	 * client disconnects before the server has accepted the connection.
	 * Symptom: the server accept() fails with EPIPE or EPROTO, but the
	 * socket stays readable, so that the program goes into a wasteful loop.
	 * 
	 * The initial fix was to use FIFOs, but those turn out to have their own
	 * problems, witness the workarounds in the fifo_listen() routine.
	 * Therefore we support both FIFOs and UNIX-domain sockets, so that the
	 * user can choose whatever works best.
	 * 
	 * Well, I give up. Solaris UNIX-domain sockets still don't work properly,
	 * so it will have to limp along with a streams-specific alternative.
	 */
	if (stream == 0) {
		if (transport == 0)
			acl_msg_fatal("no transport type specified");
		if (strcasecmp(transport, ACL_MASTER_XPORT_NAME_UNIX) == 0) {
			__service_accept = __service_accept_local;
			fdtype = ACL_VSTREAM_TYPE_LISTEN | ACL_VSTREAM_TYPE_LISTEN_INET;
		} else if (strcasecmp(transport, ACL_MASTER_XPORT_NAME_FIFO) == 0) {
			__service_accept = __service_accept_fifo;
			fdtype = ACL_VSTREAM_TYPE_LISTEN | ACL_VSTREAM_TYPE_LISTEN_UNIX;
#ifdef ACL_MASTER_XPORT_NAME_PASS
		} else if (strcasecmp(transport, ACL_MASTER_XPORT_NAME_PASS) == 0) {
			__service_accept = __service_accept_pass;
			fdtype = ACL_VSTREAM_TYPE_LISTEN;
#endif
		} else
			acl_msg_fatal("unsupported transport type: %s", transport);
	}

	/*
	 * Retrieve process generation from environment.
	 */
	if ((generation = getenv(ACL_MASTER_GEN_NAME)) != 0) {
		if (!acl_alldig(generation))
			acl_msg_fatal("bad generation: %s", generation);
		sscanf(generation, "%o", &trigger_server_generation);
		if (acl_msg_verbose)
			acl_msg_info("process generation: %s (%o)",
				generation, trigger_server_generation);
	}

	/*
	 * Traditionally, BSD select() can't handle multiple processes selecting
	 * on the same socket, and wakes up every process in select(). See TCP/IP
	 * Illustrated volume 2 page 532. We avoid select() collisions with an
	 * external lock file.
	 */
	if (stream == 0 && !alone) {
		lock_path = acl_concatenate(acl_var_trigger_pid_dir, "/",
			transport, ".", service_name, (char *) 0);
		why = acl_vstring_alloc(1);
		if ((__service_lock = acl_safe_open(lock_path,
			O_CREAT | O_RDWR, 0600, (struct stat *) 0,
			(uid_t) -1, (uid_t) -1, why)) == 0)
		{
			acl_msg_fatal("open lock file %s: %s",
				lock_path, acl_vstring_str(why));
		}
		acl_close_on_exec(ACL_VSTREAM_FILE(__service_lock),
			ACL_CLOSE_ON_EXEC);
		acl_myfree(lock_path);
		acl_vstring_free(why);
	}

	/*
	 * Set up call-back info.
	 */
	__service_main = service;
	__service_name = service_name;
	__service_argv = argv + optind;

	__eventp = acl_event_new_select(acl_var_trigger_delay_sec,
		acl_var_trigger_delay_usec);

	/*
	 * Run pre-jail initialization.
	 */
	if (chdir(acl_var_trigger_queue_dir) < 0)
		acl_msg_fatal("chdir(\"%s\"): %s",
			acl_var_trigger_queue_dir, acl_last_serror());
	if (pre_init)
		pre_init(__service_ctx);

#ifdef SNAPSHOT
	tzset();
#endif
	acl_chroot_uid(root_dir, user_name);

	trigger_server_open_log();

	/* �����ӽ������л������������ core �ļ� */
	if (acl_var_trigger_enable_core)
		acl_set_core_limit(0);

	/*
	 * Run post-jail initialization.
	 */
	if (post_init)
		post_init(__service_ctx);

	/*
	 * Are we running as a one-shot server with the client connection on
	 * standard input?
	 */
	if (stream != 0) {
		len = read(ACL_VSTREAM_SOCK(stream), buf, sizeof(buf));
		if (len <= 0)
			acl_msg_fatal("read: %s", acl_last_serror());
		service(buf, len, __service_name, __service_argv);
		trigger_server_exit();
	}

	/*
	 * Running as a semi-resident server. Service connection requests.
	 * Terminate when we have serviced a sufficient number of clients, when
	 * no-one has been talking to us for a configurable amount of time, or
	 * when the master process terminated abnormally.
	 */
	if (acl_var_trigger_idle_limit > 0)
		acl_event_request_timer(__eventp, trigger_server_timeout, NULL,
			(acl_int64) acl_var_trigger_idle_limit * 1000000, 0);
	
	/* socket count is the same value of listen_fd_count in parent process */

	__stream_array = (ACL_VSTREAM **) acl_mycalloc(
		ACL_MASTER_LISTEN_FD + socket_count, sizeof(ACL_VSTREAM *));

	fd = ACL_MASTER_LISTEN_FD;
	for (; fd < ACL_MASTER_LISTEN_FD + socket_count; fd++) {
		stream = acl_vstream_fdopen(fd, O_RDWR, acl_var_trigger_buf_size,
				acl_var_trigger_rw_timeout, fdtype);
		if (stream == NULL)
			acl_msg_fatal("%s(%d)->%s: stream null, fd = %d",
				__FILE__, __LINE__, myname, fd);

		acl_event_enable_read(__eventp, stream, 0,
			__service_accept, stream);
		acl_close_on_exec(ACL_VSTREAM_SOCK(stream), ACL_CLOSE_ON_EXEC);
	}

	acl_event_enable_read(__eventp, ACL_MASTER_STAT_STREAM, 0,
		trigger_server_abort, (void *) 0);
	acl_close_on_exec(ACL_MASTER_STATUS_FD, ACL_CLOSE_ON_EXEC);
	acl_close_on_exec(ACL_MASTER_FLOW_READ, ACL_CLOSE_ON_EXEC);
	acl_close_on_exec(ACL_MASTER_FLOW_WRITE, ACL_CLOSE_ON_EXEC);
	watchdog = acl_watchdog_create(acl_var_trigger_daemon_timeout,
		(ACL_WATCHDOG_FN) 0, (char *) 0);

	/*
	 * The event loop, at last.
	 */
	while (acl_var_trigger_use_limit == 0
		|| use_count < acl_var_trigger_use_limit)
	{
		if (__service_lock != 0) {
			acl_watchdog_stop(watchdog);
			if (acl_myflock(ACL_VSTREAM_FILE(__service_lock),
				ACL_INTERNAL_LOCK, ACL_FLOCK_OP_EXCLUSIVE) < 0)
			{
				acl_msg_fatal("lock error %s", acl_last_serror());
			}
		}
		acl_watchdog_start(watchdog);
		delay = loop ? loop(__service_ctx) : -1;
		acl_event_set_delay_sec(__eventp, delay);
		acl_event_loop(__eventp);
	}

	trigger_server_exit();
}
#endif /* ACL_UNIX */
