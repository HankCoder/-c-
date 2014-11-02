#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/connpool/connect_manager.hpp"

namespace acl
{

/**
 * memcache �ͻ����������ӳع�����
 */
class ACL_CPP_API memcache_manager : public acl::connect_manager
{
public:
	memcache_manager();
	virtual ~memcache_manager();

protected:
	/**
	 * ���ി�麯���������������ӳض���
	 * @param addr {const char*} ������������ַ����ʽ��ip:port
	 * @param count {int} ���ӳصĴ�С����
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 */
	virtual connect_pool* create_pool(const char* addr, int count,
		size_t idx);
};

} // namespace acl
