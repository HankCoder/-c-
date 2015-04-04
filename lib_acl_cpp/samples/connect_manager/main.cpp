#include "stdafx.h"
#include "connect_manager.h"
#include "mymonitor.h"
#include "connect_pool.h"
#include "connect_client.h"

static int __loop_count = 10;
static acl::connect_manager* __conn_manager = NULL;
static acl_pthread_pool_t* __thr_pool = NULL;

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

static void check_all_connections(void)
{
	std::vector<acl::connect_pool*>& pools = __conn_manager->get_pools();
	std::vector<acl::connect_pool*>::const_iterator cit = pools.begin();
	for (; cit != pools.end(); ++cit)
		printf(">>>addr: %s %s\r\n", (*cit)->get_addr(),
			(*cit)->aliving() ? "alive" : "dead");
}

// ��ʼ������
static void init(const char* addrs, int count,
	bool sync_check, const acl::string& proto)
{
	// ���� HTTP �������ӳؼ�Ⱥ�������
	__conn_manager = new connect_manager();

	// ��ӷ�������Ⱥ��ַ
	__conn_manager->init(addrs, addrs, 100);

	printf(">>>start monitor thread\r\n");

	// ������̨����߳�
	int  check_inter = 1, conn_timeout = 5;

	acl::connect_monitor* monitor = new mymonitor(*__conn_manager, proto);
	monitor->set_check_inter(check_inter);
	monitor->set_conn_timeout(conn_timeout);
	if (sync_check)
		monitor->open_rpc_service(10, NULL);
	(void) __conn_manager->start_monitor(monitor);

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

	// ��ӡ�������ӳؼ�Ⱥ�Ĵ��״̬
	printf("\r\n");

	check_all_connections();

	printf("\r\n>>> STOPPING check thread now\r\n");

#if 0
	int i = 0;
	while (i++ < 10)
	{
		sleep(1);
		printf("----------- sleep %d seconds -----------\r\n", i);
	}
#endif

	// ֹͣ��̨����߳�
	acl::connect_monitor* monitor = __conn_manager->stop_monitor(true);

	// ɾ�����������
	delete monitor;

	// �������ӳ�
	delete __conn_manager;
}

// ������̣�����������������ӷ�������ȡ��Ӧ����
static bool get(connect_client* conn, int n)
{
	printf(">>>check addr: %s, n: %d\r\n", conn->get_addr(), n);

	return true;
}

// ���̴߳������
static void thread_main(void*)
{
	for (int i = 0; i < __loop_count; i++)
	{
		connect_pool* pool = (connect_pool*) __conn_manager->peek();
		if (pool == NULL)
		{
			printf("\r\n>>>%lu(%d): peek pool failed<<<\r\n",
				(unsigned long) acl_pthread_self(), __LINE__);
			check_all_connections();
			break;
		}

		// �������ӵĳ�ʱʱ�估����ʱʱ��
		pool->set_timeout(2, 2);

		// �����ӳ��л�ȡһ������
		connect_client* conn = (connect_client*) pool->peek();
		if (conn == NULL)
		{
			printf("\r\n>>>%lu: peek connect failed from %s<<<\r\n",
				(unsigned long) acl_pthread_self(),
				pool->get_addr());
			check_all_connections();
			break;
		}

		// ��Ҫ�Ի�õ���������״̬��������ϴ�������̵���ʱ����
		else
			conn->reset();

		// ��ʼ�µ� HTTP �������
		if (get(conn, i) == false)
		{
			printf("one request failed, close connection\r\n");
			// ����������Ҫ�ر�
			pool->put(conn, false);
		}
		else
			pool->put(conn, true);
	}

	printf(">>>>thread: %lu OVER<<<<\r\n", (unsigned long) acl_pthread_self());
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
		"	-s server_addrs [www.sina.com.cn:80;www.263.net:80;www.qq.com:80]\r\n"
		"	-c cocurrent [default: 10]\r\n"
		"	-S [sync check io]\r\n"
		"	-P protocol [http|pop3]\r\n"
		"	-n loop_count[default: 10]\r\n", procname);
}

int main(int argc, char* argv[])
{
	int   ch, cocurrent = 10;
	bool  sync_check = false;
	acl::string addrs("www.sina.com.cn:80;www.263.net:80;www.qq.com:81");
	acl::string proto("pop3");

	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	// ��־�������׼���
	acl::log::stdout_open(true);

	while ((ch = getopt(argc, argv, "hs:n:c:SP:")) > 0)
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
		case 'S':
			sync_check = true;
			break;
		case 'P':
			proto = optarg;
			break;
		default:
			usage(argv[0]);
			return 0;
		}
	}

	init(addrs, cocurrent, sync_check, proto);
	run(cocurrent);
	end();

#ifdef WIN32
	printf("enter any key to exit...\r\n");
	getchar();
#endif

	return 0;
}
