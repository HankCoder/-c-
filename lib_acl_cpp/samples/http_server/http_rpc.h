#pragma once
#include "acl_cpp/ipc/rpc.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
//class acl::aio_socket_stream;
//class acl::socket_stream;

class http_rpc : public acl::rpc_request
{
public:
	http_rpc(acl::aio_socket_stream* client, unsigned buf_size);
	~http_rpc();

protected:
	// ���̴߳�����
	virtual void rpc_run();

	// ���̴߳�����̣��յ����߳�������ɵ���Ϣ
	virtual void rpc_onover();

private:
	bool proc_quit_;  // �����Ƿ�Ӧ���˳�
	acl::aio_handle& handle_;  // �첽�������
	acl::aio_socket_stream* client_;  // �ͻ���������
	bool keep_alive_; // �Ƿ���ͻ��˱��ֳ�����
	char* res_buf_;  // ��ŷ��ظ��ͻ������ݵĻ�����
	unsigned buf_size_; // res_buf_ �Ŀռ��С

	// �����߳�����������ʽ����ͻ�������
	void handle_conn(acl::socket_stream* stream);
};
