#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/db/db_service.hpp"

namespace acl {

class ACL_CPP_API db_service_mysql : public db_service
{
	/**
	 * ��Ϊ mysql ���ݿ�ʱ�Ĺ��캯��
	 * @param dbaddr {const char*} mysql ��������ַ
	 * @param dbname {const char*} ���ݿ���
	 * @param dbuser {const char*} ���ݿ��û���
	 * @param dbpass {const char*} ���ݿ��û�����
	 * @param dbflags {unsigned long} ���ݿ����ӱ�־λ
	 * @param auto_commit {bool} �����޸�ʱ�Ƿ��Զ��ύ
	 * @param conn_timeout {int} ���ݿ����ӳ�ʱʱ��
	 * @param rw_timeout {int} ���ݿ����ʱIO��д��ʱʱ��
	 * @param dblimit {size_t} ���ݿ����ӳصĸ�������
	 * @param nthread {int} ���̳߳ص�����߳���
	 * @param win32_gui {bool} �Ƿ��Ǵ��������Ϣ������ǣ����ڲ���
	 *  ͨѶģʽ�Զ�����Ϊ���� WIN32 ����Ϣ��������Ȼ����ͨ�õ��׽�
	 *  ��ͨѶ��ʽ
	 */
	db_service_mysql(const char* dbaddr, const char* dbname,
		const char* dbuser, const char* dbpass,
		unsigned long dbflags = 0, bool auto_commit = true,
		int conn_timeout = 60, int rw_timeout = 60,
		size_t dblimit = 100, int nthread = 2, bool win32_gui = false);

	~db_service_mysql(void);

private:
	// ���ݿ��������ַ
	string dbaddr_;
	// ���ݿ���
	string dbname_;
	// ���ݿ��û���
	string dbuser_;
	// ���ݿ��û�����
	string dbpass_;
	// ���ݿ����ӱ�־λ
	unsigned long dbflags_;
	// �޸�����ʱ�Ƿ��Զ��ύ����
	bool auto_commit_;
	// �������ݿⳬʱʱ��
	int conn_timeout_;
	// ���ݿ����ʱ�Ķ�д��ʱʱ��
	int rw_timeout_;

	// ���ി�麯��
	virtual db_handle* db_create(void);
};

}
