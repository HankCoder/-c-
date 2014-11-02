#pragma once

class ServerConnection;

/**
 * ��������� IO ����Ļص�������
 */
class ServerIOCallback : public acl::aio_callback
{
public:
	ServerIOCallback(ServerConnection* conn);

protected:
	bool read_callback(char* data, int len);
	void close_callback();
	bool timeout_callback();

private:
	ServerConnection* conn_;

	~ServerIOCallback();
};
