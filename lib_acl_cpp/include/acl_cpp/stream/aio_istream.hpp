#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_timer_callback.hpp"
#include "acl_cpp/stream/aio_stream.hpp"

namespace acl
{

class aio_istream;

/**
 * �ӳ��첽�������࣬����Ϊ aio_timer_callback (see aio_handle.hpp)��
 * ��ν�ӳ��첽�������ǰ��첽����(aio_istream)���ڶ�ʱ���У�������
 * �������첽�����������(���� aio_handle �ļ���н��)����ָ��
 * ʱ�䵽����������첽������(�� timer_callback �ص��������½��첽
 * �����첽��������)��ͬʱ�ö�ʱ���Զ�����(���� destroy ����)��
 * ��������û��̳��� aio_timer_reader �࣬�����಻���ڶ��Ϸ���ģ�
 * ��������� destroy������ͬʱ������� destroy ��ִ������Դ�ͷŵ�
 * ��ز������������δ���� destroy���򵱶�ʱ���������ڲ����Զ�����
 * ���� aio_timer_reader �� destroy--��������� delete this����ʱ��
 * �ᵼ�·Ƿ��ڴ��ͷŲ���)
 * 
 */
class ACL_CPP_API aio_timer_reader : public aio_timer_callback
{
public:
	aio_timer_reader() {}

	/**
	 * �� aio_istream �е��ô˺������ͷ����������Ӧ��ʵ�ָú���
	 */
	virtual void destroy()
	{
		delete this;
	}
protected:
	virtual ~aio_timer_reader() {}
	/**
	 * �ӳٶ�����ʱ�Ļص��������� aio_timer_callback ���м̳ж���
	 */
	virtual void timer_callback(unsigned int id);
private:
	// ���� aio_istream ����ֱ���޸ı����˽�г�Ա����
	friend class aio_istream;

	aio_istream* in_;
	//int   read_delayed_;
	bool  delay_gets_;
	int   delay_timeout_;
	bool  delay_nonl_;
	int   delay_count_;
};

/**
 * �첽���������ඨ�壬����ֻ���ڶ��ϱ�ʵ������������ʱ��Ҫ���� close
 * �������ͷŸ������
 */
class ACL_CPP_API aio_istream : virtual public aio_stream
{
public:
	/**
	 * ���캯��
	 * @param handle {aio_handle*} �첽�¼�������
	 */
	aio_istream(aio_handle* handle);

	/**
	 * �����ɶ�ʱ�Ļص������ָ�룬����ûص�������Ѿ����ڣ���ֻ��
	 * ʹ�ö����ڴ򿪿���״̬
	 * @param callback {aio_callback*} �̳� aio_callback ������ص������
	 *  ���첽��������ʱ���ȵ��ô˻ص�������е� read_callback �ӿ�
	 */
	void add_read_callback(aio_callback* callback);

	/**
	 * �Ӷ��ص����󼯺���ɾ��
	 * @param callback {aio_read_callback*} ��ɾ���Ļص�����
	 * ����ֵΪ�գ���ɾ�����еĻص�����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */

	/**
	 * �Ӷ��ص����󼯺���ɾ���ص�����
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ���ɾ�����еĶ��ص�����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */
	int del_read_callback(aio_callback* callback = NULL);

	/**
	 * ��ֹ�ص������༯���е�ĳ���ص�����󣬵������ӻص������
	 * ������ɾ����ֻ�ǲ������ö���
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ����ֹ���еĶ��ص�����
	 * @return {int} ���ر��ӻص����󼯺��н��õĻص�����ĸ���
	 */
	int disable_read_callback(aio_callback* callback = NULL);

	/**
	 * �������еĻص����󱻵���
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ����������еĶ��ص�����
	 * @return {int} ���ر����õĻص�����ĸ���
	 */
	int enable_read_callback(aio_callback* callback = NULL);

	/**
	 * �첽��ȡһ�����ݣ����ӳ��첽��ʱ������������ô˹��̣�
	 * ��ֻ�����һ���ӳٶ�������Ч
	 * @param timeout {int} ����ʱʱ��(��)����Ϊ 0 ���ʾ
	 *  ��Զ�ȴ�ֱ����������һ�����ݻ����
	 * @param nonl {bool} �Ƿ��Զ�ȥ��β���Ļس����з�
	 * @param delay {int64} ����Է��������ݱȽϿ�ʱ���˲���
	 *  ���� 0 ʱ�����ӳٽ��նԷ������ݣ���ֵ�����ӳٶ�����
	 *  ��ʱ��(��λΪ΢��)
	 * @param callback {aio_timer_reader*} ��ʱ������ʱ�Ļص����������
	 *  �� delay > 0�������ֵΪ�գ������ȱʡ�Ķ���
	 */
#ifdef	WIN32
	void gets(int timeout = 0, bool nonl = true,
		__int64 delay = 0, aio_timer_reader* callback = NULL);
#else
	void gets(int timeout = 0, bool nonl = true,
		long long int delay = 0, aio_timer_reader* callback = NULL);
#endif

	/**
	 * �첽��ȡ���ݣ����ӳ��첽��ʱ������������ô˹��̣�
	 * ��ֻ�����һ���ӳٶ�������Ч
	 * @param count {int} ��Ҫ������������������Ϊ 0 ��ֻҪ������
	 *  �ɶ��ͷ��أ�����ֱ������ʱ���������������Ҫ����ֽ���
	 * @param timeout {int} ����ʱʱ��(��)����Ϊ 0 ���ʾ
	 *  ��Զ�ȴ�ֱ��������Ҫ������ݻ����
	 * @param delay {int64} ����Է��������ݱȽϿ�ʱ���˲���
	 *  ���� 0 ʱ�����ӳٽ��նԷ������ݣ���ֵ�����ӳٶ�����
	 *  ��ʱ��(��λΪ΢��)
	 * @param callback {aio_timer_reader*} ��ʱ������ʱ�Ļص����������
	 *  �����ֵΪ�գ������ȱʡ�Ķ���
	 */
#ifdef WIN32
	void read(int count = 0, int timeout = 0,
		__int64 delay = 0, aio_timer_reader* callback = NULL);
#else
	void read(int count = 0, int timeout = 0,
		long long int delay = 0, aio_timer_reader* callback = NULL);
#endif

	/**
	 * �첽�ȴ��������ɶ����ú��������첽���Ķ�����״̬���������ݿɶ�
	 * ʱ���ص����������������û��Լ��������ݵĶ�ȡ
	 * @param timeout {int} ����ʱʱ��(��)������ֵΪ 0 ʱ����û�ж���ʱ
	 */
	void read_wait(int timeout = 0);

	/**
	 * ��ֹ�첽�����첽��״̬�������첽�����첽����ļ����
	 * �Ƴ���ֱ���û������κ�һ���첽������(��ʱ���첽�����
	 * �Զ����¼�ظ����Ŀɶ�״̬)
	 */
	void disable_read();

	/**
	 * �������Ƿ�������Ӷ�����
	 * @param onoff {bool}
	 */
	void keep_read(bool onoff);

	/**
	 * ������Ƿ�������������������
	 * @return {bool}
	 */
	bool keep_read() const;

	/**
	 * ���ý��ջ���������󳤶ȣ��Ա��⻺���������Ĭ��ֵΪ 0 ��ʾ������
	 * @param int {max}
	 * @return {aio_istream&}
	 */
	aio_istream& set_buf_max(int max);

	/**
	 * ��õ�ǰ���ջ���������󳤶�����
	 * @return {int} ����ֵ  <= 0 ��ʾû������
	 */
	int get_buf_max(void) const;

protected:
	virtual ~aio_istream();

	/**
	 * �ͷŶ�̬�������麯��
	 */
	virtual void destroy();

	/**
	 * ע��ɶ��Ļص�����
	 */
	void hook_read();

private:
	friend class aio_timer_reader;
	aio_timer_reader* timer_reader_;
	std::list<AIO_CALLBACK*> read_callbacks_;
	bool read_hooked_;

	static int read_callback(ACL_ASTREAM*,  void*, char*, int);
	static int read_wakeup(ACL_ASTREAM* stream, void* ctx);
};

}  // namespace acl
