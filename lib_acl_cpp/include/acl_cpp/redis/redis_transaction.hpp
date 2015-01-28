#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;
class redis_result;

class ACL_CPP_API redis_transaction : public redis_command
{
public:
	redis_transaction(redis_client* conn = NULL);
	~redis_transaction();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ����һ��(����) key �����������ִ��֮ǰ���(����Щ) key �������������Ķ���
	 * ��ô���񽫱����
	 * @param keys {const std::vector<string>&} key ����
	 * @return {bool} �����Ƿ�ɹ�����ʹ key �����е��� key ������Ҳ�᷵�سɹ�
	 */
	bool watch(const std::vector<string>& keys);

	/**
	 * ȡ�� WATCH ��������� key �ļ���
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool unwatch();

	/**
	 * ���һ�������Ŀ�ʼ��������ڵĶ�������ᰴ���Ⱥ�˳�򱻷Ž�һ�����е��У�
	 * ����� EXEC ����ԭ����(atomic)��ִ��
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool multi();

	/**
	 * ִ������������ڵ��������ĳ��(��ĳЩ) key ������ WATCH ����ļ���֮�£�
	 * ����������к����(����Щ) key ��ص������ô EXEC ����ֻ�����(����Щ)
	 * key û�б������������Ķ��������ִ�в���Ч����������񱻴��(abort)��
	 * ��ִ�б�������ɹ��󣬿��Ե�������� get_size()/get_child() ���ÿ�������
	 * �������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool exec();

	/**
	 * ȡ�����񣬷���ִ��������ڵ���������������ʹ�� WATCH �������ĳ��(��ĳЩ)
	 * key����ôȡ�����м��ӣ���ͬ��ִ������ UNWATCH
	 */
	bool discard();

	/**
	 * �� multi �� exec ֮��ִ�ж��� redis �ͻ�������
	 * @param cmd {const char*} redis ����
	 * @param argv {const char* []} ��������
	 * @param lens [const size_t []} �����ĳ�������
	 * @param argc {size_t} ��������ĳ���
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool queue_cmd(const char* cmd, const char* argv[],
		const size_t lens[], size_t argc);

	/**
	 * �ڳɹ����� exec ����ñ�������ò����������ĳ���
	 * @return {size_t}
	 */
	size_t get_size() const;

	/**
	 * ��ȡָ���±�Ķ�Ӧ�������ִ�н������
	 * @param i {size_t} ����ִ�н���ڽ�������е��±�
	 * @param cmd {string*} �ò����ǿ�ʱ��Ŷ�Ӧ�� redis ����
	 * @return {const redis_result*} ִ�е�ĳ������Ľ������
	 *  �� i Խ��ʱ���� NULL
	 */
	const redis_result* get_child(size_t i, string* cmd) const;

private:
	std::vector<string> cmds_;
};

} // namespace acl
