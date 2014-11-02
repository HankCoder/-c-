#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#include "acl_cpp/ipc/ipc_service.hpp"
#include "acl_cpp/stdlib/string.hpp"

namespace acl {

typedef enum
{
	DB_OK,
	DB_ERR_OPEN,
	DB_ERR_EXEC_SQL,
} db_status;

//////////////////////////////////////////////////////////////////////////

class db_rows;

class ACL_CPP_API db_query
{
public:
	db_query(void) {}
	virtual ~db_query(void) {}

	virtual void on_error(db_status status) = 0;
	virtual void on_ok(const db_rows* rows, int affected) = 0;

	/**
	 * ����������ϻ����ʱ���ڲ�������̻��Զ����� destroy �ӿڣ�
	 * ��������ڸýӿ��ڽ���һЩ�ͷŹ��̣����䵱�ö����Ƕ�̬����ʱ��
	 * ����Ӧ���ڸú����� delete this ��ɾ���Լ�����Ϊ�ú������տ϶�
	 * �ᱻ���ã��������಻Ӧ�������ط�������������
	 */
	virtual void destroy(void) {}
protected:
private:
};

//////////////////////////////////////////////////////////////////////////

class db_handle;
class aio_socket_stream;

/**
 * ���ݿ�����࣬������һ���첽���ݿ���������࣬����������ڵ��̱߳�����
 * һ�����������̹߳���
 */
class ACL_CPP_API db_service : public ipc_service
{
public:
	/**
	 * ��Ϊ sqlite ���ݿ�ʱ�Ĺ��캯��
	 * @param dblimit {size_t} ���ݿ����ӳصĸ�������
	 * @param nthread {int} ���̳߳ص�����߳���
	 * @param win32_gui {bool} �Ƿ��Ǵ��������Ϣ������ǣ����ڲ���
	 *  ͨѶģʽ�Զ�����Ϊ���� WIN32 ����Ϣ��������Ȼ����ͨ�õ��׽�
	 *  ��ͨѶ��ʽ
	 */
	db_service(size_t dblimit = 100, int nthread = 2, bool win32_gui = false);
	virtual ~db_service(void);

	/**
	 * �첽ִ�� SQL ��ѯ���
	 * @param sql {const char*} Ҫִ�еı�׼ SQL ���
	 * @param query {db_query*} ��������ִ�н���������
	 */
	void sql_select(const char* sql, db_query* query);

	/**
	 * �첽ִ�� SQL �������
	 * @param sql {const char*} Ҫִ�еı�׼ SQL ���
	 * @param query {db_query*} ��������ִ�н���������
	 */
	void sql_update(const char* sql, db_query* query);

	/**
	 * �����ݿ����ӳ���������Ӷ���
	 * @param db {db_handle*} ���ݿ����Ӷ���
	 */
	void push_back(db_handle* db);
protected:
	/**
	 * ��Ҫ����ʵ�ִ˺��������������ݿ����
	 * @return {db_handle*}
	 */
	virtual db_handle* db_create() = 0;

	/**
	 * �����麯�������������ӵ���ʱ����ص��˺���
	 * @param client {aio_socket_stream*} ���յ����µĿͻ�������
	 */
	virtual void on_accept(aio_socket_stream* client);

#ifdef WIN32
	/**
	 * �����麯�������յ����������̵߳� win32 ��Ϣʱ�Ļص�����
	 * @param hWnd {HWND} ���ھ��
	 * @param msg {UINT} �û��Զ�����Ϣ��
	 * @param wParam {WPARAM} ����
	 * @param lParam {LPARAM} ����
	 */
	virtual void win32_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

private:
	// ���ݿ������
	std::list<db_handle*> dbpool_;

	// ���ݿ����ӳصĸ�������
	size_t dblimit_;

	// ��ǰ���ݿ����ӳصĸ���
	size_t dbsize_;
};

} // namespace acl
