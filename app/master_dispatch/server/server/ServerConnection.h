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
	 * ���õ�ǰ�����ĳ����ʵ�����ӵĸ���
	 * @param conns {unsigned int}
	 * @return {ServerConnection&}
	 */
	ServerConnection& set_conns(unsigned int conns)
	{
		conns_ = conns;
		return *this;
	}

	/**
	 * ����ǰ������ĳ����ʵ���Ŀͻ����������� 1
	 * @return {ServerConnection&}
	 */
	ServerConnection& inc_conns();

	/**
	 * ��õ�ǰ�����ĳ����ʵ�����ӵĸ���
	 * @return {unsigned int}
	 */
	unsigned int get_conns() const
	{
		return conns_;
	}

	/**
	 * ���õ�ǰ�����ĳ����ʵ�����ܹ�������������
	 * @param n {unsigned int}
	 * @return {ServerConnection&}
	 */
	ServerConnection& set_used(unsigned int n)
	{
		used_ = n;
		return *this;
	}

	/**
	 * ��õ�ǰ�����ĳ����ʵ�����ܹ�������������
	 * @return {unsigned int}
	 */
	unsigned int get_used() const
	{
		return used_;
	}

	/**
	 * ���õ�ǰ����˵�ĳ������ʵ���Ľ��̺�
	 * @param pid {pid_t}
	 * @return {ServerConnection&}
	 */
	ServerConnection& set_pid(pid_t pid)
	{
		pid_ = pid;
		return *this;
	}

	/**
	 * ��õ�ǰ����˵�ĳ������ʵ���Ľ��̺�
	 * @return {unsigned int}
	 */
	unsigned int get_pid() const
	{
		return pid_;
	}

	/**
	 * ���õ�ǰ����˵�ĳ������ʵ��������߳���
	 * @param {unsigned int}
	 * @return {ServerConnection&}
	 */
	ServerConnection& set_max_threads(unsigned int n)
	{
		max_threads_ = n;
		return *this;
	}

	/**
	 * ��õ�ǰ����˵�ĳ������ʵ��������߳���
	 * @return {unsigned int}
	 */
	unsigned int get_max_threads() const
	{
		return max_threads_;
	}

	/**
	 * ���õ�ǰ����˵�ĳ������ʵ���ĵ�ǰ�߳���
	 * @param n {unsigned int}
	 * @return {ServerConnection&}
	 */
	ServerConnection& set_curr_threads(unsigned int n)
	{
		curr_threads_ = n;
		return *this;
	}

	/**
	 * ��õ�ǰ����˵�ĳ������ʵ���ĵ�ǰ�߳���
	 * @return {unsigned int}
	 */
	unsigned int get_curr_threads() const
	{
		return curr_threads_;
	}

	/**
	 * ���õ�ǰ����˵�ĳ������ʵ���ķ�æ�߳���
	 * @param n {unsigned int}
	 * @return {ServerConnection&}
	 */
	ServerConnection& set_busy_threads(unsigned int n)
	{
		busy_threads_ = n;
		return *this;
	}

	/**
	 * ��õ�ǰ����˵�ĳ������ʵ���ķ�æ�߳���
	 * @return {unsigned int}
	 */
	unsigned int get_busy_threads() const
	{
		return busy_threads_;
	}

	/**
	 * ���õ�ǰ����˵�ĳ������ʵ�������ѹ����
	 * @param n {unsigned int}
	 * @return {ServerConnection&}
	 */
	ServerConnection& set_qlen(unsigned int n)
	{
		qlen_ = n;
		return *this;
	}

	/**
	 * ��õ�ǰ����˵�ĳ������ʵ�������ѹ����
	 * @return {unsigned int}
	 */
	unsigned int get_qlen() const
	{
		return qlen_;
	}

	/**
	 * ���õ�ǰ����˵�ĳ������ʵ�������ͱ�ʶ��
	 * @param type {const char*}
	 * @return {ServerConnection&}
	 */
	ServerConnection& set_type(const char* type)
	{
		type_ = type;
		return *this;
	}

	/**
	 * ��õ�ǰ����˵�ĳ������ʵ�������ͱ�ʶ��
	 * @return {unsigned int}
	 */
	const acl::string& get_type() const
	{
		return type_;
	}

	/**
	 * �رշ�������ӣ������ӹر�ʱ�ᴥ�� ServiceIOCallback �е�
	 * close_callback ���̣�ͬʱ�� ServiceIOCallback �������������
	 * �л�ɾ������˱���������Ӷ���
	 */
	void close();

protected:
	/**
	 * �����麯��ʵ��
	 * @override
	 */
	void run();

private:
	unsigned int conns_;
	unsigned int used_;
	pid_t pid_;
	acl::string type_;
	unsigned int max_threads_;
	unsigned int curr_threads_;
	unsigned int busy_threads_;
	unsigned int qlen_;
};
