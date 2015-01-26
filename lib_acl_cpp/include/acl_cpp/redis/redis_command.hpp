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
	 * ��ñ��� redis �������̵Ľ��
	 * @return {redis_result*}
	 */
	const redis_result* get_result() const;

protected:
	redis_client* conn_;

	const redis_result** scan_keys(const char* cmd, const char* key,
		int& cursor, size_t& size, const char* pattern,
		const size_t* count);
};

} // namespace acl
