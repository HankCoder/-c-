#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/connpool/connect_manager.hpp"

namespace acl
{

class ACL_CPP_API redis_manager : public connect_manager
{
public:
	redis_manager();
	virtual ~redis_manager();

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
};

} // namespace acl
