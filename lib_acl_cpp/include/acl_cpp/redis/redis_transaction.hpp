#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;
class redis_result;

class ACL_CPP_API redis_transaction : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	redis_transaction();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	redis_transaction(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_client_cluster*�� size_t)
	 */
	redis_transaction(redis_client_cluster* cluster, size_t max_conns);

	virtual ~redis_transaction();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ����һ��(����) key �����������ִ��֮ǰ���(����Щ) key �������������Ķ���
	 * ��ô���񽫱����
	 * watch the given keys to determine execution of the MULTI/EXEC
	 * block, before EXEC some of the given keys were changed outer,
	 * the transaction will break
	 * @param keys {const std::vector<string>&} key ����
	 *  the given keys collection
	 * @return {bool} �����Ƿ�ɹ�����ʹ key �����е��� key ������Ҳ�᷵�سɹ�
	 *  if success of this operation
	 */
	bool watch(const std::vector<string>& keys);

	/**
	 * ȡ�� WATCH ��������� key �ļ���
	 * forget about all watched keys
	 * @return {bool} �����Ƿ�ɹ�
	 * if success of this operation
	 */
	bool unwatch();

	/**
	 * ���һ�������Ŀ�ʼ��������ڵĶ�������ᰴ���Ⱥ�˳�򱻷Ž�һ�����е��У�
	 * ����� EXEC ����ԭ����(atomic)��ִ��
	 * mark the start of a transaction block
	 * @return {bool} �����Ƿ�ɹ�
	 *  if success of this operation
	 */
	bool multi();

	/**
	 * ִ������������ڵ��������ĳ��(��ĳЩ) key ������ WATCH ����ļ���֮�£�
	 * ����������к����(����Щ) key ��ص������ô EXEC ����ֻ�����(����Щ)
	 * key û�б������������Ķ��������ִ�в���Ч����������񱻴��(abort)��
	 * ��ִ�б�������ɹ��󣬿��Ե�������� get_size()/get_child() ���ÿ�������
	 * �������
	 * execute all commands issued after MULTI
	 * @return {bool} �����Ƿ�ɹ�
	 *  if success of this operation
	 */
	bool exec();

	/**
	 * ȡ�����񣬷���ִ��������ڵ���������������ʹ�� WATCH �������ĳ��(��ĳЩ)
	 * key����ôȡ�����м��ӣ���ͬ��ִ������ UNWATCH
	 * discard all commands issued after MULTI
	 * @return {bool}
	 */
	bool discard();

	/**
	 * �� multi �� exec ֮��ɶ�ε��ñ�����ִ�ж��� redis �ͻ�������
	 * run one command between MULTI and EXEC
	 * @param cmd {const char*} redis ����
	 *  the command
	 * @param argv {const char* []} ��������
	 *  the args array associate with the command
	 * @param lens [const size_t []} �����ĳ�������
	 *  the length array of the args array
	 * @param argc {size_t} ��������ĳ���
	 *  the length of the array for args
	 * @return {bool} �����Ƿ�ɹ�
	 *  if successful
	 */
	bool run_cmd(const char* cmd, const char* argv[],
		const size_t lens[], size_t argc);

	/**
	 * �� multi �� exec ֮���ε��ñ�����ִ�ж��� redis �ͻ�������
	 * run one command between MULTI and exec, this function can be
	 * called more than once
	 * @param cmd {const char*} redis ����
	 *  the redis command
	 * @param args {const std::vector<string>&} ��������
	 *  the args array for the command
	 * @return {bool} �����Ƿ�ɹ�
	 *  if successful
	 */
	bool run_cmd(const char* cmd, const std::vector<string>& args);

	/**
	 * �ڳɹ����� exec ����ñ�������ò����������ĳ���
	 * get the result array's length after EXEC
	 * @return {size_t}
	 */
	size_t get_size() const;

	/**
	 * ��ȡָ���±�Ķ�Ӧ�������ִ�н������
	 * get the result of the given subscript
	 * @param i {size_t} ����ִ�н���ڽ�������е��±�
	 *  the given subscript
	 * @param cmd {string*} �ò����ǿ�ʱ��Ŷ�Ӧ�� redis ����
	 *  if not NULL, it will store the command of the given subscript
	 * @return {const redis_result*} ִ��ĳ������Ľ������ i Խ��ʱ���� NULL
	 *  return the result of one command, NULL if i was out of bounds
	 */
	const redis_result* get_child(size_t i, string* cmd) const;

	/**
	 * ��õ�ǰ�������ص������
	 * get all the commands issued between MULTI and EXEC
	 * @return {const std::vector<string>&}
	 */
	const std::vector<string>& get_commands() const
	{
		return cmds_;
	}

private:
	std::vector<string> cmds_;
};

} // namespace acl
