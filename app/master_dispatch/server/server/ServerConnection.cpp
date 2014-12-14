#include "stdafx.h"
#include "server/ServerIOCallback.h"
#include "server/ServerConnection.h"

ServerConnection::ServerConnection(acl::aio_socket_stream* conn)
: IConnection(conn)
, conns_(0)
, used_(0)
, pid_(0)
, type_("unknown")
, max_threads_(0)
, curr_threads_(0)
, busy_threads_(0)
, qlen_(0)
{
}

void ServerConnection::run()
{
	// ������������� IO ����Ļص��������
	ServerIOCallback* callback = new ServerIOCallback(this);

	conn_->add_read_callback(callback);
	conn_->add_close_callback(callback);
	conn_->add_timeout_callback(callback);

	// �첽�ӷ���˻�ȡһ������
	conn_->gets();
}

void ServerConnection::close()
{
	conn_->close();
}

ServerConnection& ServerConnection::inc_conns()
{
	conns_++;
	return *this;
}
