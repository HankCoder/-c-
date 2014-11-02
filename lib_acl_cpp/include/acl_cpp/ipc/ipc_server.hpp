#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/aio_listen_stream.hpp"

namespace acl {

class aio_handle;
class aio_listen_stream;

/**
 * �첽��Ϣ����ˣ�������
 */
class ACL_CPP_API ipc_server : private aio_accept_callback
{
public:
	ipc_server();

	virtual ~ipc_server();

	/**
	 * ���첽����������
	 * @param handle {aio_handle*} �첽���������ǿ�
	 * @param addr {const char*} ������ַ
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool open(aio_handle* handle, const char* addr = "127.0.0.1:0");

	/**
	 * �� open �ɹ���ͨ���˺�����ü�����ַ
	 * @return {const char*} ������ַ����ʽΪ�� IP:PORT
	 */
	const char* get_addr() const;

	/**
	 * ����첽�����
	 * @return {aio_listen_stream*}
	 */
	aio_listen_stream* get_stream() const;

	/**
	 * ����첽������
	 */
	aio_handle& get_handle() const;

protected:
	/**
	 * ���������ɹ��򿪺�Ļص�����
	 * @param addr {const char*} ʵ�ʵļ�����ַ����ʽ��IP:PORT
	 */
	virtual void on_open(const char*addr)
	{
		(void) addr;
	}

	/**
	 * ���������ر�ʱ�Ļص�����
	 */
	virtual void on_close() {}

	/**
	 * ���첽���������һ���ͻ������Ӻ�Ļص�����
	 * @param client {aio_socket_stream*} �ͻ��� IPC ��
	 */
	virtual void on_accept(aio_socket_stream* client)
	{
		(void) client;
	}

#ifdef WIN32
	/**
	 * ���ڻ��� WIN32 ������Ϣ������������� open ʱ�����ڲ�
	 * ���Զ����� create_windows ����
	 */
	virtual bool create_window()
	{
		return false;
	}
#endif

private:
	aio_handle* handle_;
	aio_listen_stream* sstream_;

	/**
	 * �����麯�������������ӵ������ô˻ص�����
	 * @param client {aio_socket_stream*} �첽�ͻ�����
	 * @return {bool} ���� true ��֪ͨ��������������
	 */
	virtual bool accept_callback(aio_socket_stream* client);

	/**
	 * �����麯�������������ر�ʱ�Ļص�����
	 */
	virtual void close_callback();

	/**
	 * �����麯��������������ʱ�Ļص�����
	 */
	virtual bool timeout_callback();
};

}  // namespace acl
