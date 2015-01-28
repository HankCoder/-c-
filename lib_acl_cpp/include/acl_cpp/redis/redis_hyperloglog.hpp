#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;

class ACL_CPP_API redis_hyperloglog : public redis_command
{
public:
	redis_hyperloglog(redis_client* conn = NULL);
	~redis_hyperloglog();

	/**
	 * ������������Ԫ����ӵ�ָ���� HyperLogLog ����
	 * @param key {const char*} ָ�� key ֵ
	 * @param first_element {const char*} Ԫ�ؼ��ϵĵ�һ��Ԫ��ֵ���ǿ��ַ���
	 * @return {int} �����Ƿ�ɹ���ͬʱ�����Ƿ����˱��������ֵ�������£�
	 *  1�������ɹ��������ݷ����˱�����������ݻ������ݷ��������
	 *  0���޸�������δ�����仯
	 * -1��������Ӧ�� key ����� hyperloglog ����
	 */
	int pfadd(const char* key, const char* first_element, ...);
	int pfadd(const char* key, const std::vector<const char*>& elements);
	int pfadd(const char* key, const std::vector<string>& elements);

	/**
	 * ��ø������б�� HyperLoglog ȥ�غ�Ԫ�صĽ�������
	 * @param first_key {const char*} key ���ϵĵ�һ�� key���ǿ��ַ���
	 * @return {int} ���б����о�ȥ�غ�Ԫ�صĽ�������
	 */
	int pfcount(const char* first_key, ...);
	int pfcount(const std::vector<const char*>& keys);
	int pfcount(const std::vector<string>& keys);

	/**
	 * ����� HyperLogLog �ϲ���merge��Ϊһ�� HyperLogLog �� �ϲ����
	 * HyperLogLog �Ļ����ӽ����������� HyperLogLog �Ŀɼ����ϵĲ���
	 * @param dst {const char*} Ŀ��洢 HyperLogLog ����ļ�ֵ
	 * @param first_src {const char*} Դ���󼯺��е�һ��Դ HyperLogLog ����ļ�
	 * @return {bool} �����Ƿ�ɹ������� false ���������Ŀ��/Դ�����
	 *  HyperLogLog ����
	 */
	bool pfmerge(const char* dst, const char* first_src, ...);
	bool pfmerge(const char* dst, const std::vector<const char*>& keys);
	bool pfmerge(const char* dst, const std::vector<string>& keys);
};

} // namespace acl
