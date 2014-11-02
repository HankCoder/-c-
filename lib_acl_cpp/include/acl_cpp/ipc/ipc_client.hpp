#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#include "acl_cpp/stream/aio_socket_stream.hpp"

namespace acl {

typedef struct MSG_HDR 
{
	int nMsg;
	int dlen;
#ifdef WIN32
	__int64   magic;
#else
	long long int magic;
#endif
} MSG_HDR;

typedef enum
{
	IO_WAIT_HDR,
	IO_WAIT_DAT
} io_status;

class aio_handle;
class ipc_adapter;
class aio_socket_stream;
class socket_stream;

/**
 * �첽IP��Ϣ��
 */
class ACL_CPP_API ipc_client : private aio_open_callback
{
public:
#ifdef WIN32
	ipc_client(__int64 magic = -1);
#else
	ipc_client(long long int magic = -1);
#endif
	virtual ~ipc_client();

	/**
	 * ֱ�����ٽӿڣ�����������ظýӿ�
	 */
	virtual void destroy()
	{
		delete this;
	}

	/**
	 * ������ open ����������Ϣ�������ɹ�ʱ���ô˺���
	 */
	virtual void on_open() {}

	/**
	 * ���첽���ر�ʱ�Ļص��ӿ�
	 */
	virtual void on_close() {}

	/**
	 * ���յ���Ϣʱ�Ļص��������������ʵ�ָýӿ�
	 * @param nMsg {int} �û���ӵ��Զ�����Ϣֵ
	 * @param data {void*} ��Ϣ����
	 * @param dlen {int} ��Ϣ���ݵĳ���
	 */
	virtual void on_message(int nMsg, void* data, int dlen);

	/**
	 * ����Ϣ������֮�佨�����Ӳ������첽��
	 * @param handle {aio_handle*} �첽������
	 * @param addr {const char*} ��Ϣ������������ַ����ʽΪ:
	 *  IP:PORT(֧��WIN32/UNIX)��unix_path (��֧��UNIX)
	 * @param timeout {int} ���ӳ�ʱʱ��
	 */
	bool open(aio_handle* handle, const char* addr, int timeout);

	/**
	 * �첽���Ѿ����������ô˺������ ipc_client ���ӹ���
	 * @param client {aio_socket_stream*} �첽������
	 */
	void open(aio_socket_stream* client);

	/**
	 * ����Ϣ������֮�佨�����Ӳ�����ͬ����
	 * @param addr {const char*} ��Ϣ������������ַ����ʽΪ:
	 *  IP:PORT(֧��WIN32/UNIX)��unix_path (��֧��UNIX)
	 * @param timeout {int} ���ӳ�ʱʱ��
	 */
	bool open(const char* addr, int timeout);

	/**
	 * ͬ�����Ѿ����������ô˺������ ipc_client ���ӹ���
	 * @param client {socket_stream*} �첽������
	 */
	void open(socket_stream* client);

	/**
	 * ��Ϣ���Ѿ����������ô˺����� IPC ͨ��
	 */
	void wait();

	/**
	 * �����ر���Ϣ��
	 */
	void close();

	/**
	 * �������Ƿ���������
	 * @return {bool}
	 */
	bool active() const;

	/**
	 * ���ָ����Ϣ�Ļص����̶���
	 * @param nMsg {int} ��Ϣ��
	 */
	void append_message(int nMsg);

	/**
	 * ɾ��ָ����Ϣ�Ļص����̶���
	 * @param nMsg {int} ��Ϣ��
	 */
	void delete_message(int nMsg);

	/**
	 * ������Ϣ
	 * @param nMsg {int} ��Ϣ��
	 * @param data {const void*} ����
	 * @param dlen {int} ���ݳ���
	 */
	void send_message(int nMsg, const void* data, int dlen);

	/**
	 * ����첽�����
	 * @return {aio_socket_stream*}
	 */
	aio_socket_stream* get_async_stream() const;

	/**
	 * ����첽������
	 */
	aio_handle& get_handle() const;

	/**
	 * ���ͬ��������
	 * @return {socket_stream*}
	 */
	socket_stream* get_sync_stream() const;
protected:
	/**
	 * ������Ϣ����
	 * @param nMsg {int} ��ϢID
	 * @param data {void*} ���յ�����Ϣ���ݵ�ַ
	 * @param dlen {int} ���յ�����Ϣ���ݳ���
	 */
	void trigger(int nMsg, void* data, int dlen);
private:
#ifdef WIN32
	__int64   magic_;
#else
	long long int magic_;
#endif
	char* addr_;
	std::list<int> messages_;
	aio_handle* handle_;
	aio_socket_stream* async_stream_;
	socket_stream* sync_stream_;
	socket_stream* sync_stream_inner_;
	bool closing_;

	io_status status_;
	MSG_HDR hdr_;

	// �����麯��

	virtual bool read_callback(char* data, int len);
	virtual bool write_callback();
	virtual void close_callback();
	virtual bool timeout_callback();
	virtual bool open_callback();
};

}  // namespace acl
