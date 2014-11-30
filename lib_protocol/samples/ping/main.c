#include "lib_acl.h"
#include "lib_protocol.h"
#include "lib_acl.h"
#include <signal.h>

static void add_ip_list(ICMP_CHAT *chat, const ACL_ARGV *domain_list,
	int npkt, int delay)
{
	ACL_DNS_DB* dns_db;
	const char* ptr;
	int   i, j;
	char *pdomain, *pip;
	ACL_ARGV* ip_list = acl_argv_alloc(10);

	for (i = 0; i < domain_list->argc; i++) {
		dns_db = acl_gethostbyname(domain_list->argv[i], NULL);
		if (dns_db == NULL) {
			acl_msg_warn("Can't find domain %s", domain_list->argv[i]);
			continue;
		}

		for (j = 0; j < acl_netdb_size(dns_db); j++) {
			ptr = acl_netdb_index_ip(dns_db, j);
			if (ptr == NULL)
				continue;
			acl_argv_add(ip_list, domain_list->argv[i], ptr, NULL);
		}
		acl_netdb_free(dns_db);
	}

	for (i = 0; i < ip_list->argc;) {
		pdomain = ip_list->argv[i++];
		pip = ip_list->argv[i++];

		if (strcmp(pdomain, pip) == 0)
			icmp_ping_one(chat, NULL, pip, npkt, delay, 1);
		else
			icmp_ping_one(chat, pdomain, pip, npkt, delay, 1);
	}
}

static ICMP_CHAT *__chat = NULL;

static void display_res2(ICMP_CHAT *chat)
{
	if (chat) {
		/* ��ʾ PING �Ľ���ܽ� */
		icmp_stat(chat);
		printf(">>>max pkts: %d\r\n", icmp_chat_seqno(chat));
	}
}

static void display_res(void)
{
	if (__chat) {
		display_res2(__chat);

		/* �ͷ� ICMP ���� */
		icmp_chat_free(__chat);
		__chat = NULL;
	}
}

/* ���߳��첽 PING �����ַ�ĺ������ */
static void ping_main_async(const ACL_ARGV *ip_list, int npkt)
{
	int   delay = 1;
	ACL_AIO *aio;

	/* �����������첽ͨ�ž�� */
	aio = acl_aio_create(ACL_EVENT_SELECT);
	acl_aio_set_keep_read(aio, 0);

	/* ���� ICMP ���� */
	__chat = icmp_chat_create(aio, 1);

	/* �����Ҫ PING �ĵ�ַ�б� */
	add_ip_list(__chat, ip_list, npkt, delay);

	while (1) {
		/* ��� PING ���������˳�ѭ�� */
		if (icmp_chat_finish(__chat)) {
			printf("over now!, hosts' size=%d, count=%d\r\n",
				icmp_chat_size(__chat), icmp_chat_count(__chat));
			break;
		}

		/* �첽�¼�ѭ������ */
		acl_aio_loop(aio);
	}

	/* ��ʾ PING ��� */
	display_res();

	/* ���ٷ�������� */
	acl_aio_free(aio);
}

/* ���߳� PING ������ַ�ĺ������ */
static void ping_main_sync(const char *dest, int npkt)
{
	ACL_DNS_DB* dns_db;
	int   delay = 1;
	const char* ip;

	/* ���� ICMP ���� */
	__chat = icmp_chat_create(NULL, 1);

	/* ������������ IP ��ַ */
	dns_db = acl_gethostbyname(dest, NULL);
	if (dns_db == NULL) {
		acl_msg_warn("Can't find domain %s", dest);
		return;
	}

	ip = acl_netdb_index_ip(dns_db, 0);
	if (ip == NULL || *ip == 0)
		acl_msg_fatal("ip invalid");

	/* ��ʼ PING һ�� IP ��ַ */
	if (strcmp(dest, ip) == 0)
		icmp_ping_one(__chat, NULL, ip, npkt, delay, 1);
	else
		icmp_ping_one(__chat, dest, ip, npkt, delay, 1);

	/* �ͷ� DNS ��ѯ��� */
	acl_netdb_free(dns_db);

	/* ��ʾ PING ���С�� */
	display_res();
}

/* PING �߳���� */
static int __npkt = 10;
static void *ping_thread(void *arg)
{
	const char *ip, *dest = (char *) arg;
	ACL_DNS_DB* dns_db;
	int   delay = 1;
	ICMP_CHAT *chat;

	/* ͨ������������IP��ַ */
	dns_db = acl_gethostbyname(dest, NULL);
	if (dns_db == NULL) {
		acl_msg_warn("Can't find domain %s", dest);
		return (NULL);
	}

	/* ֻȡ��������һ�� IP ��ַ PING */
	ip = acl_netdb_index_ip(dns_db, 0);
	if (ip == NULL || *ip == 0) {
		acl_msg_error("ip invalid");
		acl_netdb_free(dns_db);
		return (NULL);
	}

	/* ���� ICMP ���� */
	chat = icmp_chat_create(NULL, 1);

	/* ��ʼ PING */
	if (strcmp(dest, ip) == 0)
		icmp_ping_one(chat, NULL, ip, __npkt, delay, 1);
	else
		icmp_ping_one(chat, dest, ip, __npkt, delay, 1);
	acl_netdb_free(dns_db);  /* �ͷ������������� */
	display_res2(chat);  /* ��ʾ PING ��� */
	icmp_chat_free(chat);  /* �ͷ� ICMP ���� */
	return (NULL);
}

/* ���̷߳�ʽ PING ���Ŀ���ַ��ÿ���̲߳���ͬ�� PING ��ʽ */
static void ping_main_threads(const ACL_ARGV *ip_list, int npkt)
{
	int   i, n;
	acl_pthread_t tids[128];
	acl_pthread_attr_t attr;

	__npkt = npkt;
	acl_pthread_attr_init(&attr);
	acl_pthread_attr_setdetachstate(&attr, 0);

	/* �޶�ÿ�������߳�������ֹϵͳ����̫�� */
	n = ip_list->argc > 128 ? 128 : ip_list->argc;
	for (i = 0; i < n; i++)
		/* ���� PING �߳� */
		acl_pthread_create(&tids[i], &attr, ping_thread, ip_list->argv[i]);

	for (i = 0; i < n; i++)
		/* �����߳�״̬ */
		acl_pthread_join(tids[i], NULL);
}

static void usage(const char* progname)
{
	printf("usage: %s [-h help] -s [sync] -t [use thread mode] [-n npkt] [\"dest1 dest2 dest3...\"]\r\n", progname);
	printf("example: %s -n 10 www.sina.com.cn www.baidu.com www.qq.com\r\n", progname);
	printf("example: %s -s -n 10 www.sina.com.cn\r\n", progname);
#ifdef WIN32
	printf("please enter any key to exit\r\n");
	getchar();
#endif
}

/* ���յ� SIGINT �ź�(���� PING �������û����� ctrl + c)ʱ���źŴ����� */
static void OnSigInt(int signo acl_unused)
{
	display_res();
	exit(0);
}

int main(int argc, char* argv[])
{
	char  ch;
	int   npkt = 5, i, syn = 0, thread = 0;
	ACL_ARGV* dest_list = acl_argv_alloc(10);

	signal(SIGINT, OnSigInt);  /* �û����� ctr + c ʱ�ж� PING ���� */
	acl_socket_init();  /* �� WIN32 ����Ҫ��ʼ��ȫ���׽��ֿ� */
	acl_msg_stdout_enable(1);  /* ���� acl_msg_xxx ��¼����Ϣ�������Ļ */

	while ((ch = getopt(argc, argv, "htsl:n:")) > 0) {
		switch (ch) {
		case 'h':
			usage(argv[0]);
			return (0);
		case 's':
			syn = 1;
			break;
		case 't':
			thread = 1;
			break;
		case 'n':
			npkt = atoi(optarg);
			break;
		default:
			usage(argv[0]);
			return (0);
		}
	}

	if (optind == argc) {
		usage(argv[0]);
		return (0);
	}

	for (i = optind; i < argc; i++) {
		acl_argv_add(dest_list, argv[i], NULL);
	}

	if (npkt <= 0)
		npkt = 0;

	/* ͬ�� PING ��ʽ�����ڶ��Ŀ���ַ������һ���߳� PING һ����ַ */
	if (thread)
		ping_main_threads(dest_list, npkt);

	/* ͬ�� PING ��ʽ��ֻ��ͬʱ PING һ����ַ */
	else if (syn)
		ping_main_sync(dest_list->argv[0], npkt);

	/* �첽 PING ��ʽ��������һ���߳���ͬʱ PING �����ַ */
	else
		ping_main_async(dest_list, npkt);

	acl_argv_free(dest_list);

#ifdef WIN32
	printf("please enter any key to exit\r\n");
	getchar();
#endif

	acl_socket_end();
	return 0;
}
