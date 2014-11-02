#pragma once

class ClientConnection;

/**
 * ���������ͻ������Ӷ���Ķ�ʱ�����ö�ʱ���ᶨʱ������
 * �鿴�ͻ��˹��������Ƿ���δ����ͻ������Ӳ����д���
 */
class ManagerTimer : public acl::event_timer
{
public:
	ManagerTimer() {}

	/**
	 * ���ٶ�̬����
	 */
	virtual void destroy();

	/**
	 * ��̬�������������ͻ������������ִ��ݸ������
	 * @param client {ClientConnection*} �ǿն���
	 */
	static bool transfer(ClientConnection* client);

protected:
	// �����麯��
	virtual void timer_callback(unsigned int id);

private:
	~ManagerTimer() {}
};
