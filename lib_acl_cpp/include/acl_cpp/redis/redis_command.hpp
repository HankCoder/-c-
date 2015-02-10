#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/redis/redis_result.hpp"

namespace acl
{

class redis_client;

/**
 * redis �ͻ���������Ĵ��鸸��
 */
class ACL_CPP_API redis_command
{
public:
	redis_command(redis_client* conn = NULL);
	virtual ~redis_command() = 0;

	/**
	 * ���ظ�ʹ��һ���̳��� redis_command ��������� redis ʱ����Ҫ��
	 * ��һ�ε���ǰ���ñ��������ͷ��ϴβ�������ʱ����
	 */
	void reset();

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
	 * ��õ�ǰ���������������
	 * @return {redis_result_t}
	 */
	redis_result_t get_type() const;

	/**
	 * ������ֵΪ REDIS_RESULT_STATUS ����ʱ������������״̬��Ϣ
	 * @return {const char*} ���� "" ��ʾ����
	 */
	const char* get_status() const;

	/**
	 * ������ʱ����ֵΪ REDIS_RESULT_ERROR ���ͣ����������س�����Ϣ
	 * @return {const char*} ���ؿմ� "" ��ʾû�г�����Ϣ
	 */
	const char* get_error() const;

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
	size_t get_size() const;

	/**
	 * ������ֵΪ REDIS_RESULT_INTEGER ����ʱ�����������ض�Ӧ�� 32 λ����ֵ
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 * @return {int}
	 */
	int get_integer(bool* success = NULL) const;

	/**
	 * ������ֵΪ REDIS_RESULT_INTEGER ����ʱ�����������ض�Ӧ�� 64 λ����ֵ
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 * @return {long long int}
	 */
	long long int get_integer64(bool* success = NULL) const;

	/**
	 * ���ض�Ӧ�±������(���������ͷ� REDIS_RESULT_ARRAY ʱ��
	 * @param i {size_t} �����±�
	 * @param len {size_t*} ��Ϊ�� NULL ָ��ʱ�洢���������ݵĳ���
	 * @return {const char*} ���� NULL ��ʾ�±�Խ��
	 */
	const char* get(size_t i, size_t* len = NULL) const;

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
	const redis_result* get_child(size_t i) const;

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
	const char* get_value(size_t i, size_t* len = NULL) const;

protected:
	redis_client* conn_;

	const redis_result** scan_keys(const char* cmd, const char* key,
		int& cursor, size_t& size, const char* pattern,
		const size_t* count);
};

} // namespace acl
