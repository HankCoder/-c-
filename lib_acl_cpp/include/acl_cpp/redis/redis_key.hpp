#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl {

class redis_client;

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

class ACL_CPP_API redis_key : public redis_command
{
public:
	redis_key(redis_client* conn = NULL);
	~redis_key();

	/**
	 * ɾ��һ�� KEY�����ڱ�εĽӿڣ���Ҫ�����һ������������ NULL ����
	 * @return {int} ������ɾ���� KEY �ĸ��������£�
	 *  0: δɾ���κ� KEY
	 *  -1: ����
	 *  >0: ����ɾ���� KEY �ĸ�������ֵ�п������������ KEY �ĸ���
	 */
	int del(const char* first_key, ...);
	int del(const std::vector<string>& keys);
	int del(const std::vector<const char*>& keys);
	int del(const std::vector<int>& keys);
	int del(const char* keys[], size_t argc);
	int del(const int keys[], size_t argc);
	int del(const char* keys[], const size_t lens[], size_t argc);

	/**
	 * ���л����� key �������ر����л���ֵ��ʹ�� RESTORE ������Խ����ֵ�����л�
	 * Ϊ Redis ��
	 * @param key {const char*} ��ֵ
	 * @param out {string&} �洢���л��Ķ���������
	 * @return {int} ���л������ݳ���
	 */
	int dump(const char* key, string& out);

	/**
	 * �ж� KEY �Ƿ����
	 * @param key {const char*} KEY ֵ
	 * @return {bool} ���� true ��ʾ���ڣ������ʾ����򲻴���
	 */
	bool exists(const char* key);

	/**
	 * ���� KEY ���������ڣ���λ���룩
	 * @param key {const char*} ��ֵ
	 * @param n {int} �������ڣ��룩
	 * @return {int} ����ֵ�������£�
	 *  > 0: �ɹ���������������
	 *  0���� key ������
	 *  < 0: ����
	 */
	int expire(const char* key, int n);

	/**
	 * �� UNIX ʱ������� KEY ����������
	 * @param key {const char*} �����ֵ
	 * @param stamp {time_t} UNIX ʱ��أ����� 1970 ������������
	 * @return {int} ����ֵ�ĺ��壺
	 *  1 -- ���óɹ���0 -- �� key �����ڣ�-1 -- ����
	 */
	int expireat(const char* key, time_t stamp);

	/**
	 * �������з��ϸ���ģʽ pattern �� key
	 * @param pattern {const char*} ƥ��ģʽ
	 * @param out {std::vector<string>*} �� NULL ʱ�����洢�����
	 * @return {int} �������������0--Ϊ�գ�<0 -- ��ʾ����
	 *  ƥ��ģʽ������
	 *   KEYS * ƥ�����ݿ������� key ��
	 *   KEYS h?llo ƥ�� hello �� hallo �� hxllo �ȡ�
	 *   KEYS h*llo ƥ�� hllo �� heeeeello �ȡ�
	 *   KEYS h[ae]llo ƥ�� hello �� hallo ������ƥ�� hillo ��
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
	int keys_pattern(const char* pattern, std::vector<string>* out);
	
	/**
	 * �����ݴ�һ�� redis-server Ǩ������һ�� redis-server
	 * @param key {const char*} ���ݶ�Ӧ�ļ�ֵ
	 * @param addr {const char*} Ŀ�� redis-server ��������ַ����ʽ��ip:port
	 * @param dest_db {unsigned} Ŀ�� redis-server �����������ݿ� ID ��
	 * @param timeout {unsigned} Ǩ�ƹ��̵ĳ�ʱʱ��(���뼶)
	 * @param option {const char*} COPY �� REPLACE
	 * @return {bool} Ǩ���Ƿ�ɹ�
	 */
	bool migrate(const char* key, const char* addr, unsigned dest_db,
		unsigned timeout, const char* option = NULL);

	/**
	 * ������������ redis-server �е���һ�����ݿ���
	 * @param key {const char*} ���ݼ�ֵ
	 * @param dest_db {unsigned} Ŀ�����ݿ� ID ��
	 * @return {int} Ǩ���Ƿ�ɹ���-1: ��ʾ����0��Ǩ��ʧ�ܣ���ΪĿ�����ݿ��д���
	 *  ��ͬ��ֵ��1��Ǩ�Ƴɹ�
	 */
	int move(const char* key, unsigned dest_db);

	/**
	 * ���ظ��� key �����������ֵ�Ĵ�������������Ҫ���ڳ���
	 * @param key {const char*} ���ݼ�ֵ
	 * @return {int} ���� 0 ��ʾ�� key �����ڣ�< 0 ��ʾ����
	 */
	int object_refcount(const char* key);

	/**
	 * ���ظ��� key �������ֵ��ʹ�õ��ڲ���ʾ
	 * @param key {const char*} ���ݼ�ֵ
	 * @param out {string&} ���ڽ��
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool object_encoding(const char* key, string& out);

	/**
	 * ���ظ��� key �Դ��������Ŀ���ʱ��(idle�� û�б���ȡҲû�б�д��)������Ϊ��λ
	 * @param key {const char*} ���ݼ�ֵ
	 * @return {int} ����ֵ < 0 ��ʾ����
	 */
	int object_idletime(const char* key);

	/**
	 * �Ƴ����� key ������ʱ�䣬����� key ��"��ʧ��"(������ʱ�� key )ת����
	 * "�־õ�"(һ����������ʱ�䡢�������ڵ� key )
	 * @param key {const char*} �����ֵ
	 * @return {int} ����ֵ�ĺ������£�
	 *  1 -- ���óɹ���0 -- �� key �����ڻ�δ���ù���ʱ�䣬-1 -- ����
	 */
	int persist(const char* key);

	/**
	 * ���� KEY ���������ڣ���λ�����룩
	 * @param key {const char*} ��ֵ
	 * @param n {int} �������ڣ����룩
	 * @return {int} ����ֵ�������£�
	 *  > 0: �ɹ���������������
	 *  0���� key ������
	 *  < 0: ����
	 */
	int pexpire(const char* key, int n);

	/**
	 * �Ժ���Ϊ��λ���� key �Ĺ��� unix ʱ���
	 * @param key {const char*} ��ֵ
	 * @param n {long long int} UNIX ʱ��أ����� 1970 �������ĺ�����
	 * @return {int} ����ֵ�������£�
	 *  > 0: �ɹ���������������
	 *  0���� key ������
	 *  < 0: ����
	 */
	int pexpireat(const char* key, long long int n);

	/**
	 * ��� KEY ��ʣ���������ڣ���λ�����룩
	 * @param key {const char*} ��ֵ
	 * @return {int} ���ض�Ӧ��ֵ����������
	 *  > 0: �� key ʣ����������ڣ����룩
	 * -3������
	 * -2��key ������
	 * -1���� key ���ڵ�û������ʣ��ʱ��
	 * ע������ redis-server 2.8 ��ǰ�汾��key �����ڻ���ڵ�δ�����������򷵻� -1
	 */
	long long int pttl(const char* key);

	/**
	 * �ӵ�ǰ���ݿ����������(����ɾ��)һ�� key
	 ��@param buf {string&} �ɹ������� KEY ʱ�洢���
	 * @return {bool} �����Ƿ�ɹ���������� key ������ʱ���� falsse
	 */
	bool randmkey(string& buf);

	/**
	 * �� key ����Ϊ newkey
	 * @return {bool}
	 */
	bool rename_key(const char* key, const char* newkey);

	/**
	 * ���ҽ��� newkey ������ʱ���� key ����Ϊ newkey
	 * @param key {const char*} �� key
	 * @param newkey {const char*} �� key
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool renamenx(const char* key, const char* newkey);

	/**
	 * �����л����������л�ֵ���������͸����� key ����
	 * @param ttl {int} �Ժ���Ϊ��λΪ key ��������ʱ�䣬��� ttl Ϊ 0��
	 *  ��ô����������ʱ��
	 * @return {bool}
	 */
	bool restore(const char* key, const char* value, size_t len,
		int ttl, bool replace = false);

	/**
	 * ��� KEY ��ʣ���������ڣ���λ���룩
	 * @param key {const char*} ��ֵ
	 * @return {int} ���ض�Ӧ��ֵ����������
	 *  > 0: �� key ʣ����������ڣ��룩
	 * -3������
	 * -2��key ������
	 * -1���� key ���ڵ�û������ʣ��ʱ��
	 * ע������ redis-server 2.8 ��ǰ�汾��key �����ڻ���ڵ�δ�����������򷵻� -1
	 */
	int ttl(const char* key);

	/**
	 * ��� KEY �Ĵ洢����
	 * @para key {const char*} KEY ֵ
	 * @return {redis_key_t} ���� KEY �Ĵ洢����
	 */
	redis_key_t type(const char* key);

	/**
	 * �������ڵ�����ǰ���ݿ��е����ݿ��
	 * @param cursor {int} �α�ֵ����ʼ����ʱ��ֵд 0
	 * @param out {std::vector<string>&} �洢��������ڲ���׷�ӷ�ʽ������
	 *  �������������ӽ��������У�Ϊ��ֹ���ܽ���������¸�����������û�����
	 *  ���ñ�����ǰ��������������
	 * @param pattern {const char*} ƥ��ģʽ��glob ��񣬷ǿ�ʱ��Ч
	 * @param count {const size_t*} �޶��Ľ�����������ǿ�ָ��ʱ��Ч
	 * @return {int} ��һ���α�λ�ã��������£�
	 *   0����������
	 *  -1: ����
	 *  >0: �α����һ��λ��
	 */
	int scan(int cursor, std::vector<string>& out,
		const char* pattern = NULL, const size_t* count = NULL);
};

} // namespace acl
