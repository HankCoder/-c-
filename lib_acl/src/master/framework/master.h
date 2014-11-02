
#ifndef	__ACL_MASTER_INCLUDE_H__
#define	__ACL_MASTER_INCLUDE_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#ifdef ACL_UNIX

#include "stdlib/acl_vstream.h"
#include "stdlib/acl_array.h"
#include "event/acl_events.h"
#include "net/acl_netdb.h"

typedef struct ACL_MASTER_NV {
	char *name;
	char *value;
} ACL_MASTER_NV;

typedef struct ACL_MASTER_ADDR {
	int   type;
	char *addr;
} ACL_MASTER_ADDR;

 /*
  * Server processes that provide the same service share a common "listen"
  * socket to accept connection requests, and share a common pipe to the
  * master process to send status reports. Server processes die voluntarily
  * when idle for a configurable amount of time, or after servicing a
  * configurable number of requests; the master process spawns new processes
  * on demand up to a configurable concurrency limit and/or periodically.
  */
typedef struct ACL_MASTER_SERV {
	int     flags;			/* status, features, etc. */
	char   *name;			/* service endpoint name */
	ACL_ARRAY  *children_env;	/* the env array of the children */
	int     type;			/* UNIX-domain, INET, etc. */
	int     wakeup_time;		/* wakeup interval */
	/* EVENT  *eventp;		 * event pointer */
	int    *listen_fds;		/* incoming requests */
	ACL_VSTREAM **listen_streams;	/* multi-listening stream */
	int     listen_fd_count;	/* nr of descriptors */
	int     defer_accept;		/* accept timeout if no data from client */
	ACL_ARRAY *addrs;		/* in which ACL_MASTER_ADDR save */
	int     max_qlen;		/* max listening qlen */
	int     max_proc;		/* upper bound on # processes */
	int     prefork_proc;		/* prefork processes */
	char   *path;			/* command pathname */
	char   *notify_addr;		/* warning address when not null */
	char   *notify_recipients;	/* users warned to */
	struct ACL_ARGV *args;		/* argument vector */
	int     avail_proc;		/* idle processes */
	int     total_proc;		/* number of processes */
	int     throttle_delay;		/* failure recovery parameter */
	int     status_fd[2];		/* child status reports */
	ACL_VSTREAM *status_read_stream;	/* status stream */
	struct ACL_BINHASH *children;	/* linkage */
	struct ACL_MASTER_SERV *next;	/* linkage */
} ACL_MASTER_SERV;

 /*
  * Per-service flag bits. We assume trouble when a child process terminates
  * before completing its first request: either the program is defective,
  * some configuration is wrong, or the system is out of resources.
  */
#define ACL_MASTER_FLAG_THROTTLE	(1<<0)	/* we're having trouble */
#define ACL_MASTER_FLAG_MARK		(1<<1)	/* garbage collection support */
#define ACL_MASTER_FLAG_CONDWAKE	(1<<2)	/* wake up if actually used */
#define	ACL_MASTER_FLAG_RELOADING	(1<<3)	/* the service is reloading */

#define ACL_MASTER_THROTTLED(f)		((f)->flags & ACL_MASTER_FLAG_THROTTLE)

#define ACL_MASTER_LIMIT_OK(limit, count) ((limit) == 0 || ((count) < (limit)))

 /*
  * Service types.
  */
#define	ACL_MASTER_SERV_TYPE_NULL	0	/* invalid type */
#define ACL_MASTER_SERV_TYPE_UNIX	1	/* AF_UNIX domain socket */
#define ACL_MASTER_SERV_TYPE_INET	2	/* AF_INET domain socket */
#define ACL_MASTER_SERV_TYPE_FIFO	3	/* fifo (named pipe) */
#define	ACL_MASTER_SERV_TYPE_SOCK	4	/* AF_UNIX/AF_INET socket */
#define	ACL_MASTER_SERV_TYPE_UDP	5	/* AF_INET UDP socket */

 /*
  * Default process management policy values. This is only the bare minimum.
  * Most policy management is delegated to child processes. The process
  * manager runs at high privilege level and has to be kept simple.
  */
#define ACL_MASTER_DEF_MIN_IDLE	1	/* preferred # of idle processes */

 /*
  * Structure of child process.
  */
typedef int ACL_MASTER_PID;		/* pid is key into binhash table */

typedef struct ACL_MASTER_PROC {
    ACL_MASTER_PID pid;			/* child process id */
    unsigned gen;			/* child generation number */
    int     avail;			/* availability */
    ACL_MASTER_SERV *serv;		/* parent linkage */
    int     use_count;			/* number of service requests */
} ACL_MASTER_PROC;

 /*
  * Other manifest constants.
  */
#define ACL_MASTER_BUF_LEN	2048		/* logical config line length */

 /*
  * acl_master_ent.c
  */
extern void acl_set_master_service_path(const char *);
extern void acl_set_master_ent(void);
extern void acl_end_master_ent(void);
extern void acl_print_master_ent(ACL_MASTER_SERV *);
extern ACL_MASTER_SERV *acl_get_master_ent(void);
extern void acl_free_master_ent(ACL_MASTER_SERV *);

 /*
  * acl_master_conf.c
  */
extern void acl_master_config(void);
extern void acl_master_refresh(void);

 /*
  * acl_master_service.c
  */
extern ACL_MASTER_SERV *acl_var_master_head;
extern ACL_EVENT *acl_var_master_global_event;
extern void acl_master_start_service(ACL_MASTER_SERV *);
extern void acl_master_stop_service(ACL_MASTER_SERV *);
extern void acl_master_restart_service(ACL_MASTER_SERV *);

 /*
  * acl_master_events.c
  */
extern int acl_var_master_gotsighup;
extern int acl_var_master_gotsigchld;
extern void acl_master_sigsetup(void);

 /*
  * acl_master_status.c
  */
extern void acl_master_status_init(ACL_MASTER_SERV *);
extern void acl_master_status_cleanup(ACL_MASTER_SERV *);

 /*
  * acl_master_wakeup.c
  */
extern void acl_master_wakeup_init(ACL_MASTER_SERV *);
extern void acl_master_wakeup_cleanup(ACL_MASTER_SERV *);

 /*
  * acl_master_listen.c
  */
extern void acl_master_listen_init(ACL_MASTER_SERV *);
extern void acl_master_listen_cleanup(ACL_MASTER_SERV *);

/*
 * acl_msg_prefork.c
 */
extern void acl_master_prefork(ACL_MASTER_SERV *);

 /*
  * acl_master_avail.c
  */
extern void acl_master_avail_listen(ACL_MASTER_SERV *);
extern void acl_master_avail_cleanup(ACL_MASTER_SERV *);
extern void acl_master_avail_more(ACL_MASTER_SERV *, ACL_MASTER_PROC *);
extern void acl_master_avail_less(ACL_MASTER_SERV *, ACL_MASTER_PROC *);

 /*
  * acl_master_spawn.c
  */
extern struct ACL_BINHASH *acl_var_master_child_table;
extern void acl_master_spawn(ACL_MASTER_SERV *);
extern void acl_master_reap_child(void);
extern void acl_master_delete_children(ACL_MASTER_SERV *);

 /*
  * acl_master_flow.c
  */
extern void acl_master_flow_init(void);
extern int acl_var_master_flow_pipe[2];

 /*
  * master_warning.c
  */
extern void master_warning(const char *notify_addr, const char *recipients,
	const char *path, int pid, const char *info);

#endif /* ACL_UNIX */

#ifdef  __cplusplus
}
#endif

#endif

