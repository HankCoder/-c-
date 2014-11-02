#include "stdafx.h"

using namespace acl;

static int __loop_count = 10;
static connect_pool* __conn_pool = NULL;
static acl_pthread_pool_t* __thr_pool = NULL;
static bool __unzip = false;

// ��ʼ������
static void init(const char* addr, int count)
{
	// ���� HTTP �������ӳض���
	__conn_pool = new http_request_pool(addr, count);

	// �����̳߳�
	__thr_pool = acl_thread_pool_create(count, 60);
}

// �����˳�ǰ������Դ
static void end(void)
{
	// �����̳߳�
	acl_pthread_pool_destroy(__thr_pool);

	// �������ӳ�
	delete __conn_pool;
}

// HTTP ������̣�����������������ӷ�������ȡ��Ӧ����
static bool http_get(http_request* conn, int n)
{
	// ���� HTTP ����ͷ����
	http_header& header = conn->request_header();
	header.set_url("/")
		.set_keep_alive(true)
		.set_method(HTTP_METHOD_GET)
		.accept_gzip(__unzip);

	printf("%lu--%d: begin send request\r\n",
		(unsigned long) acl_pthread_self(), n);
	// ���� HTTP ��������ͬʱ���� HTTP ��Ӧͷ
	if (conn->request(NULL, 0) == false)
	{
		printf("%lu--%d: send GET request error\r\n",
			(unsigned long) acl_pthread_self(), n);
		return false;
	}

	char  buf[8192];
	int   ret, length = 0;

	// ���� HTTP ��Ӧ������
	while (true)
	{
		ret = conn->read_body(buf, sizeof(buf));
		if (ret == 0)
			break;
		else if (ret < 0)
		{
			printf("%lu--%d: error, length: %d\r\n",
				(unsigned long) acl_pthread_self(), n, length);
			return false;
		}
		length += ret;
		if (0)
			printf("%lu--%d: read length: %d, %d\r\n",
				(unsigned long) acl_pthread_self(), n, length, ret);
	}
	printf("%lu--%d: read body over, length: %d\r\n",
		(unsigned long) acl_pthread_self(), n, length);
	return true;
}

// �̴߳������
static void thread_main(void*)
{
	for (int i = 0; i < __loop_count; i++)
	{
		// �����ӳ��л�ȡһ�� HTTP ����
		http_request* conn = (http_request*) __conn_pool->peek();
		if (conn == NULL)
		{
			printf("peek connect failed\r\n");
			break;
		}

		// ��Ҫ�Ի�õ���������״̬��������ϴ�������̵���ʱ����
		else
			conn->reset();
		// ��ʼ�µ� HTTP �������
		if (http_get(conn, i) == false)
		{
			printf("one request failed, close connection\r\n");
			// ����������Ҫ�ر�
			__conn_pool->put(conn, false);
		}
		else
			__conn_pool->put(conn, true);
	}
}

static void run(int cocurrent)
{
	// ���̳߳����������
	for (int i = 0; i < cocurrent; i++)
		acl_pthread_pool_add(__thr_pool, thread_main, NULL);
}

static void usage(const char* procname)
{
	printf("usage: %s -h [help]\r\n"
		"	-s http_server_addr [www.sina.com.cn:80]\r\n"
		"	-z [unzip response body, default: false]\r\n"
		"	-c cocurrent [default: 10]\r\n"
		"	-n loop_count[default: 10]\r\n", procname);
}

int main(int argc, char* argv[])
{
	int   ch, cocurrent = 10;
	string addr("www.sina.com.cn:80");

	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	while ((ch = getopt(argc, argv, "hs:n:c:z")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 'c':
			cocurrent = atoi(optarg);
			break;
		case 'n':
			__loop_count = atoi(optarg);
			break;
		case 'z':
			__unzip = true;
			break;
		default:
			usage(argv[0]);
			return 0;
		}
	}

	init(addr, cocurrent);
	run(cocurrent);
	end();

#ifdef WIN32
	printf("enter any key to exit...\r\n");
	getchar();
#endif

	return 0;
}
