#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"

namespace acl {

class redis_result;

// redis ����֧�ֵ��������ͷ���
typedef enum
{
	REDIS_KEY_UNKNOWN,
	REDIS_KEY_NONE,
	REDIS_KEY_STRING,	// string
	REDIS_KEY_HASH,		// hash
	REDIS_KEY_LIST,		// list
	REDIS_KEY_SET,		// set
	REDIS_KEY_ZSET		// sorted set
} redis_key_t;

class ACL_CPP_API redis_key
{
public:
	redis_key(redis_client* conn = NULL);
	~redis_key();

	void reset();

	void set_client(redis_client* conn);

	redis_client* get_client() const
	{
		return conn_;
	}

	/**
	 * ɾ��һ�� KEY
	 * @return {int} ������ɾ���� KEY �ĸ��������£�
	 *  0: δɾ���κ� KEY
	 *  -1: ����
	 *  >0: ����ɾ���� KEY �ĸ�������ֵ�п������������ KEY �ĸ���
	 */
	int del(const char* first_key, ...) ACL_CPP_PRINTF(2, 3);
	int del(const std::vector<string>& keys);
	int del(const std::vector<char*>& keys);
	int del(const std::vector<const char*>& keys);
	int del(const std::vector<int>& keys);
	int del(const char* keys[], size_t argc);
	int del(const int keys[], size_t argc);
	int del(const char* keys[], const size_t lens[], size_t argc);

	/**
	 * ���� KEY ���������ڣ���λ���룩
	 * @param key {const char*} ��ֵ
	 * @param n {int} �������ڣ��룩
	 * @return {int} ����ֵ�������£�
	 *  > 0: �ɹ���������������
	 *  0���� key �����ڻ�ü�δ������������
	 *  < 0: ����
	 */
	int expire(const char* key, int n);

	/**
	 * ��� KEY ��ʣ���������ڣ���λ���룩
	 * @param key {const char*} ��ֵ
	 * @return {int} ���ض�Ӧ��ֵ����������
	 *  > 0: �� key ʣ����������ڣ��룩
	 *  0���� key �����ڻ�ü�δ������������
	 *  < 0: ����
	 */
	int ttl(const char* key);

	/**
	 * �ж� KEY �Ƿ����
	 * @param key {const char*} KEY ֵ
	 * @return {bool} ���� true ��ʾ���ڣ������ʾ����򲻴���
	 */
	bool exists(const char* key);

	/**
	 * ��� KEY �Ĵ洢����
	 * @para key {const char*} KEY ֵ
	 * @return {redis_key_t} ���� KEY �Ĵ洢����
	 */
	redis_key_t type(const char* key);

	bool migrate(const char* key, const char* addr, unsigned dest_db,
		unsigned timeout, const char* option = NULL);

	int move(const char* key, unsigned dest_db);

private:
	redis_client* conn_;
	const redis_result* result_;

	int del(const string& req);
};

} // namespace acl
