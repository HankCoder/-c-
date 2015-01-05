#pragma once

// �����࣬�������������ڿͻ��˼�����˵�����
class IConnection
{
public:
	IConnection(acl::aio_socket_stream* conn) : conn_(conn) {}
	virtual ~IConnection() {}

	/**
	 * ���麯�����������ʵ��
	 */
	virtual void run() = 0;

	/**
	 * ������Ӷ���� socket ������
	 * @return {int}
	 */
	int   sock_handle() const;

	/**
	 * ��÷���������������
	 * @return {acl::aio_socket_stream*}
	 */
	acl::aio_socket_stream* get_conn() const
	{
		return conn_;
	}

	/**
	 * ������Ӷ���ĵ�ַ
	 * @return {const char*}
	 */
	const char* get_peer(bool full = true) const;

protected:
	acl::aio_socket_stream* conn_;
};
