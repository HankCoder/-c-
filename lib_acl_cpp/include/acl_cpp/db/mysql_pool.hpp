#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/db/db_pool.hpp"

namespace acl {

class db_handle;

class ACL_CPP_API mysql_pool : public acl::db_pool
{
public:
	/**
	 * ���� mysql ���ݿ�ʱ�Ĺ��캯��
	 * @param dbaddr {const char*} mysql ��������ַ����ʽ��IP:PORT��
	 *  �� UNIX ƽ̨�¿���Ϊ UNIX ���׽ӿ�
	 * @param dbname {const char*} ���ݿ���
	 * @param dbuser {const char*} ���ݿ��û�
	 * @param dbpass {const char*} ���ݿ��û�����
	 * @param dblimit {int} ���ݿ����ӳص��������������
	 * @param dbflags {unsigned long} mysql ���λ
	 * @param auto_commit {bool} �Ƿ��Զ��ύ
	 * @param conn_timeout {int} �������ݿⳬʱʱ��(��)
	 * @param rw_timeout {int} �����ݿ�ͨ��ʱ��IOʱ��(��)
	 */
	mysql_pool(const char* dbaddr, const char* dbname,
		const char* dbuser, const char* dbpass,
		int dblimit = 64, unsigned long dbflags = 0,
		bool auto_commit = true, int conn_timeout = 60,
		int rw_timeout = 60);
	~mysql_pool();
protected:
	// �����麯�����������ݿ����Ӿ��
	virtual db_handle* create();
private:
	char* dbaddr_;  // ���ݿ������ַ
	char* dbname_;  // ���ݿ���
	char* dbuser_;  // ���ݿ��˺�
	char* dbpass_;  // ���ݿ��˺�����
	unsigned long dbflags_;
	int   conn_timeout_;
	int   rw_timeout_;
	bool  auto_commit_;
};

} // namespace acl
