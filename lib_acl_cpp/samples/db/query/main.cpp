#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "acl_cpp/db/query.hpp"

#ifdef WIN32
# ifndef snprintf
#  define snprintf _snprintf
# endif
#endif

int main(void)
{
	acl::query query;

	// ������ѯ��ʽ�� SQL ��ѯ��
	query.create_sql("select * from person where name = :name"
		" and age >= :age and male = 1 and nick = :name");
	query.set_parameter("name", "'zsxxsz'\\");
	query.set_parameter("age", 30);

	printf("sql: %s\r\n", query.to_string().c_str());

	// �ظ�ʹ�øò�ѯ������ʱ��Ҫ���øò�ѯ��״̬
	query.reset();

	int age = 20;
	query.create_sql("update table set name=:name, age=%d"
		", home=:home where nick=:name", age)
		.set_parameter("name", "zsx1&xsz1")
		.set_parameter("home", "������");
	printf("sql: %s\r\n", query.to_string().c_str());

	query.reset();
	query.create_sql("insert into table(name, age, home, nick, date, time)"
		" values(:name, %d, :home, :nick, :date, :time)", age)
		.set_parameter("name", "zsx'xsz")
		.set_parameter("home", "����.��ƽ.������")
		.set_parameter("nick", "'��ң��'")
		.set_date("date", time(NULL), "%Y-%m-%d")
		.set_date("time", time(NULL));

	printf("sql: %s\r\n", query.to_string().c_str());

	query.reset();
	query.create_sql("select * from table where name=:name")
		.set_format("name", "zsx\"%s-%d", "xsz", 1);
	printf("sql: %s\r\n", query.to_string().c_str());

	/////////////////////////////////////////////////////////////////////

	time_t now = time(NULL);
	acl::string buf(128);
	if (acl::query::to_date(now, buf) == NULL)
		printf("to_date_time error, now: %ld\r\n", (long) now);
	else
		printf("to_date_time ok, now: %ld, %s\r\n",
			(long) now, buf.c_str());

	const char* date_fmt = "%Y-%m-%d";
	buf.clear();
	if (acl::query::to_date(now, buf, date_fmt) == NULL)
		printf("to_date_time error, now: %ld\r\n", (long) now);
	else
		printf("to_date_time ok, now: %ld, %s\r\n",
			(long) now, buf.c_str());

	date_fmt = "%Y-%m";
	buf.clear();
	if (acl::query::to_date(now, buf, date_fmt) == NULL)
		printf("to_date_time error, now: %ld, fmt: %s\r\n",
			(long) now, date_fmt);
	else
		printf("to_date_time ok, now: %ld, %s\r\n",
		(long) now, buf.c_str());

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif

	return 0;
}

