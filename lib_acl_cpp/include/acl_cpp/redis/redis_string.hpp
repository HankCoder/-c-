#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>
#include <vector>
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class string;
class redis_client;
class redis_result;

/**
 * ���е��ַ�������������ʵ��
 * all the commands in redis Strings are be implemented.
 */
class ACL_CPP_API redis_string : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	redis_string();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	redis_string(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_client_cluster*�� size_t)
	 */
	redis_string(redis_client_cluster* cluster, size_t max_conns);
	virtual ~redis_string();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ���ַ���ֵ value ������ key
	 * set the string value of a key
	 * @param key {const char*} �ַ�������� key
	 *  the key of a string
	 * @param value {const char*} �ַ�������� value
	 *  the value of a string
	 * @return {bool} �����Ƿ�ɹ������� false ��ʾ������ key ������ַ�������
	 *  true if SET was executed correctly, false if error happened or
	 *  the key's object isn't a string.
	 */
	bool set(const char* key, const char* value);
	bool set(const char* key, size_t key_len,
		const char* value, size_t value_len);

	/**
	 * ��ֵ value ������ key ������ key ������ʱ����Ϊ timeout (����Ϊ��λ)��
	 * ��� key �Ѿ����ڣ� SETEX �����д��ֵ
	 * set key to hold the strnig value, and set key to timeout after
	 * a given number of seconds.
	 * @param key {const char*} �ַ�������� key
	 *  the key of a string
	 * @param value {const char*} �ַ�������� value
	 *  the value of a string
	 * @param timeout {int} ����ֵ����λΪ��
	 *  the timeout in seconds of a string
	 * @return {bool} �����Ƿ�ɹ������� false ��ʾ������ key ������ַ�������
	 *  true if SETEX was executed correctly, false if error happened
	 *  or the object specified by the key is not a string
	 */
	bool setex(const char* key, const char* value, int timeout);
	bool setex(const char* key, size_t key_len, const char* value,
		size_t value_len, int timeout);

	/**
	 * ��ֵ value ������ key ������ key ������ʱ����Ϊ timeout (�Ժ���Ϊ��λ)��
	 * ��� key �Ѿ����ڣ� SETEX �����д��ֵ
	 * set key to hold the string value, and set key to timeout after
	 * a given number of milliseconds.
	 * @param key {const char*} �ַ�������� key
	 *  the key of a string
	 * @param value {const char*} �ַ�������� value
	 *  the value of a string
	 * @param timeout {int} ����ֵ����λΪ����
	 *  the timeout in milliseconds of a string
	 * @return {bool} �����Ƿ�ɹ������� false ��ʾ������ key ������ַ�������
	 *  true if SETEX was executed correctly, false if error happened
	 *  or the object specified by the key is not a string
	 */
	bool psetex(const char* key, const char* value, int timeout);
	bool psetex(const char* key, size_t key_len, const char* value,
		size_t value_len, int timeout);

	/**
	 * �� key ��ֵ��Ϊ value �����ҽ��� key �����ڣ��������� key �Ѿ����ڣ�
	 * �� SETNX �����κζ���
	 * set the value of a key, only if the key does not exist.
	 * @param key {const char*} �ַ�������� key
	 *  the key of the string
	 * @param value {const char*} �ַ�������� value
	 *  the value of the string
	 * @return {int} ����ֵ�������£�
	 *  return the value as below:
	 *  -1������� key ���ַ�������
	 *      error happened or the object by the key isn't a string
	 *   0������ key �Ķ������
	 *      the string of the key already exists
	 *   1����ӳɹ�
	 *      the command was executed correctly
	 */
	int setnx(const char* key, const char* value);
	int setnx(const char* key, size_t key_len,
		const char* value, size_t value_len);

	/**
	 * ��� key �Ѿ����ڲ�����һ���ַ����� APPEND ��� value ׷�ӵ� key ԭ��
	 * ��ֵ��ĩβ����� key �����ڣ� APPEND �ͼ򵥵ؽ����� key ��Ϊ value
	 * append a value to a key
	 * @param key {const char*} �ַ�������� key
	 *  the key of a string
	 * @param value {const char*} �ַ��������ֵ
	 *  the value to be appended to a key
	 * @return {int} ���ص�ǰ���ַ����ĳ��ȣ�-1 ��ʾ����� key ���ַ�������
	 *  return the length of the string after appending, -1 if error
	 *  happened or the key's object isn't a string
	 */
	int append(const char* key, const char* value);
	int append(const char* key, const char* value, size_t size);

	/**
	 * ���� key ���������ַ���ֵ
	 * get the value of a key 
	 * @param key {const char*} �ַ�������� key
	 *  the key of a string
	 * @param buf {string&} �����ɹ���洢�ַ��������ֵ
	 *  store the value of a key after GET executed correctly
	 * @return {bool} �����Ƿ�ɹ������� false ��ʾ����� key ���ַ�������
	 *  if the GET was executed correctly, false if error happened or
	 *  is is not a string of the key
	 */
	bool get(const char* key, string& buf);
	bool get(const char* key, size_t len, string& buf);

	/**
	 * ���� key ���������ַ���ֵ�������ص��ַ���ֵ�Ƚϴ�ʱ���ڲ����Զ�������Ƭ������
	 * һ�����ڴ��г�һЩ��������С�ڴ棬ʹ������Ҫ���ݷ��صĽ���������¶Խ�����ݽ���
	 * ��װ��������Ե��ã� redis_result::get(size_t, size_t*) �������ĳ����
	 * Ƭ��Ƭ�����ݣ����� redis_result::get_size() ��÷�Ƭ����ĳ���
	 * @param key {const char*} �ַ�������� key
	 * @param buf {string&} �����ɹ���洢�ַ��������ֵ
	 * @return {bool} �����Ƿ�ɹ������� false ��ʾ����� key ���ַ�������
	 */
	const redis_result* get(const char* key);
	const redis_result* get(const char* key, size_t len);

	/**
	 * ������ key ��ֵ��Ϊ value �������� key �ľ�ֵ���� key ���ڵ�����
	 * �ַ�������ʱ������һ������
	 * @param key {const char*} �ַ�������� key
	 * @param value {const char*} �趨���µĶ����ֵ
	 * @param buf {string&} �洢����ľɵ�ֵ
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool getset(const char* key, const char* value, string& buf);
	bool getset(const char* key, size_t key_len, const char* value,
		size_t value_len, string& buf);

	/////////////////////////////////////////////////////////////////////

	/**
	 * ���ָ�� key ���ַ��������ֵ�����ݳ���
	 * @param key {const char*} �ַ�������� key
	 * @return {int} ����ֵ�������£�
	 *  -1���������ַ�������
	 *   0���� key ������
	 *  >0�����ַ������ݵĳ���
	 */
	int get_strlen(const char* key);
	int get_strlen(const char* key, size_t len);

	/**
	 * �� value ������д(overwrite)���� key ��������ַ���ֵ����ƫ���� offset ��ʼ��
	 * �����ڵ� key �����հ��ַ�������
	 * @param key {const char*} �ַ�������� key
	 * @param offset {unsigned} ƫ������ʼλ�ã���ֵ���Դ����ַ��������ݳ��ȣ���ʱ
	 *  �Ǽ�Ŀն����� \0 ����
	 * @param value {const char*} ���ǵ�ֵ
	 * @return {int} ��ǰ�ַ�����������ݳ���
	 */
	int setrange(const char* key, unsigned offset, const char* value);
	int setrange(const char* key, size_t key_len, unsigned offset,
		const char* value, size_t value_len);

	/**
	 * ���� key ���ַ���ֵ�����ַ������ַ����Ľ�ȡ��Χ�� start �� end ����ƫ��������
	 * (���� start �� end ����)
	 * @param key {const char*} �ַ�������� key
	 * @param start {int} ��ʼ�±�ֵ
	 * @param end {int} �����±�ֵ
	 * @param buf {string&} �ɹ�ʱ�洢���
	 * @return {bool} �����Ƿ�ɹ�
	 *  ע���±�λ�ÿ���Ϊ��ֵ����ʾ���ַ���β����ǰ��ʼ�������� -1 ��ʾ���һ��Ԫ��
	 */
	bool getrange(const char* key, int start, int end, string& buf);
	bool getrange(const char* key, size_t key_len,
		int start, int end, string& buf);

	/////////////////////////////////////////////////////////////////////

	/**
	 * �� key ��������ַ���ֵ�����û����ָ��ƫ�����ϵ�λ(bit)��
	 * λ�����û����ȡ���� value ������������ 0 Ҳ������ 1
	 * @param key {const char*} �ַ�������� key
	 * @param offset {unsigned} ָ��ƫ������λ��
	 * @param bit {bool} Ϊ true ��ʾ���ñ�־λ������Ϊȡ����־λ
	 * @return {int} �����ɹ��󷵻ظ�λ��ԭ���Ĵ洢λ��ֵ���������£�
	 *  -1�������� key ���ַ�������
	 *   0��ԭ���Ĵ洢λΪ 0
	 *   1��ԭ���Ĵ洢λΪ 1
	 */
	bool setbit(const char* key, unsigned offset, bool bit);
	bool setbit(const char* key, size_t len, unsigned offset, bool bit);

	/**
	 * �� key ��������ַ���ֵ����ȡָ��ƫ�����ϵ�λ(bit)���� offset ���ַ���ֵ
	 * �ĳ��ȴ󣬻��� key ������ʱ������ 0
	 * @param key {const char*} �ַ�������� key
	 * @param offset {unsigned} ָ��ƫ������λ��
	 * @param bit {int&} �ɹ���洢ָ��λ�õı�־λ
	 * @return {bool} �����Ƿ�ɹ������� false ��ʾ������ key ���ַ�������
	 */
	bool getbit(const char* key, unsigned offset, int& bit);
	bool getbit(const char* key, size_t len, unsigned offset, int& bit);

	/**
	 * ��������ַ����У�������Ϊ 1 �ı���λ����������ָ���� start/end���������ָ��
	 * �����ڽ���
	 * @param key {const char*} �ַ�������� key
	 * @return {int} ��־λΪ 1 ��������-1 ��ʾ�������ַ�������
	 */
	int bitcount(const char* key);
	int bitcount(const char* key, size_t len);
	int bitcount(const char* key, int start, int end);
	int bitcount(const char* key, size_t len, int start, int end);

	/**
	 * ��һ������ key ���߼���������������浽 destkey
	 * @param destkey {const char*} Ŀ���ַ�������� key
	 * @param keys Դ�ַ������󼯺�
	 * @return {int}
	 */
	int bitop_and(const char* destkey, const std::vector<string>& keys);
	int bitop_and(const char* destkey, const std::vector<const char*>& keys);
	int bitop_and(const char* destkey, const char* key, ...);
	int bitop_and(const char* destkey, const char* keys[], size_t size);

	/**
	 * ��һ������ key ���߼��򣬲���������浽 destkey
	 * @param destkey {const char*} Ŀ���ַ�������� key
	 * @param keys Դ�ַ������󼯺�
	 * @return {int}
	 */
	int bitop_or(const char* destkey, const std::vector<string>& keys);
	int bitop_or(const char* destkey, const std::vector<const char*>& keys);
	int bitop_or(const char* destkey, const char* key, ...);
	int bitop_or(const char* destkey, const char* keys[], size_t size);

	/**
	 * ��һ������ key ���߼���򣬲���������浽 destkey
	 * @param destkey {const char*} Ŀ���ַ�������� key
	 * @param keys Դ�ַ������󼯺�
	 * @return {int}
	 */
	int bitop_xor(const char* destkey, const std::vector<string>& keys);
	int bitop_xor(const char* destkey, const std::vector<const char*>& keys);
	int bitop_xor(const char* destkey, const char* key, ...);
	int bitop_xor(const char* destkey, const char* keys[], size_t size);

	/////////////////////////////////////////////////////////////////////

	/**
	 * ͬʱ����һ������ key-value ��
	 * @param objs key-value �Լ���
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool mset(const std::map<string, string>& objs);

	bool mset(const std::vector<string>& keys,
		const std::vector<string>& values);

	bool mset(const char* keys[], const char* values[], size_t argc);
	bool mset(const char* keys[], const size_t keys_len[],
		const char* values[], const size_t values_len[], size_t argc);

	/////////////////////////////////////////////////////////////////////

	/**
	 * ���ҽ������и��� key ��������ʱͬʱ����һ������ key-value ��
	 * @param objs key-value �Լ���
	 * @return {int} ����ֵ�������£�
	 *  -1���������ַ�������
	 *   0����ӵ� key ������������һ���Ѿ�����
	 *   1����ӳɹ�
	 */
	int msetnx(const std::map<string, string>& objs);

	int msetnx(const std::vector<string>& keys,
		const std::vector<string>& values);

	int msetnx(const char* keys[], const char* values[], size_t argc);
	int msetnx(const char* keys[], const size_t keys_len[],
		const char* values[], const size_t values_len[], size_t argc);

	/////////////////////////////////////////////////////////////////////

	/**
	 * ��������(һ������)���� key ��ֵ����������� key ���棬��ĳ�� key �����ڣ�
	 * ��ô��� key ���ؿմ���ӽ����������
	 * @param keys {const std::vector<string>&} �ַ��� key ����
	 * @param out {std::vector<string>*} �ǿ�ʱ�洢�ַ���ֵ�������飬���ڲ�����
	 *  �� key Ҳ��洢һ���մ�����
	 * @return {bool} �����Ƿ�ɹ��������ɹ������ͨ��������һ�ַ�ʽ������ݣ�
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
	bool mget(const std::vector<string>& keys,
		std::vector<string>* out = NULL);
	bool mget(const std::vector<const char*>& keys,
		std::vector<string>* out = NULL);

	bool mget(std::vector<string>* result, const char* first_key, ...);
	bool mget(const char* keys[], size_t argc,
		std::vector<string>* out = NULL);
	bool mget(const char* keys[], const size_t keys_len[], size_t argc,
		std::vector<string>* out = NULL);

	/////////////////////////////////////////////////////////////////////

	/**
	 * �� key �д��������ֵ��һ
	 * 1����� key �����ڣ���ô key ��ֵ���ȱ���ʼ��Ϊ 0 ��Ȼ����ִ�� INCR ������
	 * 2�����ֵ������������ͣ����ַ������͵�ֵ���ܱ�ʾΪ���֣���ô����һ������
	 * 3����������ֵ������ 64 λ(bit)�з������ֱ�ʾ֮��
	 * @param key {const char*} �ַ�������� key
	 * @param result {long long int*} �ǿ�ʱ�洢�������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool incr(const char* key, long long int* result = NULL);

	/**
	 * �� key �������ֵ�������� increment
	 * 1����� key �����ڣ���ô key ��ֵ���ȱ���ʼ��Ϊ 0 ��Ȼ����ִ�� INCRBY ����
	 * 2�����ֵ������������ͣ����ַ������͵�ֵ���ܱ�ʾΪ���֣���ô����һ������
	 * 3����������ֵ������ 64 λ(bit)�з������ֱ�ʾ֮��
	 * @param key {const char*} �ַ�������� key
	 * @param inc {long long int} ����ֵ
	 * @param result {long long int*} �ǿ�ʱ�洢�������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool incrby(const char* key, long long int inc,
		long long int* result = NULL);

	/**
	 * Ϊ key ���������ֵ���ϸ���������
	 * 1) ��� key �����ڣ���ô INCRBYFLOAT ���Ƚ� key ��ֵ��Ϊ 0 ����ִ�мӷ�����
	 * 2) �������ִ�гɹ�����ô key ��ֵ�ᱻ����Ϊ��ִ�мӷ�֮��ģ���ֵ��������ֵ��
	 *    ���ַ�������ʽ���ظ�������
	 * 3) ������Ҳ���ֻ�ܱ�ʾС����ĺ�ʮ��λ
	 * @param key {const char*} �ַ�������� key
	 * @param inc {double} ����ֵ
	 * @param result {double*} �ǿ�ʱ�洢�������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool incrbyfloat(const char* key, double inc, double* result = NULL);

	/**
	 * �� key �д��������ֵ��һ
	 * 1) ��� key �����ڣ���ô key ��ֵ���ȱ���ʼ��Ϊ 0 ��Ȼ����ִ�� DECR ����
	 * 2) ���ֵ������������ͣ����ַ������͵�ֵ���ܱ�ʾΪ���֣���ô����һ������
	 * 3) ��������ֵ������ 64 λ(bit)�з������ֱ�ʾ֮��
	 * @param key {const char*} �ַ�������� key
	 * @param result {long long int*} �ǿ�ʱ�洢�������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool decr(const char* key, long long int* result = NULL);

	/**
	 * �� key �������ֵ��ȥ���� decrement
	 * 1) ��� key �����ڣ���ô key ��ֵ���ȱ���ʼ��Ϊ 0 ��Ȼ����ִ�� DECRBY ����
	 * 2) ���ֵ������������ͣ����ַ������͵�ֵ���ܱ�ʾΪ���֣���ô����һ������
	 * 3) ��������ֵ������ 64 λ(bit)�з������ֱ�ʾ֮��
	 * @param key {const char*} �ַ�������� key
	 * @param dec {long long int} ����ֵ
	 * @param result {long long int*} �ǿ�ʱ�洢�������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool decrby(const char* key, long long int dec,
		long long int* result = NULL);

private:
	int bitop(const char* op, const char* destkey,
		const std::vector<string>& keys);
	int bitop(const char* op, const char* destkey,
		const std::vector<const char*>& keys);
	int bitop(const char* op, const char* destkey,
		const char* keys[], size_t size);

	bool incoper(const char* cmd, const char* key, long long int inc,
		long long int* result);

};

} // namespace acl
