#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>
#include <vector>
#include "acl_cpp/stdlib/string.hpp"

namespace acl
{

class redis_client;
class redis_result;

class ACL_CPP_API redis_hash
{
public:
	redis_hash(redis_client& conn);
	~redis_hash();

	const redis_result* get_result() const
	{
		return result_;
	}

	redis_client& get_client() const
	{
		return conn_;
	}

	/////////////////////////////////////////////////////////////////////

	/**
	 * �����"��-ֵ"������� KEY ��Ӧ�Ĺ�ϣ����
	 * @param key {const char*} KEY ֵ
	 * @return {bool} ����Ƿ�ɹ�
	 */
	bool hmset(const char* key, const std::map<string, string>& attrs);
	bool hmset(const char* key, const std::map<string, char*>& attrs);
	bool hmset(const char* key, const std::map<string, const char*>& attrs);

	bool hmset(const char* key, const std::map<int, string>& attrs);
	bool hmset(const char* key, const std::map<int, char*>& attrs);
	bool hmset(const char* key, const std::map<int, const char*>& attrs);
	bool hmset(const string& req);

	/////////////////////////////////////////////////////////////////////

	/**
	 * ���� KEY ֵ�����"��-ֵ"�Դӹ�ϣ����ȡ��
	 * @param key {const char*} KEY ֵ
	 * @return {bool} �����Ƿ�ɹ���������سɹ�������ͨ������ hmget_result ����
	 *  �±�ֵȡ�ö�Ӧ��ֵ����ֱ�ӵ��� get_result ����ȡ�� redis_result
	 */
	bool hmget(const char* key, const std::vector<string>& names);
	bool hmget(const char* key, const std::vector<char*>& names);
	bool hmget(const char* key, const std::vector<const char*>& names);
	bool hmget(const char* key, const std::vector<int>& names);

	bool hmget(const char* key, const char* names[], size_t argc);
	bool hmget(const char* key, const int names[], size_t argc);
	bool hmget(const char* key, const char* names[],
		const size_t lens[], size_t argc);
	bool hmget(const string& req);

	/**
	 * �� hmget ��� true ʱ���ñ���������ö�Ӧ�±��ֵ���±�˳���� hmget �е�����
	 * ���±�˳����ͬ
	 * @param i {size_t} �±꣨�� 0 ��ʼ��
	 * @param len {size_t*} ����ָ��ǿգ���洢�����ؽ���ĳ��ȣ������÷������ط�
	 *  ��ָ��ʱ��Ч��
	 * @return {const char*} ���ض�Ӧ�±��ֵ�������� NULL ʱ��ʾ���±�û��ֵ��
	 *  Ϊ�˱�֤ʹ���ϵİ�ȫ�ԣ����ص��������ܱ�֤������� \0 ��β���ڼ������ݳ���ʱ
	 *  �������ý�β��
	 */
	const char* hmget_result(size_t i, size_t* len = NULL) const;

	/////////////////////////////////////////////////////////////////////

	int hset(const char* key, const char* name, const char* value);
	int hset(const char* key, const char* name,
		const char* value, size_t value_len);
	int hset(const char* key, const char* name, size_t name_len,
		const char* value, size_t value_len);
	int hset(const string& req);

	int hsetnx(const char* key, const char* name, const char* value);
	int hsetnx(const char* key, const char* name,
		const char* value, size_t value_len);
	int hsetnx(const char* key, const char* name, size_t name_len,
		const char* value, size_t value_len);
	int hsetnx(const string& req);

	bool hget(const char* key, const char* name, string& result);
	bool hget(const char* key, const char* name,
		size_t name_len, string& result);

	bool hgetall(const char* key, std::map<string, string>& result);
	bool hgetall(const char* key, std::vector<string>& names,
		std::vector<string>& values);
	bool hgetall(const char* key, std::vector<const char*>& names,
		std::vector<const char*>& values);

	int hdel(const char* key, const char* first_name, ...)
		ACL_CPP_PRINTF(3, 4);;
	int hdel(const char* key, const char* names[], size_t argc);
	int hdel(const char* key, const char* names[],
		const size_t names_len[], size_t argc);
	int hdel(const char* key, const std::vector<string>& names);
	int hdel(const char* key, const std::vector<char*>& names);
	int hdel(const char* key, const std::vector<const char*>& names);
	int hdel(const string& req);

	bool hincrby(const char* key, const char* name,
		long long int inc, long long int* result = NULL);
	bool hincrbyfloat(const char* key, const char* name,
		double inc, double* result = NULL);

	bool hkeys(const char* key, std::vector<string>& names);

	bool hexists(const char* key, const char* name);
	bool hexists(const char* key, const char* name, size_t name_len);

	int hlen(const char* key);

	/////////////////////////////////////////////////////////////////////

private:
	redis_client& conn_;
	const redis_result* result_;
};

} // namespace acl
