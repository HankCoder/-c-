#ifndef	ACL_IOCTL_INCLUDE_H
#define	ACL_IOCTL_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include "event/acl_events.h"
#include "stdlib/acl_vstream.h"

typedef struct ACL_IOCTL ACL_IOCTL;

/**
 * ������������ʱ������ص��������Ͷ���, �û���Ҫ���ݴ˺���ԭ��ʵ���Լ�������
 * @param event_type {int} ���������¼�״̬, Ϊ����״̬֮һ:
 *        ACL_EVENT_READ: �����������ݿɶ�; 
 *	  ACL_EVENT_WRITE: �������пռ��д;
 *	  ACL_EVENT_RW_TIMEOUT: ����������д��ʱ;
 *	  ACL_EVENT_XCPT: �������ڲ������쳣.
 * @param ioc {ACL_IOCTL*} io ���ƾ��
 * @param stream {ACL_VSTREAM*} ���������
 * @param context {void*} �û��Զ������
 */
typedef void (*ACL_IOCTL_NOTIFY_FN)(int event_type, ACL_IOCTL *ioc,
	ACL_VSTREAM *stream, void *context);

typedef void (*ACL_IOCTL_WORKER_FN)(ACL_IOCTL *ioc, void *arg);
typedef void (*ACL_IOCTL_THREAD_INIT_FN)(void *);
typedef void (*ACL_IOCTL_THREAD_EXIT_FN)(void *);

/*----------------------------------------------------------------------------*/
/* in acl_ioctl.c */
/**
 * ����һ�����������
 * @param max_threads {int} ��������������ڵ�����߳���
 * @param idle_timeout {int} ÿ���߳̿���ʱ��, ��ĳ�����ֵĿ���ʱ�䳬��
 *  ��ֵʱ���Զ��˳�, ��λΪ��
 * @return {ACL_IOCTL*} ����������ؾ��
 */
ACL_API ACL_IOCTL *acl_ioctl_create(int max_threads, int idle_timeout);

/**
 * ����һ�����������
 * @param event_mode {int} �¼���ʽ: ACL_EVENT_SELECT/ACL_EVENT_KERNEL
 * @param max_threads {int} ��������������ڵ�����߳���
 * @param idle_timeout {int} ÿ���߳̿���ʱ��, ��ĳ�����ֵĿ���ʱ�䳬��
 *  ��ֵʱ���Զ��˳�, ��λΪ��
 * @param delay_sec {int} ���¼�ѭ���е���ֵ
 * @param delay_usec {int} ���¼�ѭ���е�΢��ֵ
 * @return {ACL_IOCTL*} ����������ؾ��
 */
ACL_API ACL_IOCTL *acl_ioctl_create_ex(int event_mode, int max_threads,
	int idle_timeout, int delay_sec, int delay_usec);

/**
 * Ϊ�˷�ֹ�ڶ��߳�ģʽ�� select ���¼�ѭ����ʱ��ȴ���������Ӵ������жϵȴ���
 * �ӿ��¼�ѭ������
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 */
ACL_API void acl_ioctl_add_dog(ACL_IOCTL *ioc);

/**
 * ���÷���������صĿ��Ʋ���
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param name {int} �����б��еĵ�һ������, ACL_IOCTL_CTL_
 * @param ... ��β�������
 */
ACL_API void acl_ioctl_ctl(ACL_IOCTL *ioc, int name, ...);
#define	ACL_IOCTL_CTL_END                       0  /**< ���ƽ�����־ */
#define	ACL_IOCTL_CTL_THREAD_MAX                1  /**< ��������߳��� */
#define	ACL_IOCTL_CTL_THREAD_IDLE               2  /**< �����߳̿����˳�ʱ�� */
#define	ACL_IOCTL_CTL_DELAY_SEC                 3  /**< ���� select ʱ���뼶��Ϣֵ */
#define	ACL_IOCTL_CTL_DELAY_USEC                4  /**< ���� select ʱ��΢�뼶��Ϣֵ */
#define	ACL_IOCTL_CTL_INIT_FN                   5  /**< �����̱߳�����ʱ���̳߳�ʼ������ */
#define	ACL_IOCTL_CTL_EXIT_FN                   6  /**< �����߳��˳�ʱ���߳��˳��ص����� */
#define	ACL_IOCTL_CTL_INIT_CTX                  7  /**< �����̳߳�ʼ��ʱ�Ļص����� */
#define	ACL_IOCTL_CTL_EXIT_CTX                  8  /**< �����߳��˳�ʱ�Ļص����� */
#define ACL_IOCTL_CTL_THREAD_STACKSIZE          9  /**< �����̵߳Ĺ�ģ�ߴ��С(�ֽ�) */

/**
 * �����������Դ
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 */
ACL_API void acl_ioctl_free(ACL_IOCTL *ioc);

/**
 * ������������
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @return {int} �Ƿ��������������������. 0: ok; < 0: err.
 */
ACL_API int acl_ioctl_start(ACL_IOCTL *ioc);

/**
 * ��Ϣѭ��(�������ڵ��߳�ģʽ)
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 */
ACL_API void acl_ioctl_loop(ACL_IOCTL *ioc);

/**
 * ����¼�������
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @return {ACL_EVENT*}
 */
ACL_API ACL_EVENT *acl_ioctl_event(ACL_IOCTL *ioc);

/**
 * �����������¼��Ķ���д������ȥ��
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} �ͻ���������ָ��
 */
ACL_API void acl_ioctl_disable_readwrite(ACL_IOCTL *ioc, ACL_VSTREAM *stream);

/**
 * �����������¼��Ķ�������ȥ��
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} �ͻ���������ָ��
 */
ACL_API void acl_ioctl_disable_read(ACL_IOCTL *h_ioctl, ACL_VSTREAM *stream);

/**
 * �����������¼���д������ȥ��
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} �ͻ���������ָ��
 */
ACL_API void acl_ioctl_disable_write(ACL_IOCTL *ioc, ACL_VSTREAM *stream);

/**
 * �ж�ĳ�����Ƿ����ܼ��״̬, ֻҪ����д�κ�һ��״̬��������
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} �ͻ���������ָ��
 * @return {int} 1����ʾ��; 0: ��ʾ��
 */
ACL_API int acl_ioctl_isset(ACL_IOCTL *ioc, ACL_VSTREAM *stream);

/**
 * �ж�ĳ�����Ƿ����ڶ����״̬
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} �ͻ���������ָ��
 * @return {int} 1����ʾ��; 0: ��ʾ��
 */
ACL_API int acl_ioctl_isrset(ACL_IOCTL *ioc, ACL_VSTREAM *stream);

/**
 * �ж�ĳ�����Ƿ���д�ܼ��״̬
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} �ͻ���������ָ��
 * @return {int} 1����ʾ��; 0: ��ʾ��
 */
ACL_API int acl_ioctl_iswset(ACL_IOCTL *ioc, ACL_VSTREAM *stream);

/**
 * ������״, ������IO���ʱ�Զ��ر���
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} �ͻ���������ָ��
 * @return {int} �Ƿ��������ر�
 *         0: ��ʾ���л�������δ������, �������첽�رչ���;
 *         1: ��ʾ������δ��������, �Ѿ���ͬ���ر�
 */
ACL_API int acl_ioctl_iocp_close(ACL_IOCTL *ioc, ACL_VSTREAM *stream);

/**
 * ������������һ����������������
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} �ͻ���������ָ��
 * @param timeout {int} �����ӿ��г�ʱʱ��
 * @param callback {ACL_IOCTL_NOTIFY_FN} ���������ɶ�������ʱʱ�Ļص�����
 * @param context {void*} �ص����� callback �Ĳ���֮һ, ��Ҫ���ڴ����û��Լ��Ĳ���,
 *  �û���Ҫ�� callback �ڽ��ò���ת�����Լ��Ŀ�ʶ������
 */
ACL_API void acl_ioctl_enable_read(ACL_IOCTL *ioc, ACL_VSTREAM *stream,
	int timeout, ACL_IOCTL_NOTIFY_FN callback, void *context);

/**
 * ������������һ��д��ع�������
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} �ͻ���������ָ��
 * @param timeout {int} �����ӿ��г�ʱʱ��
 * @param callback {ACL_IOCTL_NOTIFY_FN} ����������д������ʱʱ�Ļص�����
 * @param context {void*} �ص����� callback �Ĳ���֮һ, ��Ҫ���ڴ����û��Լ��Ĳ���,
 *  �û���Ҫ�� callback �ڽ��ò���ת�����Լ��Ŀ�ʶ������
 */
ACL_API void acl_ioctl_enable_write(ACL_IOCTL *ioc, ACL_VSTREAM *stream,
	int timeout, ACL_IOCTL_NOTIFY_FN callback, void *context);

/**
 * �첽�����ӷ�����, ���ӳɹ������ӳ�ʱʱ������û��Ļص�����
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} ��������Զ�̷�����״̬�ı��ؿͻ���������
 * @param timeout {int} ���ӳ�ʱʱ��
 * @param callback {ACL_IOCTL_NOTIFY_FN} ����������д������ʱʱ�Ļص�����
 * @param context {void*} �ص����� callback �Ĳ���֮һ, ��Ҫ���ڴ����û��Լ��Ĳ���,
 *  �û���Ҫ�� callback �ڽ��ò���ת�����Լ��Ŀ�ʶ������
 */
ACL_API void acl_ioctl_enable_connect(ACL_IOCTL *ioc, ACL_VSTREAM *stream,
	int timeout, ACL_IOCTL_NOTIFY_FN callback, void *context);

/**
 * ��Ϊ�����������ĳ����������ַ
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param stream {ACL_VSTREAM*} ��������Զ�̷�����״̬�ı��ؿͻ���������
 * @param timeout {int} �����׽��ּ�����ʱʱ��, ���˳�ʱʱ�䵽����û�������ӵ���ʱ,
 *  �����߿����ڻص������ﴦ�������¼�, �����ֵΪ 0 ��һֱ����
 *  ���������ӵ�������ʱ�û��Ļص������ű�����
 * @param callback {ACL_IOCTL_NOTIFY_FN} ���������ӵ��������׽��ֳ���
 *  �������ʱʱ�Ļص�����
 * @param context {void*} callback �Ĳ���֮һ, �μ�����˵��
 */
ACL_API void acl_ioctl_enable_listen(ACL_IOCTL *ioc, ACL_VSTREAM *stream,
	int timeout, ACL_IOCTL_NOTIFY_FN callback, void *context);

/*----------------------------------------------------------------------------*/
/**
 * ����Զ�̷�����
 * @param addr {const char*} �������˷����ַ, ��ʽ: ip:port, ��: 192.168.0.1:80
 * @param timeout {int} ���ӳ�ʱʱ��, �京������:
 *         1) 0:   ������������Զ�̷�����
 *         2) -1:  ����������Զ�̷�����ֱ�����ӳɹ�������ʧ��Ϊֹ
 *         3) > 0: ����ʱ������Զ�̷�����
 * @return {ACL_VSTREAM*} �ͻ���������.
 *  != NULL ��ʾ���ӳɹ�������������; == NULL ����ʧ�ܻ����
 * ע:
 *     ������������� 1) ʱ, ��Ҫ�����ص� ACL_VSTREAM �����������Ӽ�����ͨ���ص�
 *     �������Ը������ж�д����, ������Ҫ���� acl_ioctl_enable_connect() ��ȷ����
 *     �ӳɹ�.
 */
ACL_API ACL_VSTREAM *acl_ioctl_connect(const char *addr, int timeout);

/**
 * ����һ�������׽�����
 * @param addr {const char*} ���ر������ĵ�ַ, ��ʽ: ip:port, ��: 127.0.0.1:80
 * @param qlen {int} �������г���
 * @return {ACL_VSTREAM*} �����׽���������. != NULL ok; == NULL err.
 * ע: ��Ҫ�첽����, ����Ե��� acl_ioctl_enable_listen() ���첽��
 *     ���һ���ͻ�������
 */
ACL_API ACL_VSTREAM *acl_ioctl_listen(const char *addr, int qlen);

/**
 * ����һ�������׽�����
 * @param addr {const char*} ���ر������ĵ�ַ, ��ʽ: ip:port, ��: 127.0.0.1:80
 * @param qlen {int} �������г���
 * @param block_mode {int} �Ƿ���÷�����ģʽ, ACL_BLOCKING: ����ģʽ,
 *  ACL_NON_BLOCKING: ������ģʽ
 * @param io_bufsize {int} ��ÿͻ����������Ļ�������С(�ֽ�)
 * @param io_timeout {int} �ͻ������Ķ�д��ʱʱ��(��)
 * @return {ACL_VSTREAM*} �����׽���������. != NULL ok; == NULL err.
 * ע: ��Ҫ�첽����, ����Ե��� acl_ioctl_enable_listen() ���첽��
 *     ���һ���ͻ�������
 */
ACL_API ACL_VSTREAM *acl_ioctl_listen_ex(const char *addr, int qlen,
	int block_mode, int io_bufsize, int io_timeout);

/**
 * �Ӽ����׿ڻ��һ���ͻ�������
 * @param sstream {ACL_VSTREAM*} ����������
 * @param ipbuf {char*} �ͻ������ĵ�ַ
 * @param size {int} ipbuf �ռ��С
 * @return {ACL_VSTREAM*} �ͻ���������. != NULL �ɹ����һ���ͻ������ӵ�������;
 *  == NULL ���ܱ�ϵͳ�ж���һ��, ������Ӧ���Դ����
 */
ACL_API ACL_VSTREAM *acl_ioctl_accept(ACL_VSTREAM *sstream,
	char *ipbuf, int size);

/**
 * �������������һ����ʱ������, �ú������� acl_event_request_timer �ļ򵥷�װ.
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param timer_fn {ACL_EVENT_NOTIFY_TIME} ��ʱ������ص�����.
 * @param context {void*} timer_fn �Ĳ���֮һ.
 * @param idle_limit {acl_int64} ������ʱ��������ʱ��(΢�뼶)
 * @return {acl_int64} ʣ���ʱ��, ��λΪ΢��.
 */
ACL_API acl_int64 acl_ioctl_request_timer(ACL_IOCTL *ioc,
	ACL_EVENT_NOTIFY_TIME timer_fn, void *context, acl_int64 idle_limit);

/**
 * ȡ��ĳ����ʱ������, �ú������� acl_event_cancel_timer �ļ򵥷�װ.
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param timer_fn {ACL_EVENT_NOTIFY_TIME} ��ʱ������ص�����.
 * @param context {void*} timer_fn �Ĳ���֮һ.
 * @return {acl_int64} ʣ���ʱ��, ��λΪ΢��.
 */
ACL_API acl_int64 acl_ioctl_cancel_timer(ACL_IOCTL *ioc,                                                            
	ACL_EVENT_NOTIFY_TIME timer_fn, void *context);

/**
 * ��ǰ�̳߳�������һ���µ�����
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @param callback {ACL_IOCTL_WORKER_FN} ��������Ļص�����
 * @param arg {void*} callback �Ĳ���֮һ
 * @return {int} 0: ok; < 0: error
 */
ACL_API int acl_ioctl_add(ACL_IOCTL *ioc,
	ACL_IOCTL_WORKER_FN callback, void *arg);

/**
 * ��õ�ǰ�̳߳��й����̵߳�����.
 * @param ioc {ACL_IOCTL*} ����������ؾ��
 * @return {int} ���ص�ǰ�����̵߳����� == -1: error; >= 0: ok.
 */
ACL_API int acl_ioctl_nworker(ACL_IOCTL *ioc);

#ifdef	__cplusplus
}
#endif

#endif
