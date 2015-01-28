#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;

class ACL_CPP_API redis_list : public redis_command
{
public:
	redis_list(redis_client* conn = NULL);
	~redis_list();

	/////////////////////////////////////////////////////////////////////

	/**
	 * �� key �б�����е���һ��Ԫ�ض���name/value�ԣ�������������ʽ��ͷ��������
	 * ��������� key ����ʱ�������� key ���Ⱥ�˳�����μ������б�������һ��
	 * �ǿ��б��ͷԪ��
	 * @param result {std::pair<string, string>&} �洢���Ԫ�ض��󣬸ö����
	 *  ��һ���ַ�����ʾ���б����� key���ڶ���Ϊ�ö����ͷ��Ԫ��
	 * @param timeout {size_t} �ȴ�����ʱ�䣨�룩���ڳ�ʱʱ����û�л��Ԫ�ض���
	 *  �򷵻� false�������ֵΪ 0 ��һֱ�ȴ������Ԫ�ض�������
	 * @param first_key {const char*} ��һ���ǿ��ַ����� key ��
	 * @return {bool} �Ƿ�����ͷ��Ԫ�ض���������� false �������¿���ԭ��
	 *  1������
	 *  2����һ�� key ���б����
	 *  3��key �����ڻ�ʱδ���Ԫ�ض���

	 */
	bool blpop(std::pair<string, string>& result, size_t timeout,
		const char* first_key, ...);
	bool blpop(const std::vector<const char*>& keys, size_t timeout,
		std::pair<string, string>& result);
	bool blpop(const std::vector<string>& keys, size_t timeout,
		std::pair<string, string>& result);

	/**
	 * ����μ� blpop��Ψһ����Ϊ�÷�������β��Ԫ�ض���
	 * @sess blpop
	 */
	bool brpop(std::pair<string, string>& result, size_t timeout,
		const char* first_key, ...);
	bool brpop(const std::vector<const char*>& keys, size_t timeout,
		std::pair<string, string>& result);
	bool brpop(const std::vector<string>& keys, size_t timeout,
		std::pair<string, string>& result);

	/**
	 * ����ʽִ����������������
	 * 1) ���б� src �е����һ��Ԫ��(βԪ��)�����������ظ��ͻ��ˡ�
	 * 2) �� src ������Ԫ�ز��뵽�б� dst ����Ϊ dst �б�ĵ�ͷԪ��
	 * @param src {const char*} Դ�б���� key
	 * @param dst {const char*} Ŀ���б���� key
	 * @param buf {string*} �ǿ�ʱ�洢 src ��β��Ԫ�� key ֵ
	 * @param timeout {size_t} �ȴ���ʱʱ�䣬���Ϊ 0 ��һֱ�ȴ�ֱ�������ݻ����
	 * @return {bool} ���� src �б��гɹ�����β��Ԫ�ز����� dst �б�ͷ����
	 *  �÷������� true������ false ��ʾ��ʱ������� src/dst ��һ�����б����
	 * @see rpoplpush
	 */
	bool brpoplpush(const char* src, const char* dst, size_t timeout,
		string* buf = NULL);

	/**
	 * ���� key ��Ӧ���б�����У�ָ���±��Ԫ��
	 * @param key {const char*} �б����� key
	 * @param idx {size_t} �±�ֵ
	 * @param buf {string&} �洢���
	 * @return {bool} ���� true ���������ɹ�����ʱ�� buf ���ݷǿ��������ȷ�����
	 *  ָ���±��Ԫ�أ���� buf.empty()��ʾû�л��Ԫ�أ����� false ʱ��������ʧ��
	 */
	bool lindex(const char* key, size_t idx, string& buf);

	/**
	 * ���б�����н�һ����Ԫ�������ָ��Ԫ�ص�ǰ��
	 * @param key {const char*} �б����� key
	 * @param pivot {const char*} �б�����е�һ��ָ��Ԫ����
	 * @param value {const char*} �µ�Ԫ����
	 * @reutrn {int} ���ظ��б�����Ԫ�ظ������������£�
	 *  -1 -- ��ʾ����� key ���б����
	 *  >0 -- ��ǰ�б�����Ԫ�ظ���
	 */
	int linsert_before(const char* key, const char* pivot,
		const char* value);
	int linsert_before(const char* key, const char* pivot,
		size_t pivot_len, const char* value, size_t value_len);

	/**
	 * ���б�����н�һ����Ԫ�������ָ��Ԫ�صĺ���
	 * @param key {const char*} �б����� key
	 * @param pivot {const char*} �б�����е�һ��ָ��Ԫ����
	 * @param value {const char*} �µ�Ԫ����
	 * @reutrn {int} ���ظ��б�����Ԫ�ظ������������£�
	 *  -1 -- ��ʾ����� key ���б����
	 *  >0 -- ��ǰ�б�����Ԫ�ظ���
	 */
	int linsert_after(const char* key, const char* pivot,
		const char* value);
	int linsert_after(const char* key, const char* pivot,
		size_t pivot_len, const char* value, size_t value_len);

	/**
	 * ����ָ���б�����Ԫ�ظ���
	 * @param key {const char*} �б����� key
	 * @return {int} ����ָ���б����ĳ��ȣ���Ԫ�ظ�������
	 */
	int llen(const char* key);

	/**
	 * ���б�������Ƴ�������ͷ��Ԫ��
	 * @param key {const char*} Ԫ�ض���� key
	 * @param buf {string&} �洢������Ԫ��ֵ
	 * @return {int} ����ֵ���壺1 -- ��ʾ�ɹ�����һ��Ԫ�أ�-1 -- ��ʾ�������
	 *  ������б���󣬻�ö����Ѿ�Ϊ��
	 */
	int lpop(const char* key, string& buf);

	/**
	 * ��һ������ֵԪ�ز��뵽�б���� key �ı�ͷ
	 * @param key {const char*} �б����� key
	 * @param first_value {const char*} ��һ���ǿ��ַ������ñ�ε��б�����һ��
	 *  ������Ϊ NULL
	 * @return {int} ����������ǰ�б�����е�Ԫ�ظ��������� -1 ��ʾ������ key
	 *  ������б���󣬵��� key ������ʱ������µ��б���󼰶����е�Ԫ��
	 */
	int lpush(const char* key, const char* first_value, ...);
	int lpush(const char* key, const char* values[], size_t argc);
	int lpush(const char* key, const std::vector<string>& values);
	int lpush(const char* key, const std::vector<const char*>& values);
	int lpush(const char* key, const char* values[], size_t lens[],
		size_t argc);

	/**
	* ��һ���µ��б�����Ԫ��������Ѿ����ڵ�ָ���б�����ͷ���������б����
	* ������ʱ�����
	 * @param key {const char*} �б����� key
	 * @param value {const char*} �¼ӵ��б�����Ԫ��
	 * @return {int} ���ص�ǰ�б�����Ԫ�ظ������������£�
	 *  -1��������� key ���б����
	 *   0���� key ���󲻴���
	 *  >0��������ǰ�б�����е�Ԫ�ظ���
	 */
	int lpushx(const char* key, const char* value);
	int lpushx(const char* key, const char* value, size_t len);

	/**
	 * �����б� key ��ָ�������ڣ������䣩��Ԫ�أ�������ƫ���� start �� end ָ����
	 * �±���ʼֵ�� 0 ��ʼ��-1 ��ʾ���һ���±�ֵ
	 * @param key {const char*} �б����� key
	 * @param start {int} ��ʼ�±�ֵ
	 * @param end {int} �����±�ֵ
	 * @param result {std::vector<string>&} �洢�б������ָ�������Ԫ�ؼ���
	 * @return {bool} �����Ƿ�ɹ��������� false ��ʾ����� key ���б����
	 *  ������
	 *  1) �� start = 0, end = 10 ʱ��ָ�����±� 0 ��ʼ�� 10 �� 11 ��Ԫ��
	 *  2) �� start = -1, end = -2 ʱ��ָ�������һ��Ԫ�صڵ����ڶ����� 2 ��Ԫ�� 
	 */
	bool lrange(const char* key, int start, int end,
		std::vector<string>& result);

	/**
	 * ����Ԫ��ֵ���б�������Ƴ�ָ��������Ԫ��
	 * @param key {const char*} �б����� key
	 * @param count {int} �Ƴ�Ԫ�ص��������ƣ�count �ĺ������£�
	 *  count > 0 : �ӱ�ͷ��ʼ���β�������Ƴ��� value ��ȵ�Ԫ�أ�����Ϊ count
	 *  count < 0 : �ӱ�β��ʼ���ͷ�������Ƴ��� value ��ȵ�Ԫ�أ�����Ϊ count �ľ���ֵ
	 *  count = 0 : �Ƴ����������� value ��ȵ�ֵ
	 * @param value {const char*} ָ����Ԫ��ֵ����Ҫ���б�����б����������ֵ�Ƚ�
	 * @return {int} ���Ƴ��Ķ�������������ֵ�������£�
	 *  -1�������� key ������б����
	 *   0��key �����ڻ��Ƴ���Ԫ�ظ���Ϊ 0
	 *  >0�����ɹ��Ƴ���Ԫ������
	 */
	int lrem(const char* key, int count, const char* value);
	int lrem(const char* key, int count, const char* value, size_t len);

	/**
	 * ���б� key �±�Ϊ idx ��Ԫ�ص�ֵ����Ϊ value���� idx ����������Χ�����
	 * һ�����б�( key ������)���� lset ʱ������һ������
	 * @param key {const char*} �б����� key
	 * @param idx {int} �±�λ�ã���Ϊ��ֵʱ���β����ͷβ����λ���������˳��ʽ��
	 *  �磺0 ��ʾͷ����һ��Ԫ�أ�-1 ��ʾβ����ʼ�ĵ�һ��Ԫ��
	 * @param value {const char*} Ԫ����ֵ
	 * @return {bool} �� key ���б����� key �����ڻ� idx ������Χ�򷵻� false
	 */
	bool lset(const char* key, int idx, const char* value);
	bool lset(const char* key, int idx, const char* value, size_t len);

	/**
	 * ��ָ�����б���󣬸����޶����䷶Χ����ɾ����������ƫ���� start �� end ָ����
	 * �±���ʼֵ�� 0 ��ʼ��-1 ��ʾ���һ���±�ֵ
	 * @param key {const char*} �б����� key
	 * @param start {int} ��ʼ�±�ֵ
	 * @param end {int} �����±�ֵ
	 * @return {bool} �����Ƿ�ɹ��������� false ʱ��ʾ�����ָ���� key �����
	 *  �б���󣻵��ɹ�ɾ���� key ���󲻴���ʱ�򷵻� true
	 */
	bool ltrim(const char* key, int start, int end);

	/**
	 * ���б�������Ƴ�������β��Ԫ��
	 * @param key {const char*} Ԫ�ض���� key
	 * @param buf {string&} �洢������Ԫ��ֵ
	 * @return {int} ����ֵ���壺1 -- ��ʾ�ɹ�����һ��Ԫ�أ�-1 -- ��ʾ�������
	 *  ������б���󣬻�ö����Ѿ�Ϊ��
	 */
	int rpop(const char* key, string& buf);

	/**
	 * ��һ��ԭ��ʱ���ڣ���������ʽִ����������������
	 * ���б� src �е����һ��Ԫ��(βԪ��)�����������ظ��ͻ��ˡ�
	 * �� src ������Ԫ�ز��뵽�б� dst ����Ϊ dst �б�ĵ�ͷԪ��
	 * @param src {const char*} Դ�б���� key
	 * @param dst {const char*} Ŀ���б���� key
	 * @param buf {string*} �ǿ�ʱ�洢 src ��β��Ԫ�� key ֵ
	 * @return {bool} ���� src �б��гɹ�����β��Ԫ�ز����� dst �б�ͷ����
	 *  �÷������� true������ false ����� src/dst ��һ�����б����
	 */
	bool rpoplpush(const char* src, const char* dst, string* buf = NULL);

	/**
	 * ��һ������ֵԪ�ز��뵽�б���� key �ı�β
	 * @param key {const char*} �б����� key
	 * @param first_value {const char*} ��һ���ǿ��ַ������ñ�ε��б�����һ��
	 *  ������Ϊ NULL
	 * @return {int} ����������ǰ�б�����е�Ԫ�ظ��������� -1 ��ʾ������ key
	 *  ������б���󣬵��� key ������ʱ������µ��б���󼰶����е�Ԫ��
	 */
	int rpush(const char* key, const char* first_value, ...);
	int rpush(const char* key, const char* values[], size_t argc);
	int rpush(const char* key, const std::vector<string>& values);
	int rpush(const char* key, const std::vector<const char*>& values);
	int rpush(const char* key, const char* values[], size_t lens[],
		size_t argc);

	/**
	 * ��һ���µ��б�����Ԫ��������Ѿ����ڵ�ָ���б�����β���������б����
	 * ������ʱ�����
	 * @param key {const char*} �б����� key
	 * @param value {const char*} �¼ӵ��б�����Ԫ��
	 * @return {int} ���ص�ǰ�б�����Ԫ�ظ������������£�
	 *  -1��������� key ���б����
	 *   0���� key ���󲻴���
	 *  >0��������ǰ�б�����е�Ԫ�ظ���
	 */
	int rpushx(const char* key, const char* value);
	int rpushx(const char* key, const char* value, size_t len);

private:
	int linsert(const char* key, const char* pos, const char* pivot,
		size_t pivot_len, const char* value, size_t value_len);
	int pushx(const char* cmd, const char* key,
		const char* value, size_t len);
	int pop(const char* cmd, const char* key, string& buf);
	bool bpop(const char* cmd, const std::vector<const char*>& keys,
		size_t timeout, std::pair<string, string>& result);
	bool bpop(const char* cmd, const std::vector<string>& keys,
		size_t timeout, std::pair<string, string>& result);
	bool bpop(std::pair<string, string>& result);
};

} // namespace acl
