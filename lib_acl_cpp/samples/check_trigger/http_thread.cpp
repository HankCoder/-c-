#include "stdafx.h"
#include "util.h"
#include "db_store.h"
#include "http_thread.h"

http_thread::http_thread(const char* domain, const char* ip, int port,
	const char* url, double spent_dns)
: domain_(domain)
, ip_(ip)
, port_(port)
, url_(url)
, spent_dns_(spent_dns)
, spent_connect_(-1)
, spent_http_(-1)
, spent_total_(-1)
, length_(0)
, success_(false)
{
	snprintf(addr_, sizeof(addr_), "%s:%d", ip, port);
}

http_thread::~http_thread()
{
	db_store store;

	// �߳��˳�ǰ��������
	store.db_update(*this);
}

void* http_thread::run()
{
	char  domain[256];
	unsigned short port;

	if (acl::http_utils::get_addr(url_.c_str(), domain,
		sizeof(domain), &port) == false)
	{
		logger_error("invalid url: %s", url_.c_str());
		return NULL;
	}

	char  host[256], *phost;
	if (port == 80)
		phost = domain;
	else
	{
		snprintf(host, sizeof(host), "%s:%d", domain, port);
		phost = host;
	}

	struct timeval begin0;
	gettimeofday(&begin0, NULL);

	struct timeval begin, end;

	// ���� HTTP �������������ʱ
	gettimeofday(&begin, NULL);
	acl::socket_stream* conn = connect_server();
	gettimeofday(&end, NULL);
	spent_connect_ = util::stamp_sub(&end, &begin);

	if (conn == NULL)
		return NULL;

	gettimeofday(&begin, NULL);
	if (http_request(conn, phost) == false)
		logger_error("http request failed!");
	gettimeofday(&end, NULL);

	struct timeval finish;
	gettimeofday(&finish, NULL);

	spent_http_ = util::stamp_sub(&end, &begin);

	spent_total_ = spent_dns_ + util::stamp_sub(&finish, &begin0);

	delete conn;
	return NULL;
}

acl::socket_stream* http_thread::connect_server()
{
	// ����Զ�� HTTP ������
	acl::socket_stream* conn = new acl::socket_stream();
	if (conn->open(addr_, var_cfg_conn_timeout,
		var_cfg_rw_timeout) == false)
	{
		logger_error("connect server %s error %s",
			addr_, acl::last_serror());
		return NULL;
	}

	return conn;
}

bool http_thread::http_request(acl::socket_stream* conn, const char* host)
{
	acl::http_request req(conn);
	acl::http_header& header = req.request_header();

	// ���� HTTP ����ͷ
	header.set_url(url_.c_str())
		.set_keep_alive(false)
		.set_host(host)
		.set_method(acl::HTTP_METHOD_GET)
		.set_content_type("text/plain")
		.add_entry("Accept-Encoding", "plain")
		.accept_gzip(true);

	// ���� HTTP ����ͷ��ͬʱ��ȡ��Ӧͷ
	if (req.request(NULL, 0) == false)
	{
		logger_error("send request to %s error %s",
			conn->get_peer(true), acl::last_serror());
		return false;
	}

	acl::http_client* client = req.get_client();
	long long int content_length = client->body_length();

#if 1
	acl::string buf;
	int   ret, len;

	// ��ʼ��������
	while (true)
	{
		ret = req.read_body(buf, true, &len);
		if (ret == 0)
			break;
		if (ret < 0)
		{
			logger_error("get body from %s error",
				conn->get_peer(true));
			return false;
		}
		length_ += len;
	}
#else
	// sohu �� www.sohu.com �Ƚ�������Ȼ�ͻ��˷�����ֻ���� plain ����
	// ����Ȼ�ᷢ gzip ��Ӧ����
	char  buf[8192];
	int   ret;

	// ��ʼ��������
	while (true)
	{
		ret = req.get_body(buf, sizeof(buf) - 1);
		if (ret == 0)
			break;
		if (ret < 0)
		{
			logger_error("get body from %s error",
				conn->get_peer(true));
			return false;
		}
		length_ += ret;
	}
#endif

	// ��������������� HTTP ��Ӧͷ�е����ݲ�һ�£��򱨴�
	if (content_length > 0 && length_ != content_length)
	{
		logger_error("length: %d != content_length: %lld, server: %s",
			length_, content_length, conn->get_peer(true));
		return false;
	}

	logger("ok, url: %s, host: %s, addr: %s", url_.c_str(), host, addr_);
	success_ = true;
	return true;
}
