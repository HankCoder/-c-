#pragma once
#include "IConnection.h"

/**
 * ��������Ӷ���
 */
class ServerConnection : public IConnection
{
public:
	ServerConnection(acl::aio_socket_stream* conn);
	~ServerConnection() {}

	/**
	 * ���õ�ǰ��������ӵĸ���
	 * @param nconns {unsigned int}
	 */
	void set_nconns(unsigned int nconns);

	/**
	 * ��õ�ǰ��������ӵĸ���
	 * @return {unsigned int}
	 */
	unsigned int get_nconns() const
	{
		return nconns_;
	}

	/**
	 * ��ǰ��������Ӹ����� 1
	 */
	void inc_nconns();

	/**
	 * �رշ�������ӣ������ӹر�ʱ�ᴥ�� ServiceIOCallback �е�
	 * close_callback ���̣�ͬʱ�� ServiceIOCallback �������������
	 * �л�ɾ������˱���������Ӷ���
	 */
	void close();

protected:
	void run();

private:
	unsigned int nconns_;
};
