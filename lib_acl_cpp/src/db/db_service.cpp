#include "acl_stdafx.hpp"
#include <assert.h>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/ipc/ipc_client.hpp"
#include "acl_cpp/db/db_handle.hpp"
#include "acl_cpp/db/db_sqlite.hpp"
#include "acl_cpp/db/db_service.hpp"

namespace acl
{

struct DB_IPC_DAT 
{
	db_handle* db;
	const db_rows* rows;
	int  affected_rows;
	db_query* query;
};

//////////////////////////////////////////////////////////////////////////
// �������̵߳Ĵ�����

class db_ipc_request : public ipc_request
{
public:
	db_ipc_request(db_handle* db, const char* sql, db_query* query, bool has_res)
	: db_(db)
	, sql_(sql)
	, query_(query)
	, has_res_(has_res)
	{

	}

	~db_ipc_request(void)
	{

	}

protected:
	// ���� ipc_request ���Զ����ô˻ص������������
	virtual void run(ipc_client* ipc)
	{
		DB_IPC_DAT data;

		data.db = db_;
		data.query = query_;
		data.rows = NULL;
		data.affected_rows = 0;

		if (db_->open() == false)
			ipc->send_message(DB_ERR_OPEN, &data, sizeof(data));
		else if (has_res_)
		{
			if (db_->sql_select(sql_.c_str()) == false)
				ipc->send_message(DB_ERR_EXEC_SQL, &data, sizeof(data));
			else
			{
				data.rows = db_->get_result();
				ipc->send_message(DB_OK, &data, sizeof(data));
			}
		}
		else if (db_->sql_update(sql_.c_str()) == false)
			ipc->send_message(DB_ERR_EXEC_SQL, &data, sizeof(data));
		else
		{
			data.rows = db_->get_result();
			// �޸Ĳ�������Ҫȡһ�� SQL ����Ӱ�������
			data.affected_rows = db_->affect_count();
			ipc->send_message(DB_OK, &data, sizeof(data));
		}
		// ��Ϊ����������Ƕ�̬�����ģ�������Ҫ�ͷ�
		delete this;
	}

#ifdef WIN32

	// ������ӿڣ�ʹ���߳̿�����ִ��������������̷߳��� WIN32 ������Ϣ

	virtual void run(HWND hWnd)
	{
		DB_IPC_DAT* data = (DB_IPC_DAT*) acl_mymalloc(sizeof(DB_IPC_DAT));

		data->db = db_;
		data->query = query_;
		data->rows = NULL;
		data->affected_rows = 0;

		if (db_->open() == false)
			::PostMessage(hWnd, DB_ERR_OPEN + WM_USER, 0, (LPARAM) data);
		else if (has_res_)
		{
			if (db_->sql_select(sql_.c_str()) == false)
				::PostMessage(hWnd, DB_ERR_EXEC_SQL + WM_USER, 0, (LPARAM) data);
			{
				data->rows = db_->get_result();
				::PostMessage(hWnd, DB_OK + WM_USER, 0, (LPARAM) data);
			}
		} else if (db_->sql_update(sql_.c_str()) == false)
			::PostMessage(hWnd, DB_ERR_EXEC_SQL + WM_USER, 0, (LPARAM) data);
		else
		{
			data->rows = db_->get_result();
			// �޸Ĳ�������Ҫȡһ�� SQL ����Ӱ�������
			data->affected_rows = db_->affect_count();
			::PostMessage(hWnd, DB_OK + WM_USER, 0, (LPARAM) data);
			//::SendMessage(hWnd, DB_OK + WM_USER, 0, (LPARAM) data);
		}
		// ��Ϊ����������Ƕ�̬�����ģ�������Ҫ�ͷ�
		delete this;
	}
#endif
private:
	db_handle* db_;
	acl::string sql_;
	db_query* query_;
	bool has_res_;
};

//////////////////////////////////////////////////////////////////////////
// �����߳������̳߳�֮��� IPC ͨ���ඨ��

class db_ipc : public ipc_client
{
public:
	db_ipc(db_service* dbs, acl_int64 magic)
	: ipc_client(magic)
	, dbservice_(dbs)
	{

	}

	~db_ipc(void)
	{

	}

	virtual void on_message(int nMsg, void* data, int dlen acl_unused)
	{
		DB_IPC_DAT* dat = (DB_IPC_DAT*) data;
		db_query* query = dat->query;

		switch (nMsg)
		{
		case DB_OK:
			query->on_ok(dat->rows, dat->affected_rows);
			break;
		case DB_ERR_OPEN:
			query->on_error(DB_ERR_OPEN);
			break;
		case DB_ERR_EXEC_SQL:
			query->on_error(DB_ERR_EXEC_SQL);
			break;
		default:
			break;
		}

		dat->db->free_result();
		dbservice_->push_back(dat->db);
		query->destroy();
	}
protected:
	virtual void on_close(void)
	{
		delete this;
	}
private:
	db_service* dbservice_;
};

//////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <process.h>
#endif

db_service::db_service(size_t dblimit /* = 100 */, int nthread /* = 2 */,
	bool win32_gui /* = false */)
: ipc_service(nthread, win32_gui)
, dbsize_(0)
{
	// �������̳߳ط�ʽ�������ݿ����ӳص����ֵ��Ӧ�����߳���
	if (nthread > 1)
		dblimit_ = (int) dblimit > nthread ? nthread : dblimit;
	else
		dblimit_ = dblimit;
#ifdef WIN32
	magic_ = _getpid() + time(NULL);
#else
	magic_ = getpid() + time(NULL);
#endif
}

db_service::~db_service(void)
{
	std::list<db_handle*>::iterator it = dbpool_.begin();
	for (; it != dbpool_.end(); ++it)
		delete (*it);
}

void db_service::on_accept(acl::aio_socket_stream* client)
{
	ACL_SOCKET fd = client->get_socket();
	// �ڴ˴����÷���˽��յ����׽ӿڵ� SO_LINGER ѡ�
	// �Ա�֤���׽ӿڹرպ�����Դ�ܵõ������ͷţ���Ȼһ��
	// �������ô�ѡ�����Σ���������ǵ������ֻ�н��յ�
	// �����Ŀͻ������ݺ�Ż���ùرղ���������Ӧ�ò���
	// ������ݷ��Ͳ�ȫ�����⣬�мǣ���Ӧ�ڿͻ��˵Ĺر�
	// ���������� SO_LINGER ѡ��Է�����δ��������
	// �ڷ�������ý������ӵ� SO_LINGER ѡ������ڲ���
	// ϵͳ���ٻ����׽ӿ���Դ
	acl_tcp_so_linger(fd, 1, 0);

	ipc_client* ipc = NEW db_ipc(this, magic_);
	ipc->open(client);

	// ��ӷ����̵߳���Ϣ����

	ipc->append_message(DB_OK);
	ipc->append_message(DB_ERR_OPEN);
	ipc->append_message(DB_ERR_EXEC_SQL);

	// �첽�ȴ���Ϣ
	ipc->wait();
}

#ifdef WIN32

void db_service::win32_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DB_IPC_DAT* dat = NULL;
	db_query* query = NULL;

	switch (msg - WM_USER)
	{
	case DB_OK:
		dat = (DB_IPC_DAT*) lParam;
		query = dat->query;
		query->on_ok(dat->rows, dat->affected_rows);
		break;
	case DB_ERR_OPEN:
		dat = (DB_IPC_DAT*) lParam;
		query = dat->query;
		query->on_error(DB_ERR_OPEN);
		break;
	case DB_ERR_EXEC_SQL:
		dat = (DB_IPC_DAT*) lParam;
		query = dat->query;
		query->on_error(DB_ERR_EXEC_SQL);
		break;
	default:
		break;
	}

	if (dat)
	{
		dat->db->free_result();
		push_back(dat->db);
		query->destroy();

		// �ڲ��� WIN32 ��Ϣʱ�ö���ռ��Ƕ�̬����ģ�������Ҫ�ͷ�
		acl_myfree(dat);
	}
}

#endif

void db_service::sql_select(const char* sql, db_query* query)
{
	assert(sql && *sql);

	db_handle* db;

	std::list<db_handle*>::iterator it = dbpool_.begin();
	if (it == dbpool_.end())
		db = db_create();
	else
	{
		db = *it;
		dbpool_.erase(it);
	}

	// �������̵߳��������
	db_ipc_request* ipc_req = NEW db_ipc_request(db, sql, query, true);

	// ���û��� ipc_service �������
	request(ipc_req);
}

void db_service::sql_update(const char* sql, db_query* query)
{
	assert(sql && *sql);

	db_handle* db;

	std::list<db_handle*>::iterator it = dbpool_.begin();
	if (it == dbpool_.end())
		db = db_create();
	else
	{
		db = *it;
		dbpool_.erase(it);
	}

	// �������̵߳��������
	db_ipc_request* ipc_req = NEW db_ipc_request(db, sql, query, false);

	// ���û��� ipc_service �������
	request(ipc_req);
}

void db_service::push_back(db_handle* db)
{
	if (dbsize_ >= dblimit_)
		delete db;
	else
	{
		dbsize_++;
		dbpool_.push_back(db);
	}
}

} // namespace acl
