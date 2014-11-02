#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/db/db_service.hpp"

namespace acl {

class ACL_CPP_API db_service_sqlite : public db_service
{
public:
	/**
	 * ��Ϊ sqlite ���ݿ�ʱ�Ĺ��캯��
	 * @param dbname {const char*} ���ݿ���
	 * @param dbfile {const char*} ���ݿ��ļ���(����һЩ��Ƕ���ݿ�����)
	 * @param dblimit {size_t} ���ݿ����ӳصĸ�������
	 * @param nthread {int} ���̳߳ص�����߳���
	 * @param win32_gui {bool} �Ƿ��Ǵ��������Ϣ������ǣ����ڲ���
	 *  ͨѶģʽ�Զ�����Ϊ���� WIN32 ����Ϣ��������Ȼ����ͨ�õ��׽�
	 *  ��ͨѶ��ʽ
	 */
	db_service_sqlite(const char* dbname, const char* dbfile,
		size_t dblimit = 100, int nthread = 2, bool win32_gui = false);
	~db_service_sqlite();

private:
	// ���ݿ�����
	string dbname_;
	// sqlite ���ݿ��ļ���
	string dbfile_;

	// ���ി�麯��
	virtual db_handle* db_create(void);
};

}
