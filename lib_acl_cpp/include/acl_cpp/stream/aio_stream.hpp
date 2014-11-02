#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#ifdef WIN32
#include <WinSock2.h>
#endif

struct ACL_ASTREAM;
struct ACL_VSTREAM;

namespace acl
{

/**
 * �첽���ص���
 */
class ACL_CPP_API aio_callback
{
public:
	aio_callback(void) {}
	virtual ~aio_callback(void) {};

	virtual void close_callback() {}
	virtual bool timeout_callback()
	{
		return false;
	}

	/**
	 * ���ص��麯�����ûص��������������� aio_istream ʵ���е�
	 * gets/read �Ŀɶ������󱻵��ã����첽����ڲ���������������
	 * �ݶ�����ֱ�Ӵ��ݸ��û�������
	 * @param data {char*} ���������ݵ�ָ���ַ
	 * @param len {int} ���������ݳ���(> 0)
	 * @return {bool} �ú������� false ֪ͨ�첽����رո��첽��
	 */
	virtual bool read_callback(char* data, int len)
	{
		(void) data;
		(void) len;
		return true;
	}

	/**
	 * ���ص��麯�����ûص��������������� aio_istream ʵ���е�
	 * read_wait �Ŀɶ��������첽���������ݿɶ�ʱ�����ã�����ʱʱ��
	 * ���� timeout_callback�����쳣���ر�ʱ����� close_callback
	 */
	virtual bool read_wakeup()
	{
		return true;
	}

	/**
	 * д�ɹ���Ļص��麯��
	 * @return {bool} �ú������� false ֪ͨ�첽����رո��첽��
	 */
	virtual bool write_callback()
	{
		return true;
	}

	/**
	 * ���ص��麯�����ûص��������������� aio_ostream ʵ���е�
	 * write_wait �Ŀ�д�������첽����дʱ�����ã�����ʱʱ��
	 * ���� timeout_callback�����쳣���ر�ʱ����� close_callback
	 */
	virtual bool write_wakeup()
	{
		return true;
	}
protected:
private:
};

struct AIO_CALLBACK 
{
	aio_callback* callback;
	bool enable;
};

class aio_handle;

/**
 * �첽�����࣬����Ϊ�����࣬���ܱ�ֱ��ʵ������ֻ�ܱ�����̳�ʹ��
 * ����ֻ���ڶ��Ϸ��䣬������ջ�Ϸ���
 */
class ACL_CPP_API aio_stream
{
public:
	/**
	 * ���캯��
	 * @param handle {aio_handle*}
	 */
	aio_stream(aio_handle* handle);

	/**
	 * �ر��첽��
	 */
	void close();

	/**
	 * ��ӹر�ʱ�Ļص������ָ�룬����ûص�������Ѿ����ڣ���ֻ��
	 * ʹ�ö����ڴ򿪿���״̬
	 * @param callback {aio_callback*} �̳� aio_callback ������ص������
	 *  ���첽���ر�ǰ���ȵ��ô˻ص�������е� close_callback �ӿ�
	 */
	void add_close_callback(aio_callback* callback);

	/**
	 * ��ӳ�ʱʱ�Ļص������ָ�룬����ûص�������Ѿ����ڣ���ֻ��
	 * ʹ�ö����ڴ򿪿���״̬
	 * @param callback {aio_callback*} �̳� aio_callback ������ص������
	 *  ���첽���ر�ǰ���ȵ��ô˻ص�������е� timeout_callback �ӿ�
	 */
	void add_timeout_callback(aio_callback* callback);

	/**
	 * ɾ���ر�ʱ�Ļص������ָ��
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ���ɾ�����еĹرջص�����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */
	int del_close_callback(aio_callback* callback = NULL);

	/**
	 * ɾ����ʱʱ�Ļص������ָ��
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ���ɾ�����еĳ�ʱ�ص�����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */
	int del_timeout_callback(aio_callback* callback = NULL);

	/**
	 * ��ֹ�رյĻص�����󣬵������ӹرն��󼯺���ɾ��
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ����ֹ���еĹرջص�����
	 * @return {int} ���ر��ӻص����󼯺��н��õĻص�����ĸ���
	 */
	int disable_close_callback(aio_callback* callback = NULL);

	/**
	 * ��ֹ��ʱ�Ļص�����󣬵������ӳ�ʱ���󼯺���ɾ��
	 * @param callback {aio_callback*} �� aio_callback �̳е��������ָ�룬
	 *  ����ֵΪ�գ����ֹ���еĳ�ʱ�ص�����
	 * @return {int} ���ر��ӻص����󼯺��н��õĻص�����ĸ���
	 */
	int disable_timeout_callback(aio_callback* callback = NULL);

	/**
	 * �������еĻص����󱻵���
	 * @param callback {aio_callback*} ����ָ���Ļص����������ֵΪ�գ�
	 *  ���������еĹرջص�����
	 * @return {int} ���ر����õĻص�����ĸ���
	 */
	int enable_close_callback(aio_callback* callback = NULL);

	/**
	 * �������еĻص����󱻵���
	 * @param callback {aio_callback*} ����ָ���Ļص����������ֵΪ�գ�
	 *  ���������еĳ�ʱ�ص�����
	 * @return {int} ���ر����õĻص�����ĸ���
	 */
	int enable_timeout_callback(aio_callback* callback = NULL);

	/**
	 * ����첽������ ACL_ASTREAM
	 * @return {ACL_ASTREAM*}
	 */
	ACL_ASTREAM* get_astream() const;

	/**
	 * ����첽�������е�ͬ�������� ACL_VSTREAM
	 * @return {ACL_VSTREAM*}
	 */
	ACL_VSTREAM* get_vstream() const;

	/**
	 * ����첽���е� SOCKET ������
	 * @return {ACL_SOCKET} ���������򷵻� -1(UNIX) �� INVALID_SOCKET(win32)
	 */
#ifdef WIN32
	SOCKET get_socket() const;
	SOCKET sock_handle() const
#else
	int get_socket() const;
	int sock_handle() const
#endif
	{
		return get_socket();
	}

	/**
	 * ���Զ�����ӵĵ�ַ
	 * @param full {bool} �Ƿ���������ַ������IP:PORT������ò���
	 *  Ϊ false��������� IP�����򷵻� IP:PORT
	 * @return {const char*} Զ�����ӵ�ַ��������ֵ == '\0' ���ʾ
	 *  �޷����Զ�����ӵ�ַ
	 */
	const char* get_peer(bool full = false) const;

	/**
	 * ������ӵı��ص�ַ
	 * @param full {bool} �Ƿ���������ַ������IP:PORT������ò���
	 *  Ϊ false��������� IP�����򷵻� IP:PORT
	 * @return {const char*} �����ӵı��ص�ַ��������ֵ == "" ���ʾ
	 *  �޷���ñ��ص�ַ
	 */
	const char* get_local(bool full = false) const;

	/**
	 * ����첽���¼����
	 * @return {aio_handle&}
	 */
	aio_handle& get_handle() const;

	/**
	 * �ж����Ƿ��� hooked ״̬
	 * @return {bool}
	 */
	bool is_hooked() const;

protected:
	aio_handle* handle_;
	ACL_ASTREAM* stream_;

	virtual ~aio_stream();

	/**
	 * ͨ���˺�������̬�ͷ�ֻ���ڶ��Ϸ�����첽�������
	 */
	virtual void destroy();

	/**
	 * ����Ӧ�ڴ����ɹ�����øú���֪ͨ���������첽�������,
	 * ͬʱ hook ���رռ�����ʱʱ�Ļص�����
	 */
	void hook_error();

private:
	bool error_hooked_;
	std::list<AIO_CALLBACK*> close_callbacks_;
	std::list<AIO_CALLBACK*> timeout_callbacks_;

	static int close_callback(ACL_ASTREAM*, void*);
	static int timeout_callback(ACL_ASTREAM*, void*);

private:
	char  dummy_[1];
	char  peer_ip_[33];
	char  local_ip_[33];
	const char* get_ip(const char* addr, char* buf, size_t size);
};

}  // namespace acl
