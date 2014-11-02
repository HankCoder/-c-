#pragma once

/**
 * ��������������񽻸����̴߳���Ϊ�˷���ʹ�ã�����������Ϊ������
 */
class rpc_manager : public acl::singleton<rpc_manager>
{
public:
	/**
	 * ���캯��
	 * @param max_threads {int} ���̳߳ص�����߳�����
	 */
	rpc_manager(int max_threads = 10);
	~rpc_manager();

	/**
	 * ����һ���������̣����ù��̽������̴߳���
	 * @param req {acl::rpc_request*} �����������
	 */
	void fork(acl::rpc_request* req);
protected:
private:
	// �첽��Ϣ���
	acl::aio_handle* handle_;
	// �첽 RPC ͨ�ŷ�����
	acl::rpc_service* service_;
};
