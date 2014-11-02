#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/db/db_pool.hpp"

namespace acl {

class db_handle;

class ACL_CPP_API sqlite_pool : public acl::db_pool
{
public:
	/**
	 * ���캯��
	 * @param dbfile {const char*} sqlite ���ݿ�������ļ�
	 * @param dblimit {int} ���ݿ����ӳ��������������
	 */
	sqlite_pool(const char* dbfile, int dblimit = 64);
	~sqlite_pool();
protected:
	// �����麯�����������ݿ����Ӿ��
	virtual db_handle* create();
private:
	// sqlite �����ļ���
	char* dbfile_;
};

} // namespace acl
