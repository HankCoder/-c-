#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;

/**
 * redis Connection �࣬�����������£�
 * AUTH��ECHO��PING��QUIT��SELECT
 */
class ACL_CPP_API redis_connection : virtual public redis_command
{
public:
	redis_connection();
	redis_connection(redis_client* conn);
	redis_connection(redis_cluster* cluster, size_t max_conns);
	virtual ~redis_connection();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ������ redis-server ʱ���������֤
	 * @param passwd {const char*} �� redis �����ļ���ָ������֤����
	 * @return {bool} �����֤�Ƿ�ɹ������� false ��ʾ��֤ʧ�ܻ����ʧ��
	 */
	bool auth(const char* passwd);

	/**
	 * ѡ�� redis-server �е����ݿ� ID
	 * @param dbnum {int} redis ���ݿ� ID
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool select(int dbnum);

	/**
	 * ̽�� redis �����Ƿ�����
	 * @return {bool} �����Ƿ�����
	 */
	bool ping();

	/**
	 * ����������� redis-server ���Ը����ַ���
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool echo(const char* s);

	/**
	 * �ر� redis ����
	 * @return {bool}
	 */
	bool quit();
};

} // namespace acl
