#include "stdafx.h"
#include "http_thread.h"
#include "db_store.h"

db_store::db_store()
{
}

db_store::~db_store()
{
}

// ���ݿ��ṹ
static const char* CREATE_TBL =
	"create table http_tbl\r\n"
	"(\r\n"
	"  addr char(128) not null,\r\n"
	"  url varchar(256) not null,\r\n"
	"  total_spent float(16,2) not null,\r\n"
	"  dns_spent float(16, 2) not null,\r\n"
	"  connect_spent float(16, 2) not null,\r\n"
	"  http_spent float(16, 2) not null,\r\n"
	"  length integer not null,\r\n"
	"  speed float(16, 2) not null,\r\n"
	"  success char(64) not null,\r\n"
	"  currtime TIMESTAMP not null\r\n"
	");\r\n"
	"create index addr_idx on http_tbl(addr);\r\n"
	"create index url_idx on http_tbl(url);\r\n";

// �������ݿ��
bool db_store::db_create()
{
	// ��ȡ���ݿ�����
	acl::db_handle* db = var_dbpool->peek();
	if (db == NULL)
	{
		logger_error("peek db failed!");
		return false;
	}

	// ���ȴ����ݿ�
	if (db->open(var_cfg_dbcharset) == false)
	{
		logger_error("open db error, charset: %s",
			var_cfg_dbcharset);
		var_dbpool->put(db);
		return false;
	}


	if (db->tbl_exists("http_tbl"))
	{
		var_dbpool->put(db);
		return true;
	}

	// �������ݿ��
	if (db->sql_update(CREATE_TBL) == false)
	{
		logger_error("create db table failed, table: %s", CREATE_TBL);
		var_dbpool->put(db);
		return false;
	}

	var_dbpool->put(db);
	return true;
}

bool db_store::db_update(const http_thread& http)
{
	// ��ȡ���ݿ�����
	acl::db_handle* db = var_dbpool->peek();
	if (db == NULL)
	{
		logger_error("peek db connect failed!");
		return false;
	}

	// ���ȴ����ݿ�
	if (db->open(var_cfg_dbcharset) == false)
	{
		logger_error("open db error, charset: %s",
			var_cfg_dbcharset);
		var_dbpool->put(db);
		return false;
	}

	// ���������ٶ�
	double speed = (http.length_ * 1000) / http.spent_http_;

	acl::string sql;
	sql.format("insert into http_tbl(addr, url, total_spent,"
		" dns_spent, connect_spent, http_spent, length, speed,"
		" success, currtime) values('%s', '%s', %.2f, %.2f, %.2f,"
		" %.2f, %d, %.2f, '%s', CURRENT_TIMESTAMP)",
		http.addr_, http.url_.c_str(), http.spent_total_,
		http.spent_dns_, http.spent_connect_, http.spent_http_,
		http.length_, speed, http.success_ ? "ok" : "error");

	// �������ݿ���ֶ�
	if (db->sql_update(sql.c_str()) == false)
		logger_error("sql(%s) error", sql.c_str());
	else
		logger("sql: %s ok", sql.c_str());

	// �黹���ݿ�����
	var_dbpool->put(db);
	return true;
}
