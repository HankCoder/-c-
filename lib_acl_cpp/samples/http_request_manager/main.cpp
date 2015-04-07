#include "stdafx.h"

using namespace acl;

static int __loop_count = 10;
static connect_manager* __conn_manager = NULL;
static acl_pthread_pool_t* __thr_pool = NULL;
static bool __unzip = false;

static void sleep_while(int n)
{
	for (int i = 0; i < n; i++)
	{
		putchar('.');
		fflush(stdout);
		sleep(1);
	}
	printf("\r\n");
}

// ��ʼ������
static void init(const char* addrs, int count)
{
	// ���� HTTP �������ӳؼ�Ⱥ�������
	__conn_manager = new http_request_manager();

	// ��ӷ�������Ⱥ��ַ
	__conn_manager->init(addrs, addrs, 100);

	printf(">>>start monitor thread\r\n");

	int  check_inter = 1, conn_timeout = 5;
	acl::connect_monitor* monitor = new acl::connect_monitor(*__conn_manager);
	(*monitor).set_check_inter(check_inter).set_conn_timeout(conn_timeout);

	// ������̨����߳�
	__conn_manager->start_monitor(monitor);


	int   n = 10;
	printf(">>>sleep %d seconds for monitor check\r\n", n);
	sleep_while(n);

	printf(">>>create thread pool\r\n");
	// �����̳߳�
	__thr_pool = acl_thread_pool_create(count, 60);
}

// �����˳�ǰ������Դ
static void end(void)
{
	// �����̳߳�
	acl_pthread_pool_destroy(__thr_pool);

#if 0
	int   n = 10;
	printf("\r\n>>>sleep %d seconds to stop monitor\r\n", n);
	sleep_while(n);
#endif

	// ��ӡ�������ӳؼ�Ⱥ�Ĵ��״̬
	printf("\r\n");
	std::vector<connect_pool*>& pools = __conn_manager->get_pools();
	std::vector<connect_pool*>::const_iterator cit = pools.begin();
	for (; cit != pools.end(); ++cit)
	{
		printf(">>>server: %s, %s\r\n",
			(*cit)->get_addr(), (*cit)->aliving()
			? "alive" : "dead");
	}
	printf("\r\n>>> STOPPING check thread now\r\n");

	// ֹͣ��̨����߳�
	acl::connect_monitor* monitor = __conn_manager->stop_monitor(true);
	// ɾ�����������
	delete monitor;

	// �������ӳ�
	delete __conn_manager;
}

// HTTP ������̣�����������������ӷ�������ȡ��Ӧ����
static bool http_get(http_request* conn, const char* addr, int n)
{
	if (0)
	printf(">>>check addr: %s, n: %d\r\n", addr, n);

	// ���� HTTP ����ͷ����
	http_header& header = conn->request_header();
	header.set_url("/")
		.set_host(addr)
		.set_keep_alive(true)
		.set_method(HTTP_METHOD_GET)
		.accept_gzip(__unzip);

	if (0)
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
				(unsigned long) acl_pthread_self(),
				n, length);
			return false;
		}
		length += ret;
		if (0)
			printf("%lu--%d: read length: %d, %d\r\n",
				(unsigned long) acl_pthread_self(),
				n, length, ret);
	}
	if (0)
		printf("%lu--%d: read body over, length: %d\r\n",
			(unsigned long) acl_pthread_self(), n, length);
	return true;
}

static void check_all_connections(void)
{
	std::vector<connect_pool*>& pools = __conn_manager->get_pools();
	std::vector<connect_pool*>::const_iterator cit = pools.begin();
	for (; cit != pools.end(); ++cit)
		printf(">>>addr: %s %s\r\n", (*cit)->get_addr(),
			(*cit)->aliving() ? "alive" : "dead");
}

// �̴߳������
static void thread_main(void*)
{
	for (int i = 0; i < __loop_count; i++)
	{
		http_request_pool* pool = (http_request_pool*)
			__conn_manager->peek();
		if (pool == NULL)
		{
			printf("\r\n>>>%lu(%d): peek pool failed<<<\r\n",
				(unsigned long) acl_pthread_self(), __LINE__);
			check_all_connections();
			exit (1);
		}

		// �������ӵĳ�ʱʱ�估����ʱʱ��
		pool->set_timeout(2, 2);

		// �����ӳ��л�ȡһ�� HTTP ����
		http_request* conn = (http_request*) pool->peek();
		if (conn == NULL)
		{
			printf("\r\n>>>%lu: peek connect failed from %s<<<\r\n",
				(unsigned long) acl_pthread_self(),
				pool->get_addr());
			check_all_connections();
			exit (1);
		}

		// ��Ҫ�Ի�õ���������״̬��������ϴ�������̵���ʱ����
		else
			conn->reset();

		// ��ʼ�µ� HTTP �������
		if (http_get(conn, pool->get_addr(), i) == false)
		{
			printf("one request failed, close connection\r\n");
			// ����������Ҫ�ر�
			pool->put(conn, false);
		}
		else
			pool->put(conn, true);
	}

	if (0)
		printf(">>>>thread: %lu OVER<<<<\r\n",
			(unsigned long) acl_pthread_self());
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
		"	-s http_server_addrs [www.sina.com.cn:80;www.263.net:80;www.qq.com:80]\r\n"
		"	-z [unzip response body, default: false]\r\n"
		"	-c cocurrent [default: 10]\r\n"
		"	-n loop_count[default: 10]\r\n", procname);
}

int main(int argc, char* argv[])
{
	int   ch, cocurrent = 10;
	string addrs("www.sina.com.cn:80;www.263.net:80;www.qq.com:81");

	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	// ��־�������׼���
	acl::log::stdout_open(true);

	while ((ch = getopt(argc, argv, "hs:n:c:z")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addrs = optarg;
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

	init(addrs, cocurrent);
	run(cocurrent);
	end();

#ifdef WIN32
	printf("enter any key to exit...\r\n");
	getchar();
#endif

	return 0;
}
