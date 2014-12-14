#pragma once
#include "IConnection.h"

class ClientConnection : public IConnection
{
public:
	ClientConnection(acl::aio_socket_stream* conn, int ttl);
	~ClientConnection();

	bool expired() const;

protected:
	// ���ി�麯��
	void run();

private:
	long long int expire_;
};
