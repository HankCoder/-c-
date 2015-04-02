#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;

/**
 * redis Connection �࣬�����������£�
 * AUTH��ECHO��PING��QUIT��SELECT
 * redis connection command clss, including as below:
 * AUTH, ECHO, PING, QUIT, SELECT
 */
class ACL_CPP_API redis_connection : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	redis_connection();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	redis_connection(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_client_cluster*�� size_t)
	 */
	redis_connection(redis_client_cluster* cluster, size_t max_conns);

	virtual ~redis_connection();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ������ redis-server ʱ���������֤
	 * AUTH command to login the redis server.
	 * @param passwd {const char*} �� redis �����ļ���ָ������֤����
	 *  the password in redis-server configure
	 * @return {bool} �����֤�Ƿ�ɹ������� false ��ʾ��֤ʧ�ܻ����ʧ��
	 *  return true if success, or false because auth failed or error.
	 */
	bool auth(const char* passwd);

	/**
	 * ѡ�� redis-server �е����ݿ� ID
	 * SELECT command to select the DB id in redis-server
	 * @param dbnum {int} redis ���ݿ� ID
	 *  the DB id
	 * @return {bool} �����Ƿ�ɹ�
	 *  return true if success, or false for failed.
	 */
	bool select(int dbnum);

	/**
	 * ̽�� redis �����Ƿ�����
	 * PING command for testing if the connection is OK
	 * @return {bool} �����Ƿ�����
	 *  return true if success
	 */
	bool ping();

	/**
	 * ����������� redis-server ���Ը����ַ���
	 * ECHO command, request redis-server to echo something.
	 * @return {bool} �����Ƿ�ɹ�
	 *  return true if success
	 */
	bool echo(const char* s);

	/**
	 * �ر� redis ����
	 * QUIT command to close the redis connection
	 * @return {bool}
	 *  return true if success
	 */
	bool quit();
};

} // namespace acl
