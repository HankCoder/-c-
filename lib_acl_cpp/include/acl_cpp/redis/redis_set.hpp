#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;

class ACL_CPP_API redis_set : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	redis_set();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	redis_set(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_cluster*�� size_t)
	 */
	redis_set(redis_cluster* cluster, size_t max_conns);

	virtual ~redis_set();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ��һ������ member Ԫ�ؼ��뵽���� key ���У��Ѿ������ڼ��ϵ� member Ԫ��
	 * ��������;
	 * 1) ���� key �����ڣ��򴴽�һ��ֻ���� member Ԫ������Ա�ļ���
	 * 2) �� key ���Ǽ�������ʱ������һ������
	 * @param key {const char*} ���϶���ļ�
	 * @param first_member {const char*} ��һ���� NULL �ĳ�Ա
	 * @return {int} ����ӵ������е���Ԫ�ص������������������Ե�Ԫ��
	 */
	int sadd(const char* key, const char* first_member, ...);
	int sadd(const char* key, const std::vector<const char*>& memsbers);
	int sadd(const char* key, const std::vector<string>& members);
	int sadd(const char* key, const char* argv[], size_t argc);
	int sadd(const char* key, const char* argv[], const size_t lens[],
		size_t argc);

	/**
	 * �Ӽ��϶���������Ƴ�������ĳ����Ա
	 * @param key {const char*} ���϶���ļ�
	 * @param buf {string&} �洢���Ƴ��ĳ�Ա
	 * @return {bool} �� key �����ڻ� key �ǿռ�ʱ���� false
	 */
	bool spop(const char* key, string& buf);

	/**
	 * ��ü��϶����г�Ա������
	 * @param key {const char*} ���϶���ļ�
	 * @return {int} ���ظü��϶����г�Ա�������������£�
	 *  -1�������Ǽ��϶���
	 *   0����Ա����Ϊ�ջ�� key ������
	 *  >0����Ա�����ǿ�
	 */
	int scard(const char* key);

	/**
	 * ���ؼ��� key �е����г�Ա
	 * @param key {const char*} ���϶���ļ�ֵ
	 * @param members {std::vector<string>*} �ǿ�ʱ�洢�����
	 * @return {int} ��������������� -1 ��ʾ�������һ�� key �Ǽ��϶���
	 *  �����ɹ������ͨ��������һ��ʽ�������
	 *  1�����෽�� get_value ���ָ���±��Ԫ������
	 *  2�����෽�� get_child ���ָ���±��Ԫ�ض���(redis_result����Ȼ����ͨ��
	 *     redis_result::argv_to_string �������Ԫ������
	 *  3�����෽�� get_result ����ȡ���ܽ�������� redis_result��Ȼ����ͨ��
	 *     redis_result::get_child ���һ��Ԫ�ض���Ȼ����ͨ����ʽ 2 ��ָ��
	 *     �ķ�����ø�Ԫ�ص�����
	 *  4�����෽�� get_children ��ý��Ԫ�����������ͨ�� redis_result ��
	 *     �ķ��� argv_to_string ��ÿһ��Ԫ�ض����л��Ԫ������
	 *  5���ڵ��÷����д���ǿյĴ洢�������ĵ�ַ
	 */
	int smembers(const char* key, std::vector<string>* members);

	/**
	 * �� member Ԫ�ش� src �����ƶ��� dst ����
	 * @param src {const char*} Դ���϶���ļ�ֵ
	 * @param dst {const char*} Ŀ�꼯�϶���ļ�ֵ
	 * @param member {const char*} Դ���϶���ĳ�Ա
	 * @return {int} ����ֵ�������£�
	 *  -1�������Դ/Ŀ�������һ���Ǽ��϶���
	 *   0��Դ���󲻴��ڻ��Ա��Դ�����в�����
	 *   1���ɹ���Դ�����н�һ����Ա�ƶ���Ŀ�������
	 */
	int smove(const char* src, const char* dst, const char* member);
	int smove(const char* src, const char* dst, const string& member);
	int smove(const char* src, const char* dst,
		const char* member, size_t len);

	/**
	 * ����һ�����ϵ�ȫ����Ա���ü��������и�������֮��Ĳ
	 * @param members {std::vector<string>*} �ǿ�ʱ�洢�����
	 * @param first_key {const char*} ��һ���ǿյļ��϶��� key
	 * @return {int} ��������������� -1 ��ʾ�������һ�� key �Ǽ��϶���
	 *  �����ɹ������ͨ��������һ��ʽ�������
	 *  1�����෽�� get_value ���ָ���±��Ԫ������
	 *  2�����෽�� get_child ���ָ���±��Ԫ�ض���(redis_result����Ȼ����ͨ��
	 *     redis_result::argv_to_string �������Ԫ������
	 *  3�����෽�� get_result ����ȡ���ܽ�������� redis_result��Ȼ����ͨ��
	 *     redis_result::get_child ���һ��Ԫ�ض���Ȼ����ͨ����ʽ 2 ��ָ��
	 *     �ķ�����ø�Ԫ�ص�����
	 *  4�����෽�� get_children ��ý��Ԫ�����������ͨ�� redis_result ��
	 *     �ķ��� argv_to_string ��ÿһ��Ԫ�ض����л��Ԫ������
	 *  5���ڵ��÷����д���ǿյĴ洢�������ĵ�ַ
	 */
	int sdiff(std::vector<string>* members, const char* first_key, ...);
	int sdiff(const std::vector<const char*>& keys,
		std::vector<string>* members);
	int sdiff(const std::vector<string>& keys,
		std::vector<string>* members);

	/**
	 * ����һ�����ϵ�ȫ����Ա���ü��������и������ϵĽ���
	 * @param members {std::vector<string>*} �ǿ�ʱ�洢�����
	 * @param first_key {const char*} ��һ�����϶��� key����NULL��
	 * @return {int} ��������������� -1 ��ʾ�������һ�� key �Ǽ��϶���
	 */
	int sinter(std::vector<string>* members, const char* first_key, ...);
	int sinter(const std::vector<const char*>& keys,
		std::vector<string>* members);
	int sinter(const std::vector<string>& keys,
		std::vector<string>* members);

	/**
	 * ����һ�����ϵ�ȫ����Ա���ü��������и������ϵĲ���
	 * @param members {std::vector<string>*} �ǿ�ʱ�洢�����
	 * @param first_key {const char*} ��һ�����϶��� key����NULL��
	 * @return {int} ��������������� -1 ��ʾ�������һ�� key �Ǽ��϶���
	 */
	int sunion(std::vector<string>* members, const char* first_key, ...);
	int sunion(const std::vector<const char*>& keys,
		std::vector<string>* members);
	int sunion(const std::vector<string>& keys,
		std::vector<string>* members);

	/**
	 * �����������ú� SDIFF ���ƣ�������������浽 dst ���ϣ������Ǽ򵥵ط��ؽ����
	 * @param dst {const char*} Ŀ�꼯�϶����ֵ
	 * @param first_key {const char*} ��һ���ǿյļ��϶����ֵ
	 * @return {int} ������еĳ�Ա����
	 */
	int sdiffstore(const char* dst, const char* first_key, ...);
	int sdiffstore(const char* dst, const std::vector<const char*>& keys);
	int sdiffstore(const char* dst, const std::vector<string>& keys);

	/**
	 * ������������� SINTER ���������������浽 dst ���ϣ������Ǽ򵥵ط��ؽ����
	 * @param dst {const char*} Ŀ�꼯�϶����ֵ
	 * @param first_key {const char*} ��һ���ǿյļ��϶����ֵ
	 * @return {int} ������еĳ�Ա����
	 */
	int sinterstore(const char* dst, const char* first_key, ...);
	int sinterstore(const char* dst, const std::vector<const char*>& keys);
	int sinterstore(const char* dst, const std::vector<string>& keys);

	/**
	 * ������������� SUNION ���������������浽 dst ���ϣ������Ǽ򵥵ط��ؽ����
	 * @param dst {const char*} Ŀ�꼯�϶����ֵ
	 * @param first_key {const char*} ��һ���ǿյļ��϶����ֵ
	 * @return {int} ������еĳ�Ա����
	 */
	int sunionstore(const char* dst, const char* first_key, ...);
	int sunionstore(const char* dst, const std::vector<const char*>& keys);
	int sunionstore(const char* dst, const std::vector<string>& keys);

	/**
	 * �ж� member Ԫ���Ƿ񼯺� key �ĳ�Ա
	 * @param key {const char*} ���϶���ļ�ֵ
	 * @param member {const char*} ���϶����е�һ����ԱԪ��
	 * @return {bool} ���� true ��ʾ�ǣ������������Ϊ���ǻ������ key ����
	 *  �Ǽ��϶���
	 */
	bool sismember(const char* key, const char* member);
	bool sismember(const char* key, const char* member, size_t len);

	/**
	 * �������ִ��ʱ��ֻ�ṩ�� key ��������ô���ؼ����е�һ�����Ԫ�أ������ͬʱָ��
	 * ��Ԫ�ظ�������᷵��һ���������ø������ƵĽ����
	 * @param key {const char*} ���϶���ļ�ֵ
	 * @param out �洢���������
	 * @return {int} ����ĸ�����Ϊ -1 ��ʾ����0 ��ʾû�г�Ա
	 */
	int srandmember(const char* key, string& out);
	int srandmember(const char* key, size_t n, std::vector<string>& out);

	/**
	 * �Ƴ����� key �е�һ������ member Ԫ�أ������ڵ� member Ԫ�ػᱻ����
	 * @param key {const char*} ���϶���ļ�ֵ
	 * @param first_member {const char*} ��Ҫ���Ƴ��ĳ�Ա�б�ĵ�һ���� NULL��Ա��
	 *  �ڱ�ε���������Ҫ�����һ�����д NULL
	 * @retur {int} ���Ƴ��ĳ�ԱԪ�صĸ������������Ǽ��϶���ʱ���� -1���� key ��
	 *  ���ڻ��Ա������ʱ���� 0
	 */
	int srem(const char* key, const char* first_member, ...);
	int srem(const char* key, const std::vector<string>& members);
	int srem(const char* key, const std::vector<const char*>& members);
	int srem(const char* key, const char* members[],
		size_t lens[], size_t argc);

	/**
	 * �������ڵ�����ǰ���ݿ��е����ݿ��
	 * @param key {const char*} ��ϣ��ֵ
	 * @param cursor {int} �α�ֵ����ʼ����ʱ��ֵд 0
	 * @param out {std::vector<string>&} �洢��������ڲ���׷�ӷ�ʽ�����α���
	 *  ���������ӽ��������У�Ϊ��ֹ���ܽ���������¸�����������û����ڵ��ñ�
	 *  ����ǰ��������������
	 * @param pattern {const char*} ƥ��ģʽ��glob ��񣬷ǿ�ʱ��Ч
	 * @param count {const size_t*} �޶��Ľ�����������ǿ�ָ��ʱ��Ч
	 * @return {int} ��һ���α�λ�ã��������£�
	 *   0����������
	 *  -1: ����
	 *  >0: �α����һ��λ��
	 */
	int sscan(const char* key, int cursor, std::vector<string>& out,
		const char* pattern = NULL, const size_t* count = NULL);
};

} // namespace acl
