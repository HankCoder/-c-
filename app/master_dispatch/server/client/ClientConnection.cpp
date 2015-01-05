#include "stdafx.h"
#include "client/ManagerTimer.h"
#include "client/ClientManager.h"
#include "client/ClientConnection.h"

ClientConnection::ClientConnection(acl::aio_socket_stream* conn, int ttl)
: IConnection(conn)
{
	struct timeval now;

	gettimeofday(&now, NULL);
	expire_ = ((acl_uint64) now.tv_sec + ttl) * 1000000
		+ ((acl_uint64) now.tv_usec);
}

ClientConnection::~ClientConnection()
{
	conn_->close();
}

void ClientConnection::run()
{
	// �����Ƚ��׽���Ϊ����״̬����������ߵ��ö�ʱ�����̷��� -1
	acl_non_blocking(conn_->sock_handle(), ACL_BLOCKING);

	// ���������ַ��͹��̽��ͻ����׽��ִ��������
	if (ManagerTimer::transfer(this) == false)
		// �������������ʧ�ܣ�������������У��ɶ�ʱ��
		// ���д���
		ClientManager::get_instance().set(this);
	else
	{
		// ���ԴӼ�����ɾ��
		ClientManager::get_instance().del(this);
		delete this;
	}
}

bool ClientConnection::expired() const
{
	struct timeval now;

	gettimeofday(&now, NULL);
	long long present = ((acl_uint64) now.tv_sec) * 1000000
		+ ((acl_uint64) now.tv_usec);

	return present >= expire_ ? true : false;
}
