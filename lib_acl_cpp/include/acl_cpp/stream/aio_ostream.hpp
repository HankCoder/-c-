#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <stdarg.h>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_timer_callback.hpp"
#include "acl_cpp/stream/aio_stream.hpp"

namespace acl
{

class aio_ostream;

/**
 * �ӳ��첽д�����࣬����Ϊ aio_timer_callback (see aio_handle.hpp)��
 * ��ν�ӳ��첽д�����ǰ��첽д��(aio_ostream)���ڶ�ʱ���У�������
 * �������첽д���������(���� aio_handle �ļ���н��)����ָ��
 * ʱ�䵽����������첽д����(�� timer_callback �ص��������½��첽
 * �����첽д������)��ͬʱ�ö�ʱ���Զ�����(���� destroy ����)��
 * ��������û��̳��� aio_timer_writer �࣬�����಻���ڶ��Ϸ���ģ�
 * ��������� destroy������ͬʱ������� destroy ��ִ������Դ�ͷŵ�
 * ��ز������������δ���� destroy���򵱶�ʱ���������ڲ����Զ�����
 * ���� aio_timer_writer �� destroy--��������� delete this����ʱ��
 * �ᵼ�·Ƿ��ڴ��ͷŲ���)
 * 
 */
class ACL_CPP_API aio_timer_writer : public aio_timer_callback
{
public:
	aio_timer_writer();

	/**
	 * �� aio_istream �е��ô˺������ͷ����������Ӧ��ʵ�ָú���
	 */
	virtual void destroy()
	{
		delete this;
	}

protected:
	virtual ~aio_timer_writer();

	/**
	 * �ӳٶ�����ʱ�Ļص��������� aio_timer_callback ���м̳ж���
	 */
	virtual void timer_callback(unsigned int id);
private:
	friend class aio_ostream;

	aio_ostream* out_;
	//int   write_delayed_;
	acl::string buf_;
};

/**
 * �첽д�������ඨ�壬����ֻ���ڶ��ϱ�ʵ������������ʱ��Ҫ���� close
 * �������ͷŸ������
 */
class ACL_CPP_API aio_ostream : virtual public aio_stream
{
public:
	/**
	 * ���캯��
	 * @param handle {aio_handle*} �첽�¼�������
	 */
	aio_ostream(aio_handle* handle);

	/**
	 * ������дʱ�Ļص������ָ�룬����ûص�������Ѿ����ڣ���ֻ��
	 * ʹ�ö����ڴ򿪿���״̬
	 * @param callback {aio_callback*} �̳� aio_callback ������ص������
	 *  ���첽��������ʱ���ȵ��ô˻ص�������е� write_callback �ӿ�
	 */
	void add_write_callback(aio_callback* callback);

	/**
	 * ��д�ص����󼯺���ɾ��
	 * @param callback {aio_callback*} ��ɾ����д�ص�����
	 *  ����ֵΪ�գ���ɾ�����еĻص�д����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */
	int del_write_callback(aio_callback* callback = NULL);

	/**
	 * ��ֹ�ص������༯���е�ĳ���ص�����󣬵������ӻص������
	 * ������ɾ����ֻ�ǲ������ö���
	 * @param callback {aio_callback*} �����õ�д�ص�����
	 *  ����ֵΪ�գ���������е�д�ص�����
	 * @return {int} ���ر��ӻص����󼯺��н��õĻص�����ĸ���
	 */
	int disable_write_callback(aio_callback* callback = NULL);

	/**
	 * �������еĻص����󱻵���
	 * @param callback {aio_callback*} ����ָ����д�ص�����
	 *  �����ֵΪ�գ����������е�д�ص�����
	 * @return {int} ���ر����õ�д�ص�����ĸ���
	 */
	int enable_write_callback(aio_callback* callback = NULL);

	/**
	 * �첽д�涨�ֽ��������ݣ�����ȫд�ɹ�������ʱʱ��
	 * �����û�ע��Ļص����������ӳ��첽дʱ������һ������
	 * ���������ô˹���ʱ��ÿ���ӳ��첽д�����ᱻ�����ӳ�д
	 * �Ķ����У��Ա�֤ÿ���ӳ��첽д���������ڸ��ԵĶ�ʱ��
	 * ����ʱ��ִ��
	 * @param data {const void*} ���ݵ�ַ
	 * @param len {int} ���ݳ���
	 * @param delay {int64} �����ֵ > 0 ������ӳٷ��͵�ģʽ(��λΪ΢��)
	 * @param callback {aio_timer_writer*} ��ʱ������ʱ�Ļص����������
	 */
#ifdef WIN32
	void write(const void* data, int len, __int64 delay = 0,
		aio_timer_writer* callback = NULL);
#else
	void write(const void* data, int len, long long int delay = 0,
		aio_timer_writer* callback = NULL);
#endif

	/**
	 * ��ʽ����ʽ�첽д���ݣ�����ȫд�ɹ�������ʱʱ��
	 * �����û�ע��Ļص�����
	 * @param fmt {const char*} ��ʽ�ַ���
	 */
	void format(const char* fmt, ...) ACL_CPP_PRINTF(2, 3);

	/**
	 * ��ʽ����ʽ�첽д���ݣ�����ȫд�ɹ�������ʱʱ��
	 * �����û�ע��Ļص�����
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ap {va_list} ����ֵ�б�
	 */
	void vformat(const char* fmt, va_list ap);

	/**
	 * �첽�ȴ���������д���ú��������첽����д����״̬�����п�дʱ��
	 * �ص����������������û��Լ��������ݵĶ�ȡ
	 * @param timeout {int} д��ʱʱ��(��)������ֵΪ 0 ʱ����û��д��ʱ
	 */
	void write_wait(int timeout = 0);

	/**
	 * ��ֹ�첽�����첽д״̬���򽫸��첽�����첽����ļ��
	 * �¼����Ƴ���ֱ���û������κ�һ��д����ʱ���Զ�����
	 * ��д״̬(��ʱ���������±��첽������)
	 */
	void disable_write();
protected:
	virtual ~aio_ostream();

	/**
	 * �ͷŶ�̬�������麯��
	 */
	virtual void destroy();

	/**
	 * hook д����
	 */
	void hook_write();

private:
	friend class aio_timer_writer;
	std::list<aio_timer_writer*> timer_writers_;
	std::list<AIO_CALLBACK*> write_callbacks_;
	bool write_hooked_;

	static int write_callback(ACL_ASTREAM*, void*);
	static int write_wakup(ACL_ASTREAM*, void*);
};

}  // namespace acl
