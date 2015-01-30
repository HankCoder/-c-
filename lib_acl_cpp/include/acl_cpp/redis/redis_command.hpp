#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

class redis_client;
class redis_result;

/**
 * redis �ͻ���������Ĵ��鸸��
 */
class ACL_CPP_API redis_command
{
public:
	redis_command(redis_client* conn = NULL);
	virtual ~redis_command() = 0;

	/**
	 * ���ظ�ʹ��һ���̳��� redis_command ��������� redis ʱ����Ҫ����һ��
	 * ����ǰ���ñ��������ͷ��ϴβ�������ʱ����
	 */
	void reset();

	/**
	 * ��ʹ�����ӳط�ʽʱ��ͨ���������������ӳػ�õ����Ӷ���(redis_client)��
	 * redis �ͻ���������й���
	 */
	void set_client(redis_client* conn);

	/**
	 * ��õ�ǰ redis �ͻ�����������Ӷ���
	 * @return {redis_client*}
	 */
	redis_client* get_client() const
	{
		return conn_;
	}

	/**
	 * �жϵ�ǰ���󶨵� redis ����������(redis_client) �����Ƿ��Ѿ��رգ�ֻ��
	 * �ڲ��� conn_ ������ǿ�ʱ���ô˺�����������
	 * @return {bool}
	 */
	bool eof() const;

	/**
	 * ��ñ��� redis �������̵Ľ��
	 * @return {redis_result*}
	 */
	const redis_result* get_result() const;

	/**
	 * ����ѯ���Ϊ�������ʱ���ñ�������ò�ѯ������ĸ������÷������Ի�ý��Ϊ����
	 * �������� (get_child/get_value) ����Ҫ������Ԫ�صĸ���
	 * @return {size_t}
	 */
	size_t get_size() const;

	/**
	 * ����ѯ���Ϊ�������ʱ���ñ��������һ������Ԫ�ض���
	 * @param i {size_t} ���������±�ֵ
	 * @return {const redis_result*} �����������������Ϊ�ջ����ʱ�÷���
	 *  ���� NULL
	 */
	const redis_result* get_child(size_t i) const;

	/**
	 * ���� redis-server ��õ�������һ���ַ������͵Ľ����ʱ�����Ե��ñ��������
	 * ĳ��ָ���±�λ�õ�����
	 * @param i {size_t} �±꣨�� 0 ��ʼ��
	 * @param len {size_t*} ����ָ��ǿգ���洢�����ؽ���ĳ��ȣ������÷���
	 *  ���طǿ�ָ��ʱ��Ч��
	 * @return {const char*} ���ض�Ӧ�±��ֵ�������� NULL ʱ��ʾ���±�û��ֵ��
	 *  Ϊ�˱�֤ʹ���ϵİ�ȫ�ԣ����ص��������ܱ�֤������� \0 ��β���ڼ������ݳ���
	 *  ʱ�������ý�β������Ϊ�˼��ݶ��������Σ������߻���Ӧ��ͨ�����ص� len ���
	 *  �ĳ���ֵ��������ݵ���ʵ����
	 */
	const char* get_value(size_t i, size_t* len = NULL) const;

protected:
	redis_client* conn_;

	const redis_result** scan_keys(const char* cmd, const char* key,
		int& cursor, size_t& size, const char* pattern,
		const size_t* count);
};

} // namespace acl
