/**
 * @file	acl_aio.h
 * @author	zsx
 * @date	2010-1-2
 * @brief	���ļ��ж����˹��� ACL_ASTREAM���첽ͨ��������������˵���������ӿ�.
 * @version	1.1
 */

#ifndef	ACL_AIO_INCLUDE_H
#define	ACL_AIO_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include <stdarg.h>
#ifdef	ACL_UNIX
#include <sys/uio.h>
#endif

#include "stdlib/acl_stdlib.h"
#include "event/acl_events.h"

/*------------------------------- ���ݽṹ���Ͷ��� ---------------------------*/

/**
 * �첽������������Ͷ���
 */
typedef struct ACL_AIO ACL_AIO;

/**
 * �첽�����Ͷ���
 */
typedef struct ACL_ASTREAM ACL_ASTREAM;

/**
 * �¼�֪ͨ�����������, ��ĳ���ܼ�ص��������ݿɶ������ʱ�Ļص��û���ע�ắ��,
 * Ŀǰ�����������ص��첽������:
 *   acl_aio_gets, acl_aio_gets_nonl, acl_aio_read, acl_aio_readn.
 * @param astream {ACL_ASTREAM*} �첽��ָ��
 * @param context {void*} �û������ݵĲ���
 * @param data {const char*} �����ж�ȡ������ָ��
 * @param dlen {int} data ���ݵĳ���
 * @return {int} �ú���ָ������������-1�����Ӧ��Ҫ��ر��첽��
 */
typedef int (*ACL_AIO_READ_FN)(ACL_ASTREAM *astream,
	void *context, char *data, int dlen);

/**
 * �¼�֪ͨ����������ͣ���ĳ���첽���ɶ�/��дʱ���ô����͵��û��ص�����
 * @param astream {ACL_ASTREAM*} �첽��ָ��
 * @param context {void*} �û������ݵĲ���
 * @return {int} ����ú������ͷ��� -1 �����Ӧ��Ҫ��ر��첽��
 */
typedef int (*ACL_AIO_NOTIFY_FN)(ACL_ASTREAM *astream, void *context);

/**
 * �¼�֪ͨ�����������, ��ĳ���ܼ�ص���������д������ʱ�Ļص��û���ע�ắ��,
 * Ŀǰ�����������ص��첽������:
 *   acl_aio_writen, acl_aio_writev, acl_aio_fprintf, acl_aio_vfprintf.
 * @param astream {ACL_ASTREAM*} �첽��ָ��
 * @param context {void*} �û������ݵĲ���
 * @return {int} �ú���ָ������������-1�����Ӧ��Ҫ��ر��첽��
 */
typedef int (*ACL_AIO_WRITE_FN)(ACL_ASTREAM *astream, void *context);

/**
 * ��ĳ���������������µĿͻ�������ʱ, �첽��ܽ��ո����Ӳ����ݸ��û�; �������,
 * ���û������˸ü������ļ�����ʱֵ�ҵ���ó�ʱֵ, ��Ҳ�ᴥ���ú������;��. ��ú���
 * ���;����ص��첽������: acl_aio_accept.
 * @param cstream {ACL_ASTREAM*} �� sstream ������ͨ�� accept() ��õĿͻ���������
 * @param context {void*} �û������ݵĲ���
 * @return {int} ����ú������÷��� -1 ��ʾ���ټ��������µĿͻ�������
 */
typedef int (*ACL_AIO_ACCEPT_FN)(ACL_ASTREAM *cstream,	void *context);

/**
 * ��ĳ�����������������µĿͻ�������ʱ, �첽��ܻص����û���ע�ắ��, �û���Ҫ��
 * �ü������� accept �ÿͻ�������. ��ú���������ص��첽������: acl_aio_listen.
 * @param sstream {ACL_ASTREAM*} ���������
 * @param context {void*} �û������ݵĲ���
 * @return {int} ����ú����ĵ��÷���-1������Ӱ���������������
 * ע: ��ע��ú��������� ACL_AIO_ACCEPT_FN �Ĺ��ܲ��.
 */
typedef int (*ACL_AIO_LISTEN_FN)(ACL_ASTREAM *sstream, void *context);

/**
 * �첽����Զ�̷�����ʱ, ������ʧ�ܡ���ʱ��ɹ�ʱ���¼�֪ͨ�������
 * ���ص��û���ע�ắ��. ��ú���������ص��첽������: acl_aio_connect.
 * @param cstream {ACL_ASTREAM*} �ܼ�ص�����������״̬�Ŀͻ�����
 * @param context {void*} �û������ݵĲ���
 * @return {int} �����øú�������-1����Ҫ�رո��첽������
 */
typedef int (*ACL_AIO_CONNECT_FN)(ACL_ASTREAM *cstream, void *context);

/**
 * ������д����������ʱ�Ļص�����ָ��
 * @param astream {ACL_ASTREAM*} �첽��ָ��
 * @param context {void*} �û����ݵĲ���
 * @return {int} ���ú������÷���-1ʱ�����ڶ�д����ʾ��Ҫ�رո��첽��д����
 *  ���ڼ�������ʾ���ټ��������µĿͻ������ӣ�������0ʱ����ʾ����
 */
typedef int (*ACL_AIO_TIMEO_FN)(ACL_ASTREAM *astream, void *context);

/**
 * ����Ҫ�ر��첽��д��ʱ��Ҫ�ص����û�ע��ĺ���
 * @param astream {ACL_ASTREAM*} �첽��ָ��
 * @param context {void*} �û����ݵĲ���
 * @return {int} ���۸�ֵ��Σ����첽������Ҫ���ر�
 */
typedef int (*ACL_AIO_CLOSE_FN)(ACL_ASTREAM *astream, void *context);

/* �첽�����Ͷ��� */

struct ACL_ASTREAM {
	ACL_AIO *aio;		/**< �첽���¼���� */
	ACL_VSTREAM *stream;	/**< ͬ���� */

	ACL_VSTRING strbuf;	/**< �ڲ������� */
	int   timeout;		/**< IO��ʱʱ�� */
	int   nrefer;		/**< ͨ�������ü�����ֹ������ǰ�ر� */
	int   flag;		/**< ��־λ */
#define ACL_AIO_FLAG_IOCP_CLOSE     (1 << 0)
#define	ACL_AIO_FLAG_ISRD           (1 << 1)
#define	ACL_AIO_FLAG_ISWR           (1 << 2)
#define ACL_AIO_FLAG_DELAY_CLOSE    (1 << 3)
#define ACL_AIO_FLAG_DEAD           (1 << 4)

	ACL_FIFO write_fifo;	/**< �첽дʱ���Ƚ��ȳ��������� */
	int   write_left;	/**< д������δд��������� */
	int   write_offset;	/**< д�����е���һ��λ��ƫ�� */
	int   write_nested;	/**< дʱ��Ƕ�ײ��� */
	int   write_nested_limit;  /**< дʱ��Ƕ�ײ������� */

	int   (*read_ready_fn) (ACL_VSTREAM *, ACL_VSTRING *, int *);
	int   read_nested;	/**< ��ʱ��Ƕ�ײ��� */
	int   read_nested_limit;  /**< ��ʱ��Ƕ�ײ������� */
	int   count;		/**< ���� acl_aio_readn()/2 ʱ���õĵڶ�������ֵ */
	int   keep_read;	/**< �Ƿ����ó����Զ� */
	int   accept_nloop;	/**<  acl_aio_accept �ڲ�ѭ�� accept �������� */
	int   error;		/**< ��ǰ�׽ӿڵĴ���� */
	int   line_length;	/**< ������Ϊ��λ������ʱ��ֵ����ÿ����󳤶� */

	ACL_AIO_ACCEPT_FN  accept_fn;	/**< accept ���ʱ�Ļص����� */
	ACL_AIO_LISTEN_FN  listen_fn;	/**< �������ӵ���ʱ�Ļص����� */
	void *context;			/**< �û����õĲ��� */

	ACL_AIO_NOTIFY_FN  can_read_fn; /**< ���Զ�ʱ�Ļص����� */
	void *can_read_ctx;		/**< can_read_fn ����֮һ */
	ACL_AIO_NOTIFY_FN  can_write_fn; /**< ����дʱ�Ļص����� */
	void *can_write_ctx;		/**< can_write_fn ����֮һ */

	ACL_ARRAY *read_handles;	/**< �����ʱ�ĸ����ص����� */
	ACL_ARRAY *write_handles;	/**< д���ʱ�ĸ����ص����� */
	ACL_ARRAY *close_handles;	/**< �ر�ʱ�ĸ����ص����� */
	ACL_ARRAY *timeo_handles;	/**< ��ʱʱ�ĸ����ص����� */
	ACL_ARRAY *connect_handles;	/**< ���ӳɹ�ʱ�����ص����� */
	ACL_FIFO   reader_fifo;		/**< ��ʱ��Żص����� */
	ACL_FIFO   writer_fifo;		/**< ��ʱ��Żص����� */

	/* �ɶ�ʱ�Ļص����� */
	void (*event_read_callback)(int event_type, ACL_ASTREAM *astream);
};

/**
 * ��������IO��ʱʱ��
 */
#define ACL_AIO_SET_TIMEOUT(stream_ptr, _timeo_) do {  \
	ACL_ASTREAM *__stream_ptr = stream_ptr;        \
	__stream_ptr->timeout = _timeo_;               \
} while(0)

/**
 * �������� context ����
 */
#define ACL_AIO_SET_CTX(stream_ptr, _ctx_) do {  \
	ACL_ASTREAM *__stream_ptr = stream_ptr;  \
	__stream_ptr->context = _ctx_;           \
} while(0)

/*--------------------------- �첽���������ӿ� -------------------------------*/

/**
 * ����һ���첽ͨ�ŵ��첽���ʵ�����, ����ָ���Ƿ���� epoll/devpoll
 * @param event_mode {int} �¼�������ʽ: ACL_EVENT_SELECT, ACL_EVENT_POLL
 *  , ACL_EVENT_KERNEL, ACL_EVENT_WMSG
 * @return {ACL_AIO*} ����һ���첽���������. OK: != NULL; ERR: == NULL.
 */
ACL_API ACL_AIO *acl_aio_create(int event_mode);

/**
 * �����첽���ʵ�����, ����ָ���Ƿ���� epoll/devpoll/windows message
 * @param event_mode {int} �¼�������ʽ: ACL_EVENT_SELECT, ACL_EVENT_POLL
 *  , ACL_EVENT_KERNEL, ACL_EVENT_WMSG
 * @param nMsg {unsigned int} ���� WIN32 �������Ϣ����ʱ���� event_mode ��Ϊ
 *  ACL_EVENT_WMSG ʱ��ֵ����Ч�����ʾ���첽����󶨵���Ϣֵ
 * @return {ACL_AIO*} ����һ���첽���������. OK: != NULL; ERR: == NULL.
 */
ACL_API ACL_AIO *acl_aio_create2(int event_mode, unsigned int nMsg);

/**
 * �ͷ�һ���첽ͨ���첽���ʵ�����
 * @param aio {ACL_AIO*} �첽���������
 */
ACL_API void acl_aio_free(ACL_AIO *aio);

/**
 * �첽IO��Ϣѭ��(���ڵ��߳�ģʽ�µ���)
 * @param aio {ACL_AIO*} �첽���������
 */
ACL_API void acl_aio_loop(ACL_AIO *aio);

/**
 * ������� ACL_AIO �����д��رյ��첽���Ƿ�Ӧ�ùرգ����ô˺�����һЩ��Ҫ
 * �ӳٹرյ��첽���ᱻ�����ر�
 * @param aio {ACL_AIO*} �첽���������
 */
ACL_API void acl_aio_check(ACL_AIO *aio);

/**
 * ����¼�����ľ��
 * @param aio {ACL_AIO*} �첽���������
 * @return {ACL_EVENT*}
 */
ACL_API ACL_EVENT *acl_aio_event(ACL_AIO *aio);

/**
 * ����¼������õ�ģʽ
 * @param aio {ACL_AIO*} �첽���������
 * @return {int} ACL_EVENT_KERNEL/ACL_EVENT_SELECT/ACL_EVENT_POLL
 */
ACL_API int acl_aio_event_mode(ACL_AIO *aio);

/**
 * �첽IO����Ƿ��ǲ��ó�����ģʽ
 * @param aio {ACL_AIO*} �첽���������
 * @return {int} != 0: ��; == 0: ��
 */
ACL_API int acl_aio_get_keep_read(ACL_AIO *aio);

/**
 * �����첽IO��ܵĳ�����ģʽ
 * @param aio {ACL_AIO*} �첽���������
 * @param onoff {int} 0: �رճ���������; != 0: �򿪳���������
 */
ACL_API void acl_aio_set_keep_read(ACL_AIO *aio, int onoff);

/**
 * ��õ�ǰ�첽����ѭ��ʱ�ĵȴ�ʱ����벿��
 * @param aio {ACL_AIO*} �첽���������
 * @return {int} �� select/poll/epoll/kqueue/devpoll ʱ���뼶�ȴ�ʱ��
 */
ACL_API int acl_aio_get_delay_sec(ACL_AIO *aio);

/**
 * ��õ�ǰ�첽����ѭ��ʱ�ĵȴ�ʱ���΢�벿��
 * @param aio {ACL_AIO*} �첽���������
 * @return {int} �� select/poll/epoll/kqueue/devpoll ʱ��΢�뼶�ȴ�ʱ��
 */
ACL_API int acl_aio_get_delay_usec(ACL_AIO *aio);

/**
 * �����첽����ѭ���ĵȴ�ʱ���е��뼶����
 * @param aio {ACL_AIO*} �첽���������
 * @param delay_sec {int} ������ select/poll/epoll/kqueue/devpoll
 *  ʱ���뼶�ȴ�ʱ��
 */
ACL_API void acl_aio_set_delay_sec(ACL_AIO *aio, int delay_sec);

/**
 * �����첽����ѭ���ĵȴ�ʱ���е�΢�뼶����
 * @param aio {ACL_AIO*} �첽���������
 * @param delay_usec {int} ������ select/poll/epoll/kqueue/devpoll
 *  ʱ��΢�뼶�ȴ�ʱ��
 */
ACL_API void acl_aio_set_delay_usec(ACL_AIO *aio, int delay_usec);

/**
 * �����첽���Ķ���������С
 * @param aio {ACL_AIO*} �첽���������
 * @param rbuf_size {int} ����������С
 */
ACL_API void acl_aio_set_rbuf_size(ACL_AIO *aio, int rbuf_size);

/**
 * ���ü����첽��ÿ�ν��տͻ�������ʱѭ�����ո���
 * @param astream {ACL_ASTREAM*} ������
 * @param nloop {int}
 */
ACL_API void acl_aio_set_accept_nloop(ACL_ASTREAM *astream, int nloop);

/**
 * ���첽���л���첽���������
 * @param stream {ACL_ASTREAM*} �첽IO��
 * @return {ACL_AIO*} �첽���������
 */
ACL_API ACL_AIO *acl_aio_handle(ACL_ASTREAM *stream);

/**
 * �����첽���Ĳ���
 * @param stream {ACL_ASTREAM*} �첽IO��
 * @param ctx {void*} ����
 */
ACL_API void acl_aio_set_ctx(ACL_ASTREAM *stream, void *ctx);

/**
 * ����첽���Ĳ���
 * @param stream {ACL_ASTREAM*} �첽IO��
 * @return {void*} �첽�� stream �Ĳ���
 */
ACL_API void *acl_aio_get_ctx(ACL_ASTREAM *stream);

/**
 * ��һ���첽ͨ�����ľ��
 * @param aio {ACL_AIO*} �첽���������
 * @param stream {ACL_VSTREAM*} �ܼ�ص���, ��������������һ�����ݡ�����
 *  �����ʱʱ���ص��û���ע�ắ��.
 * @return {ACL_ASTREAM*} �첽ͨ�������
 */
ACL_API ACL_ASTREAM *acl_aio_open(ACL_AIO *aio, ACL_VSTREAM *stream);

/**
 * �첽IO��ɺ�ر�������������첽�رն���������Ҫ�ȶ�д�����ʱ�Źر���
 * @param astream {ACL_ASTREAM*} �첽������
 */
ACL_API void acl_aio_iocp_close(ACL_ASTREAM *astream);

/**
 * ȡ���첽IO���̣��ù�����Ҫ��Ϊ�˽��첽IO��ת��Ϊͬ��IO����д
 * @param astream {ACL_ASTREAM*} �첽IO��
 * @return {ACL_VSTREAM*} �����
 */
ACL_API ACL_VSTREAM *acl_aio_cancel(ACL_ASTREAM *astream);

/**
 * ��ü���������ÿ�ν��տͻ������ӵ�������
 * @param astream {ACL_ASTREAM *} ������������
 * @return {int} ÿ�ν������ӵ�������
 * @return {int} ������������ÿ�ν��չ����п���ѭ�����յ��������
 *  ��������ֵ��СΪ1
 */
ACL_API int acl_aio_get_accept_max(ACL_ASTREAM *astream);

/**
 * ���ü���������ÿ�ν��տͻ������ӵ�������
 * @param astream {ACL_ASTREAM *} ������������
 * @param accept_max {int} ������������ÿ�ν��չ����п���ѭ�����յ��������
 *  ��������ֵ��СΪ1
 */
ACL_API void acl_aio_set_accept_max(ACL_ASTREAM *astream, int accept_max);

/**
 * ��Ӹ��Ӷ��ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 * @param callback {ACL_AIO_READ_FN} �ص�����������Ϊ��
 * @param ctx {void*} callback �ص������Ļص�����������Ϊ��
 */
ACL_API void acl_aio_add_read_hook(ACL_ASTREAM *astream,
	ACL_AIO_READ_FN callback, void *ctx);

/**
 * ��Ӹ���д�ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 * @param callback {ACL_AIO_READ_FN} �ص�����������Ϊ��
 * @param ctx {void*} callback �ص������Ļص�����������Ϊ��
 */
ACL_API void acl_aio_add_write_hook(ACL_ASTREAM *astream,
	ACL_AIO_WRITE_FN callback, void *ctx);

/**
 * ��Ӹ��ӹرջص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 * @param callback {ACL_AIO_READ_FN} �ص�����������Ϊ��
 * @param ctx {void*} callback �ص������Ļص�����������Ϊ��
 */
ACL_API void acl_aio_add_close_hook(ACL_ASTREAM *astream,
	ACL_AIO_CLOSE_FN callback, void *ctx);

/**
 * ��Ӹ��ӳ�ʱ�ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 * @param callback {ACL_AIO_READ_FN} �ص�����������Ϊ��
 * @param ctx {void*} callback �ص������Ļص�����������Ϊ��
 */
ACL_API void acl_aio_add_timeo_hook(ACL_ASTREAM *astream,
	ACL_AIO_TIMEO_FN callback, void *ctx);

/**
 * ��Ӹ������ӳɹ��ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 * @param callback {ACL_AIO_READ_FN} �ص�����������Ϊ��
 * @param ctx {void*} callback �ص������Ļص�����������Ϊ��
 */
ACL_API void acl_aio_add_connect_hook(ACL_ASTREAM *astream,
	ACL_AIO_CONNECT_FN callback, void *ctx);

/**
 * ɾ�����Ӷ��ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 * @param callback {ACL_AIO_READ_FN} �ص�����������Ϊ��
 * @param ctx {void*} callback �ص������Ļص�����������Ϊ��
 */
ACL_API void acl_aio_del_read_hook(ACL_ASTREAM *astream,
	ACL_AIO_READ_FN callback, void *ctx);

/**
 * ɾ������д�ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 * @param callback {ACL_AIO_READ_FN} �ص�����������Ϊ��
 * @param ctx {void*} callback �ص������Ļص�����������Ϊ��
 */
ACL_API void acl_aio_del_write_hook(ACL_ASTREAM *astream,
	ACL_AIO_WRITE_FN callback, void *ctx);

/**
 * ɾ�����ӹرջص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 * @param callback {ACL_AIO_READ_FN} �ص�����������Ϊ��
 * @param ctx {void*} callback �ص������Ļص�����������Ϊ��
 */
ACL_API void acl_aio_del_close_hook(ACL_ASTREAM *astream,
	ACL_AIO_CLOSE_FN callback, void *ctx);

/**
 * ɾ�����ӳ�ʱ�ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 * @param callback {ACL_AIO_READ_FN} �ص�����������Ϊ��
 * @param ctx {void*} callback �ص������Ļص�����������Ϊ��
 */
ACL_API void acl_aio_del_timeo_hook(ACL_ASTREAM *astream,
	ACL_AIO_TIMEO_FN callback, void *ctx);

/**
 * ɾ���������ӳɹ��ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 * @param callback {ACL_AIO_READ_FN} �ص�����������Ϊ��
 * @param ctx {void*} callback �ص������Ļص�����������Ϊ��
 */
ACL_API void acl_aio_del_connect_hook(ACL_ASTREAM *astream,
	ACL_AIO_CONNECT_FN callback, void *ctx);

/**
 * ������еĸ��Ӷ��ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 */
ACL_API void acl_aio_clean_read_hooks(ACL_ASTREAM *astream);

/**
 * ������еĸ���д�ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 */
ACL_API void acl_aio_clean_write_hooks(ACL_ASTREAM *astream);

/**
 * ������еĸ��ӹرջص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 */
ACL_API void acl_aio_clean_close_hooks(ACL_ASTREAM *astream);

/**
 * ������еĸ��ӳ�ʱ�ص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 */
ACL_API void acl_aio_clean_timeo_hooks(ACL_ASTREAM *astream);

/**
* ������еĸ������ӳɹ��ص�����
* @param astream {ACL_ASTREAM*} �첽��������Ϊ��
*/
ACL_API void acl_aio_clean_connect_hooks(ACL_ASTREAM *astream);

/**
 * ������еĸ��ӻص�����
 * @param astream {ACL_ASTREAM*} �첽��������Ϊ��
 */
ACL_API void acl_aio_clean_hooks(ACL_ASTREAM *astream);

/**
 * �����첽��������
 * @param astream {ACL_ASTREAM*} �첽������
 * @param name {int} ��һ�����Ʋ���
 * @param ... ����б���ʽΪ��ACL_AIO_CTL_XXX, xxx, ���һ�����Ʋ���
 *  Ϊ ACL_AIO_CTL_END
 */
ACL_API void acl_aio_ctl(ACL_ASTREAM *astream, int name, ...);
#define ACL_AIO_CTL_END                 0   /**< ���ƽ�����־ */
#define ACL_AIO_CTL_ACCEPT_FN           1   /**< ���ý������Ӻ�ص����� */
#define ACL_AIO_CTL_LISTEN_FN           2   /**< ���������ӵ���ʱ�ص����� */
#define ACL_AIO_CTL_CTX                 3   /**< ����Ӧ�õĲ��� */
#define ACL_AIO_CTL_TIMEOUT             4   /**< ���ó�ʱʱ�� */
#define	ACL_AIO_CTL_LINE_LENGTH         5   /**< �������������ݵ���󳤳��� */
#define ACL_AIO_CTL_STREAM              10  /**< ����ACL_VSTREAM��ָ�� */
#define ACL_AIO_CTL_READ_NESTED         11  /**< ��������Ƕ�ײ��� */
#define ACL_AIO_CTL_WRITE_NESTED        12  /**< �������дǶ�ײ��� */
#define ACL_AIO_CTL_KEEP_READ           13  /**< �����Ƿ���������־ */
#define	ACL_AIO_CTL_READ_HOOK_ADD       14  /**< ��Ӹ��Ӷ��ص����� */
#define	ACL_AIO_CTL_READ_HOOK_DEL       15  /**< ɾ�����Ӷ��ص����� */
#define	ACL_AIO_CTL_WRITE_HOOK_ADD      16  /**< ��Ӹ���д�ص����� */
#define	ACL_AIO_CTL_WRITE_HOOK_DEL      17  /**< ɾ������д�ص����� */
#define	ACL_AIO_CTL_CLOSE_HOOK_ADD      18  /**< ��Ӹ��ӹرջص����� */
#define	ACL_AIO_CTL_CLOSE_HOOK_DEL      19  /**< ɾ�����ӹرջص����� */
#define	ACL_AIO_CTL_TIMEO_HOOK_ADD      20  /**< ��Ӹ��ӳ�ʱ�ص����� */
#define	ACL_AIO_CTL_TIMEO_HOOK_DEL      21  /**< ɾ�����ӳ�ʱ�ص����� */
#define	ACL_AIO_CTL_CONNECT_HOOK_ADD    22  /**< ��Ӹ������ӻص����� */
#define	ACL_AIO_CTL_CONNECT_HOOK_DEL    23  /**< ɾ���������ӻص����� */

/**
 * ���첽������ȡ ACL_VSTREAM ��
 * @param astream {ACL_ASTREAM*} �첽IO��
 * @return {ACL_VSTREAM*} ͨ����ָ��
 */
ACL_API ACL_VSTREAM *acl_aio_vstream(ACL_ASTREAM *astream);

/*---------------------------- �첽�������ӿ� --------------------------------*/

/**
 * �첽�����ж�ȡһ������, ���ɹ���ȡһ�����ݡ���������ʱʱ���ص��û���
 * ע�ắ��: notify_fn
 * @param astream {ACL_ASTREAM*} �ܼ�ص���, ��������������һ�����ݡ�����
 *  �����ʱʱ���ص��û���ע�ắ��.
 * ע: �������������첽�����.
 *     ��ͨ�� acl_aio_stream_set_line_length ����������󳤶����ƣ��򵱽��յ�
 *     �����й���ʱ��Ϊ���⻺����������ú����Ĵ�����̽����ڻ������ﵽ�ó���
 *     ����ʱ��������ֱ�ӽ����ݽ���ʹ����ע��Ļص�����
 */
ACL_API void acl_aio_gets(ACL_ASTREAM *astream);

/**
 * �첽�����ж�ȡһ������, ���ɹ���ȡһ�����ݡ���������ʱʱ���ص��û���
 * ע�ắ��: notify_fn, �� acl_aio_gets ��������, ��Ψһ�������Ƿ��ص�����
 * data �в����� "\r\n" �� "\n", ������һ������ʱ, �� dlen == 0.
 * @param astream {ACL_ASTREAM*} �ܼ�ص���, ��������������һ�����ݡ�����
 *  �����ʱʱ���ص��û���ע�ắ��.
 * ע: �������������첽�����.
 *     �����й���ʱ��Ϊ���⻺����������ú����Ĵ�����̽����ڻ������ﵽ�ó���
 *     ����ʱ��������ֱ�ӽ����ݽ���ʹ����ע��Ļص�����
 */
ACL_API void acl_aio_gets_nonl(ACL_ASTREAM *astream);

/**
 * �첽�����ж�ȡ����, ��ȡ�����ݸ�ʽ������û������Ҫ��.
 * @param astream {ACL_ASTREAM*} ���ڶ���ص���. ������������ʱ���Ѿ���ȡ��һ��
 *  ���ȵ�����ʱ�������¼�֪ͨ����
 * ע: �������������첽�����.
 */
ACL_API void acl_aio_read(ACL_ASTREAM *astream);

/**
 * �첽�����ж�ȡҪ�󳤶ȵ�����, ����������ʱ���������Ҫ������ݳ���ʱ��
 * �����¼�֪ͨ����
 * @param astream {ACL_ASTREAM*} ���ڶ���ص���. ������������ʱ���Ѿ���ȡ����
 *  Ҫ�󳤶ȵ�����ʱ�������¼�֪ͨ����
 * @param count {int} ��Ҫ������ݵĳ���, ������� 0.
 * ע: �������������첽�����.
 */
ACL_API void acl_aio_readn(ACL_ASTREAM *astream, int count);

/**
 * �����Զ�ȡһ������
 * @param astream {ACL_ASTREM*} �첽������
 * @return {ACL_VSTRING*} ����������һ���򷵻طǿն����û������ ACL_VSTRING
 *  ���ݺ�Ӧ���� ACL_VSTRING_RESET(s) ��ջ�����; ��δ�����������򷵻ؿ�
 */
ACL_API ACL_VSTRING *acl_aio_gets_peek(ACL_ASTREAM *astream);

/**
 * �����Զ�ȡһ������(������ \n �� \r\n)
 * @param astream {ACL_ASTREM*} �첽������
 * @return {ACL_VSTRING*} ����������һ���򷵻طǿն����û������ ACL_VSTRING
 *  ���ݺ�Ӧ���� ACL_VSTRING_RESET(s) ��ջ�����, �����������һ�����У��򷵻ص�
 *  ACL_VSTRING �Ļ����������ݳ���(ACL_VSTRING_LEN ��ô�ֵ) ӦΪ 0;
 *  ��δ�����������򷵻ؿ�
 */
ACL_API ACL_VSTRING *acl_aio_gets_nonl_peek(ACL_ASTREAM *astream);

/**
 * �����Դ��첽���ж�ȡ���ݣ�����������򷵻�û���򷵻ؿ�
 * @param astream {ACL_ASTREM*} �첽������
 * @return {ACL_VSTRING*} �������������򷵻صĻ������ǿ�(ʹ��������˻�������
 *  ��Ҫ���� ACL_VSTRING_RESET(s) ��մ˻�����), ���򷵻ؿ�
 */
ACL_API ACL_VSTRING *acl_aio_read_peek(ACL_ASTREAM *astream);

/**
 * �����Դ��첽���ж��������ȵ����ݣ������������������Ҫ���򷵻ػ�����
 * @param astream {ACL_ASTREM*} �첽������
 * @param count {int} Ҫ����������ݳ���
 * @return {ACL_VSTRING*} �������涨�����򷵻طǿջ�����(ʹ��������˻�������
 *  ��Ҫ���� ACL_VSTRING_RESET(s) ��մ˻�����), ���򷵻ؿ�
 */
ACL_API ACL_VSTRING *acl_aio_readn_peek(ACL_ASTREAM *astream, int count);

/**
 * �����첽��Ϊ������״̬���������ɶ�ʱ������û��Ļص�����
 * @param astream {ACL_ASTREM*} �첽������
 * @param can_read_fn {ACL_AIO_NOTIFY_FN} �û��ص�����
 * @param context {void*} can_read_fn �Ĳ���֮һ
 */
ACL_API void acl_aio_enable_read(ACL_ASTREAM *astream,
	ACL_AIO_NOTIFY_FN can_read_fn, void *context);

/**
 * ����첽���ж������ݿɶ�
 * @param astream {ACL_ASTREM*} �첽������
 * @return {int} ACL_VSTREAM_EOF ��ʾ����Ӧ�ùرո���; 0 ��ʾ�����ݿɶ�;
 *  > 0 ��ʾ�����ݿɶ�
 */
ACL_API int acl_aio_can_read(ACL_ASTREAM *astream);

/**
 * ֹͣ��һ������������IO������
 * @param astream {ACL_ASTREAM*} �첽������
 */
ACL_API void acl_aio_disable_read(ACL_ASTREAM *astream);

/**
 * �ж����Ƿ����첽�¼��Ķ�����������
 * @param astream {ACL_ASTREAM*} �첽������
 * @return {int} 0: ��!= 0: ��
 */
ACL_API int acl_aio_isrset(ACL_ASTREAM *astream);

/**
 * ���ö�һ������ʱÿ�����ݵ���󳤶����ƣ�������Ŀ����Ҫ��Ϊ�˷�ֹ�Է����͵�
 * һ�����ݹ�������ɱ��ؽ��ջ������ڴ����
 * @param astream {ACL_ASTREAM*} �첽������
 * @param len {int} ����ֵ > 0 ʱ�������ư��ж������ݳ���
 */
ACL_API void acl_aio_stream_set_line_length(ACL_ASTREAM *astream, int len);

/**
 * ��������õ������ж�����ʱ����󳤶�����
 * @param astream {ACL_ASTREAM*} �첽������
 * @return {int}
 */
ACL_API int acl_aio_stream_get_line_length(ACL_ASTREAM *astream);

/**
 * ���������첽������������ǣ�ȱʡ������Զ��̳� ACL_AIO �е� keep_read
 * ���(��Ĭ���������������)
 * @param astream {ACL_ASTREAM*} �첽������
 * @param onoff {int} 0 ��ʾ�ر����������ܣ���0��ʾ������������
 */
ACL_API void acl_aio_stream_set_keep_read(ACL_ASTREAM *astream, int onoff);

/**
 * ����첽���Ƿ������������������
 * @return {int} 0 ��ʾ�ر������������ܣ���0��ʾ��������������
 */
ACL_API int acl_aio_stream_get_keep_read(ACL_ASTREAM *astream);

/*---------------------------- �첽д�����ӿ� --------------------------------*/

/**
 * �첽������д����, ��������д��ʱ��д�ɹ�ʱ�������¼�֪ͨ����
 * @param astream {ACL_ASTREAM*} ����д��ص���.
 * @param data {const char*} ��д���ݵ��ڴ濪ʼָ��λ��
 * @param dlen {int} data �����ݳ���
 */
ACL_API void acl_aio_writen(ACL_ASTREAM *astream, const char *data, int dlen);

/**
 * �첽������д����, ��������д��ʱ��д�ɹ�ʱ�������¼�֪ͨ���̣�����ϵͳ��
 * writev
 * @param astream {ACL_ASTREAM*} ����д��ص���.
 * @param vector {const struct iovec*} ���ݼ�������
 * @param count {int} vector ����ĳ���
 */
ACL_API void acl_aio_writev(ACL_ASTREAM *astream,
		const struct iovec *vector, int count);

/**
 * �Ը�ʽ��ʽ�첽������д����, ��������д��ʱ��д�ɹ�ʱ�������¼�֪ͨ����
 * @param astream {ACL_ASTREAM*} ����д��ص���
 * @param fmt {const char*} ��ʽ�ַ���
 * @param ap {va_list} ��ʽ�ַ����Ĳ����б�
 */
ACL_API void acl_aio_vfprintf(ACL_ASTREAM *astream, const char *fmt, va_list ap);

/**
 * �Ը�ʽ��ʽ�첽������д����, ��������д��ʱ��д�ɹ�ʱ�������¼�֪ͨ����
 * @param astream {ACL_ASTREAM*} ����д��ص���
 * @param fmt {const char*} ��ʽ�ַ���
 * @param ... ��β�����
 */
ACL_API void ACL_PRINTF(2, 3) acl_aio_fprintf(ACL_ASTREAM *astream, const char *fmt, ...);

/**
 * �����첽��Ϊд����״̬����������дʱ������û��Ļص�����
 * @param astream {ACL_ASTREM*} �첽������
 * @param can_write_fn {ACL_AIO_NOTIFY_FN} �û��ص�����
 * @param context {void*} can_write_fn �Ĳ���֮һ
 */
ACL_API void acl_aio_enable_write(ACL_ASTREAM *astream,
	ACL_AIO_NOTIFY_FN can_write_fn, void *context);

/**
 * ֹͣ��һ������������IOд����
 * @param astream {ACL_ASTREAM*} �첽������
 */
ACL_API void acl_aio_disable_write(ACL_ASTREAM *astream);

/**
 * �ж����Ƿ����첽�¼���д����������
 * @param astream {ACL_ASTREAM*} �첽������
 * @return {int} 0: ��!= 0: ��
 */
ACL_API int acl_aio_iswset(ACL_ASTREAM *astream);

/*---------------------------- �첽���������ӿ� ------------------------------*/

/**
 * �첽����һ���ͻ���������, �����ÿͻ������ش����û�
 * @param astream {ACL_ASTREAM*} ���ڼ���״̬����
 */
ACL_API void acl_aio_accept(ACL_ASTREAM *astream);

/**
 * �첽����, ���������ϳ�����ʱ���������ӵ���ʱ�����������¼�֪ͨ����, ����
 * ������ʱ�û������Լ���ע�ắ���� accept() ��������.
 * @param astream {ACL_ASTREAM*} ���ڼ���״̬����
 */
ACL_API void acl_aio_listen(ACL_ASTREAM *astream);

/*---------------------------- �첽���Ӳ����ӿ� ------------------------------*/

/**
 * �첽����һ��Զ�̷�����, ��������������ʱ�����ӳɹ�ʱ�������¼�֪ͨ����.
 * @param aio {ACL_AIO*} �첽���������
 * @param saddr {const char*} Զ�̷�������ַ, ��ʽ: ip:port, ��: 192.168.0.1:80
 * @param timeout {int} ���ӳ�ʱ��ʱ��ֵ����λΪ��
 * @return {ACL_ASTREAM*} �����첽���ӹ����Ƿ�ɹ�
 */
ACL_API ACL_ASTREAM *acl_aio_connect(ACL_AIO *aio, const char *addr, int timeout);

/*---------------------------- ����ͨ���첽�����ӿ� --------------------------*/

/**
 * ֹͣ��һ������������IO��д����
 * @param astream {ACL_ASTREAM*} �첽������
 */
ACL_API void acl_aio_disable_readwrite(ACL_ASTREAM *astream);

/**
 * �ж����Ƿ����첽�¼��Ķ���д����������
 * @param astream {ACL_ASTREAM*} �첽������
 * @return {int} 0: ��!= 0: ��
 */
ACL_API int acl_aio_isset(ACL_ASTREAM *astream);

/**
 * ��õ�ǰ�첽�������ü���ֵ
 * @param astream {ACL_ASTREAM*} �첽������
 * @return {int} >=0���첽�������ü���ֵ
 */
ACL_API int acl_aio_refer_value(ACL_ASTREAM * astream);

/**
 * ���첽�������ü���ֵ��1
 * @param astream {ACL_ASTREAM*} �첽������
 */
ACL_API void acl_aio_refer(ACL_ASTREAM *astream);

/**
 * ���첽�������ü���ֵ��1
 * @param astream {ACL_ASTREAM*} �첽������
 */
ACL_API void acl_aio_unrefer(ACL_ASTREAM *astream);

/**
 * ���һ����ʱ������, �ú������� acl_event_request_timer �ļ򵥷�װ
 * @param aio {ACL_AIO*} �첽ͨ��������
 * @param timer_fn {ACL_EVENT_NOTIFY_TIME} ��ʱ������ص�����.
 * @param context {void*} timer_fn �Ĳ���֮һ.
 * @param idle_limit {acl_int64} ������ʱ��������ʱ�䣬��λΪ΢��.
 * @param keep {int} �Ƿ��ظ���ʱ������
 * @return {acl_int64} ʣ���ʱ��, ��λΪ΢��.
 */
ACL_API acl_int64 acl_aio_request_timer(ACL_AIO *aio,
		ACL_EVENT_NOTIFY_TIME timer_fn, void *context,
		acl_int64 idle_limit, int keep);

/**
 * ȡ��ĳ����ʱ������, �ú������� acl_event_cancel_timer �ļ򵥷�װ.
 * @param aio {ACL_AIO*} �첽ͨ��������
 * @param timer_fn {ACL_EVENT_NOTIFY_TIME} ��ʱ������ص�����.
 * @param context {void*} timer_fn �Ĳ���֮һ.
 * @return {acl_int64} ʣ���ʱ��, ��λΪ΢��.
 */
ACL_API acl_int64 acl_aio_cancel_timer(ACL_AIO *aio,
		ACL_EVENT_NOTIFY_TIME timer_fn, void *context);

/**
 * �����Ƿ���Ҫѭ������ͨ�� acl_aio_request_timer ���õĶ�ʱ������
 * @param aio {ACL_AIO*} �첽ͨ��������
 * @param timer_fn {ACL_EVENT_NOTIFY_TIME} ��ʱ������ص�����.
 * @param context {void*} timer_fn �Ĳ���֮һ.
 * @param onoff {int} �Ƿ��ظ���ʱ������
 */
ACL_API void acl_aio_keep_timer(ACL_AIO *aio, ACL_EVENT_NOTIFY_TIME callback,
		void *context, int onoff);

/**
 * �ж������õĶ�ʱ���������ظ�ʹ��״̬
 * @param aio {ACL_AIO*} �첽ͨ��������
 * @param timer_fn {ACL_EVENT_NOTIFY_TIME} ��ʱ������ص�����.
 * @param context {void*} timer_fn �Ĳ���֮һ.
 * @return {int} !0 ��ʾ�����õĶ�ʱ���������ظ�ʹ��״̬
 */
ACL_API int acl_aio_timer_ifkeep(ACL_AIO *aio, ACL_EVENT_NOTIFY_TIME callback,
		void *context);

#ifdef	__cplusplus
}
#endif

#endif
