#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;
class redis_result;

/**
 * redis Hahs(��ϣ��) �࣬�����ʵ�ֵ���Ҫ���
 * HDEL/HEXISTS/HGET/HGETALL/HINCRBY/HINCRBYFLOAT/HKEYS/HLEN/HMGET/HMSET
 * HSET/HSETNX/HVALS/HSCAN
 */
class ACL_CPP_API redis_hash : public redis_command
{
public:
	redis_hash(redis_client* conn = NULL);
	~redis_hash();

	/////////////////////////////////////////////////////////////////////

	/**
	 * �����"��-ֵ"������� KEY ��Ӧ�Ĺ�ϣ����
	 * @param key {const char*} ��ϣ�� key ֵ
	 * @return {bool} ����Ƿ�ɹ�
	 */
	bool hmset(const char* key, const std::map<string, string>& attrs);
	bool hmset(const char* key, const std::map<string, char*>& attrs);
	bool hmset(const char* key, const std::map<string, const char*>& attrs);

	bool hmset(const char* key, const std::map<int, string>& attrs);
	bool hmset(const char* key, const std::map<int, char*>& attrs);
	bool hmset(const char* key, const std::map<int, const char*>& attrs);

	/////////////////////////////////////////////////////////////////////

	/**
	 * ���� KEY ֵ�����"��-ֵ"�Դӹ�ϣ����ȡ��
	 * @param key {const char*} ��ϣ�� key ֵ
	 * @param names ��Ӧ key ����ֵ��
	 * @param result {std::vector<string>*} ���ö���ָ��ǿ�ʱ�洢��ѯ���
	 * @return {bool} �����Ƿ�ɹ���������سɹ�������ͨ������ hmget_result ����
	 *  �±�ֵȡ�ö�Ӧ��ֵ����ֱ�ӵ��� get_result ����ȡ�� redis_result��������
	 *  ���÷����д���ǿյĴ洢�������ĵ�ַ
	 */
	bool hmget(const char* key, const std::vector<string>& names,
		std::vector<string>* result = NULL);
	bool hmget(const char* key, const std::vector<char*>& names,
		std::vector<string>* result = NULL);
	bool hmget(const char* key, const std::vector<const char*>& names,
		std::vector<string>* result = NULL);
	bool hmget(const char* key, const std::vector<int>& names,
		std::vector<string>* result = NULL);

	bool hmget(const char* key, const char* names[], size_t argc,
		std::vector<string>* result = NULL);
	bool hmget(const char* key, const int names[], size_t argc,
		std::vector<string>* result = NULL);
	bool hmget(const char* key, const char* names[], const size_t lens[],
		size_t argc, std::vector<string>* result = NULL);

	/**
	 * �� hmget ��� true ʱ���ñ���������ö�Ӧ�±��ֵ���±�˳���� hmget
	 * �е�������±�˳����ͬ
	 * @param i {size_t} �±꣨�� 0 ��ʼ��
	 * @param len {size_t*} ����ָ��ǿգ���洢�����ؽ���ĳ��ȣ������÷���
	 *  ���طǿ�ָ��ʱ��Ч��
	 * @return {const char*} ���ض�Ӧ�±��ֵ�������� NULL ʱ��ʾ���±�û��ֵ��
	 *  Ϊ�˱�֤ʹ���ϵİ�ȫ�ԣ����ص��������ܱ�֤������� \0 ��β���ڼ������ݳ���
	 *  ʱ�������ý�β��
	 */
	const char* hmget_value(size_t i, size_t* len = NULL) const;

	/**
	 * ����ѯ���Ϊ�������ʱ���ñ��������һ������Ԫ��
	 * @param i {size_t} ���������±�ֵ
	 * @return {const redis_result*} �����������������Ϊ�ջ����ʱ�÷���
	 *  ���� NULL
	 */
	const redis_result* hmget_child(size_t i) const;

	/**
	 * ���ز�ѯ������ĸ���
	 * @return {size_t}
	 */
	size_t hmget_size() const;

	/////////////////////////////////////////////////////////////////////

	/**
	 * ���� key ������ĳ�����ֶε�ֵ
	 * @param key {const char*} key ��ֵ
	 * @param name {const char*} key �����������
	 * @param value {const char*} key �������ֵ
	 * @return {int} ����ֵ���壺
	 *  1 -- ��ʾ����ӵ����ֶ���ӳɹ�
	 *  0 -- ��ʾ�����Ѿ����ڵ����ֶγɹ�
	 * -1 -- ��ʾ������ key ����ǹ�ϣ�����ӽ���ֹ�޸�
	 */
	int hset(const char* key, const char* name, const char* value);
	int hset(const char* key, const char* name,
		const char* value, size_t value_len);
	int hset(const char* key, const char* name, size_t name_len,
		const char* value, size_t value_len);

	/**
	 * ���ҽ��� key �����е�ĳ�����ֶβ�����ʱ�Ÿ��¸����ֶ�ֵ
	 * @param key {const char*} key ��ֵ
	 * @param name {const char*} key �����������
	 * @param value {const char*} key �������ֵ
	 * @return {int} ����ֵ���壺
	 *  1 -- ��ʾ����ӵ����ֶ���ӳɹ�
	 *  0 -- �����ֶδ�����δ������и���
	 * -1 -- ��ʾ������ key ����ǹ�ϣ�����ӽ���ֹ�޸�
	 */
	int hsetnx(const char* key, const char* name, const char* value);
	int hsetnx(const char* key, const char* name,
		const char* value, size_t value_len);
	int hsetnx(const char* key, const char* name, size_t name_len,
		const char* value, size_t value_len);

	/**
	 * �� redis ��ϣ���л�ȡĳ�� key �����ĳ�����ֵ
	 * @param key {const char*} key ��ֵ
	 * @param name {const char*} key ��������ֶ�����
	 * @param result {string&} �洢��ѯ���ֵ(�ڲ��Ը� string ��������׷��)
	 * @return {bool} ����ֵ���壺
	 *  true -- �����ɹ��������򲻴���ʱҲ���سɹ�����Ҫ��� result �����Ƿ�仯��
	 *          �������ͨ����� result.length() �ı仯�������Ƿ��ѯ�����
	 *  false -- ����ʧ�ܻ�� key ����ǹ�ϣ����
	 */
	bool hget(const char* key, const char* name, string& result);
	bool hget(const char* key, const char* name,
		size_t name_len, string& result);

	/**
	 * �� redis ��ϣ���л�ȡĳ�� key ������������ֶε�ֵ
	 * @param key {const char*} key ��ֵ
	 * @param result {std::map<string, string>&} �洢���ֶ���-ֵ��ѯ�����
	 * @return {bool} �����Ƿ�ɹ������壺
	 *  true -- �����ɹ��������򲻴���ʱҲ���سɹ�����Ҫ��� result �����Ƿ�仯��
	 *          �������ͨ����� result.size() �ı仯�������Ƿ��ѯ�����
	 *  false -- ����ʧ�ܻ�� key ����ǹ�ϣ����
	 */
	bool hgetall(const char* key, std::map<string, string>& result);
	bool hgetall(const char* key, std::vector<string>& names,
		std::vector<string>& values);
	bool hgetall(const char* key, std::vector<const char*>& names,
		std::vector<const char*>& values);

	/**
	 * �� redis ��ϣ����ɾ��ĳ�� key �����ĳЩ���ֶ�
	 * @param key {const char*} key ��ֵ
	 * @param first_name {const char*} ��һ�����ֶ��������һ���ֶα����� NULL
	 * @return {int} �ɹ�ɾ�������ֶθ��������� -1 ��ʾ������ key ����ǹ�ϣ����
	 */
	int hdel(const char* key, const char* first_name, ...)
		ACL_CPP_PRINTF(3, 4);;
	int hdel(const char* key, const char* names[], size_t argc);
	int hdel(const char* key, const char* names[],
		const size_t names_len[], size_t argc);
	int hdel(const char* key, const std::vector<string>& names);
	int hdel(const char* key, const std::vector<char*>& names);
	int hdel(const char* key, const std::vector<const char*>& names);

	/**
	 * ��ĳ�� key �����е�ĳ�����ֶ�Ϊ����ʱ��������мӼ�����
	 * @param key {const char*} key ��ֵ
	 * @param name {const char*} key ��������ֶ�����
	 * @param inc {long long int} ���ӵ�ֵ������Ϊ��ֵ
	 * @param result {long long int*} �� NULL ʱ�洢���ֵ
	 * @return {bool} �����Ƿ�ɹ��������� false ʱ���������� key ����ǹ�ϣ
	 *  ���������ֶη���������
	 */
	bool hincrby(const char* key, const char* name,
		long long int inc, long long int* result = NULL);

	/**
	 * ��ĳ�� key �����е�ĳ�����ֶ�Ϊ������ʱ��������мӼ�����
	 * @param key {const char*} key ��ֵ
	 * @param name {const char*} key ��������ֶ�����
	 * @param inc {double} ���ӵ�ֵ������Ϊ��ֵ
	 * @param result {double*} �� NULL ʱ�洢���ֵ
	 * @return {bool} �����Ƿ�ɹ��������� false ʱ���������� key ����ǹ�ϣ
	 *  ���������ֶηǸ���������
	 */
	bool hincrbyfloat(const char* key, const char* name,
		double inc, double* result = NULL);

	/**
	 * ���� key �������������ֶ�����
	 * @param key {const char*} key ��ֵ
	 * @param names {std::vector<string>&} �洢�� key �����������ֶ�����
	 * @return {bool} �����Ƿ�ɹ������� false ���������� key ����ǹ�ϣ����
	 */
	bool hkeys(const char* key, std::vector<string>& names);

	/**
	 * ��� key ������ĳ�����ֶ��Ƿ����
	 * @param key {const char*} key ��ֵ
	 * @param name {const char*} key ��������ֶ�����
	 * @return {bool} �����Ƿ�ɹ������� false ���������� key ����ǹ�ϣ����
	 *  ������ֶβ�����
	 */
	bool hexists(const char* key, const char* name);
	bool hexists(const char* key, const char* name, size_t name_len);

	/**
	 * ���ĳ�� key �������������ֶε�����
	 * @param key {const char*} key ��ֵ
	 * @return {int} ����ֵ���壺
	 *  -1 -- ������ key ����ǹ�ϣ����
	 *  >0 -- ���ֶ�����
	 *   0 -- �� key �����ڻ����ֶ�����Ϊ 0
	 */
	int hlen(const char* key);

	/**
	 * �������ڵ�����ϣ���еļ�ֵ��
	 * @param key {const char*} ��ϣ��ֵ
	 * @param cursor {int} �α�ֵ����ʼ����ʱ��ֵд 0
	 * @param out {std::map<string>&} �����
	 * @param pattern {const char*} ƥ��ģʽ��glob ��񣬷ǿ�ʱ��Ч
	 * @param count {const size_t*} �޶��Ľ�����������ǿ�ָ��ʱ��Ч
	 * @return {int} ��һ���α�λ�ã��������£�
	 *   0����������
	 *  -1: ����
	 *  >0: �α����һ��λ��
	 */
	int hscan(const char* key, int cursor, std::map<string, string>& out,
		const char* pattern = NULL, const size_t* count = NULL);
};

} // namespace acl
