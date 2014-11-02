#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>

namespace acl {

class beanstalk;
class locker;

/**
 * beanstalk �ͻ������ӳأ�����ͬʱ���Ӳ�ͬ�� beanstalkd ��������
 * ÿ�� beanstalkd �ж�����ӣ��ڲ��Զ������������������������ƣ�
 * �û�Ӧ���п������ӳص������������
 */
class ACL_CPP_API beanstalk_pool
{
public:
	beanstalk_pool();
	~beanstalk_pool();

	/**
	 * �����ӳ���ȡ��һ�� beanstalkd �Ŀͻ�������
	 * @param addr {const char*} beanstalkd �����ַ(domain:port)
	 * @param clean_watch {bool} ��ȡ�����Ӷ�����Ƿ��Զ�ȡ�����е�
	 *  �ѹ�ע����
	 * @param conn_timeout {int} ���� beanstalkd �ĳ�ʱʱ��
	 * @return {beanstalk*} ���طǿձ�ʾ�����������ʾ����
	 */
	beanstalk* peek(const char* addr, bool clean_watch = true,
		int conn_timeout = 60);

	/**
	 * �����õ� beanstalkd ���ӷŻص����ӳ���
	 * @param client {beanstalk*} beanstalkd �ͻ�������
	 * @param clean_watch {bool} �Ƿ�ȡ���Ѿ���ע�Ķ���
	 * @param keep {bool} ���Ϊ true �� client �Ż������ӳأ�
	 *  �����ͷŸ�����
	 */
	void put(beanstalk* client, bool clean_watch = true,
		bool keep = true);
private:
	locker* lock_;
	typedef std::multimap<string, beanstalk*> bspool;
	typedef bspool::const_iterator pool_cit;
	typedef bspool::iterator pool_it;
	typedef std::pair<pool_it, pool_it> pool_range;

	bspool pool_;
};

} // namespace acl
