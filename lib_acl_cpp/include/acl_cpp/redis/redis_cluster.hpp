#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/connpool/connect_manager.hpp"

namespace acl
{

class redis_pool;

class ACL_CPP_API redis_cluster : public connect_manager
{
public:
	/**
	 * ���캯��
	 * @param conn_timeout {int} ���������ӳ�ʱʱ��(��)
	 * @param rw_timeout {int}������ IO ��д��ʱʱ��(��)
	 * @param max_slot {int} ��ϣ�����ֵ
	 */
	redis_cluster(int conn_timeout, int rw_timeout, int max_slot = 16384);
	virtual ~redis_cluster();

	/**
	 * ���ݹ�ϣ��ֵ��ö�Ӧ�����ӳ�
	 * @param slot {int} ��ϣ��ֵ
	 * @return {redis_pool*} �����Ӧ�Ĺ�ϣ�۲������򷵻� NULL
	 */
	redis_pool* peek_slot(int slot);

	/**
	 * ���ù�ϣ��ֵ��Ӧ�� redis �����ַ
	 * @param slot {int} ��ϣ��ֵ
	 * @param addr {const char*} redis ��������ַ
	 */
	void set_slot(int slot, const char* addr);

	/**
	 * �����ϣ�۶�Ӧ�� redis �����ַ���Ա������¼���λ��
	 * @param slot {int} ��ϣ��ֵ
	 */
	void clear_slot(int slot);

	/**
	 * ��ù�ϣ�����ֵ
	 * @return {int}
	 */
	int get_max_slot() const
	{
		return max_slot_;
	}

	//////////////////////////////////////////////////////////////////////
	/**
	 * ����Э���ض�������ķ�ֵ��Ĭ��ֵΪ 15
	 * @param max {int} �ض��������ֵ��ֻ�е���ֵ > 0 ʱ����Ч
	 */
	void set_redirect_max(int max);

	/**
	 * ����Э���ض�������ķ�ֵ
	 * @return {int}
	 */
	int get_redirect_max() const
	{
		return redirect_max_;
	}

	/**
	 * ���ض������ >= 2 ʱ�������ߵ�ʱ��(��)��Ĭ��ֵΪ 1 �룬��������
	 * �ô��ǵ�һ�� redis �����������ߺ������ӽ������Ϊ���������Ҫ
	 * ʱ���(�� redis.conf �е� cluster-node-timeout ���������)������
	 * Ϊ�����ض���Ĵ�����Χ�ڲ�������Ҫ�ȴ��ӽ������Ϊ�����
	 * @param n {int} ÿ���ض���ʱ����Ϣʱ��(��)��Ĭ��ֵΪ 1 ��
	 */
	void set_redirect_sleep(int n);

	/**
	 * ��� set_redirect_sleep ���õĻ�Ĭ�ϵ�ʱ��
	 * @return {int} ��λΪ��
	 */
	int get_redirect_sleep() const
	{
		return redirect_sleep_;
	}

protected:
	/**
	 * ���ി�麯���������������ӳض���
	 * @param addr {const char*} ������������ַ����ʽ��ip:port
	 * @param count {int} ���ӳصĴ�С����
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 */
	virtual acl::connect_pool* create_pool(const char* addr,
		int count, size_t idx);
private:
	int   conn_timeout_;
	int   rw_timeout_;
	int   max_slot_;
	const char**  slot_addrs_;
	std::vector<char*> addrs_;
	int   redirect_max_;
	int   redirect_sleep_;
};

} // namespace acl
