#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/db/db_handle.hpp"

typedef struct st_mysql MYSQL;

namespace acl {

class ACL_CPP_API db_mysql : public db_handle
{
public:
	db_mysql(const char* dbaddr, const char* dbname,
		const char* dbuser, const char* dbpass,
		unsigned long dbflags = 0, bool auto_commit = true,
		int conn_timeout = 60, int rw_timeout = 60);
	~db_mysql(void);

	/**
	 * ��� mysql �ͻ��˿�İ汾��
	 * @return {unsigned long}
	 */
	unsigned long mysql_libversion() const;

	/**
	 * ��� mysql �ͻ��˿����Ϣ
	 * @return {const char*}
	 */
	const char* mysql_client_info() const;

	/**
	 * ֱ�ӻ�� mysql �����Ӿ����������� NULL ���ʾ mysql ��û�д�
	 * �����ʱ�ڲ��Զ��ر��� mysql ����
	 * @return {MYSQL*}
	 */
	MYSQL* get_conn() const
	{
		return conn_;
	}

	/************************************************************************/
	/*            ����Ϊ���� db_handle ����ӿ�                             */
	/************************************************************************/

	/**
	 * �������ݿ����������
	 * @return {const char*}
	 */
	virtual const char* dbtype() const;

	/**
	 * ����ϴ����ݿ�����ĳ�������
	 * @return {int}
	 */
	virtual int get_errno() const;

	/**
	 * ����ϴ����ݿ�����ĳ��������
	 * @return {const char*}
	 */
	virtual const char* get_error() const;

	/**
	 * ���� db_handle �Ĵ���ӿ�
	 * @param local_charset {const char*} �����ַ���
	 * @return {bool} ���Ƿ�ɹ�
	 */
	virtual bool open(const char* local_charset = "GBK");

	/**
	 * ���� db_handle �Ĵ���ӿڣ����ݿ��Ƿ��Ѿ�����
	 * @return {bool} ���� true �������ݿ��Ѿ�����
	 */
	virtual bool is_opened() const;

	/**
	 * ���� db_handle �Ĵ���ӿ�
	 * @return {bool} �ر��Ƿ�ɹ�
	 */
	virtual bool close(void);

	/**
	 * ���� db_handle �Ĵ���ӿڣ��������ʵ�ִ˽ӿ������ж����ݱ��Ƿ����
	 * @return {bool} �Ƿ����
	 */
	virtual bool tbl_exists(const char* tbl_name);

	/**
	 * ���� db_handle �Ĵ���ӿ�
	 * @param sql {const char*} ��׼�� SQL ��䣬�ǿգ�����һ����Ҫע���
	 *  SQL �����뾭��ת�崦���Է�ֹ SQL ע�빥��
	 * @return {bool} ִ���Ƿ�ɹ�
	 */
	virtual bool sql_select(const char* sql);

	/**
	 * ���� db_handle �Ĵ���ӿ�
	 * @param sql {const char*} ��׼�� SQL ��䣬�ǿգ�����һ����Ҫע���
	 *  SQL �����뾭��ת�崦���Է�ֹ SQL ע�빥��
	 * @return {bool} ִ���Ƿ�ɹ�
	 */
	virtual bool sql_update(const char* sql);

	/**
	 * ���� db_handle �Ĵ���ӿڣ��ϴ� sql ����Ӱ��ļ�¼����
	 * @return {int} Ӱ���������-1 ��ʾ����
	 */
	virtual int affect_count() const;
protected:
private:
	char* dbaddr_;  // ���ݿ������ַ
	char* dbname_;  // ���ݿ���
	char* dbuser_;  // ���ݿ��˺�
	char* dbpass_;  // ���ݿ��˺�����

	unsigned long dbflags_;
	int   conn_timeout_;
	int   rw_timeout_;
	bool  auto_commit_;
	MYSQL* conn_;

	bool sane_mysql_query(const char* sql);
};

} // namespace acl
