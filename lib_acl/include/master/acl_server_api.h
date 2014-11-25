#ifndef	ACL_MASTER_SERVER_API_INCLUDE_H
#define	ACL_MASTER_SERVER_API_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#ifdef ACL_UNIX

 /*
  * Utility library.
  */
#include "stdlib/acl_vstream.h"
#include "ioctl/acl_ioctl.h"
#include "aio/acl_aio.h"
#include "event/acl_events.h"

#define	ACL_MASTER_SERVER_END			0
#define ACL_MASTER_SERVER_INT_TABLE		1
#define ACL_MASTER_SERVER_STR_TABLE		2
#define ACL_MASTER_SERVER_BOOL_TABLE		3
#define ACL_MASTER_SERVER_TIME_TABLE		4
#define ACL_MASTER_SERVER_RAW_TABLE		5
#define ACL_MASTER_SERVER_INT64_TABLE		6
#define ACL_MASTER_SERVER_IN_FLOW_DELAY		8

#define	ACL_MASTER_SERVER_PRE_INIT		10
#define ACL_MASTER_SERVER_POST_INIT		11
#define ACL_MASTER_SERVER_LOOP			12
#define ACL_MASTER_SERVER_EXIT			13
#define ACL_MASTER_SERVER_SOLITARY		14
#define ACL_MASTER_SERVER_UNLIMITED		15
#define	ACL_MASTER_SERVER_ON_CLOSE		16
#define	ACL_MASTER_SERVER_ON_ACCEPT		17
#define	ACL_MASTER_SERVER_ON_TIMEOUT		18
#define	ACL_MASTER_SERVER_ON_HANDSHAKE		19

#define	ACL_MASTER_SERVER_THREAD_INIT		20
#define	ACL_MASTER_SERVER_THREAD_INIT_CTX	21
#define	ACL_MASTER_SERVER_THREAD_EXIT		22
#define	ACL_MASTER_SERVER_THREAD_EXIT_CTX	23
#define	ACL_MASTER_SERVER_CTX			24
#define	ACL_MASTER_SERVER_DENY_INFO		25

#define	ACL_MASTER_SERVER_EXIT_TIMER		26

#define	ACL_APP_CTL_END			ACL_MASTER_SERVER_END
#define	ACL_APP_CTL_CFG_INT		ACL_MASTER_SERVER_INT_TABLE
#define	ACL_APP_CTL_CFG_STR		ACL_MASTER_SERVER_STR_TABLE
#define	ACL_APP_CTL_CFG_BOOL		ACL_MASTER_SERVER_BOOL_TABLE
#define	ACL_APP_CTL_CFG_INT64		ACL_MASTER_SERVER_INT64_TABLE
#define	ACL_APP_CTL_INIT_FN		ACL_MASTER_SERVER_POST_INIT
#define	ACL_APP_CTL_PRE_JAIL		ACL_MASTER_SERVER_PRE_INIT
#define	ACL_APP_CTL_EXIT_FN		ACL_MASTER_SERVER_EXIT
#define	ACL_APP_CTL_THREAD_INIT		ACL_MASTER_SERVER_THREAD_INIT
#define	ACL_APP_CTL_THREAD_INIT_CTX	ACL_MASTER_SERVER_THREAD_INIT_CTX
#define	ACL_APP_CTL_THREAD_EXIT		ACL_MASTER_SERVER_THREAD_EXIT
#define	ACL_APP_CTL_THREAD_EXIT_CTX	ACL_MASTER_SERVER_THREAD_EXIT_CTX
#define	ACL_APP_CTL_DENY_INFO		ACL_MASTER_DENY_INFO
#define	ACL_APP_CTL_ON_ACCEPT		ACL_MASTER_SERVER_ON_ACCEPT
#define	ACL_APP_CTL_ON_CLOSE		ACL_MASTER_SERVER_ON_CLOSE
#define	ACL_APP_CTL_ON_TIMEOUT		ACL_MASTER_SERVER_ON_TIMEOUT

typedef void (*ACL_MASTER_SERVER_INIT_FN) (void *);
typedef int  (*ACL_MASTER_SERVER_LOOP_FN) (void *);
typedef void (*ACL_MASTER_SERVER_EXIT_FN) (void *);
typedef int  (*ACL_MASTER_SERVER_ACCEPT_FN) (ACL_VSTREAM *);
typedef int  (*ACL_MASTER_SERVER_HANDSHAKE_FN) (ACL_VSTREAM *);
typedef void (*ACL_MASTER_SERVER_DISCONN_FN) (ACL_VSTREAM *, void *);
typedef int  (*ACL_MASTER_SERVER_TIMEOUT_FN) (ACL_VSTREAM *, void *);
typedef int  (*ACL_MASTER_SERVER_EXIT_TIMER_FN)(size_t, size_t);

typedef int  (*ACL_MASTER_SERVER_THREAD_INIT_FN)(void *);
typedef void (*ACL_MASTER_SERVER_THREAD_EXIT_FN)(void *);

 /*
  * acl_single_server.c
  */
typedef void (*ACL_SINGLE_SERVER_FN) (ACL_VSTREAM *, char *, char **);

void acl_single_server_main(int, char **, ACL_SINGLE_SERVER_FN, ...);
ACL_EVENT *acl_single_server_event(void);
ACL_VSTREAM **acl_single_server_sstreams(void);

 /*
  * acl_multi_server.c
  */
typedef void (*ACL_MULTI_SERVER_FN) (ACL_VSTREAM *, char *, char **);

void acl_multi_server_main(int, char **, ACL_MULTI_SERVER_FN,...);
void acl_multi_server_disconnect(ACL_VSTREAM *);
int acl_multi_server_drain(void);
ACL_EVENT *acl_multi_server_event(void);
void acl_multi_server_enable_read(ACL_VSTREAM *stream);

 /*
  * acl_ioctl_server.c
  * @deprecated ��ʹ�� acl_threads_server.c �еĺ���
  */
typedef void (*ACL_IOCTL_SERVER_FN) (ACL_IOCTL*, ACL_VSTREAM*, char*, char **);
ACL_DEPRECATED void acl_ioctl_server_request_timer(ACL_EVENT_NOTIFY_TIME timer_fn,
	void *arg, int delay);
ACL_DEPRECATED void acl_ioctl_server_cancel_timer(ACL_EVENT_NOTIFY_TIME timer_fn,
	void *arg);
ACL_DEPRECATED void acl_ioctl_server_main(int, char **, ACL_IOCTL_SERVER_FN,...);
ACL_DEPRECATED ACL_IOCTL *acl_ioctl_server_handle(void);
ACL_DEPRECATED ACL_EVENT *acl_ioctl_server_event(void);
ACL_DEPRECATED ACL_VSTREAM **acl_ioctl_server_streams(void);
ACL_DEPRECATED void acl_ioctl_server_enable_read(ACL_IOCTL*, ACL_VSTREAM*,
	int timeout, ACL_IOCTL_NOTIFY_FN notify_fn, void *context);

 /*
  * acl_threads_server.c
  */

typedef int (*ACL_THREADS_SERVER_FN) (ACL_VSTREAM*, void*);

void acl_threads_server_main(int argc, char **argv, ACL_THREADS_SERVER_FN,
	void *service_ctx, int name, ...);
#define	acl_ioctl_app_main	acl_threads_server_main

ACL_EVENT *acl_threads_server_event(void);
acl_pthread_pool_t *acl_threads_server_threads(void);
ACL_VSTREAM **acl_threads_server_streams(void);

 /*
  * acl_aio_server.c
  */

typedef void (*ACL_AIO_SERVER_FN) (ACL_ASTREAM *, void *);
typedef void (*ACL_AIO_SERVER2_FN) (ACL_SOCKET, void *);

void acl_aio_server_main(int, char **, ACL_AIO_SERVER_FN, ...);
void acl_aio_server2_main(int, char **, ACL_AIO_SERVER2_FN, ...);

typedef int (*ACL_AIO_RUN_FN)(ACL_ASTREAM *stream, void *run_ctx);
typedef int (*ACL_AIO_RUN2_FN)(ACL_SOCKET fd, void *run_ctx);

ACL_DEPRECATED void acl_aio_app_main(int argc, char *argv[],
	ACL_AIO_RUN_FN run_fn, void *run_ctx, ...);
ACL_DEPRECATED void acl_aio_app2_main(int argc, char *argv[],
	ACL_AIO_RUN2_FN run2_fn, void *run_ctx, ...);

void acl_aio_server_request_timer(ACL_EVENT_NOTIFY_TIME timer_fn,
	void *arg, int delay);
void acl_aio_server_cancel_timer(ACL_EVENT_NOTIFY_TIME timer_fn, void *arg);
void acl_aio_server_request_rw_timer(ACL_ASTREAM *);
void acl_aio_server_cancel_rw_timer(ACL_ASTREAM *);
ACL_AIO *acl_aio_server_handle(void);
ACL_EVENT *acl_aio_server_event(void);
int acl_aio_server_read(ACL_ASTREAM *astream, int timeout,
	ACL_AIO_READ_FN notify_fn, void *context);
int acl_aio_server_readn(ACL_ASTREAM *astream, int count, int timeout,
	ACL_AIO_READ_FN notify_fn, void *context);
int acl_aio_server_gets(ACL_ASTREAM *astream, int timeout,
	ACL_AIO_READ_FN notify_fn, void *context);
int acl_aio_server_gets_nonl(ACL_ASTREAM *astream, int timeout,
	ACL_AIO_READ_FN notify_fn, void *context);
int acl_aio_server_writen(ACL_ASTREAM *astream, ACL_AIO_WRITE_FN notify_fn,
	void *context, const char *data, int dlen);
int acl_aio_server_vfprintf(ACL_ASTREAM *astream, ACL_AIO_WRITE_FN notify_fn,
	void *context, const char *fmt, va_list ap);
int acl_aio_server_fprintf(ACL_ASTREAM *astream, ACL_AIO_WRITE_FN notify_fn,
	void *context, const char *fmt, ...);
int acl_aio_server_connect(const char *saddr, int timeout,
	ACL_AIO_CONNECT_FN connect_fn, void *context);
ACL_ASTREAM **acl_aio_server_streams(void);
void acl_aio_server_on_close(ACL_ASTREAM *stream);

 /*
  * acl_udp_server.c
  */

typedef void (*ACL_UDP_SERVER_FN) (ACL_VSTREAM *, char *, char **);

void acl_udp_server_request_timer(ACL_EVENT_NOTIFY_TIME timer_fn, void *arg,
	acl_int64 delay, int keep);
void acl_udp_server_cancel_timer(ACL_EVENT_NOTIFY_TIME timer_fn, void *arg);
void acl_udp_server_main(int, char **, ACL_UDP_SERVER_FN, ...);
ACL_EVENT *acl_udp_server_event(void);
ACL_VSTREAM **acl_udp_server_streams(void);

 /*
  * acl_trigger_server.c
  */
typedef void (*ACL_TRIGGER_SERVER_FN) (char *, int, char *, char **);
void acl_trigger_server_main(int, char **, ACL_TRIGGER_SERVER_FN, ...);
ACL_EVENT *acl_trigger_server_event(void);

#define ACL_TRIGGER_BUF_SIZE	1024

#endif /* ACL_UNIX */

#ifdef	__cplusplus
}
#endif

#endif

