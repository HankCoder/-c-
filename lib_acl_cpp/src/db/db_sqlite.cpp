#include "acl_stdafx.hpp"
#include "sqlite3.h"
#include "acl_cpp/stdlib/charset_conv.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/db/db_sqlite.hpp"

#ifdef HAS_SQLITE

#if defined(WIN32) || defined(USE_DYNAMIC)
 typedef char* (*sqlite3_libversion_fn)(void);
 typedef int   (*sqlite3_open_fn)(const char*, sqlite3**);
 typedef int   (*sqlite3_close_fn)(sqlite3*);
 typedef int   (*sqlite3_get_table_fn)(sqlite3*, const char*, char***, int*, int*, char**);
 typedef void  (*sqlite3_free_table_fn)(char**);
 typedef int   (*sqlite3_busy_handler_fn)(sqlite3*, int(*)(void*,int), void*);
 typedef const char* (*sqlite3_errmsg_fn)(sqlite3*);
 typedef int   (*sqlite3_errcode_fn)(sqlite3*);
 typedef int   (*sqlite3_changes_fn)(sqlite3*);
 typedef int   (*sqlite3_total_changes_fn)(sqlite3*);

 static sqlite3_libversion_fn __sqlite3_libversion = NULL;
 static sqlite3_open_fn __sqlite3_open = NULL;
 static sqlite3_close_fn __sqlite3_close = NULL;
 static sqlite3_get_table_fn __sqlite3_get_table = NULL;
 static sqlite3_free_table_fn __sqlite3_free_table = NULL;
 static sqlite3_busy_handler_fn __sqlite3_busy_handler = NULL;
 static sqlite3_errmsg_fn __sqlite3_errmsg = NULL;
 static sqlite3_errcode_fn __sqlite3_errcode = NULL;
 static sqlite3_changes_fn __sqlite3_changes = NULL;
 static sqlite3_total_changes_fn __sqlite3_total_changes = NULL;

 static acl_pthread_once_t __sqlite_once = ACL_PTHREAD_ONCE_INIT;
 static ACL_DLL_HANDLE __sqlite_dll = NULL;

 // �����˳��ͷŶ�̬���صĿ�
 static void __sqlite_dll_unload(void)
 {
	 if (__sqlite_dll != NULL)
	 {
		 acl_dlclose(__sqlite_dll);
		 __sqlite_dll = NULL;
		 logger("sqlite3.dll unload ok");
	 }
 }

 // ��̬���� sqlite3.dll ��
 static void __sqlite_dll_load(void)
 {
	if (__sqlite_dll != NULL)
		logger_fatal("__sqlite_dll not null");

	__sqlite_dll = acl_dlopen("sqlite3.dll");
	if (__sqlite_dll == NULL)
		logger_fatal("load sqlite3.dll error: %s", acl_last_serror());

	__sqlite3_libversion = (sqlite3_libversion_fn)
		acl_dlsym(__sqlite_dll, "sqlite3_libversion");
	if (__sqlite3_libversion == NULL)
		logger_fatal("load sqlite3_libversion from sqlite3.dll error: %s",
			acl_last_serror());

	__sqlite3_open = (sqlite3_open_fn)
		acl_dlsym(__sqlite_dll, "sqlite3_open");
	if (__sqlite3_open == NULL)
		logger_fatal("load sqlite3_open from sqlite3.dll error: %s",
			acl_last_serror());

	__sqlite3_close = (sqlite3_close_fn)
		acl_dlsym(__sqlite_dll, "sqlite3_close");
	if (__sqlite3_close == NULL)
		logger_fatal("load sqlite3_close from sqlite3.dll error: %s",
			acl_last_serror());

	__sqlite3_get_table = (sqlite3_get_table_fn)
		acl_dlsym(__sqlite_dll, "sqlite3_get_table");
	if (__sqlite3_get_table == NULL)
		logger_fatal("load sqlite3_get_table from sqlite3.dll error: %s",
			acl_last_serror());

	__sqlite3_free_table = (sqlite3_free_table_fn)
		acl_dlsym(__sqlite_dll, "sqlite3_free_table");
	if (__sqlite3_free_table == NULL)
		logger_fatal("load sqlite3_free_table from sqlite3.dll error: %s",
			acl_last_serror());

	__sqlite3_busy_handler = (sqlite3_busy_handler_fn)
		acl_dlsym(__sqlite_dll, "sqlite3_busy_handler");
	if (__sqlite3_busy_handler == NULL)
		logger_fatal("load sqlite3_busy_handler from sqlite3.dll error: %s",
			acl_last_serror());

	__sqlite3_errmsg = (sqlite3_errmsg_fn)
		acl_dlsym(__sqlite_dll, "sqlite3_errmsg");
	if (__sqlite3_errmsg == NULL)
		logger_fatal("load sqlite3_errmsg from sqlite3.dll error: %s",
			acl_last_serror());

	__sqlite3_errcode = (sqlite3_errcode_fn)
		acl_dlsym(__sqlite_dll, "sqlite3_errcode");
	if (__sqlite3_errcode == NULL)
		logger_fatal("load sqlite3_errcode from sqlite3.dll error: %s",
			acl_last_serror());

	__sqlite3_changes = (sqlite3_changes_fn)
		acl_dlsym(__sqlite_dll, "sqlite3_changes");
	if (__sqlite3_changes == NULL)
		logger_fatal("load sqlite3_changes from sqlite3.dll error: %s",
			acl_last_serror());

	__sqlite3_total_changes = (sqlite3_total_changes_fn)
		acl_dlsym(__sqlite_dll, "sqlite3_total_changes");
	if (__sqlite3_total_changes == NULL)
		logger_fatal("load sqlite3_total_changes from sqlite3.dll error: %s",
			acl_last_serror());

	logger("sqlite3.dll loaded");
	atexit(__sqlite_dll_unload);
 }
#else
# define __sqlite3_libversion sqlite3_libversion
# define __sqlite3_open sqlite3_open
# define __sqlite3_close sqlite3_close
# define __sqlite3_get_table sqlite3_get_table
# define __sqlite3_free_table sqlite3_free_table
# define __sqlite3_busy_handler sqlite3_busy_handler
# define __sqlite3_errmsg sqlite3_errmsg
# define __sqlite3_errcode sqlite3_errcode
# define __sqlite3_changes sqlite3_changes
# define __sqlite3_total_changes sqlite3_total_changes
#endif

namespace acl
{

//////////////////////////////////////////////////////////////////////////
// sqlite �ļ�¼�����Ͷ���

class db_sqlite_rows : public db_rows
{
public:
	db_sqlite_rows(char** results, int nrow, int ncolumn)
	{
		results_ = results;

		int   n = 0;

		// ȡ��������
		for (int j = 0; j < ncolumn; j++)
		{
			names_.push_back(results[j]);
			n++;
		}

		// ��ʼȡ�����������ݽ�������붯̬������
		for (int i = 0; i < nrow; i++)
		{
			db_row* row = NEW db_row(names_);
			for (int j = 0; j < ncolumn; j++)
			{
				row->push_back(results[n]);
				n++;
			}
			rows_.push_back(row);
		}
	}

	~db_sqlite_rows()
	{
		if (results_)
			__sqlite3_free_table(results_);
	}

private:
	char** results_;
};

//////////////////////////////////////////////////////////////////////////

db_sqlite::db_sqlite(const char* dbfile)
: db_(NULL)
, dbfile_(dbfile)
, conv_(NULL)
{
	acl_assert(dbfile && *dbfile);
#if defined(WIN32) || defined(USE_DYNAMIC)
	acl_pthread_once(&__sqlite_once, __sqlite_dll_load);
#endif
}

db_sqlite::~db_sqlite(void)
{
	close();
	delete conv_;
}

const char* db_sqlite::version() const
{
	return (__sqlite3_libversion());
}

static int sqlite_busy_callback(void *ctx acl_unused, int nretry acl_unused)
{
	acl_doze(10);
	return (1);
}

const char* db_sqlite::dbtype() const
{
	static const char* type = "sqlite";
	return type;
}

int db_sqlite::get_errno() const
{
	if (db_)
		return __sqlite3_errcode(db_);
	else
		return -1;
}

const char* db_sqlite::get_error() const
{
	if (db_)
		return __sqlite3_errmsg(db_);
	else
		return "sqlite not opened yet!";
}
bool db_sqlite::open(const char* local_charset /* = "GBK" */)
{
	// ������ݿ��Ѿ��򿪣���ֱ�ӷ��� true
	if (db_ != NULL)
		return (true);

	string buf;

	if (local_charset && strcasecmp(local_charset, "UTF-8"))
	{
		local_charset_ = local_charset;
		conv_ = NEW charset_conv();
	}

	const char* ptr;

	// ת���� [UTF-8] �����ʽ

	if (conv_ == NULL)
		ptr = dbfile_.c_str();
	else if (conv_->convert(local_charset_.c_str(), "UTF-8",
		dbfile_.c_str(), dbfile_.length(), &buf) == false)
	{
		logger_error("charset convert(%s) from %s to UTF-8 error",
			dbfile_.c_str(), local_charset_.c_str());
		return (false);
	}
	else
		ptr = buf.c_str();

	// �� sqlite ���ݿ�
	int   ret = __sqlite3_open(ptr, &db_);
	if (ret != SQLITE_OK)
	{
		logger_error("open %s error(%s, %d)", dbfile_.c_str(),
			__sqlite3_errmsg(db_), __sqlite3_errcode(db_));
		__sqlite3_close(db_);
		db_ = NULL;

		return (false);
	}

	// �� SQLITE æʱ�Ļص�����
	__sqlite3_busy_handler(db_, sqlite_busy_callback, NULL);

	// �ر� SQLITE ʵʱˢ�´��̵����ԣ��Ӷ��������
	set_conf("PRAGMA synchronous = off");
	return (true);
}

bool db_sqlite::is_opened() const
{
	return (db_ != NULL ? true : false);
}

bool db_sqlite::close()
{
	if (db_ == NULL)
		return (false);

	// �ر� sqlite ���ݿ�
	int   ret = __sqlite3_close(db_);
	if (ret == SQLITE_BUSY)
	{
		logger_error("close %s error SQLITE_BUSY", dbfile_.c_str());
		return (false);
	}

	db_ = NULL;
	return (true);
}

bool db_sqlite::set_conf(const char* pragma)
{
	bool ret = exec_sql(pragma);
	if (result_)
		free_result();
	return (ret);
}

const char* db_sqlite::get_conf(const char* pragma, string& out)
{
	bool ret = exec_sql(pragma);
	if (ret == false)
		return (NULL);
	else if (length() == 0)
	{
		free_result();
		return (NULL);
	}
	else
	{
		const db_row* row = (*this)[(size_t) 0];
		acl_assert(row != NULL);
		const char* ptr = (*row)[(size_t) 0];
		if (ptr == NULL)
		{
			free_result();
			return (NULL);
		}
		out = ptr;
		free_result();
		return (out.c_str());
	}
}

// sqlite ��һЩ����ѡ��
const char* __pragmas[] =
{
	"PRAGMA auto_vacuum",
	"PRAGMA cache_size",
	"PRAGMA case_sensitive_like",
	"PRAGMA count_changes",
	"PRAGMA default_cache_size",
	"PRAGMA default_synchronous",
	"PRAGMA empty_result_callbacks",
	"PRAGMA encoding",
	"PRAGMA full_column_names",
	"PRAGMA fullfsync",
	"PRAGMA legacy_file_format",
	"PRAGMA locking_mode",
	"PRAGMA page_size",
	"PRAGMA max_page_count",
	"PRAGMA read_uncommitted",
	"PRAGMA short_column_names",
	"PRAGMA synchronous",
	"PRAGMA temp_store",
	"PRAGMA temp_store_directory",
	NULL
};

void db_sqlite::show_conf(const char* pragma /* = NULL */)
{
	if (db_ == NULL)
	{
		logger_error("db not open yet!");
		return;
	}

	string buf;

	if (pragma != NULL)
	{
		if (get_conf(pragma, buf) != NULL)
			printf("%s: %s\r\n", pragma, buf.c_str());
		else
			printf("%s: UNKNOWN\r\n", pragma);
		return;
	}

	int   i;
	for (i = 0; __pragmas[i] != NULL; i++)
	{
		if (get_conf(__pragmas[i], buf) != NULL)
			printf("%s: %s\r\n", __pragmas[i], buf.c_str());
		else
			printf("%s: UNKNOWN\r\n", __pragmas[i]);
	}
}

bool db_sqlite::tbl_exists(const char* tbl_name)
{
	if (tbl_name == NULL || *tbl_name == 0)
	{
		logger_error("tbl_name null");
		return (false);
	}

	acl::string sql;
	sql.format("select count(*) from sqlite_master"
		" where type='table' and name='%s'", tbl_name);

	if (exec_sql(sql.c_str()) == false)
	{
		free_result();
		return (false);
	}

	if (length() == 0)
	{
		free_result();
		return (false);
	}
	else
	{
		const db_row* row = (*this)[0];
		acl_assert(row != NULL);

		int  n = row->field_int((size_t) 0, (int) 0);
		free_result();

		if (n == 0)
			return (false);
		return (true);
	}
}

bool db_sqlite::sql_select(const char* sql)
{
	return (exec_sql(sql));
}

bool db_sqlite::sql_update(const char* sql)
{
	return (exec_sql(sql));
}

bool db_sqlite::exec_sql(const char* sql)
{
	// ���뽫�ϴεĲ�ѯ���ɾ��
	if (result_)
	{
		logger_warn("You forgot free result of last query");
		free_result();
	}

	if (sql == NULL || *sql == 0)
	{
		logger_error("invalid params");
		return (false);
	}
	else if (db_ == NULL)
	{
		logger_error("db not open yet!");
		return (false);
	}

	char** results = NULL, *err;
	int   nrow, ncolumn;

	// ִ�� sqlite �Ĳ�ѯ����
	int   ret = __sqlite3_get_table(db_, sql, &results,
		&nrow, &ncolumn, &err);
	if (ret != SQLITE_OK)
	{
		logger_error("sqlites_get_table(%s) error(%s)",
			sql, __sqlite3_errmsg(db_));
		__sqlite3_free_table(results);
		return (false);
	}

	if (nrow > 0)
		result_ = NEW db_sqlite_rows(results, nrow, ncolumn);
	else if (results)
	{
		result_ = NULL;
		__sqlite3_free_table(results);
	}
	return (true);
}

int db_sqlite::affect_count() const
{
	if (db_ == NULL)
	{
		logger_error("db not opened yet!");
		return (-1);
	}

	return (__sqlite3_changes(db_));
}

int db_sqlite::affect_total_count() const
{
	return (__sqlite3_total_changes(db_));
}

} // namespace acl

#else

namespace acl
{

db_sqlite::db_sqlite(const char*) {}
db_sqlite::~db_sqlite(void) {}
const char* db_sqlite::dbtype() const { return NULL; }
bool db_sqlite::open(const char*) { return false; }
bool db_sqlite::is_opened() const { return false; }
bool db_sqlite::close(void) { return false; }
bool db_sqlite::tbl_exists(const char*) { return false; }
bool db_sqlite::sql_select(const char*) { return false; }
bool db_sqlite::sql_update(const char*) { return false; }
int db_sqlite::affect_count() const { return 0; }
int db_sqlite::get_errno() const { return -1; }
const char* db_sqlite::get_error() const { return "unknown"; }

}  // namespace acl

#endif  // HAS_SQLITE
