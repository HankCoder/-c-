#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/redis/redis_result.hpp"

namespace acl
{

class redis_client;
class redis_pool;
class redis_cluster;

/**
 * redis �ͻ���������Ĵ��鸸��
 */
class ACL_CPP_API redis_command
{
public:
	redis_command();
	redis_command(redis_client* conn);
	redis_command(redis_cluster* cluster, size_t max_conns);
	virtual ~redis_command() = 0;

	/**
	 * ���ظ�ʹ��һ���̳��� redis_command ��������� redis ʱ����Ҫ��
	 * ��һ�ε���ǰ���ñ��������ͷ��ϴβ�������ʱ����
	 * @param save_slot {bool} �����ü�Ⱥģʽʱ���ò��������Ƿ���Ҫ����
	 *  �����ϣ��ֵ������������� redis ��������е� key ֵ���䣬����Ա�����
	 *  ��ϣ��ֵ�Լ����ڲ����½��м���Ĵ���
	 */
	void reset(bool save_slot = false);

	/**
	 * ��ʹ�����ӳط�ʽʱ��ͨ���������������ӳػ�õ����Ӷ���
	 * (redis_client)�� redis �ͻ���������й���
	 */
	void set_client(redis_client* conn);

	/**
	 * ��õ�ǰ redis �ͻ�����������Ӷ���
	 * @return {redis_client*} ���� NULL ��ʾû�����Ӷ����뵱ǰ���������
	 *  ���а�
	 */
	redis_client* get_client() const
	{
		return conn_;
	}

	/**
	 * �������ӳؼ�Ⱥ������
	 * @param cluster {redis_cluster*}
	 * @param max_conns {size_t} ���ڲ���̬�������ӳض���ʱ����ֵָ��ÿ����̬����
	 *  �����ӳص������������
	 */
	void set_cluster(redis_cluster* cluster, size_t max_conns);

	/**
	 * ��������õ����ӳؼ�Ⱥ������
	 * @return {redis_cluster*}
	 */
	redis_cluster* get_cluster() const
	{
		return cluster_;
	}

	/**
	 * ����ڴ�ؾ�������ڴ���� redis_command �ڲ�����
	 * @return {dbuf_pool*}
	 */
	dbuf_pool* get_pool() const
	{
		return pool_;
	}

	/**
	 * ��õ�ǰ���������������
	 * @return {redis_result_t}
	 */
	redis_result_t result_type() const;

	/**
	 * ������ֵΪ REDIS_RESULT_STATUS ����ʱ������������״̬��Ϣ
	 * @return {const char*} ���� "" ��ʾ����
	 */
	const char* result_status() const;

	/**
	 * ������ʱ����ֵΪ REDIS_RESULT_ERROR ���ͣ����������س�����Ϣ
	 * @return {const char*} ���ؿմ� "" ��ʾû�г�����Ϣ
	 */
	const char* result_error() const;

	/**
	 * ��õ�ǰ������洢�Ķ���ĸ���, �÷������Ի�ý��Ϊ������������
	 * (get_child/get_value) ����Ҫ������Ԫ�صĸ���
	 * @return {size_t} ����ֵ��洢���͵Ķ�Ӧ��ϵ���£�
	 *  REDIS_RESULT_ERROR: 1
	 *  REDIS_RESULT_STATUS: 1
	 *  REDIS_RESULT_INTEGER: 1
	 *  REDIS_RESULT_STRING: > 0 ʱ��ʾ���ַ������ݱ��зֳɷ������ڴ��ĸ���
	 *  REDIS_RESULT_ARRAY: children_->size()
	 */
	size_t result_size() const;

	/**
	 * ������ֵΪ REDIS_RESULT_INTEGER ����ʱ�����������ض�Ӧ�� 32 λ����ֵ
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 * @return {int}
	 */
	int result_number(bool* success = NULL) const;

	/**
	 * ������ֵΪ REDIS_RESULT_INTEGER ����ʱ�����������ض�Ӧ�� 64 λ����ֵ
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 * @return {long long int}
	 */
	long long int result_number64(bool* success = NULL) const;

	/**
	 * ���ض�Ӧ�±������(���������ͷ� REDIS_RESULT_ARRAY ʱ��
	 * @param i {size_t} �����±�
	 * @param len {size_t*} ��Ϊ�� NULL ָ��ʱ�洢���������ݵĳ���
	 * @return {const char*} ���� NULL ��ʾ�±�Խ��
	 */
	const char* get_result(size_t i, size_t* len = NULL) const;

	/**
	 * �жϵ�ǰ���󶨵� redis ����������(redis_client) �����Ƿ��Ѿ��رգ�
	 * ֻ���ڲ��� conn_ ������ǿ�ʱ���ô˺�����������
	 * @return {bool}
	 */
	bool eof() const;

	/**
	 * ��ñ��� redis �������̵Ľ��
	 * @return {redis_result*}
	 */
	const redis_result* get_result() const;

	/**
	 * ����ѯ���Ϊ�������ʱ���ñ��������һ������Ԫ�ض���
	 * @param i {size_t} ���������±�ֵ
	 * @return {const redis_result*} �����������������Ϊ�ջ����ʱ
	 *  �÷������� NULL
	 */
	const redis_result* result_child(size_t i) const;

	/**
	 * ���� redis-server ��õ�������һ���ַ������͵Ľ����ʱ�����Ե���
	 * ���������ĳ��ָ���±�λ�õ�����
	 * @param i {size_t} �±꣨�� 0 ��ʼ��
	 * @param len {size_t*} ����ָ��ǿգ���洢�����ؽ���ĳ��ȣ�������
	 *  �������طǿ�ָ��ʱ��Ч��
	 * @return {const char*} ���ض�Ӧ�±��ֵ�������� NULL ʱ��ʾ���±�û
	 *  ��ֵ��Ϊ�˱�֤ʹ���ϵİ�ȫ�ԣ����ص��������ܱ�֤������� \0 ��β��
	 *  �ڼ������ݳ���ʱ�������ý�β������Ϊ�˼��ݶ��������Σ������߻���
	 *  Ӧ��ͨ�����ص� len ��ŵĳ���ֵ��������ݵ���ʵ����
	 */
	const char* result_value(size_t i, size_t* len = NULL) const;

	/////////////////////////////////////////////////////////////////////
	/**
	 * �����Ƿ���������ݽ��з�Ƭ�������Ϊ true ���ڲ�����װ����Э���ʱ�򲻻�
	 * ���������ݿ�������װ��һ�������Ĵ����ݿ�
	 * @param on {bool} �ڲ�Ĭ��ֵΪ false
	 */
	void set_slice_request(bool on);

	/**
	 * �����Ƿ����Ӧ���ݽ��з�Ƭ�������Ϊ true �򵱷������ķ������ݱȽϴ�ʱ��
	 * �����ݽ��з�Ƭ���ֳ�һЩ�����������ݿ�
	 * @param on {bool} �ڲ�Ĭ��ֵΪ false
	 */
	void set_slice_respond(bool on);

protected:
	const redis_result* run(size_t nchild = 0);
	const redis_result* run(redis_cluster* cluster, size_t nchild);

	void build_request(size_t argc, const char* argv[], size_t lens[]);
	void reset_request();
	const redis_result** scan_keys(const char* cmd, const char* key,
		int& cursor, size_t& size, const char* pattern,
		const size_t* count);
	/*******************************************************************/

	void build(const char* cmd, const char* key,
		const std::map<string, string>& attrs);
	void build(const char* cmd, const char* key,
		const std::map<string, const char*>& attrs);

	void build(const char* cmd, const char* key,
		const std::map<int, string>& attrs);
	void build(const char* cmd, const char* key,
		const std::map<int, const char*>& attrs);

	void build(const char* cmd, const char* key,
		const std::vector<string>& names,
		const std::vector<string>& values);
	void build(const char* cmd, const char* key,
		const std::vector<const char*>& names,
		const std::vector<const char*>& values);

	void build(const char* cmd, const char* key,
		const std::vector<int>& names,
		const std::vector<string>& values);
	void build(const char* cmd, const char* key,
		const std::vector<int>& names,
		const std::vector<const char*>& values);

	void build(const char* cmd, const char* key,
		const char* names[], const char* values[], size_t argc);
	void build(const char* cmd, const char* key,
		const int names[], const char* values[], size_t argc);
	void build(const char* cmd, const char* key,
		const char* names[], const size_t names_len[],
		const char* values[], const size_t values_len[], size_t argc);

	/*******************************************************************/

	void build(const char* cmd, const char* key,
		const std::vector<string>& names);
	void build(const char* cmd, const char* key,
		const std::vector<const char*>& names);
	void build(const char* cmd, const char* key,
		const std::vector<int>& names);

	void build(const char* cmd, const char* key,
		const char* names[], size_t argc);
	void build(const char* cmd, const char* key,
		const int names[], size_t argc);
	void build(const char* cmd, const char* key,
		const char* names[], const size_t lens[], size_t argc);

protected:
	int get_number(bool* success = NULL);
	long long int get_number64(bool* success = NULL);
	int get_number(std::vector<int>& out);
	int get_number64(std::vector<long long int>& out);
	bool check_status(const char* success = "OK");

	int get_status(std::vector<bool>& out);
	const char* get_status();

	int get_string(string& buf);
	int get_string(string* buf);
	int get_string(char* buf, size_t size);
	int get_strings(std::vector<string>& result);
	int get_strings(std::vector<string>* result);
	int get_strings(std::map<string, string>& result);
	int get_strings(std::vector<string>& names,
		std::vector<string>& values);
	int get_strings(std::vector<const char*>& names,
		std::vector<const char*>& values);

	/************************** common *********************************/
protected:
	dbuf_pool* pool_;

	// ���ݼ�ֵ�����ϣ��ֵ
	void hash_slot(const char* key);
	void hash_slot(const char* key, size_t len);

private:
	redis_client* conn_;
	redis_cluster* cluster_;
	size_t max_conns_;
	unsigned long long used_;
	int slot_;
	int redirect_max_;
	int redirect_sleep_;

	redis_client* peek_conn(redis_cluster* cluster, int slot);
	redis_client* redirect(redis_cluster* cluster, const char* addr);
	const char* get_addr(const char* info);

private:
	/************************** request ********************************/
	bool slice_req_;
	string* request_buf_;
	redis_request* request_obj_;
	size_t  argv_size_;
	const char**  argv_;
	size_t* argv_lens_;
	size_t  argc_;

	void argv_space(size_t n);
	void build_request1(size_t argc, const char* argv[], size_t lens[]);
	void build_request2(size_t argc, const char* argv[], size_t lens[]);

private:
	/************************** respond ********************************/
	bool slice_res_;
	const redis_result* result_;
};

} // namespace acl
