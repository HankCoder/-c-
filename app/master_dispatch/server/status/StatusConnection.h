#pragma once
#include "IConnection.h"

/**
 * ��������Ӷ���
 */
class StatusConnection : public IConnection
{
public:
	StatusConnection(acl::aio_socket_stream* conn);
	~StatusConnection() {}

	/**
	 * �رշ�������ӣ������ӹر�ʱ�ᴥ�� ServiceIOCallback �е�
	 * close_callback ���̣�ͬʱ�� ServiceIOCallback �������������
	 * �л�ɾ������˱���������Ӷ���
	 */
	void close();

protected:
	void run();

private:
};
