#ifndef ACL_EVENTS_H_INCLUDED
#define ACL_EVENTS_H_INCLUDED

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include <time.h>
#include "stdlib/acl_vstream.h"
#include "acl_timer.h"

/*+++++++++++++++++++++++++++ ȫ�ֺ궨�� +++++++++++++++++++++++++++++++++++*/
 /* Event codes. */
#define ACL_EVENT_READ          (1 << 0)      /**< read event */
#define	ACL_EVENT_ACCEPT        (1 << 1)      /**< accept one connection */
#define ACL_EVENT_WRITE         (1 << 2)      /**< write event */
#define	ACL_EVENT_CONNECT       (1 << 3)      /**< client has connected the server*/
#define ACL_EVENT_XCPT          (1 << 4)      /**< exception */
#define ACL_EVENT_TIME          (1 << 5)      /**< timer event */
#define	ACL_EVENT_RW_TIMEOUT    (1 << 6)      /**< read/write timeout event */
#define	ACL_EVENT_TIMEOUT       ACL_EVENT_RW_TIMEOUT

#define	ACL_EVENT_FD_IDLE	0
#define	ACL_EVENT_FD_BUSY	1

#define ACL_EVENT_ERROR		ACL_EVENT_XCPT

#define	ACL_EVENT_SELECT	0
#define	ACL_EVENT_POLL		1
#define	ACL_EVENT_KERNEL	2
#define ACL_EVENT_WMSG		3

 /*
  * Dummies.
  */
#define ACL_EVENT_NULL_TYPE	0
#define ACL_EVENT_NULL_CONTEXT	((char *) 0)


/* in acl_events.c */
/*
 * Timer events. Timer requests are kept sorted, in a circular list. We use
 * the RING abstraction, so we get to use a couple ugly macros.
 */
typedef struct ACL_EVENT_TIMER ACL_EVENT_TIMER;

typedef	struct	ACL_EVENT		ACL_EVENT;
typedef	struct	ACL_EVENT_FDTABLE	ACL_EVENT_FDTABLE;

/*
 * External interface.
 */
#if 0
typedef void (*ACL_EVENT_NOTIFY_FN) (int event_type, void *context);
typedef	ACL_EVENT_NOTIFY_FN	ACL_EVENT_NOTIFY_RDWR;
typedef	ACL_EVENT_NOTIFY_FN	ACL_EVENT_NOTIFY_TIME;
#else
typedef void (*ACL_EVENT_NOTIFY_RDWR)(int event_type, ACL_EVENT *event,
		ACL_VSTREAM *stream, void *context);
typedef void (*ACL_EVENT_NOTIFY_TIME)(int event_type, ACL_EVENT *event,
		void *context);
#endif

/*----------------------------------------------------------------------------*/

/**
 * ����һ���¼�ѭ�����������ڣ��˺���������û������Ĳ�ͬ�Զ�����������¼����󴴽�����
 * @param event_mode {int} �¼�����ʽ��Ŀǰ��֧��: ACL_EVENT_SELECT, ACL_EVENT_KERNEL,
 *  ACL_EVENT_POLL, ACL_EVENT_WMSG
 * @param use_thr {int} �Ƿ�����߳��¼���ʽ����0��ʾ���߳��¼���ʽ
 * @param delay_sec {int} �¼�ѭ���ȴ�ʱ����������� event_mode Ϊ ACL_EVENT_WMSG
 *  ʱ���Ҹ�ֵ���� 0 ʱ�����ֵ��������Ϣֵ�Դ����� acl_event_new_wmsg����������
 *  ����Ϣ�����
 * @param delay_usec {int} �¼�ѭ���ȴ�ʱ���΢����(�� select ��ʽ����)
 * @return {ACL_EVENT*} �¼�����ָ�룬���Ϊ�ձ�ʾ����
 */
ACL_API ACL_EVENT *acl_event_new(int event_mode, int use_thr,
	int delay_sec, int delay_usec);

/**
 * ����һ���µ��¼�����, ���¼���֧�ֶ��߳�
 * @param delay_sec {int} �ڵ��� select() ����ʱ��Ϣ������
 * @param delay_usec {int} �ڵ��� select() ����ʱ��Ϣ��΢����
 * @return {ACL_EVENT*} �¼�����ָ�룬���Ϊ�ձ�ʾ����
 */
ACL_API ACL_EVENT *acl_event_new_select(int delay_sec, int delay_usec);

/**
 * ����һ���µ��¼�����, ���¼�֧���߳�ģʽ
 * @param delay_sec {int} �ڵ��� select() ����ʱ��Ϣ������
 * @param delay_usec {int} �ڵ��� select() ����ʱ��Ϣ��΢����
 * @return {ACL_EVENT*} �¼�����ָ�룬���Ϊ�ձ�ʾ����
 */
ACL_API ACL_EVENT *acl_event_new_select_thr(int delay_sec, int delay_usec);

/**
 * ����һ��֧�� poll ���¼����󣬲�֧�ֶ��߳�
 * @param delay_sec {int} �ڵ��� poll() ����ʱ��Ϣ������
 * @param delay_usec {int} �ڵ��� poll() ����ʱ��Ϣ��΢����
 * @return {ACL_EVENT*} �¼�����ָ�룬���Ϊ�ձ�ʾ����
 */
ACL_API ACL_EVENT *acl_event_new_poll(int delay_sec, int delay_usec);

/**
 * ����һ��֧�� poll ���¼�����֧�ֶ��߳�
 * @param delay_sec {int} �ڵ��� poll() ����ʱ��Ϣ������
 * @param delay_usec {int} �ڵ��� poll() ����ʱ��Ϣ��΢����
 * @return {ACL_EVENT*} �¼�����ָ�룬���Ϊ�ձ�ʾ����
 */
ACL_API ACL_EVENT *acl_event_new_poll_thr(int delay_sec, int delay_usec);

/**
 * ����һ���µ��¼�����, ���¼�����Ч�ʸߵ� epoll/devpoll/kqueue ��ʽ���Ҳ�֧�ֶ��߳�
 * @param delay_sec {int} �ڵ����¼�ѭ������ʱ��Ϣ������
 * @param delay_usec {int} �ڵ����¼�ѭ������ʱ��Ϣ��΢����(���Բ���)
 * @return {ACL_EVENT*} �¼�����ָ�룬���Ϊ�ձ�ʾ����
 */
ACL_API ACL_EVENT *acl_event_new_kernel(int delay_sec, int delay_usec);

/**
 * ����һ���µ��¼�����, ���¼�����Ч�ʸߵ� epoll/devpoll/kqueue ��ʽ���Ҳ����̷߳�ʽ
 * @param delay_sec {int} �ڵ����¼�ѭ������ʱ��Ϣ������
 * @param delay_usec {int} �ڵ����¼�ѭ������ʱ��Ϣ��΢����(���Բ���)
 * @return {ACL_EVENT*} �¼�����ָ�룬���Ϊ�ձ�ʾ����
 */
ACL_API ACL_EVENT *acl_event_new_kernel_thr(int delay_sec, int delay_usec);

/**
 * ����һ������ Windows ������Ϣ����һ����¼��������
 * @param nMsg {unsigned int} �����ֵ���� 0 �򽫸��첽��������Ϣֵ�󶨣�
 *  ���򽫸��첽�����ȱʡ����Ϣֵ��
 * @return {ACL_EVENT*} �¼�����ָ�룬���Ϊ�ձ�ʾ����
 */
ACL_API ACL_EVENT *acl_event_new_wmsg(unsigned int nMsg);

#ifdef WIN32
ACL_API HWND acl_event_wmsg_hwnd(ACL_EVENT *eventp);
#endif

/**
 * Ϊ�˷�ֹ�ڶ��߳�ģʽ�� select ���¼�ѭ����ʱ��ȴ���������Ӵ������жϵȴ���
 * �ӿ��¼�ѭ������
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 */
ACL_API void acl_event_add_dog(ACL_EVENT *eventp);

/**
 * �����¼�������ǰ�úͺ��ô������
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param fire_begin {void (*)(ACL_EVENT*, void*)} ���¼���ͳһ����ǰ�Ļص�����
 * @param fire_end {void (*)(ACL_EVENT*, void*)} ���¼���ͳһ������Ļص�����
 * @param ctx {void*} fire_begin / fire_end �ĵڶ�������
 */
ACL_API void acl_event_set_fire_hook(ACL_EVENT *eventp,
		void (*fire_begin)(ACL_EVENT*, void*),
		void (*fire_end)(ACL_EVENT*, void*),
		void* ctx);

/**
 * �����¼�ѭ�������ж�ʱ�������������״̬��ʱ�������ڲ�ȱ��ֵΪ 100 ms
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param n {int} ��ʱ���ʱ���� (���뼶)
 */
ACL_API void acl_event_set_check_inter(ACL_EVENT *eventp, int n);

/**
 * �ͷ��¼��ṹ
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 */
ACL_API void acl_event_free(ACL_EVENT *eventp);

/**
 * �����¼���ʱ���
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @return {acl_int64} ��ǰ�¼���ʱ���(΢�뼶��)
 */
ACL_API acl_int64 acl_event_time(ACL_EVENT *eventp);

/**
 * ���¼��е���������ִ�����
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 */
ACL_API void acl_event_drain(ACL_EVENT *eventp);

/**
 * �����������ɶ�ʱ(ָ�����ݴ�����������������������ر�)�Ļص�����
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param stream {ACL_VSTREAM*} ������ָ��, ����Ϊ��, �����е���������������Ч��
 * @param read_timeout {int} ����ʱʱ��(��)
 * @param callback {ACL_EVENT_NOTIFY_RDWR} �������ɶ�ʱ�Ļص�����
 * @param context {void*} �ص����� callback ����Ҫ�Ĳ���
 */
ACL_API void acl_event_enable_read(ACL_EVENT *eventp, ACL_VSTREAM *stream,
	int read_timeout, ACL_EVENT_NOTIFY_RDWR callback, void *context);

/**
 * ������������дʱ(ָ�пռ����д��������������������ر�)�Ļص�����
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param stream {ACL_VSTREAM*} ������ָ��, ����Ϊ��, �����е���������������Ч��
 * @param write_timeout {int} д��ʱʱ��(��)
 * @param callback {ACL_EVENT_NOTIFY_RDWR} ��������дʱ�Ļص�����
 * @param context {void*} �ص����� callback ����Ҫ�Ĳ���
 */
ACL_API void acl_event_enable_write(ACL_EVENT *eventp, ACL_VSTREAM *stream,
	int write_timeout, ACL_EVENT_NOTIFY_RDWR callback, void *context);

/**
 * ���ü����׽ӿ�(ָ�������ӵ���/��ϵͳ�ж�/����ʱ)�Ļص�����
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param stream {ACL_VSTREAM*} ������ָ��, ����Ϊ��, �����е���������������Ч��
 * @param read_timeout {int} ������ʱʱ��(��)������Ϊ0
 * @param callback {ACL_EVENT_NOTIFY_RDWR} �������ɶ�ʱ�Ļص�����
 * @param context {void*} �ص����� callback ����Ҫ�Ĳ���
 */
ACL_API void acl_event_enable_listen(ACL_EVENT *eventp, ACL_VSTREAM *stream,
	int read_timeout, ACL_EVENT_NOTIFY_RDWR callback, void *context);

/**
 * �����������¼��Ķ����������������
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param stream {ACL_VSTREAM*} ������ָ��, ����Ϊ��, �����е���������������Ч��
 */
ACL_API void acl_event_disable_read(ACL_EVENT *eventp, ACL_VSTREAM *stream);

/**
 * �����������¼���д���������������
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param stream {ACL_VSTREAM*} ������ָ��, ����Ϊ��, �����е���������������Ч��
 */
ACL_API void acl_event_disable_write(ACL_EVENT *eventp, ACL_VSTREAM *stream);

/**
 * �����������¼��Ķ�д���������������
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param stream {ACL_VSTREAM*} ������ָ��, ����Ϊ��, �����е���������������Ч��
 */
ACL_API void acl_event_disable_readwrite(ACL_EVENT *eventp, ACL_VSTREAM *stream);

/**
 * ������е��������Ƿ��Ѿ��������д���쳣�¼��ļ�����
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param stream {ACL_VSTREAM*} ������ָ��, ����Ϊ��, �����е���������������Ч��
 */
ACL_API int acl_event_isset(ACL_EVENT *eventp, ACL_VSTREAM *stream);

/**
 * ������е��������Ƿ��Ѿ�������¼��ļ�����
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param stream {ACL_VSTREAM*} ������ָ��, ����Ϊ��, �����е���������������Ч��
 */
ACL_API int acl_event_isrset(ACL_EVENT *eventp, ACL_VSTREAM *stream);

/**
 * ������е��������Ƿ��Ѿ�����д�¼��ļ�����
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param stream {ACL_VSTREAM*} ������ָ��, ����Ϊ��, �����е���������������Ч��
 */
ACL_API int acl_event_iswset(ACL_EVENT *eventp, ACL_VSTREAM *stream);

/**
 * ������е��������Ƿ��Ѿ������쳣�¼��ļ�����
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param stream {ACL_VSTREAM*} ������ָ��, ����Ϊ��, �����е���������������Ч��
 */
ACL_API int acl_event_isxset(ACL_EVENT *eventp, ACL_VSTREAM *stream);

/**
 * ���һ����ʱ�¼�
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param callback {ACL_EVENT_NOTIFY_TIME} ��ʱ�¼��Ļص�����
 * @param context {void*} callback ����Ҫ�Ļص�����
 * @param delay {acl_int64} eventp->event_present + delay Ϊ���¼�������ʼִ�е�ʱ��
 *  ��λΪ΢��
 * @param keep {int} �Ƿ��ظ���ʱ������
 * @return {acl_int64} �¼�ִ�е�ʱ��أ���λΪ΢��
 */
ACL_API acl_int64 acl_event_request_timer(ACL_EVENT *eventp,
	ACL_EVENT_NOTIFY_TIME callback, void *context, acl_int64 delay, int keep);

/**
 * ȡ��һ����ʱ�¼�
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param callback {ACL_EVENT_NOTIFY_TIME} ��ʱ�¼��Ļص�����
 * @param context {void*} callback ����Ҫ�Ļص�����
 * @return acl_int64 {acl_int64} ���뿪ʼִ���¼�������ʱ����, ��΢��Ϊ��λ
 */
ACL_API acl_int64 acl_event_cancel_timer(ACL_EVENT *eventp,
	ACL_EVENT_NOTIFY_TIME callback, void *context);

/**
 * ����ʱ��������Ϻ��Ƿ���Ҫ�ٴ����øö�ʱ�����Է��������ѭ��
 * ʹ�øö�ʱ��
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param callback {ACL_EVENT_NOTIFY_TIME} �ǿ�
 * @param context {void*} ������ callback �ı���
 * @param onoff {int} �Ƿ��ظ�ͨ�� acl_event_request_timer ���õĶ�ʱ��
 */
ACL_API void acl_event_keep_timer(ACL_EVENT *eventp, ACL_EVENT_NOTIFY_TIME callback,
	void *context, int onoff);

/**
 * �ж������õĶ�ʱ���������ظ�ʹ��״̬
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param callback {ACL_EVENT_NOTIFY_TIME} �ǿ�
 * @param context {void*} ������ callback �ı���
 * @return {int} !0 ��ʾ�����õĶ�ʱ���������ظ�ʹ��״̬
 */
ACL_API int acl_event_timer_ifkeep(ACL_EVENT *eventp, ACL_EVENT_NOTIFY_TIME callback,
	void *context);

/**
 * �¼�ѭ��ִ�еĵ��Ⱥ���
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 */
ACL_API void acl_event_loop(ACL_EVENT *eventp);

/**
 * �����¼�ѭ���Ŀ�����Ϣʱ���е��뼶��ֵ
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param sec {int} �뼶������Ϣʱ��ֵ
 */
ACL_API void acl_event_set_delay_sec(ACL_EVENT *eventp, int sec);

/**
 * �����¼�ѭ���Ŀ�����Ϣʱ���е�΢�뼶��ֵ
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @param usec {int} ΢�뼶������Ϣʱ��ֵ
 */
ACL_API void acl_event_set_delay_usec(ACL_EVENT *eventp, int usec);

/**
 * �Ƿ�����߳��¼���ʽ
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @return {int} 0: ��; !=0: ��
 */
ACL_API int acl_event_use_thread(ACL_EVENT *eventp);

/**
 * ��õ�ǰ�¼�������¼�ģ��
 * @param eventp {ACL_EVENT*} �¼�����ָ��, ��Ϊ��Ϊ��
 * @return {int} ACL_EVENT_SELECT/ACL_EVENT_KERNEL/ACL_EVENT_POLL
 */
ACL_API int acl_event_mode(ACL_EVENT *eventp);

#ifdef	__cplusplus
}
#endif

#endif
