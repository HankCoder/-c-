#pragma once

/**
 * ��������������񽻸����̴߳���Ϊ�˷���ʹ�ã�����������Ϊ������
 */
class rpc_manager : public acl::singleton<rpc_manager>
{
public:
	rpc_manager();
	~rpc_manager();

	/**
	 * ��ʼ������
	 * @param handle {acl::aio_handle*} �첽������
	 * @param max_threads {int} ���̳߳ص�����߳�����
	 * @param rpc_addr {const char*} RPC ͨ��������ַ
	 */
	void init(acl::aio_handle*, int max_threads = 10,
		const char* rpc_addr = NULL);

	void finish();

	/**
	 * ����һ���������̣����ù��̽������̴߳���
	 * @param req {acl::rpc_request*} �����������
	 */
	void fork(acl::rpc_request* req);
private:
	// �첽��Ϣ���
	acl::aio_handle* handle_;
	// �첽 RPC ͨ�ŷ�����
	acl::rpc_service* service_;
};
