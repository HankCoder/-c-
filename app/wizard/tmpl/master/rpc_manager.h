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
	 * ������ʼ������
	 * @param handle {acl::aio_handle*} �첽������������ֵΪ��ʱ��
	 *  �ڲ����Զ�����һ��
	 * @param max_threads {int} ���̳߳ص�����߳�����
	 * @param type {acl::aio_handle_type} ����Ҫ�ڲ��Զ������첽����ʱ��
	 *  ��ֵ�涨���ڲ����������첽��������ͣ��� handle Ϊ��ʱ�ò���
	 *  û������
	 */
	void init(acl::aio_handle* handle, int max_threads = 10,
		acl::aio_handle_type type = acl::ENGINE_SELECT,
		const char* addr = NULL);

	/**
	 * ����һ���������̣����ù��̽������̴߳���
	 * @param req {acl::rpc_request*} �����������
	 */
	void fork(acl::rpc_request* req);

private:
	// �첽��Ϣ���
	acl::aio_handle* handle_;
	bool internal_handle_;
	// �첽 RPC ͨ�ŷ�����
	acl::rpc_service* service_;
};
