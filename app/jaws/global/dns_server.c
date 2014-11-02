#include "lib_acl.h"
#include "sys_patch.h"

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include "dns.h"

#define MSG_SEND	acl_msgio_send

/* DNS��ѯ�߳������̷߳���IO��Ϣ����Ϊ����̶߳�Ҫ��һ����Ϣ���������Ϣ��������Ҫ���� */

static void reply_lookup_msg(DNS_CTX *dns_ctx, DNS_SERVER *dns)
{
	const char *myname = "reply_lookup_msg";

	/* ���� */
	acl_pthread_mutex_lock(&dns->lock);

	/* �����̷߳���DNS��ѯ�����Ϣ */
	if (MSG_SEND(dns_ctx->mio, DNS_MSG_LOOKUP_RESULT, dns_ctx, sizeof(DNS_CTX)) < 0) {
		acl_msg_error("%s: send msg error, domain(%s)", myname, dns_ctx->domain_key);
	}

	/* ���� */
	acl_pthread_mutex_unlock(&dns->lock);

	/* �ͷ��� msg_lookup ����������ڴ� */
	acl_myfree(dns_ctx);
}

/* DNS��ѯ�̣߳��鵽DNS�����ͨ��IO��Ϣ�����ݴ��ݸ�������ʽ���̵߳���Ϣ���� */

static void lookup_thread(void *arg)
{
	const char *myname = "lookup_thread";
#ifdef USE_THREAD_POOL
	DNS_CTX *dns_ctx = (DNS_CTX *) arg;
	DNS_SERVER *dns = dns_ctx->dns;
#else
	DNS_CTX *dns_ctx = (DNS_CTX *) arg;
	DNS_SERVER *dns = dns_ctx->dns;
#endif
	int  error = 0, n, i;

	dns_ctx->dns_db = acl_gethostbyname(dns_ctx->domain_key, &error);
	if (dns_ctx->dns_db == NULL) {
		acl_msg_error("%s: gethostbyname error(%s), domain(%s)",
			myname, acl_netdb_strerror(error), dns_ctx->domain_key);
		dns_ctx->ip_cnt = 0;
		reply_lookup_msg(dns_ctx, dns);
		return;
	}

	n = acl_netdb_size(dns_ctx->dns_db);
	dns_ctx->ip_cnt = n > DNS_IP_MAX ? DNS_IP_MAX : n;

	for (i = 0; i < dns_ctx->ip_cnt; i++) {
		snprintf(dns_ctx->ip[i], sizeof(dns_ctx->ip[i]), "%s",
			acl_netdb_index_ip(dns_ctx->dns_db, i));
		dns_ctx->port[i] = 0; /* ����DNS��ѯ���Ľ��������Ĭ�ϵĶ˿ں� */
	}
	reply_lookup_msg(dns_ctx, dns);
}

/* ���߳���Ϣ������: ���߳�ͨ���˺�������DNS�߳̽���DNS��ѯ���� */

static int msg_lookup(int msg_type acl_unused, ACL_MSGIO *mio,
		  const ACL_MSGIO_INFO *info, void *arg)
{
	DNS_CTX *dns_ctx;

	dns_ctx = (DNS_CTX *) acl_mycalloc(1, sizeof(DNS_CTX));
	memcpy(dns_ctx, acl_vstring_str(info->body.buf),
		ACL_VSTRING_LEN(info->body.buf));

	/* ����DNS��� */
	dns_ctx->dns = (DNS_SERVER *) arg;
	/* ������Ϣ������̳߳��еĲ�ѯ�߳�ͨ����˾��������Ϣ��֪ͨ
	 * ���߳��й�DNS�Ĳ�ѯ��� 
	 */
	dns_ctx->mio = mio;

	/* �����������߳̽�������ʽDNS��ѯ���� */
#ifdef USE_THREAD_POOL
	acl_pthread_pool_add(dns_ctx->dns->wq, lookup_thread, dns_ctx);
#else
	lookup_thread(dns_ctx);
#endif

	return (1);
}

/* ���߳���Ϣ������: ���߳�ͨ���˺�������DNS��ѯ�̷߳��͵�DNS��ѯ�����Ϣ */

static int msg_lookup_result(int msg_type acl_unused, ACL_MSGIO *mio acl_unused,
		  const ACL_MSGIO_INFO *info, void *arg)
{
	DNS_CTX *dns_ctx;
	DNS_SERVER *dns = (DNS_SERVER*) arg;

	/* ���DNS��ѯ�̵߳Ľ����Ϣ */
	dns_ctx = (DNS_CTX *) acl_vstring_str(info->body.buf);

	/* ����ѯ�������DNS������ */
	if (dns_ctx->ip_cnt > 0)
		dns_cache_push(dns->dns_cache, dns_ctx->dns_db);

	/* �ͷ��� lookup_thread ����� dns_db ���� */
	if (dns_ctx->dns_db)
		acl_netdb_free(dns_ctx->dns_db);
	dns_ctx->dns_db = NULL;

	/* �ص����������� */
	dns_ctx->callback(dns_ctx);

	return (1);
}

/* ����DNS��ѯ��� */

DNS_SERVER *dns_server_create(ACL_AIO *aio, int timeout)
{
	const char *myname = "dns_server_create";
	DNS_SERVER *dns;
	int   max_threads = 200;
	int   idle_timeout = 60 /* ��ֵĿǰ�������ã���Ҫ�޸�һ��ACL��ſ� */;

	if (aio == NULL)
		acl_msg_fatal("%s(%d): aio null", myname, __LINE__);

	/* acl ���DNS����ģ����Ҫ��������Ϊ��ѯ�̳߳ض�Ҫ���ʸ�ͬһ��Դ
	 * ���� acl ��DNS����ģ��Ļ���ʱ��Ϊ60�룬��Ϊ������ģ��Ҳ�л������
	 * ��Ϊ�������Լ��Ѿ�ʵ����DNS���棬���Բ���ҪACL���DNS���洦��
	 * acl_netdb_cache_init(60 , 1);
	 */

	dns = (DNS_SERVER *) acl_mycalloc(1, sizeof(DNS_SERVER));
	dns->aio = aio;
	/* ���ص�DNS����ģ����Ҫ��������Ϊֻ��һ���̷߳�������Դ */
	dns->dns_cache = dns_cache_create(timeout, 0);


	/* ���� IO �¼���Ϣ */
	dns->listener = acl_msgio_listen(aio, NULL);
	if (dns->listener == NULL)
		acl_msg_fatal("%s: listen error", myname);

	/* ע�� IO �¼���Ϣ�������������ڼ����µĲ�ѯ���� */
	acl_msgio_listen_reg(dns->listener, DNS_MSG_LOOKUP, msg_lookup, dns, 1);

	acl_msgio_addr(dns->listener, dns->addr, sizeof(dns->addr));
	dns->mio = acl_msgio_connect(aio, dns->addr, 0);
	if (dns->mio == NULL)
		acl_msg_fatal("%s: connect server(%s) error", myname, dns->addr);

	/* ע�� IO �¼���Ϣ�������������ڴ����ѯ�̷߳��ز�ѯ��� */
	acl_msgio_reg(dns->mio, DNS_MSG_LOOKUP_RESULT, msg_lookup_result, dns);

	/* ��ʼ��DNS�����߳��� */
	acl_pthread_mutex_init(&dns->lock, NULL);

	/* ��Ҫ�����������̳̲߳�ѯDNS(��Ϊ��ѯDNS������ʽ��ѯ) */
#ifdef USE_THREAD_POOL
	dns->wq = acl_thread_pool_create(max_threads, idle_timeout);
#endif
	
	return (dns);
}

/* �ر�DNS��ѯ��� */

void dns_server_close(DNS_SERVER *dns)
{
	acl_msgio_close(dns->listener);
	acl_pthread_pool_destroy(dns->wq);
	acl_myfree(dns);
}

void dns_server_static_add(DNS_SERVER *dns, const char *map, const char *delim, int def_port)
{
	ACL_DNS_DB *dns_db = NULL;
	ACL_ARGV *argv = NULL;
	char *ptr;
	int   i, port;

#undef	RETURN
#define	RETURN do {  \
	if (argv != NULL)  \
		acl_argv_free(argv);  \
	if (dns_db != NULL)  \
		acl_netdb_free(dns_db);  \
	return;  \
} while (0)

	argv = acl_argv_split(map, delim);
	if (argv == NULL)
		RETURN;
	if (argv->argc < 2)
		RETURN;

	dns_db = acl_netdb_new(argv->argv[0]);
	for (i = 1; i < argv->argc; i++) {
		ptr = strchr(argv->argv[i], ':');
		if (ptr != NULL) {
			*ptr++ = 0;
			port = atoi(ptr);	
			if (port <= 0)
				port = def_port;
		} else {
			port = def_port;
		}
		acl_netdb_add_addr(dns_db, argv->argv[i], port);
	}

	/* ���ó�ʱʱ��Ϊ0�Ӷ�ʹ��������ʱ */
	dns_cache_push_one(dns->dns_cache, dns_db, 0);
	RETURN;
}

/* ��ʼ��ѯĳ������ */

int dns_server_lookup(DNS_SERVER *dns, const DNS_CTX *ctx)
{
	const char *myname = "dns_server_lookup";
	ACL_MSGIO *mio = dns->mio;
	DNS_CTX dns_ctx;
	ACL_DNS_DB *dns_db;

	/* ֻ���Բ��ô˷�ʽ����Ϊ�˱�֤ dns_server_lookup ���� ctxΪ const ���� */
	memcpy(&dns_ctx, ctx, sizeof(dns_ctx));

	/* �Ȳ�ѯDNS��������Ƿ���ڱ����������� */
	dns_db = dns_cache_lookup(dns->dns_cache, ctx->domain_key);
	if (dns_db) {
		ACL_ITER iter;
		int   i = 0;

		acl_foreach(iter, dns_db) {
			const ACL_HOST_INFO *info;

			info = (const ACL_HOST_INFO*) iter.data;
			ACL_SAFE_STRNCPY(dns_ctx.ip[i], info->ip,
				sizeof(dns_ctx.ip[i]));
			dns_ctx.port[i++] = info->hport;
			dns_ctx.ip_cnt++;
			if (dns_ctx.ip_cnt >= DNS_IP_MAX)
				break;
		}

		/* ���Ѿ�DNS��������Ѿ����ڵ�����ֱ�Ӵ�������ص����� */
		dns_ctx.callback(&dns_ctx);

		/* �˴�֮������Ҫ�ͷŸö�������Ϊ dns_cache_lookup ���صĶ���Ϊ��̬
		 * �����(acl_netdb_clone)
		 */
		acl_netdb_free(dns_db);
		return (0);
	}

	/* ���ѯ�̳߳ط��Ͳ�ѯָ�� */
	
	if (MSG_SEND(mio, DNS_MSG_LOOKUP, &dns_ctx, sizeof(DNS_CTX)) < 0) {
		acl_msg_error("%s: send msg error, domain(%s)",
			myname, dns_ctx.domain_key);
		return (-1);
	}

	return (0);
}
