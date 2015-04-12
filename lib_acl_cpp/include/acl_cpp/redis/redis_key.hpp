#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl {

class redis_client;
class redis_client_cluster;

// redis ����֧�ֵ��������ͷ���
// the data type supported by redis
typedef enum
{
	REDIS_KEY_NONE,		// none
	REDIS_KEY_STRING,	// string
	REDIS_KEY_HASH,		// hash
	REDIS_KEY_LIST,		// list
	REDIS_KEY_SET,		// set
	REDIS_KEY_ZSET		// sorted set
} redis_key_t;

class ACL_CPP_API redis_key : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	redis_key();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	redis_key(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_client_cluster*�� size_t)
	 */
	redis_key(redis_client_cluster* cluster, size_t max_conns);

	virtual ~redis_key();

	/**
	 * ɾ��һ����һ�� KEY�����ڱ�εĽӿڣ���Ҫ�����һ������������ NULL ����
	 * delete one or some keys from redis, for deleting a variable
	 * number of keys, the last key must be NULL indicating the end
	 * of the variable args
	 * @return {int} ������ɾ���� KEY �ĸ��������£�
	 *  0: δɾ���κ� KEY
	 *  -1: ����
	 *  >0: ����ɾ���� KEY �ĸ�������ֵ�п������������ KEY �ĸ���
	 *  return the number of keys been deleted, return value as below:
	 *  0: none key be deleted
	 * -1: error happened
	 *  >0: the number of keys been deleted
	 *
	 */
	int del_one(const char* key);
	int del_one(const char* key, size_t len);
	int del(const char* first_key, ...);
	int del(const std::vector<string>& keys);
	int del(const std::vector<const char*>& keys);
	int del(const char* keys[], size_t argc);
	int del(const char* keys[], const size_t lens[], size_t argc);

	/**
	 * ���л����� key �������ر����л���ֵ��ʹ�� RESTORE ������Խ����ֵ�����л�
	 * Ϊ Redis ��
	 * serialize the object associate with the given key, and get the
	 * value after serializing, RESTORE command can be used to
	 * deserialize by the value
	 * @param key {const char*} ��ֵ
	 *  the key
	 * @param out {string&} �洢���л��Ķ���������
	 *  buffur used to store the result
	 * @return {int} ���л������ݳ���
	 *  the length of the data after serializing
	 */
	int dump(const char* key, string& out);

	/**
	 * �ж� KEY �Ƿ����
	 * check if the key exists in redis
	 * @param key {const char*} KEY ֵ
	 *  the key
	 * @return {bool} ���� true ��ʾ���ڣ������ʾ����򲻴���
	 *  true returned if key existing, false if error or not existing
	 */
	bool exists(const char* key);

	/**
	 * ���� KEY ���������ڣ���λ���룩
	 * set a key's time to live in seconds
	 * @param key {const char*} ��ֵ
	 *  the key
	 * @param n {int} �������ڣ��룩
	 *  lief cycle in seconds
	 * @return {int} ����ֵ�������£�
	 *  return value as below:
	 *  > 0: �ɹ���������������
	 *       set successfully
	 *  0���� key ������
	 *    the key doesn't exist
	 *  < 0: ����
	 *       error happened
	 */
	int expire(const char* key, int n);

	/**
	 * �� UNIX ʱ������� KEY ����������
	 * set the expiration for a key as a UNIX timestamp
	 * @param key {const char*} �����ֵ
	 *  the key
	 * @param stamp {time_t} UNIX ʱ��أ����� 1970 ������������
	 *  an absolute Unix timestamp (seconds since January 1, 1970).
	 * @return {int} ����ֵ�ĺ��壺
	 *  return value:
	 *  1: ���óɹ�
	 *     the timeout was set
	 *  0: �� key ������
	 *     the key doesn't exist or the timeout couldn't be set
	 * -1: ����
	 *     error happened
	 */
	int expireat(const char* key, time_t stamp);

	/**
	 * �������з��ϸ���ģʽ pattern �� key
	 * find all keys matching the given pattern
	 * @param pattern {const char*} ƥ��ģʽ
	 *  the give matching pattern
	 * @param out {std::vector<string>*} �� NULL ʱ�����洢�����
	 *  store the matched keys
	 * @return {int} �������������0--Ϊ�գ�<0 -- ��ʾ����
	 *  return the number of the matched keys, 0 if none, < 0 if error
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
	 * atomically transfer a key from a redis instance to another one
	 * @param key {const char*} ���ݶ�Ӧ�ļ�ֵ
	 *  the key
	 * @param addr {const char*} Ŀ�� redis-server ��������ַ����ʽ��ip:port
	 *  the destination redis instance's address, format: ip:port
	 * @param dest_db {unsigned} Ŀ�� redis-server �����������ݿ� ID ��
	 *  the databases ID in destination redis
	 * @param timeout {unsigned} Ǩ�ƹ��̵ĳ�ʱʱ��(���뼶)
	 *  timeout(microseconds) in transfering
	 * @param option {const char*} COPY �� REPLACE
	 *  transfer option: COPY or REPLACE
	 * @return {bool} Ǩ���Ƿ�ɹ�
	 *  if transfering successfully
	 */
	bool migrate(const char* key, const char* addr, unsigned dest_db,
		unsigned timeout, const char* option = NULL);

	/**
	 * ������������ redis-server �е���һ�����ݿ���
	 * move a key to another database
	 * @param key {const char*} ���ݼ�ֵ
	 *  the key
	 * @param dest_db {unsigned} Ŀ�����ݿ� ID ��
	 *  the destination database
	 * @return {int} Ǩ���Ƿ�ɹ���-1: ��ʾ����0��Ǩ��ʧ�ܣ���ΪĿ�����ݿ��д���
	 *  ��ͬ��ֵ��1��Ǩ�Ƴɹ�
	 *  if moving succcessfully. -1 if error, 0 if moving failed because
	 *  the same key already exists, 1 if successful
	 */
	int move(const char* key, unsigned dest_db);

	/**
	 * ���ظ��� key �����������ֵ�Ĵ�������������Ҫ���ڳ���
	 * get the referring count of the object, which just for debugging
	 * @param key {const char*} ���ݼ�ֵ
	 *  the key
	 * @return {int} ���� 0 ��ʾ�� key �����ڣ�< 0 ��ʾ����
	 *  0 if key not exists, < 0 if error
	 */
	int object_refcount(const char* key);

	/**
	 * ���ظ��� key �������ֵ��ʹ�õ��ڲ���ʾ
	 * get the internal storing of the object assosicate with the key
	 * @param key {const char*} ���ݼ�ֵ
	 *  the key
	 * @param out {string&} ���ڽ��
	 *  store the result
	 * @return {bool} �Ƿ�ɹ�
	 *  if successful
	 */
	bool object_encoding(const char* key, string& out);

	/**
	 * ���ظ��� key �Դ��������Ŀ���ʱ��(idle�� û�б���ȡҲû�б�д��)������Ϊ��λ
	 * get the key's idle time in seconds since its first stored
	 * @param key {const char*} ���ݼ�ֵ
	 *  the key
	 * @return {int} ����ֵ < 0 ��ʾ����
	 *  0 if error happened
	 */
	int object_idletime(const char* key);

	/**
	 * �Ƴ����� key ������ʱ�䣬����� key ��"��ʧ��"(������ʱ�� key )ת����
	 * "�־õ�"(һ����������ʱ�䡢�������ڵ� key )
	 * remove the expiration from a key
	 * @param key {const char*} �����ֵ
	 *  the key
	 * @return {int} ����ֵ�ĺ������£�
	 *  the value returned as below:
	 *  1 -- ���óɹ�
	 *       set ok
	 *  0 -- �� key �����ڻ�δ���ù���ʱ��
	 *       the key not exists or not be set expiration
	 * -1 -- ����
	 *       error happened
	 */
	int persist(const char* key);

	/**
	 * ���� KEY ���������ڣ���λ�����룩
	 * set a key's time to live in milliseconds
	 * @param key {const char*} ��ֵ
	 *  the key
	 * @param n {int} �������ڣ����룩
	 *  time to live in milliseconds
	 * @return {int} ����ֵ�������£�
	 *  value returned as below:
	 *  > 0: �ɹ���������������
	 *       set successfully
	 *    0���� key ������
	 *       the key doesn't exist
	 *  < 0: ����
	 *       error happened
	 */
	int pexpire(const char* key, int n);

	/**
	 * �Ժ���Ϊ��λ���� key �Ĺ��� unix ʱ���
	 * set the expiration for a key as UNIX timestamp specified
	 * in milliseconds
	 * @param key {const char*} ��ֵ
	 *  the key
	 * @param n {long long int} UNIX ʱ��أ����� 1970 �������ĺ�����
	 *  the UNIX timestamp in milliseconds from 1970.1.1
	 * @return {int} ����ֵ�������£�
	 *  value resturned as below:
	 *  > 0: �ɹ���������������
	 *       set successfully
	 *    0���� key ������
	 *       the key doesn't exist
	 *  < 0: ����
	 *       error happened
	 */
	int pexpireat(const char* key, long long int n);

	/**
	 * ��� KEY ��ʣ���������ڣ���λ�����룩
	 * get the time to live for a key in milliseconds
	 * @param key {const char*} ��ֵ
	 *  the key
	 * @return {int} ���ض�Ӧ��ֵ����������
	 *  value returned as below:
	 *  >0: �� key ʣ����������ڣ����룩
	 *      the time to live for a key in milliseconds
	 *  -3������
	 *      error happened
	 *  -2��key ������
	 *      the key doesn't exist
	 *  -1���� key ���ڵ�û������ʣ��ʱ��
	 *      th key were not be set expiration
	 * ע������ redis-server 2.8 ��ǰ�汾��key �����ڻ���ڵ�δ�����������򷵻� -1
	 * notice: for redis version before 2.8, -1 will be returned if the
	 * key doesn't exist or the key were not be set expiration.
	 */
	long long int pttl(const char* key);

	/**
	 * �ӵ�ǰ���ݿ����������(����ɾ��)һ�� key
	 * return a random key from the keyspace
	 ��@param buf {string&} �ɹ������� KEY ʱ�洢���
	 *  store the key
	 * @return {bool} �����Ƿ�ɹ���������� key ������ʱ���� false
	 *  true on success, or false be returned
	 */
	bool randmkey(string& buf);

	/**
	 * �� key ����Ϊ newkey
	 * rename a key
	 * @return {bool}
	 *  true on success, or error happened
	 */
	bool rename_key(const char* key, const char* newkey);

	/**
	 * ���ҽ��� newkey ������ʱ���� key ����Ϊ newkey
	 * rename a key only if the new key does not exist
	 * @param key {const char*} �� key
	 * @param newkey {const char*} �� key
	 * @return {bool} �Ƿ�ɹ�
	 *  true on success, false if the newkey already existed or error
	 */
	bool renamenx(const char* key, const char* newkey);

	/**
	 * �����л����������л�ֵ���������͸����� key ����
	 * create a key using the provided serialized value, previously
	 * obtained by using DUMP
	 * @param ttl {int} �Ժ���Ϊ��λΪ key ��������ʱ�䣬��� ttl Ϊ 0��
	 *  ��ô����������ʱ��
	 *  the time to live for the key in milliseconds, if tll is 0,
	 *  expiration will not be set
	 * @param replace {bool} ��� key �����Ƿ�ֱ�Ӹ���
	 *  if the key already exists, this parameter decides if replacing
	 *  the existing key
	 * @return {bool}
	 *  true on success, false on error
	 */
	bool restore(const char* key, const char* value, size_t len,
		int ttl, bool replace = false);

	/**
	 * ��� KEY ��ʣ���������ڣ���λ���룩
	 * get the time to live for a key in seconds
	 * @param key {const char*} ��ֵ
	 *  the key
	 * @return {int} ���ض�Ӧ��ֵ����������
	 *  return value as below:
	 *  > 0: �� key ʣ����������ڣ��룩
	 *       the time to live for a key in seconds
	 *   -3������
	 *       error happened
	 *   -2��key ������
	 *       the key doesn't exist
	 *   -1���� key ���ڵ�û������ʣ��ʱ��
	 *       the key were not be set expiration
	 * ע������ redis-server 2.8 ��ǰ�汾��key �����ڻ���ڵ�δ�����������򷵻� -1
	 * notice: for the redis version before 2.8, -1 will be returned
	 *  if the key doesn't exist or the key were not be set expiration
	 */
	int ttl(const char* key);

	/**
	 * ��� KEY �Ĵ洢����
	 * get the the type stored at key
	 * @para key {const char*} KEY ֵ
	 *  the key
	 * @return {redis_key_t} ���� KEY �Ĵ洢����
	 *  return redis_key_t defined above as REDIS_KEY_
	 */
	redis_key_t type(const char* key);

	/**
	 * �������ڵ�����ǰ���ݿ��е����ݿ��
	 * incrementally iterate the keys space in the specified database
	 * @param cursor {int} �α�ֵ����ʼ����ʱ��ֵд 0
	 *  the iterating cursor beginning with 0
	 * @param out {std::vector<acl::string>&} �洢��������ڲ���׷�ӷ�ʽ������
	 *  �������������ӽ��������У�Ϊ��ֹ���ܽ���������¸�����������û�����
	 *  ���ñ�����ǰ��������������
	 *  string array storing the results, the array will be cleared
	 *  internal and the string result will be appened to the array
	 * @param pattern {const char*} ƥ��ģʽ��glob ��񣬷ǿ�ʱ��Ч
	 &  the matching pattern with glob style, only effective if not NULL
	 * @param count {const size_t*} �޶��Ľ�����������ǿ�ָ��ʱ��Ч
	 *  limit the max number of the results stored in array, only
	 *  effective when not NULL
	 * @return {int} ��һ���α�λ�ã��������£�
	 *  return the next cursor value as follow:
	 *   0����������
	 *      iterating is finished
	 *  -1: ����
	 *      some error happened
	 *  >0: �α����һ��λ��
	 *      the next cursor value for iterating
	 */
	int scan(int cursor, std::vector<string>& out,
		const char* pattern = NULL, const size_t* count = NULL);
};

} // namespace acl
