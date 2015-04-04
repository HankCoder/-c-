#include "stdafx.h"
#include "mymonitor.h"

mymonitor::mymonitor(acl::connect_manager& manager, const acl::string& proto)
: connect_monitor(manager)
, proto_(proto)
{
}

mymonitor::~mymonitor(void)
{
}

//////////////////////////////////////////////////////////////////////////////
// ���������̣�������ĳһ�����߳̿ռ���

void mymonitor::sio_check(acl::check_client& checker,
	acl::socket_stream& conn)
{
	if (proto_ == "http")
		sio_check_http(checker, conn);
	else if (proto_ == "pop3")
		sio_check_pop3(checker, conn);
	else
	{
		printf("unknown protocol: %s\r\n", proto_.c_str());
		checker.set_alive(true);
	}
}

void mymonitor::sio_check_pop3(acl::check_client& checker,
	acl::socket_stream& conn)
{
	acl::string buf;
	if (conn.gets(buf) == false)
	{
		checker.set_alive(false);
		return;
	}

	if (strncasecmp(buf.c_str(), "+OK", 3) == 0)
	{
		printf(">>> SIO_CHECK SERVER(%s) OK: %s, len: %d <<<\r\n",
			checker.get_addr(), buf.c_str(), (int) buf.length());
		checker.set_alive(true);
	}
	else
	{
		printf(">>> SIO_CHECK SERVER(%s) ERROR: %s, len: %d <<<\r\n",
			checker.get_addr(), buf.c_str(), (int) buf.length());
		checker.set_alive(false);
	}
}

void mymonitor::sio_check_http(acl::check_client& checker,
	acl::socket_stream& conn)
{
	acl::http_request req(&conn, 60, false);
	acl::http_header& hdr = req.request_header();

	acl::string ctype("text/plain; charset=gbk");
	hdr.set_url("/").set_content_type("text/plain; charset=gbk");
	if (req.request(NULL, 0) == false)
	{
		printf(">>> send request error\r\n");
		checker.set_alive(false);
		return;
	}

	acl::string buf;
	if (req.get_body(buf) == false)
	{
		printf(">>> HTTP get_body ERROR, SERVER: %s <<<\r\n",
			checker.get_addr());
		checker.set_alive(false);
		return;
	}

	int status = req.http_status();
	if (status == 200 || status == 404)
	{
		printf(">>> SIO_CHECK HTTP SERVER(%s) OK: %d <<<\r\n",
			checker.get_addr(), status);
		checker.set_alive(true);
	}
	else
	{
		printf(">>> SIO_CHECK HTTP SERVER(%s) ERROR: %d <<<\r\n",
			checker.get_addr(), status);
		checker.set_alive(false);
	}
}

//////////////////////////////////////////////////////////////////////////////
// �����������̣������ڼ�����̵߳Ŀռ���

void mymonitor::nio_check(acl::check_client& checker,
	acl::aio_socket_stream& conn)
{
	checker_ = &checker;

	// ע������� IO ������̵Ļص�����
	conn.add_close_callback(this);
	conn.add_read_callback(this);
	conn.add_timeout_callback(this);

	int timeout = 10;

	// �첽��ȡһ�����ݣ�ͬʱҪ�󲻱��� \r\n

	conn.gets(timeout);
}

bool mymonitor::read_callback(char* data, int len)
{
	// ��Ϊ acl ���첽 IO ���������ݿ϶����������������������� \0������ֱ��
	// ���ַ����Ƚ��ڴ˴��ǰ�ȫ��

	if (strncasecmp(data, "+OK", 3) == 0)
	{
		// �������������Ϊ���״̬
		checker_->set_alive(true);

		// �����رոü������
		checker_->close();

		// �˴����� true �� false �����ԣ���Ϊ�����Ѿ�����Ҫ��رռ������
		printf(">>> NIO_CHECK SERVER(%s) OK: %s, len: %d <<<\r\n",
			checker_->get_addr(), data, len);
		return true;
	}

	// ���������Ϊ������״̬
	checker_->set_alive(false);

	printf(">>> NIO_CHECK SERVER(%s) ERROR: %s, len: %d <<<\r\n",
		checker_->get_addr(), data, len);

	// ���� false ֪ͨ����Զ��رո�����
	return false;
}

bool mymonitor::timeout_callback()
{
	// ����ʱ������ֱ�ӽ�������Ϊ������
	checker_->set_alive(false);

	// ���� false ͨ������Զ��رոü������
	return false;
}

void mymonitor::close_callback()
{

}
