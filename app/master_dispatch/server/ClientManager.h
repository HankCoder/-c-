#pragma once
#include <vector>

class ClientConnection;

/**
 * �����࣬��������ͻ������Ӷ���
 */
class ClientManager : public acl::singleton<ClientManager>
{
public:
	ClientManager() {}
	~ClientManager() {}

	/**
	 * ��ӿͻ������Ӷ��󣬲����ظ������ͬ�����Ӷ���
	 * �����ڲ�ֱ�� fatal
	 * @param conn {ClientConnection*} �ǿն���
	 */
	void set(ClientConnection* conn);

	/**
	 * ɾ���ͻ��˶���
	 * @param conn {ClientConnection*} �ǿն���
	 */
	void del(ClientConnection* conn);

	/**
	 * �����Ӷ��󼯺��е���һ�����Ӷ��󣬲��Ӽ�����ɾ��
	 * @return {ClientConnection*} ������ؿգ���˵��û�����Ӷ���
	 */
	ClientConnection* pop();

	size_t length() const
	{
		return conns_.size();
	}

private:
	// �洢�ͻ������Ӷ�������鼯��
	std::vector<ClientConnection*> conns_;
};
