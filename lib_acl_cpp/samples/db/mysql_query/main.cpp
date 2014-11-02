// mysql.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "acl_cpp/lib_acl.hpp"

const char* CREATE_TBL =
	"create table group_tbl\r\n"
	"(\r\n"
	"group_name varchar(128) not null,\r\n"
	"uvip_tbl varchar(32) not null default 'uvip_tbl',\r\n"
	"access_tbl varchar(32) not null default 'access_tbl',\r\n"
	"access_week_tbl varchar(32) not null default 'access_week_tbl',\r\n"
	"access_month_tbl varchar(32) not null default 'access_month_tbl',\r\n"
	"update_date date not null default '1970-1-1',\r\n"
	"disable integer not null default 0,\r\n"
	"add_by_hand integer not null default 0,\r\n"
	"class_level integer not null default 0,\r\n"
	"primary key(group_name, class_level)\r\n"
	")";

static bool tbl_create(acl::db_handle& db)
{
	if (db.tbl_exists("group_tbl"))
	{
		printf("table exist\r\n");
		return (true);
	}
	else if (db.sql_update(CREATE_TBL) == false)
	{
		printf("sql error\r\n");
		return (false);
	}
	else
	{
		printf("create table ok\r\n");
		return (true);
	}
}

// ��ӱ�����
static bool tbl_insert(acl::db_handle& db, int n)
{
	const char* sql_fmt = "insert into group_tbl(group_name, uvip_tbl,"
		" update_date) values(:group, :test, :date)";
	acl::query query;
	query.create_sql(sql_fmt)
		.set_format("group", "group:%d", n)
		.set_parameter("test", "test")
		.set_date("date", time(NULL), "%Y-%m-%d");

	if (db.exec_update(query) == false)
		return (false);

	const acl::db_rows* result = db.get_result();
	if (result)
	{
		const std::vector<acl::db_row*>& rows = result->get_rows();
		for (size_t i = 0; i < rows.size(); i++)
		{
			const acl::db_row* row = rows[i];
			for (size_t j = 0; j < row->length(); j++)
				printf("%s, ", (*row)[j]);
			printf("\r\n");
		}
	}

	db.free_result();
	return (true);
}

// ��ѯ������
static int tbl_select(acl::db_handle& db, int n)
{
	const char* sql_fmt = "select * from group_tbl where"
		" group_name=:group and uvip_tbl=:test";
	acl::query query;
	query.create_sql(sql_fmt)
		.set_format("group", "group:%d", n)
		.set_format("test", "test");

	if (db.exec_select(query) == false)
	{
		printf("select sql error\r\n");
		return (-1);
	}

	printf("\r\n---------------------------------------------------\r\n");

	// �г���ѯ�������һ
	const acl::db_rows* result = db.get_result();
	if (result)
	{
		const std::vector<acl::db_row*>& rows = result->get_rows();
		for (size_t i = 0; i < rows.size(); i++)
		{
			if (n > 100)
				continue;
			const acl::db_row* row = rows[i];
			for (size_t j = 0; j < row->length(); j++)
				printf("%s, ", (*row)[j]);
			printf("\r\n");
		}
	}

	// �г���ѯ���������
	for (size_t i = 0; i < db.length(); i++)
	{
		if (n > 100)
			continue;
		const acl::db_row* row = db[i];

		// ȡ�����м�¼��ĳ���ֶε�ֵ
		const char* ptr = (*row)["group_name"];
		if (ptr == NULL)
		{
			printf("error, no group name\r\n");
			continue;
		}
		printf("group_name=%s: ", ptr);
		for (size_t j = 0; j < row->length(); j++)
			printf("%s, ", (*row)[j]);
		printf("\r\n");
	}

	// �г���ѯ���������
	const std::vector<acl::db_row*>* rows = db.get_rows();
	if (rows)
	{
		std::vector<acl::db_row*>::const_iterator cit = rows->begin();
		for (; cit != rows->end(); cit++)
		{
			if (n > 100)
				continue;
			const acl::db_row* row = *cit;
			for (size_t j = 0; j < row->length(); j++)
				printf("%s, ", (*row)[j]);
			printf("\r\n");
		}

	}
	int  ret = (int) db.length();

	// �ͷŲ�ѯ���
	db.free_result();
	return (ret);
}

// ɾ��������
static bool tbl_delete(acl::db_handle& db, int n)
{
	const char* sql_fmt = "delete from group_tbl where group_name=:group";
	acl::query query;
	query.create_sql(sql_fmt).set_format("group", "group-%d", n);

	if (db.exec_update(query) == false)
	{
		printf("delete sql error\r\n");
		return (false);
	}

	for (size_t i = 0; i < db.length(); i++)
	{
		const acl::db_row* row = db[i];
		for (size_t j = 0; j < row->length(); j++)
			printf("%s, ", (*row)[j]);
		printf("\r\n");
	}
	// �ͷŲ�ѯ���
	db.free_result();

	return (true);
}

int main(void)
{
	const char* dbaddr = "127.0.0.1:3306";
	const char* dbname = "acl_test_db";
	const char* dbuser = "root", *dbpass = "111111";
	acl::db_mysql db(dbaddr, dbname, dbuser, dbpass);
	int   max = 100;

	// ����������־�������Ļ
	acl::log::stdout_open(true);

	// �ȴ����ݿ�����
	if (db.open() == false)
	{
		printf("open db(%s) error\r\n", dbname);
		getchar();
		return 1;
	}

	printf("open db %s ok\r\n", dbname);

	// �����ݱ�����ʱ������
	if (tbl_create(db) == false)
	{
		printf("create table error\r\n");
		getchar();
		return 1;
	}

	// �����������
	for (int i = 0; i < max; i++)
	{
		bool ret = tbl_insert(db, i);
		if (ret)
			printf(">>insert ok: i=%d, affected: %d\r",
			i, db.affect_count());
		else
			printf(">>insert error: i = %d\r\n", i);
	}
	printf("\r\n");
	int  n = 0;

	// ������ѯ����
	for (int i = 0; i < max; i++)
	{
		int  ret = tbl_select(db, i);
		if (ret >= 0)
		{
			n += ret;
			printf(">>select ok: i=%d, ret=%d\r", i, ret);
		}
		else
			printf(">>select error: i = %d\r\n", i);
	}
	printf("\r\n");
	printf(">>select total: %d\r\n", n);

	// ����ɾ������
	for (int i = 0; i < max; i++)
	{
		bool ret = tbl_delete(db, i);
		if (ret)
			printf(">>delete ok: %d, affected: %d\r",
			i, (int) db.affect_count());
		else
			printf(">>delete error: i = %d\r\n", i);
	}
	printf("\r\n");

#ifndef WIN32
	//	mysql_server_end();
	//	mysql_thread_end();
#endif

	printf("mysqlclient lib's version: %ld, info: %s\r\n",
		db.mysql_libversion(), db.mysql_client_info());

	printf("Enter any key to exit.\r\n");
	getchar();
	return 0;
}
