// ssl_client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "lib_acl.h"
#include <iostream>
#include "acl_cpp/acl_cpp_init.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/stream/polarssl_io.hpp"
#include "acl_cpp/stream/polarssl_conf.hpp"
#include "acl_cpp/http/http_client.hpp"

static acl::polarssl_conf __ssl_conf;

static void test0(int i)
{
	acl::socket_stream client;
	acl::string addr("127.0.0.1:441");
	if (client.open(addr.c_str(), 60, 60) == false)
	{
		std::cout << "connect " << addr.c_str()
			<< " error!" << std::endl;
		return;
	}

	acl::polarssl_io* ssl = new acl::polarssl_io(__ssl_conf, false);
	if (client.setup_hook(ssl) == ssl)
	{
		std::cout << "open ssl " << addr.c_str()
			<< " error!" << std::endl;
		ssl->destroy();
		return;
	}

	char line[1024];
	memset(line, 'x', sizeof(line));
	line[1023] = 0;
	line[1022] = '\n';
	if (client.write(line, strlen(line)) == -1)
	{
		std::cout << "write to " << addr.c_str()
			<< " error!" << std::endl;
		return;
	}

	size_t n = sizeof(line);
	if (client.gets(line, &n) == false)
	{
		std::cout << "gets from " << addr.c_str()
			<< " error!" << std::endl;
		return;
	}
	if (i < 10)
		std::cout << ">>gets: " << line << std::endl;
}

static void test1(const char* domain, int port, bool use_gzip, bool use_ssl)
{
	// ���� WEB ����������

	acl::string addr;
	addr << domain << ':' << port;

	acl::socket_stream client;
	if (client.open(addr.c_str(), 60, 60) == false)
	{
		std::cout << "connect " << addr.c_str()
			<< " error!" << std::endl;
		return;
	}

	// ���ʹ�� SSL ��ʽ������� SSL ���ֹ���
	if (use_ssl)
	{
		acl::polarssl_io* ssl = new acl::polarssl_io(__ssl_conf, false);
		if (client.setup_hook(ssl) == ssl)
		{
			std::cout << "open ssl client " << addr.c_str()
				<< " error!" << std::endl;
			ssl->destroy();
			return;
		}
	}

	// ���� HTTP ����ͷ
	acl::http_header header;
	header.set_url("/")
		.set_host(domain)
		.accept_gzip(use_gzip)
		.set_keep_alive(false);
	// mail.126.com �Ƚ������ʱ�ͻ���Ҫ���ѹ��������Ҳ�᷵��ѹ�����ݣ����Դ˴�
	// ǿ��Ҫ���ѹ������
	if (!use_gzip)
		header.add_entry("Accept-Encoding", "text/plain");

	acl::string request;
	header.build_request(request);

	std::cout << "request(len: " << request.length() << "):" << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << request.c_str();
	std::cout << "----------------------------------------" << std::endl;

	// ���� HTTP GET ����ͷ
	if (client.write(request) == false)
	{
		std::cout << "write to " << addr.c_str() <<
			" error!" << std::endl;
		return;
	}

	// ��ȡ HTTP ���������

	char  buf[8192];
	size_t size;
	int   ret;

	while (true)
	{
		size = sizeof(buf) - 1;
		if ((ret = client.read(buf, size, false)) == -1)
		{
			std::cout << "read over!" << std::endl;
			break;
		}
		 buf[ret] = 0;
		 std::cout << buf;
	}
}

static void test2(const char* domain, int port, bool use_gzip, bool use_ssl)
{
	// ���� WEB ����������

	acl::string addr;
	addr << domain << ':' << port;

	acl::http_client client;
	if (client.open(addr.c_str(), 60, 60, use_gzip) == false)
	{
		std::cout << "connect " << addr.c_str()
			<< " error!" << std::endl;
		return;
	}

	if (use_ssl)
	{
		// ���� SSL ����������ͻ����������󶨣����������ͷ�ǰ�� SSL ����
		// �����������ڲ�ͨ������ stream_hook::destroy() �ͷ�
		acl::polarssl_io* ssl = new acl::polarssl_io(__ssl_conf, false);
		if (client.get_stream().setup_hook(ssl) == ssl)
		{
			std::cout << "open ssl client " << addr.c_str()
				<< " error!" << std::endl;
			ssl->destroy();
			return;
		}
	}

	// ���� HTTP ����ͷ

	acl::http_header header;
	header.set_url("/")
		.set_host(domain)
		.accept_gzip(use_gzip)
		.add_entry("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:31.0) Gecko/20100101 Firefox/31.0")
		.add_entry("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8")
		.add_entry("Accept-Languge", "zh-cn,en;q=0.5")
		.add_entry("Pragma", "no-cache")
		.add_entry("Cache-Control", "no-cache");

	acl::string request;
	header.build_request(request);

	std::cout << "request:" << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	std::cout << request.c_str();
	std::cout << "----------------------------------------" << std::endl;

	// ���� HTTP GET ����ͷ

	if (client.write_head(header) == false)
	{
		std::cout << "write to " << addr.c_str()
			<< " error!" << std::endl;
		return;
	}

	// ��ȡ HTTP ��Ӧͷ
	if (client.read_head() == false)
	{
		std::cout << "read http respond header error!" << std::endl;
		return;
	}

	client.get_respond_head(&request);
	std::cout << "respond header:" << std::endl;
	std::cout << request.c_str();

	// ��ȡ��������Ӧ�� HTTP ���������

	char  buf[8192];
	size_t size;
	int   ret;

	while (true)
	{
		size = sizeof(buf) - 1;
		if ((ret = client.read_body(buf, size)) <= 0)
		{
			std::cout << "read over!" << std::endl;
			break;
		}
		buf[ret] = 0;
		std::cout << buf;
	}
}

int main(int argc, char* argv[])
{
	(void) argc; (void) argv;
	acl::acl_cpp_init();

	int   n = 1;
	if (argc >= 2)
		n = atoi(argv[1]);
	if (n <= 0)
		n = 100;
	ACL_METER_TIME("---------- begin ----------");
	for (int i = 0; i < 0; i++)
		test0(i);
	ACL_METER_TIME("---------- end ----------");

	// 126 �� SSL ����ʱ�� HTTP ����ͷ�е� Host ֵΪ mail.126.com:443 ʱ�� nginx
	// �ᱨ��ֻ���ǣ�Host: mail.126.com������

	test1("mail.126.com", 443, false, true);
	test2("mail.126.com", 443, false, true);
	test2("mail.qq.com", 443, false, true);
	test2("mail.sohu.com", 443, false, true);
	test2("mail.sina.com.cn", 443, false, true);

	printf("Over, enter any key to exit!\n");
	getchar();
	return (0);
}
