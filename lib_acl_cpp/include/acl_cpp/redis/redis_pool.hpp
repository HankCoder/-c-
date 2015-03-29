#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/connpool/connect_pool.hpp"

namespace acl
{

/**
 * redis ���ӳ��࣬����̳��� connect_pool���� connect_pool ������ͨ�õ��й�
 * TCP ���ӳص�ͨ�÷�����
 * redis connection pool inherting from connect_pool, which includes
 * TCP connection pool methods.
 */
class ACL_CPP_API redis_pool : public connect_pool
{
public:
	/**
	 * ���캯��
	 * @param addr {const char*} ����˵�ַ����ʽ��ip:port
	 * @param count {int} ���ӳص��������������
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 */
	redis_pool(const char* addr, int count, size_t idx = 0);

	virtual ~redis_pool();

	/**
	 * �����������ӳ�ʱʱ�估���� IO ��д��ʱʱ��(��)
	 * @param conn_timeout {int} ���ӳ�ʱʱ��
	 * @param rw_timeout {int} ���� IO ��д��ʱʱ��(��)
	 * @return {redis_pool&}
	 */
	redis_pool& set_timeout(int conn_timeout, int rw_timeout);

protected:
	/**
	 * ���ി�麯��: ���ô˺�����������һ���µ�����
	 * @return {connect_client*}
	 */
	virtual connect_client* create_connect();

private:
	int   conn_timeout_;
	int   rw_timeout_;
};

} // namespace acl
