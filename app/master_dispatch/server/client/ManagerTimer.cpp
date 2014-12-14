#include "stdafx.h"
#include "server/ServerManager.h"
#include "server/ServerConnection.h"
#include "client/ClientManager.h"
#include "client/ClientConnection.h"
#include "client/ManagerTimer.h"

void ManagerTimer::destroy()
{
	delete this;
}

bool ManagerTimer::transfer(ClientConnection* client)
{
	ServerConnection* server;
	const char* peer;
	char  buf[256];
	int   ret;

	// �ӷ�������ӹ��������ȡ����������С��һ��
	// ����˶��󣬲��������ͻ������Ӵ��ݸ�����
	// һֱ���ɹ������д��䶼ʧ��Ϊֹ

	while (true)
	{
		server = ServerManager::get_instance().min();
		if (server == NULL)
		{
			logger_error("no server available for client: %s",
				client->get_peer());
			return false;
		}

		peer = client->get_peer();
		if (peer == NULL)
			peer = "unkonwn";
		memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf), "%s", peer);

		// ���ͻ������Ӵ��ݸ�����ˣ����ʧ�ܣ�������һ��
		// ����ˣ�ͬʱ��ʧ�ܵķ���˴ӷ���˹�������ɾ��
		ret = acl_write_fd(server->sock_handle(), buf,
			strlen(buf), client->sock_handle());
		if (ret == -1)
		{
			logger_error("write fd to backend error: %s",
				acl::last_serror());
			ServerManager::get_instance().del(server);
			server->close();
		}
		// ���ͳɹ����ȸ��÷�����̵Ŀͻ�����������1���Ա����µ���
		// ���ӿ����л������������������
		else
			server->inc_conns();

		return true;
	}
}

void ManagerTimer::timer_callback(unsigned int)
{
	ClientConnection* client;

	// �ӿͻ��˹�����󵯳������ӳٴ�����Ŀͻ������Ӷ���
	// �����ݸ�����ˣ��������ʧ�ܣ����ٴ�����ͻ��˹���
	// �������´ζ�ʱ���ٴγ��Դ���

	logger("total client: %d, total server: %d",
		(int) ClientManager::get_instance().length(),
		(int) ServerManager::get_instance().length());

	while (true)
	{
		client = ClientManager::get_instance().pop();
		if (client == NULL)
			break;

		if (transfer(client) == true)
		{
			ClientManager::get_instance().del(client);
			delete client;
			continue;
		}

		// ����ڹ涨��ʱ������Ȼû�з����׼���������ӣ�
		// ��ֱ��ɾ���ö���
		if (client->expired())
		{
			logger_error("no server side, client(%s) expired!",
				client->get_peer());
			delete client;
		}
		else
		{
			logger_warn("set client(%s) into queue",
				client->get_peer());
			ClientManager::get_instance().set(client);
		}
		break;
	}
}
