#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;
class redis_client_cluster;

class ACL_CPP_API redis_hyperloglog : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	redis_hyperloglog();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	redis_hyperloglog(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_client_cluster*�� size_t)
	 */
	redis_hyperloglog(redis_client_cluster* cluster, size_t max_conns);

	virtual ~redis_hyperloglog();

	/**
	 * ������������Ԫ����ӵ�ָ���� HyperLogLog ����
	 * add the specified elements to the specified HyperLogLog
	 * @param key {const char*} ָ�� key ֵ
	 *  the key
	 * @param first_element {const char*} Ԫ�ؼ��ϵĵ�һ��Ԫ��ֵ���ǿ��ַ���
	 *  the first element of the elements list, and the last must be NULL
	 *  in the elements list
	 * @return {int} �����Ƿ�ɹ���ͬʱ�����Ƿ����˱��������ֵ�������£�
	 *  return the follow values:
	 *  1�������ɹ��������ݷ����˱�����������ݻ������ݷ��������
	 *     successful, and the data was varied
	 *  0���޸�������δ�����仯
	 *     nothing was changed after modifying the old data
	 * -1��������Ӧ�� key ����� hyperloglog ����
	 *     error or the keh isn't a hyperloglog type
	 */
	int pfadd(const char* key, const char* first_element, ...);
	int pfadd(const char* key, const std::vector<const char*>& elements);
	int pfadd(const char* key, const std::vector<string>& elements);

	/**
	 * ��ø������б�� HyperLoglog ȥ�غ�Ԫ�صĽ�������
	 * return the approximated cardinality of the set(s) observed by
	 * the hyperloglog at key(s)
	 * @param first_key {const char*} key ���ϵĵ�һ�� key���ǿ��ַ���
	 *  the firs key which must not be NULL of the keys list, and the
	 *  last parameter must be NULL in keys' list
	 * @return {int} ���б����о�ȥ�غ�Ԫ�صĽ�������
	 */
	int pfcount(const char* first_key, ...);
	int pfcount(const std::vector<const char*>& keys);
	int pfcount(const std::vector<string>& keys);

	/**
	 * ����� HyperLogLog �ϲ���merge��Ϊһ�� HyperLogLog �� �ϲ����
	 * HyperLogLog �Ļ����ӽ����������� HyperLogLog �Ŀɼ����ϵĲ���
	 * merge multiple different hyperloglogs into a single one
	 * @param dst {const char*} Ŀ��洢 HyperLogLog ����ļ�ֵ
	 *  the single key as the destination
	 * @param first_src {const char*} Դ���󼯺��е�һ��Դ HyperLogLog ����ļ�
	 *  the first source key which must not be NULL in the sources list,
	 *  and the last one must be NULL showing the end of the list
	 * @return {bool} �����Ƿ�ɹ������� false ���������Ŀ��/Դ�����
	 *  HyperLogLog ����
	 *  true on success, false if the error or the dest/src are not
	 *  hyperloglog
	 */
	bool pfmerge(const char* dst, const char* first_src, ...);
	bool pfmerge(const char* dst, const std::vector<const char*>& keys);
	bool pfmerge(const char* dst, const std::vector<string>& keys);
};

} // namespace acl
