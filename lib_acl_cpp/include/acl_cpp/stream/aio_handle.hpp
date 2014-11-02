#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/noncopyable.hpp"

struct ACL_AIO;
struct ACL_EVENT;

namespace acl
{

// �¼���������
typedef enum
{
	ENGINE_SELECT,  // select ģʽ(֧������ƽ̨)
	ENGINE_POLL,    // poll ģʽ(�� UNIX ƽ̨)
	ENGINE_KERNEL,  // kernel ģʽ(win32: iocp, Linux: epoll, FreeBsd: kqueue, Solaris: devpoll
	ENGINE_WINMSG   // win32 GUI ��Ϣģʽ
} aio_handle_type;

/**
 * ������IO���¼������࣬�����װ��ϵͳ�� select/poll/epoll/kqueue/devpoll/iocp,
 */

class aio_timer_callback;
class aio_delay_free;
class aio_timer_delay_free;

class ACL_CPP_API aio_handle : private noncopyable
{
public:
	/**
	 * ���캯�������Զ�����IO�¼����棬���������������л��Զ��ͷ�
	 * @param engine_type {aio_handle_type} �����õ���������
	 *  ENGINE_SELECT: select ��ʽ��֧�� win32/unix ƽ̨
	 *  ENGINE_POLL: poll ��ʽ��֧�� unix ƽ̨
	 *  ENGINE_KERNEL: �Զ����ݸ���ϵͳƽ̨��֧�ֵĸ�Ч�ں������������
	 *  ENGINE_WINMSG: win32 ������Ϣ��ʽ��֧�� win32 ƽ̨
	 * @param nMsg {unsigned int} �� engine_type Ϊ ENGINE_WINMSG������ֵ
	 *  ���� 0 ʱ�����첽����������Ϣ�󶨣�������ȱʡ��Ϣ�󶨣�
	 *  �� engine_type Ϊ�� ENGINE_WINMSG ʱ����ֵ�������첽�����������
	 *  
	 */
	aio_handle(aio_handle_type engine_type = ENGINE_SELECT,
		unsigned int nMsg = 0);

	/**
	 * ���캯���������߽� ACL_AIO �������������������������в�����
	 * �Զ��ͷŸ� ACL_AIO ���
	 * @param handle {ACL_AIO*} ACL_AIO ���
	 */
	aio_handle(ACL_AIO* handle);

	virtual ~aio_handle();

	/**
	 * ����첽�����������Ƿ�������������������ᱻ���еĻ���
	 * ���첽���������첽�������̳У�һ�� aio_handle �������ȱʡ
	 * ���������������
	 * @param onoff {bool} �����Ƿ���������
	 */
	void keep_read(bool onoff);

	/**
	 * ����첽�������Ƿ������˳��������ݵĹ���
	 * @return {bool}
	 */
	bool keep_read() const;

	/**
	 * ���ö�ʱ��
	 * @param callback {aio_timer_callback*} ��ʱ���ص����������
	 * @param delay {int64} ��ʱ��ʱ����(΢��)
	 * @param id {unsigned int} ��ʱ��ĳ������� ID ��
	 * @return {int64} ��ʱ����Чʱ��(��1970.1.1������΢����)
	 */
#ifdef WIN32
	__int64 set_timer(aio_timer_callback* callback,
		__int64 delay, unsigned int id = 0);
#else
	long long int set_timer(aio_timer_callback* callback,
		long long int delay, unsigned int id = 0);
#endif

	/**
	 * ɾ����ʱ�������ж�ʱ�����¼�
	 * @param callback {aio_timer_callback*} ��ʱ���ص����������
	 * @return {time_t} ��ʱ����Чʱ��(��1970.1.1������΢����)
	 */
#ifdef WIN32
	__int64 del_timer(aio_timer_callback* callback);
#else
	long long int del_timer(aio_timer_callback* callback);
#endif

	/**
	 * ɾ����ʱ����ĳ��ָ�� ID �ŵĶ�ʱ����
	 * @param callback {aio_timer_callback*} ��ʱ���ص����������
	 * @param id {unsigned int} ��ʱ��ĳ������� ID ��
	 * @return {time_t} ��ʱ����Чʱ��(��1970.1.1������΢����)
	 */
#ifdef WIN32
	__int64 del_timer(aio_timer_callback* callback, unsigned int id);
#else
	long long del_timer(aio_timer_callback* callback, unsigned int id);
#endif

	/**
	 * ����ʱ����������״̬ʱ���û���Ϊ�޷��ͷŸö�ʱ��������ڴ�й¶��
	 * ͨ���˺��������Խ���������״̬�Ķ�ʱ��������δ����״̬ʱ���¼�
	 * ���������ͷ�(���� aio_delay_free::destroy())���Ӷ����Ա���
	 * �ڴ�й¶����
	 * @param callback {aio_delay_free*}
	 */
	void delay_free(aio_delay_free* callback);

	/**
	 * ��� ACL_AIO ���
	 * @return {ACL_AIO*}
	 */
	ACL_AIO* get_handle(void) const;

	/**
	 * ����첽���������
	 * @return {aio_handle_type}
	 */
	aio_handle_type get_engine_type(void) const;

	/**
	 * ��õ�ǰ���ڼ�ص��첽��������
	 * @return {int}
	 */
	int length() const;

	/**
	 * ��������첽����״̬��������׼�����첽���Ĵ������
	 * @return {bool} �Ƿ�Ӧ��ֹ�첽����
	 */
	bool check();

	/**
	 * ֪ͨ�첽��������ֹ
	 */
	void stop();

	/**
	 * �����첽������ڲ�״̬
	 */
	void reset();

protected:
	friend class aio_stream;

	/**
	 * �첽�������� 1
	 */
	void increase();

	/**
	 * ���첽�������� 1 ʱ�Ļص��麯��
	 */
	virtual void on_increase() {}

	/**
	 * �첽�������� 1
	 */
	void decrease();

	/**
	 * ���첽�������� 1 ʱ�Ļص��麯��
	 */
	virtual void on_decrease() {}

private:
	ACL_AIO* aio_;
	bool inner_alloc_;
	bool stop_;
	int  nstream_;
	aio_handle_type engine_type_;
	aio_timer_delay_free* delay_free_timer_;

	void destroy_timer(aio_timer_callback* callback);
	static void on_timer_callback(int event_type, ACL_EVENT*,
		aio_timer_callback *callback);
};

} // namespace acl
