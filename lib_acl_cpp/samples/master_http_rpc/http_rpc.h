#pragma once

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
	acl::aio_socket_stream* client_;  // �ͻ���������
	bool keep_alive_; // �Ƿ���ͻ��˱��ֳ�����
	char* res_buf_;  // ��ŷ��ظ��ͻ������ݵĻ�����
	unsigned buf_size_; // res_buf_ �Ŀռ��С

	// �����߳�����������ʽ����ͻ�������
	void handle_conn(acl::socket_stream* stream);
};
