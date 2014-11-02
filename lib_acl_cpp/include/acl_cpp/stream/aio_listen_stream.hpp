#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/aio_stream.hpp"

namespace acl
{

class aio_socket_stream;

/**
 * ���첽���������յ��µĿͻ�����ʱ���ô˻ص����еĻص�������
 * ����Ϊ�����࣬Ҫ���������ʵ�� accept_callback �ص�����
 */
class ACL_CPP_API aio_accept_callback : public aio_callback
{
public:
	aio_accept_callback() {}
	virtual ~aio_accept_callback() {}

	/**
	 * �����յ��µĿͻ�����ʱ�Ļص�����
	 * @param client {aio_socket_stream*} �ͻ����첽��������
	 *  ���ԶԴ������ж�д����
	 * @return {bool} ���ϣ���رո��첽�����������Է��� false��
	 *  һ�㲻Ӧ���� false
	 */
	virtual bool accept_callback(aio_socket_stream* client) = 0;
protected:
private:
};

/**
 * �첽������������������������ڿͻ��˵��������ӣ�ͬʱ����ֻ��
 * �ڶ��Ϸ��䣬������ջ���䣬Ӧ�ÿ��Ե��� close �����ر��������ر�
 * ����첽�������Զ��ͷţ�������� delete ɾ���������
 *
 */
class ACL_CPP_API aio_listen_stream : public aio_stream
{
public:
	/**
	 * ���캯�������Թ����첽������
	 * @param handle {aio_handle*} �첽������
	 */
	aio_listen_stream(aio_handle* handle);

	/**
	 * ����첽���������յ��¿ͻ�����ʱ�Ļص�����
	 * @param callback {aio_accept_callback*}
	 */
	void add_accept_callback(aio_accept_callback* callback);

	/**
	 * ��ʼ����ĳ��ָ����ַ������Ϊ�����׽ӿڣ�Ҳ����Ϊ���׽ӿڣ�
	 * @param addr {const char*} ������ַ��TCP������ַ���������ַ
	 * ��ʽ��
	 *      ���TCP���ӣ�IP:PORT���磺127.0.0.1:9001
	 *      ������׽ӿڣ�{path}���磺/tmp/my.sock
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool open(const char* addr);

	/**
	 * ��÷�����������ַ
	 * @return {const char*}
	 */
	const char* get_addr() const;

	virtual void destroy();
protected:
	virtual ~aio_listen_stream();
private:
	bool accept_hooked_;
	char  addr_[256];
	std::list<aio_accept_callback*> accept_callbacks_;

	void hook_accept();
	static int accept_callback(ACL_ASTREAM*,  void*);
};

}  // namespace acl
