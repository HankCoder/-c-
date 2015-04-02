#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <map>
#include <utility>
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;

class ACL_CPP_API redis_zset : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	redis_zset();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	redis_zset(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_client_cluster*�� size_t)
	 */
	redis_zset(redis_client_cluster* cluster, size_t max_conns);
	virtual ~redis_zset();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ��Ӷ�Ӧ key ������
	 * @param key {const char*} ���򼯼�ֵ
	 * @param members "��ֵ-��Ա" ������
	 * @return {int} �³ɹ���ӵ� "��ֵ-��Ա" �Ե�����
	 *  0����ʾһ��Ҳδ��ӣ�������Ϊ�ó�Ա�Ѿ�������������
	 * -1����ʾ����� key ��������򼯶���
	 * >0������ӵĳ�Ա����
	 */
	int zadd(const char* key, const std::map<string, double>& members);
	int zadd(const char* key,
		const std::vector<std::pair<string, double> >&members);
	int zadd(const char* key,
		const std::vector<std::pair<const char*, double> >&members);
	int zadd(const char* key, const std::vector<string>& members,
		const std::vector<double>& scores);
	int zadd(const char* key, const std::vector<const char*>& members,
		const std::vector<double>& scores);
	int zadd(const char* key, const char* members[], double scores[],
		size_t size);
	int zadd(const char* key, const char* members[], size_t members_len[],
		double scores[], size_t size);

	/**
	 * �����Ӧ�������򼯵ĳ�Ա����
	 * @param key {const char*} ���򼯼�ֵ
	 * @return {int} һ���������򼯵ĳ�Ա����
	 *  0���ü�������
	 *  -1�������ü������ݶ�������Ч�����򼯶���
	 *  > 0����ǰ��ֵ��Ӧ�����ݶ����еĳ�Ա����
	 */
	int zcard(const char* key);

	/**
	 * ��� key ��������ָ����ֵ����ĳ�Ա����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param min {double} ��С��ֵ
	 * @param max {double} ����ֵ
	 * @return {int} ���������ĳ�Ա����
	 *  0���ü���Ӧ�����򼯲����ڻ�� KEY ���򼯵Ķ�Ӧ��ֵ�����ԱΪ��
	 *  -1: �����ü������ݶ�������Ч�����򼯶���
	 */
	int zcount(const char* key, double min, double max);

	/**
	 * �� key �������е�ĳ����Ա�ķ�ֵ�������� inc
	 * @param key {const char*} ���򼯼�ֵ
	 * @param inc {double} ����ֵ
	 * @param member{const char*} �����г�Ա��
	 * @param result {double*} �ǿ�ʱ�洢���ֵ
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool zincrby(const char* key, double inc, const char* member,
		double* result = NULL);
	bool zincrby(const char* key, double inc, const char* member,
		size_t len, double* result = NULL);

	/**
	 * �� key �������л��ָ��λ������ĳ�Ա���б���Ա����ֵ������ʽ����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param start {int} ��ʼ�±�λ��
	 * @param stop {int} �����±�λ�ã������ͬʱ����λ�ã�
	 * @param result {std::vector<string>*} �ǿ�ʱ�洢��������ڲ��ȵ���
	 *  result.clear() ������е�Ԫ��
	 * @return {int} ������г�Ա������
	 *  0: ��ʾ�����Ϊ�ջ� key ������
	 * -1: ��ʾ����� key ��������򼯶���
	 * >0: �����������
	 *  ע�������±�λ�ã�0 ��ʾ��һ����Ա��1 ��ʾ�ڶ�����Ա��-1 ��ʾ���һ����Ա��
	 *     -2 ��ʾ�����ڶ�����Ա���Դ�����
	 *
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
	int zrange(const char* key, int start, int stop,
		std::vector<string>* result);

	/**
	 * �� key �������л��ָ��λ������ĳ�Ա������ֵ�б���Ա����ֵ������ʽ����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param start {int} ��ʼ�±�λ��
	 * @param stop {int} �����±�λ�ã������ͬʱ����λ�ã�
	 * @param result �洢 "��Ա��-��ֵ��"��������ڲ��ȵ��� out.clear()
	 * @return {int} ������г�Ա������
	 *  0: ��ʾ�����Ϊ�ջ� key ������
	 * -1: ��ʾ����� key ��������򼯶���
	 * >0: �����������
	 *  ע�������±�λ�ã�0 ��ʾ��һ����Ա��1 ��ʾ�ڶ�����Ա��-1 ��ʾ���һ����Ա��
	 *     -2 ��ʾ�����ڶ�����Ա���Դ�����
	 */
	int zrange_with_scores(const char* key, int start, int stop,
		std::vector<std::pair<string, double> >& out);

	/**
	 * �������� key �У����� score ֵ���� min �� max ֮��(�������� min �� max )
	 * �ĳ�Ա�����򼯳�Ա�� score ֵ����(��С����)��������
	 * @param key {const char*} ���򼯼�ֵ
	 * @param min {double} ��С��ֵ
	 * @param max {double} ����ֵ
	 * @param out {std::vector<string>*} �ǿ�ʱ�洢����Ա���������
	 * @param offset {const int*} �ǿ�ʱ��ʾ���������ʼ�±�
	 * @param count {const int*} �ǿ�ʱ��ʾ��ȡ�Ľ�����г�Ա����
	 * @return {int} ������г�Ա������
	 *  0: ��ʾ�����Ϊ�ջ� key ������
	 * -1: ��ʾ����� key ��������򼯶���
	 * >0: �����������
	 *  ע��offset �� count ����ͬʱΪ�ǿ�ָ��ʱ����Ч
	 *
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
	int zrangebyscore(const char* key, double min, double max,
		std::vector<string>* out, const int* offset = NULL,
		const int* count = NULL);

	/**
	 * �������� key �У����� score ֵ���� min �� max ֮��(�������� min �� max )
	 * �ĳ�Ա�����򼯳�Ա�� score ֵ����(��С����)��������
	 * @param key {const char*} ���򼯼�ֵ
	 * @param min {const char*} ���ַ�����ʾ��С��ֵ
	 * @param max {const char*} ���ַ�����ʾ����ֵ
	 * @param out {std::vector<string>*} �ǿ�ʱ�洢����Ա���������
	 * @param offset {const int*} �ǿ�ʱ��ʾ���������ʼ�±�
	 * @param count {const int*} �ǿ�ʱ��ʾ��ȡ�Ľ�����г�Ա����
	 * @return {int} ������г�Ա������
	 *  0: ��ʾ�����Ϊ�ջ� key ������
	 * -1: ��ʾ����� key ��������򼯶���
	 * >0: �����������
	 *  ע��
	 * 1��offset �� count ����ͬʱΪ�ǿ�ָ��ʱ����Ч
	 * 2��min �� max ������ -inf �� +inf ����ʾ��������
	 * 3��Ĭ������£������ȡֵʹ�ñ����� (С�ڵ��ڻ���ڵ���)��Ҳ����ͨ��������ǰ
	 *   ���� ( ������ʹ�ÿ�ѡ�Ŀ����� (С�ڻ����)���磺
	 * 3.1��"ZRANGEBYSCORE zset (1 5" �������з������� 1 < score <= 5 �ĳ�Ա
	 * 3.2��"ZRANGEBYSCORE zset (5 (10" �������з������� 5 < score < 10 �ĳ�Ա
	 *
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
	int zrangebyscore(const char* key, const char* min, const char* max,
		std::vector<string>* out, const int* offset = NULL,
		const int* count = NULL);

	/**
	 * �������� key �У����� score ֵ���� min �� max ֮��(�������� min �� max )
	 * �ĳ�Ա����ֵ�����򼯳�Ա�� score ֵ����(��С����)�������У���ֵ(min/max)ʹ��
	 * ��������ʾ
	 * @param out �洢��������ڲ��ȵ��� out.clear()
	 * @return {int} ������г�Ա������
	 */
	int zrangebyscore_with_scores(const char* key, double min, double max,
		std::vector<std::pair<string, double> >& out,
		const int* offset = NULL, const int* count = NULL);

	/**
	 * �������� key �У����� score ֵ���� min �� max ֮��(�������� min �� max )
	 * �ĳ�Ա����ֵ�����򼯳�Ա�� score ֵ����(��С����)�������У���ֵ(min/max)ʹ��
	 * �ַ�����ʾ
	 * @param out �洢��������ڲ��ȵ��� out.clear()
	 * @return {int} ������г�Ա������
	 */
	int zrangebyscore_with_scores(const char* key, const char* min,
		const char* max, std::vector<std::pair<string, double> >& out,
		const int* offset = NULL, const int* count = NULL);

	/**
	 * �������� key �г�Ա member ������(�±�� 0 ��ʼ�����������򼯳�Ա�� score
	 * ֵ����(��С����)˳������
	 * @param key {const char*} ���򼯼�ֵ
	 * @param member {const char*} ��Ա��
	 * @param len {size_t} member �ĳ���
	 * @return {int} �±�λ��ֵ��-1 -- ������ key �����򼯶��󣬻��Ա��������
	 */
	int zrank(const char* key, const char* member, size_t len);
	int zrank(const char* key, const char* member);

	/**
	 * ��������ɾ��ĳ����Ա
	 * @param key {const char*} ���򼯼�ֵ
	 * @param first_member {const char*} Ҫɾ���ĳ�Ա�б�ĵ�һ��
	 * @return {int} �ɹ�ɾ���ĳ�Ա��������-1 ��ʾ������ key �����򼯶���
	 *  0 ��ʾ�����򼯲����ڻ��Ա�����ڣ�> 0 ��ʾ�ɹ�ɾ���ĳ�Ա����
	 */
	int zrem(const char* key, const char* first_member, ...);
	int zrem(const char* key, const std::vector<string>& members);
	int zrem(const char* key, const std::vector<const char*>& members);
	int zrem(const char* key, const char* members[], const size_t lens[],
		size_t argc);

	/**
	 * �Ƴ����� key �У�ָ������(rank)�����ڵ����г�Ա��
	 * ����ֱ����±���� start �� stop ָ�������� start �� stop ���ڣ�
	 * �±���� start �� stop ���� 0 Ϊ�ף�Ҳ����˵���� 0 ��ʾ���򼯵�һ����Ա��
	 * �� 1 ��ʾ���򼯵ڶ�����Ա���Դ����ƣ�
	 * Ҳ����ʹ�ø����±꣬�� -1 ��ʾ���һ����Ա�� -2 ��ʾ�����ڶ�����Ա���Դ�����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param start {int} ��ʼ�±�λ�ã��� 0 ��ʼ��
	 * @param stop {int} �����±�λ��
	 * @return {int} ���Ƴ��ĳ�Ա����
	 *  0����ʾ key �����ڻ��Ƴ������䲻����
	 * -1����ʾ����� key �������򼯺϶����ֵ
	 */
	int zremrangebyrank(const char* key, int start, int stop);

	/**
	 * �Ƴ����� key �У����� score ֵ���� min �� max ֮��(�������� min �� max )
	 * �ĳ�Ա���԰汾2.1.6��ʼ��score ֵ���� min �� max �ĳ�ԱҲ���Բ��������ڣ�
	 * ������μ� ZRANGEBYSCORE ����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param min {double} ��С��ֵ
	 * @param max {double} ����ֵ
	 * @return {int} �ɹ�ɾ���ĳ�Ա��������-1 ��ʾ������ key �����򼯶���
	 *  0 ��ʾ�����򼯲����ڻ��Ա�����ڣ�> 0 ��ʾ�ɹ�ɾ���ĳ�Ա����
	 */
	int zremrangebyscore(const char* key, double min, double max);

	/**
	 * �Ƴ����� key �У����� score ֵ���� min �� max ֮��(�������� min �� max )
	 * �ĳ�Ա���԰汾2.1.6��ʼ��score ֵ���� min �� max �ĳ�ԱҲ���Բ��������ڣ�
	 * ������μ� ZRANGEBYSCORE ����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param min {const char*} �ַ�����ʽ����С��ֵ������μ���zrangebyscore ע��
	 * @param max {const char*} �ַ�����ʽ������ֵ
	 * @return {int} �ɹ�ɾ���ĳ�Ա��������-1 ��ʾ������ key �����򼯶���
	 *  0 ��ʾ�����򼯲����ڻ��Ա�����ڣ�> 0 ��ʾ�ɹ�ɾ���ĳ�Ա����
	 */
	int zremrangebyscore(const char* key, const char* min, const char* max);

	/**
	 * �� key �������л��ָ��λ������ĳ�Ա���б���Ա����ֵ�ݼ���ʽ����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param start {int} ��ʼ�±�λ��
	 * @param stop {int} �����±�λ�ã������ͬʱ����λ�ã�
	 * @param result {std::vector<string>*} �ǿ�ʱ�洢�����
	 *  ע�������±�λ�ã�0 ��ʾ��һ����Ա��1 ��ʾ�ڶ�����Ա��-1 ��ʾ���һ����Ա��
	 *     -2 ��ʾ�����ڶ�����Ա���Դ�����
	 * @return {int} �����������-1 ��ʾ����
	 */
	int zrevrange(const char* key, int start, int stop,
		std::vector<string>* result);

	/**
	 * �� key �������л��ָ��λ������ĳ�Ա������ֵ�б���Ա����ֵ�ݼ���ʽ����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param start {int} ��ʼ�±�λ��
	 * @param stop {int} �����±�λ�ã������ͬʱ����λ�ã�
	 * @param result �洢 "��Ա��-��ֵ��"��������ڲ��ȵ��� out.clear()
	 *  ע�������±�λ�ã�0 ��ʾ��һ����Ա��1 ��ʾ�ڶ�����Ա��-1 ��ʾ���һ����Ա��
	 *     -2 ��ʾ�����ڶ�����Ա���Դ�����
	 * @return {int} �����������-1 ��ʾ����
	 */
	int zrevrange_with_scores(const char* key, int start, int stop,
		std::vector<std::pair<string, double> >& out);

	/**
	 * �������� key �У����� score ֵ���� min �� max ֮��(�������� min �� max )
	 * �ĳ�Ա�����򼯳�Ա�� score ֵ��8��(��С����)��������
	 * @param key {const char*} ���򼯼�ֵ
	 * @param min {const char*} ���ַ�����ʾ��С��ֵ
	 * @param max {const char*} ���ַ�����ʾ����ֵ
	 * @param out {std::vector<string>*} �ǿ�ʱ�洢����Ա���������
	 * @param offset {const int*} �ǿ�ʱ��ʾ���������ʼ�±�
	 * @param count {const int*} �ǿ�ʱ��ʾ��ȡ�Ľ�����г�Ա����
	 * @return {int} ������г�Ա������
	 *  0: ��ʾ�����Ϊ�ջ� key ������
	 * -1: ��ʾ����� key ��������򼯶���
	 * >0: �����������
	 *  ע��
	 * 1��offset �� count ����ͬʱΪ�ǿ�ָ��ʱ����Ч
	 * 2��min �� max ������ -inf �� +inf ����ʾ��������
	 * 3��Ĭ������£������ȡֵʹ�ñ����� (С�ڵ��ڻ���ڵ���)��Ҳ����ͨ��������ǰ
	 *   ���� ( ������ʹ�ÿ�ѡ�Ŀ����� (С�ڻ����)���磺
	 * 3.1��"ZRANGEBYSCORE zset (1 5" �������з������� 1 < score <= 5 �ĳ�Ա
	 * 3.2��"ZRANGEBYSCORE zset (5 (10" �������з������� 5 < score < 10 �ĳ�Ա
	 */
	int zrevrangebyscore(const char* key, const char* min, const char* max,
		std::vector<string>* out, const int* offset = NULL,
		const int* count = NULL);

	/**
	 * �������� key �У����� score ֵ���� min �� max ֮��(�������� min �� max )
	 * �ĳ�Ա����ֵ�����򼯳�Ա�� score ֵ�ݼ�(��С����)�������У���ֵ(min/max)ʹ��
	 * ��������ʾ
	 * @param out �洢����ֵ-��Ա�����ԵĽ�������ڲ��ȵ��� out.clear()
	 * @param count {const int*} �ǿ�ʱ��ʾ��ȡ�Ľ�����г�Ա����
	 */
	int zrevrangebyscore_with_scores(const char* key, double min,
		double max, std::vector<std::pair<string, double> >& out,
		const int* offset = NULL, const int* count = NULL);
	int zrevrangebyscore_with_scores(const char* key, const char* min,
		const char* max, std::vector<std::pair<string, double> >& out,
		const int* offset = NULL, const int* count = NULL);

	/**
	 * �������� key �г�Ա member ������(�±�� 0 ��ʼ)���������򼯳�Ա�� score
	 * ֵ�ݼ�(�Ӵ�С)����
	 * @param key {const char*} ���򼯼�ֵ
	 * @param member {const char*} ��Ա��
	 * @param len {size_t} member �ĳ���
	 * @return {int} �±�λ��ֵ��-1 -- ������ key �����򼯶��󣬻��Ա��������
	 */
	int zrevrank(const char* key, const char* member, size_t len);
	int zrevrank(const char* key, const char* member);

	/**
	 * ������� key �У���Ա member �� score ֵ
	 * @param key {const char*} ���򼯼�ֵ
	 * @param member {const char*} ��Ա��
	 * @param len {size_t} member �ĳ���
	 * @param result {double&} �洢��ֵ���
	 * @return {bool} �������ڻ����ʱ���� false�����򷵻� true
	 */
	bool zscore(const char* key, const char* member, size_t len,
		double& result);
	bool zscore(const char* key, const char* member, double& result);

	/**
	 * ���������һ���������򼯵Ĳ��������и��� key ������������ numkeys ����ָ����
	 * �����ò���(�����)���浽Ŀ������; Ĭ������£��������ĳ����Ա�� score
	 * ֵ�����и������¸ó�Ա score ֵ֮��
	 * @param dst {const char*} Ŀ�����򼯼�ֵ
	 * @param keys Դ���򼯼�ֵ-Ȩ�ؼ��ϣ�ʹ��Ȩ��ѡ�����Ϊ ÿ�� �������� �ֱ�
	 *  ָ��һ���˷�����(multiplication factor)��ÿ���������򼯵����г�Ա�� score
	 *  ֵ�ڴ��ݸ��ۺϺ���(aggregation function)֮ǰ��Ҫ�ȳ��Ը����򼯵����ӣ�
	 *  ���û��ָ�� WEIGHTS ѡ��˷�����Ĭ������Ϊ 1
	 * @param aggregate {const char*} �ۺϷ�ʽ��Ĭ���� SUM �ۺϷ�ʽ���ۺϷ�ʽ���£�
	 *  SUM: �����м�����ĳ����Ա�� score ֵ֮ �� ��Ϊ������иó�Ա�� score ֵ
	 *  MIN: �����м�����ĳ����Ա�� ��С score ֵ��Ϊ������иó�Ա�� score ֵ
	 *  MAX: �����м�����ĳ����Ա�� ��� score ֵ��Ϊ������иó�Ա�� score ֵ
	 * @return {int} �±��浽Ŀ�����򼯵Ľ�����е�Ԫ��(��Ա)���������Դ����
	 *  �����д�����ͬ�ĳ�Ա����ֻ����һ����Ա������ -1 ��ʾ����
	 */
	int zunionstore(const char* dst, const std::map<string, double>& keys,
		const char* aggregate = "SUM");

	int zunionstore(const char* dst, const std::vector<string>& keys,
		const std::vector<double>* weights = NULL,
		const char* aggregate = "SUM");

	/**
	 * ���������һ���������򼯵Ľ��������и��� key ������������ numkeys ����ָ����
	 * �����ò���(�����)���浽Ŀ������; Ĭ������£��������ĳ����Ա�� score
	 * ֵ�����и������¸ó�Ա score ֵ֮��
	 * @return {int} �±��浽Ŀ�����򼯵Ľ�����е�Ԫ��(��Ա)����
	 */
	int zinterstore(const char* dst, const std::map<string, double>& keys,
		const char* aggregate = "SUM");

	int zinterstore(const char* dst, const std::vector<string>& keys,
		const std::vector<double>* weights = NULL,
		const char* aggregate = "SUM");
	
	/**
	 * �������ڵ������򼯺��е�Ԫ�أ�����Ԫ�س�Ա��Ԫ�ط�ֵ��
	 * @param cursor {int} �α�ֵ����ʼ����ʱ��ֵд 0
	 * @param out �洢��������ڲ���׷�ӷ�ʽ�����α������������ӽ��������У�
	 *  Ϊ��ֹ���ܽ���������¸�����������û����ڵ��ñ�����ǰ��������������
	 * @param pattern {const char*} ƥ��ģʽ��glob ��񣬷ǿ�ʱ��Ч
	 * @param count {const size_t*} �޶��Ľ�����������ǿ�ָ��ʱ��Ч
	 * @return {int} ��һ���α�λ�ã��������£�
	 *   0����������
	 *  -1: ����
	 *  >0: �α����һ��λ��
	 */
	int zscan(const char* key, int cursor,
		std::vector<std::pair<string, double> >& out,
		const char* pattern = NULL, const size_t* count = NULL);

	/**
	 * �����򼯺ϵ����г�Ա��������ͬ�ķ�ֵʱ�� ���򼯺ϵ�Ԫ�ػ���ݳ�Ա���ֵ���
	 * ��lexicographical ordering������������ �������������Է��ظ�����
	 * ���򼯺ϼ� key �У� ֵ���� min �� max ֮��ĳ�Ա
	 * @param min {const char*} ������Сֵ
	 * @param max {const char*} �������ֵ
	 * @param out {std::vector<string>*} �ǿ�ʱ�洢�����
	 * @param offset {const int*} �ǿ�ʱ��Ч���ӽ������ѡȡ���±���ʼֵ
	 * @param count {const int*} �ǿ�ʱ��Ч���ӽ�����е�ָ���±�λ����ѡȡ������
	 * @return {int} ������г�Ա������
	 *  0: ��ʾ�����Ϊ�ջ� key ������
	 * -1: ��ʾ����� key ��������򼯶���
	 * >0: �����������
	 * ע�����������ѡ��������£�
	 * 1���Ϸ��� min �� max ����������� ( ���� [ �� ���� ( ��ʾ�����䣨ָ����ֵ
	 *   ���ᱻ�����ڷ�Χ֮�ڣ��� �� [ ���ʾ�����䣨ָ����ֵ�ᱻ�����ڷ�Χ֮�ڣ�
	 * 2������ֵ + �� - �� min �����Լ� max �����о�����������壬 ���� + ��ʾ
	 *   �����ޣ� �� - ��ʾ�����ޡ���ˣ���һ�����г�Ա�ķ�ֵ����ͬ�����򼯺Ϸ�������
	 *   ZRANGEBYLEX <zset> - + �� ����������򼯺��е�����Ԫ��
	 */
	int zrangebylex(const char* key, const char* min, const char* max,
		std::vector<string>* out, const int* offset = NULL,
		const int* count = NULL);

	/**
	 * ��һ�����г�Ա�ķ�ֵ����ͬ�����򼯺ϼ� key ��˵�� �������᷵�ظü����У� 
	 * ��Ա���� min �� max ��Χ�ڵ�Ԫ������
	 * @return {int} ����������Ԫ������
	 */
	int zlexcount(const char* key, const char* min, const char* max);

	/**
	 * ����һ�����г�Ա�ķ�ֵ����ͬ�����򼯺ϼ� key ��˵�� ���������Ƴ��ü����У�
	 * ��Ա���� min �� max ��Χ�ڵ�����Ԫ��
	 * @return {int} ���Ƴ���Ԫ������
	 */
	int zremrangebylex(const char* key, const char* min, const char* max);

private:
	int zrange_get(const char* cmd, const char* key, int start,
		int stop, std::vector<string>* result);
	int zrange_get_with_scores(const char* cmd, const char* key, int start,
		int stop, std::vector<std::pair<string, double> >& out);
	int zrangebyscore_get(const char* cmd, const char* key,
		const char* min, const char* max, std::vector<string>* out,
		const int* offset = NULL, const int* count = NULL);
	int zrangebyscore_get_with_scores(const char* cmd,
		const char* key, const char* min, const char* max,
		std::vector<std::pair<string, double> >& out,
		const int* offset = NULL, const int* count = NULL);
	int zstore(const char* cmd, const char* dst,
		const std::map<string, double>& keys, const char* aggregate);
	int zstore(const char* cmd, const char* dst, const std::vector<string>& keys,
		const std::vector<double>* weights, const char* aggregate);
};

} // namespace acl
