#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

class socket_stream;

/**
 * ����˼����׽ӿ��࣬���տͻ������ӣ��������ͻ��������Ӷ���
 */
class ACL_CPP_API server_socket
{
public:
	/**
	 * ���캯��
	 * @param backlog {int} �����׽ӿڶ��г���
	 * @param block {bool} ������ģʽ���Ƿ�����ģʽ
	 */
	server_socket(int backlog = 128, bool block = true);
	~server_socket();

	/**
	 * ��ʼ������������˵�ַ
	 * @param addr {const char*} ������������ַ����ʽΪ��
	 *  ip:port���� unix �����£������������׽ӿڣ���ʽΪ��
	 *   /path/xxx
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool open(const char* addr);

	/**
	 * �ر��Ѿ��򿪵ļ����׽ӿ�
	 * @return {bool} �Ƿ������ر�
	 */
	bool close();

	/**
	 * ���տͻ������Ӳ������ͻ���������
	 * @param timeout {int} ������ģʽ�£�����ֵ > 0 ʱ�����ó�ʱ
	 *  ��ʽ���տͻ������ӣ�����ָ��ʱ����δ��ÿͻ������ӣ��򷵻� NULL
	 * @return {socket_stream*} ���ؿձ�ʾ����ʧ��
	 */
	socket_stream* accept(int timeout = 0);

	/**
	 * ��ü����ĵ�ַ
	 * @return {const char*} ����ֵ�ǿ�ָ��
	 */
	const char* get_addr() const
	{
		return addr_;
	}

	/**
	 * ������������������ַ����ñ��������Ի�ü����׽ӿ�
	 * @return {int}
	 */
#ifdef	WIN32
	SOCKET sock_handle() const
#else
	int sock_handle() const
#endif
	{
		return fd_;
	}

	/**
	 * ���ü����׽��ֵ��ӳٽ��չ��ܣ������ͻ���������������ʱ�Ž������ӷ���
	 * ��Ӧ�ã�Ŀǰ�ù��ܽ�֧�� Linux
	 * @param timeout {int} ����ͻ��������ڹ涨��ʱ����δ�������ݣ�
	 *  Ҳ�������ӷ��ظ�Ӧ��
	 */
	void set_tcp_defer_accept(int timeout);

private:
	int   backlog_;
	bool  block_;
	bool  unix_sock_;
	char  addr_[64];

#ifdef WIN32
	SOCKET fd_;
#else
	int   fd_;
#endif
};

} // namespace acl
