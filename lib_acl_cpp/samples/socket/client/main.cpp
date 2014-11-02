#include "acl_cpp/lib_acl.hpp"
#include "lib_acl.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	acl::socket_stream client;
	acl::string addr = "127.0.0.1:9001";

	if (argc >= 2)
		addr = argv[1];

	if (client.open(addr, 0, 0) == false)
	{
		printf("open %s error\n", addr.c_str());
		return 1;
	}
	else
		printf("open %s ok\r\n", addr.c_str());

	// ̽�������Ƿ�����
	if (client.alive())
		printf("first check: ok, status: %s\r\n", acl::last_serror());
	else
		printf("first check: disconnected, status: %s\r\n",
			acl::last_serror());

	// ����������д���һ������
	acl::string buf;
	if (client.gets(buf) == false)
	{
		printf("gets error, status: %s\r\n", acl::last_serror());
		return 1;

	}
	printf("gets: %s\r\n", buf.c_str());

	// ̽�������Ƿ�����
	if (client.alive())
		printf("second check: ok, status: %s\r\n", acl::last_serror());
	else
		printf("second check: disconnected, status: %s\r\n",
			acl::last_serror());

	acl::string req = "echo1\r\n"
		"echo2\r\n"
		"echo3\r\n"
		"read_delay1\r\n"
		"read_delay2\r\n"
		"read_delay3\r\n"
		"read_delay4\r\n"
		"read_delay5\r\n"
		"quit\r\n";

	int  n = 10;
	printf("sleep %d second\r\n", n);
	// ��Ϣһ�£��Ա�֤�������϶��Ѿ��ر�������
	for (int i = 0; i < n; i++)
	{
		sleep(1);
		putchar('.');
		fflush(stdout);
	}
	printf("\r\n");

	// ����һ��д��ʱ write �Ƿ񷵻�ʧ��
	if ((n = client.write(req.c_str(), req.length())) < 0)
	{
		printf("first write error, status: %s\n", acl::last_serror());
		return 1;
	}
	else
		printf("first write ok, ret: %d, status: %s\r\n",
			n, acl::last_serror());

	// ���ڶ���д��ʱ write �Ƿ񷵻�ʧ��
	if ((n = client.write(req.c_str(), req.length())) < 0)
	{
		printf("second write error, status: %s\n", acl::last_serror());
		return 1;
	}
	else
		printf("second write ok: ret: %d, status: %s\r\n",
			n, acl::last_serror());

	return (0);
}
