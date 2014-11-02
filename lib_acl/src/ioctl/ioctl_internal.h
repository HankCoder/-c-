#ifndef	__IOCTL_INTERNAL_INCLUDE_H__
#define	__IOCTL_INTERNAL_INCLUDE_H__

#include "thread/acl_pthread_pool.h"
#include "stdlib/acl_vstream.h"

/* ����������þ�� */
struct ACL_IOCTL {
	int   event_mode;       /* ACL_EVENT_SELECT/ACL_EVENT_KERNEL */
	int   max_threads;	/* ��������߳��� */
	size_t stacksize;	/* �̵߳Ķ�ջ�ߴ� */
	int   idle_timeout;
	int   delay_sec;
	int   delay_usec;
	acl_pthread_pool_t *tp; /* �̳߳ؾ�� */

	ACL_EVENT *event;	/* �¼�ѭ����� */
	int   enable_dog;

	ACL_IOCTL_THREAD_INIT_FN thread_init_fn;
	ACL_IOCTL_THREAD_EXIT_FN thread_exit_fn;
	void *thread_init_arg;
	void *thread_exit_arg;
};

typedef struct ACL_IOCTL_CTX {
	ACL_IOCTL *ioc;
	ACL_VSTREAM *stream;
	int   event_type;

	ACL_IOCTL_NOTIFY_FN notify_fn;
	ACL_IOCTL_WORKER_FN worker_fn;
	void  *context;
} ACL_IOCTL_CTX;

/* in ioctl_thr.c */
void read_notify_callback_r(int event_type, ACL_EVENT *event,
	ACL_VSTREAM *stream, void *context);
void write_notify_callback_r(int event_type, ACL_EVENT *event,
	ACL_VSTREAM *stream, void *context);
void listen_notify_callback_r(int event_type, ACL_EVENT *event,
	ACL_VSTREAM *stream, void *context);

/* in ioctl_proc.c */
void read_notify_callback(int event_type, ACL_EVENT *event,
	ACL_VSTREAM *stream, void *context);
void write_notify_callback(int event_type, ACL_EVENT *event,
	ACL_VSTREAM *stream, void *context);
void listen_notify_callback(int event_type, ACL_EVENT *event,
	ACL_VSTREAM *stream, void *context);

#endif
