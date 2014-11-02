#include "lib_acl.h"

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include "dns.h"
#include "dns_lookup.h"

#define	STRNCPY	ACL_SAFE_STRNCPY

/*----------------------------------------------------------------------------*/

static void inner_nslookup_error(CLIENT_ENTRY *entry)
{
	if (entry->client == NULL)
		acl_msg_fatal("%s(%d): client null",
			__FILE__, __LINE__);

	if (entry->dns_errmsg) {
		acl_aio_refer(entry->client);
		acl_aio_writen(entry->client, entry->dns_errmsg,
			(int) strlen(entry->dns_errmsg));
		/* �ָ�referֵ */
		acl_aio_unrefer(entry->client);
	}

	entry->nslookup_notify_fn(entry, NSLOOKUP_ERR);
}

static void inner_nslookup_ok(CLIENT_ENTRY *entry, ACL_DNS_DB *dns_db)
{
	ACL_ITER iter;
	int   i = 0;

	entry->ip_idx = 0;
	entry->dns_ctx.ip_cnt = 0;
	acl_foreach(iter, dns_db) { 
		const ACL_HOST_INFO *info;

		info = (const ACL_HOST_INFO*) iter.data;
		ACL_SAFE_STRNCPY(entry->dns_ctx.ip[i], info->ip,
			sizeof(entry->dns_ctx.ip[i]));
		entry->dns_ctx.port[i++] = info->hport;
		entry->dns_ctx.ip_cnt++;
		if (entry->dns_ctx.ip_cnt >= DNS_IP_MAX)
			break;
	}
	entry->nslookup_notify_fn(entry, NSLOOKUP_OK);
}

static void inner_nslookup_complete(ACL_DNS_DB *dns_db, void *ctx,
	int errnum acl_unused)
{
	CLIENT_ENTRY *entry = (CLIENT_ENTRY*) ctx;

	if (dns_db != NULL)
		inner_nslookup_ok(entry, dns_db);
	else
		inner_nslookup_error(entry);
}

/* ��ѯDNS��Ϣ������Э�鷢�͵ķ�ʽ */
static void inner_nslookup(SERVICE *service, CLIENT_ENTRY *entry,
	const char *domain, int port)
{
	char *ptr;

	STRNCPY(entry->dns_ctx.domain_key, domain,
		sizeof(entry->dns_ctx.domain_key));
	acl_lowercase(entry->dns_ctx.domain_key);
	ptr = strchr(entry->dns_ctx.domain_key, ':');
	if (ptr)
		*ptr = 0;
	entry->server_port = port;
	if (entry->server_port <= 0)
		entry->server_port = 80;

	STRNCPY(entry->domain_key, entry->dns_ctx.domain_key,
		sizeof(entry->domain_key));
	acl_dns_lookup(service->dns_handle, entry->domain_key,
		inner_nslookup_complete, entry);
}

/*----------------------------------------------------------------------------*/

/* DNS��ѯ���֮��Ļص����� */
static void thrpool_nslookup_complete(const DNS_CTX *dns_ctx)
{
	const char *myname = "thrpool_nslookup_complte";
	SERVICE *service = (SERVICE *) dns_ctx->context;
	DNS_RING *list;
	ACL_RING *ring_ptr;
	CLIENT_ENTRY *entry;
	time_t inter, now;

	list = (DNS_RING *) acl_htable_find(service->dns_table,
				dns_ctx->domain_key);
	if (list == NULL) {
		acl_msg_warn(NULL, "%s: domain(%s) not found maybe handled",
			myname, dns_ctx->domain_key);
		return;
	}

	time(&now);
	inter = now - dns_ctx->begin;

	/* �����ѯʱ������������������Ϣ */
	if (inter >= 5)
		acl_msg_warn("%s(%d): dns search time=%d, domain(%s)",
			myname, __LINE__, time(NULL) - dns_ctx->begin,
			dns_ctx->domain_key);

	while (1) {
		ring_ptr = acl_ring_pop_head(&list->ring);
		if (ring_ptr == NULL)
			break;

		list->nrefer--;

		entry = ACL_RING_TO_APPL(ring_ptr, CLIENT_ENTRY, dns_entry);

		entry->tm.dns_lookup = now - entry->tm.stamp;
		entry->tm.stamp = now;

		if (dns_ctx->ip_cnt <= 0) {
			acl_msg_error("%s(%d): dns not found domain(%s)",
				myname, __LINE__, dns_ctx->domain_key);
			if (entry->client == NULL)
				acl_msg_fatal("%s(%d): client null",
					__FILE__, __LINE__);

			if (entry->dns_errmsg) {
				/* XXX: ��Ϊ�˴����ܻ��������ر� client ���ĵط�:
				 * acl_aio_writen �� forward_complete��Ϊ��ֹ�ظ�
				 * �ر���ɵ��ڴ���ʷǷ�����Ҫ * �ڵ�һ�����ܹر�
				 * �ĺ���(acl_aio_writen)����ǰ���� client ����
				 * ����ֵ�������ڸú������غ��ٻָ�����ֵ
				 */

				acl_aio_refer(entry->client);
				acl_aio_writen(entry->client, entry->dns_errmsg,
					(int) strlen(entry->dns_errmsg));
				/* �ָ�referֵ */
				acl_aio_unrefer(entry->client);
			}

			entry->nslookup_notify_fn(entry, NSLOOKUP_ERR);

			continue;
		}

		if (acl_do_debug(20, 2)) {
			int   i;

			for (i = 0; i < dns_ctx->ip_cnt; i++)
				acl_msg_info("%s(%d): domain(%s), ip%d(%s)",
					myname, __LINE__, dns_ctx->domain_key,
					i, dns_ctx->ip[i]);
		}
		
		/* ����õ�����DNS�������������������� */
		memcpy(&entry->dns_ctx, dns_ctx, sizeof(entry->dns_ctx));

		/* ����ע�Ͳ��ֱ��򿪣�����Բ����������Թ���:)-- zsx, 2008.2.28
		 * strcpy(proxy_entry->dns_ctx.ip[1], proxy_entry->dns_ctx.ip[0]);
		 * strcpy(proxy_entry->dns_ctx.ip[0], "127.0.0.1");
		 * if (proxy_entry->dns_ctx.ip_cnt < 2)
		 *	proxy_entry->dns_ctx.ip_cnt = 2;
		 */
		entry->ip_idx = 0;
		entry->nslookup_notify_fn(entry, NSLOOKUP_OK);
	}

	acl_htable_delete(service->dns_table, dns_ctx->domain_key, NULL);
	if (list->nrefer <= 0)
		acl_myfree(list);
	else
		acl_msg_fatal("%s(%d): list's nrefer=%d",
			myname, __LINE__, list->nrefer);
}

/* ��ѯDNS��Ϣ���������ģ��ķ�ʽ */
static void thrpool_nslookup(SERVICE *service, CLIENT_ENTRY *entry,
	const char *domain, int port)
{
	const char *myname = "thrpool_nslookup";
	DNS_CTX dns_ctx;
	DNS_RING *list;
	char *ptr;

	entry->tm.stamp = time(NULL);

	memset(&dns_ctx, 0, sizeof(dns_ctx));
	dns_ctx.begin = entry->tm.stamp;
	STRNCPY(dns_ctx.domain_key, domain, sizeof(dns_ctx.domain_key));
	ptr = strchr(dns_ctx.domain_key, ':');
	/* �������������� */
	if (ptr)
		*ptr = 0;

	entry->server_port = port;
	if (entry->server_port <= 0)
		entry->server_port = 80;
	
	/* �������ַ�����ת����Сд���Ա��ڽ��й�ϣ��ѯ */
	acl_lowercase(dns_ctx.domain_key);

	dns_ctx.context = service;
	dns_ctx.callback = thrpool_nslookup_complete;

	STRNCPY(entry->domain_key, dns_ctx.domain_key, sizeof(entry->domain_key));

	/* �Ȳ�ѯDNS��ѯ�����Ƿ��Ѿ�����������Ҫ����ѯ������ */
	list = (DNS_RING *) acl_htable_find(service->dns_table, dns_ctx.domain_key);
	if (list) {
		/* �����ζ�ͬһ�����Ĳ�ѯ��ӽ�ͬһ����ѯ���� */
		acl_ring_prepend(&list->ring, &entry->dns_entry);
		/* ����ѯ����������ü�����1 */
		list->nrefer++;
		/* ����ò�ѯ���Ѿ����ڣ�˵���в�ѯ����ȴ����أ��䷵�غ��һͬ��
		 * ����������д���������˴������������������ڴ���ʳ�ͻ����Ϊ
		 * ��ѯDNS�Ĺ�������һ���̳߳ؽ��в�ѯ�ġ�
		 * (void) dns_server_lookup(proxy_entry->aio_proxy->dns_server, &dns_ctx);
		 */
		return;
	}

	/* ����һ���µĲ�ѯ�����󣬲������β�ѯ�������ò�ѯ���м����ò�ѯ�������ѯ���� */

	list = (DNS_RING *) acl_mycalloc(1, sizeof(DNS_RING));
	acl_ring_init(&list->ring);
	STRNCPY(list->domain_key, dns_ctx.domain_key, sizeof(list->domain_key));

	/* �����β�ѯ��������µĲ�ѯ�����ҽ���ѯ�������ü�����1 */
	acl_ring_prepend(&list->ring, &entry->dns_entry);
	list->nrefer++;

	/* ���µĲ�ѯ�������ѯ���� */
	if (acl_htable_enter(service->dns_table, list->domain_key, (char *) list) == NULL)
		acl_msg_fatal("%s: add domain(%s) to table error", myname, list->domain_key);

	/* ��ʼ����DNS��ѯ���� */
	(void) dns_server_lookup(service->dns_server, &dns_ctx);
}

/*----------------------------------------------------------------------------*/

void dns_lookup(CLIENT_ENTRY *entry, const char *domain, int port)
{
	const char *myname = "dns_lookup";
	SERVICE *service = entry->service;

	if (acl_ipv4_valid(domain)) {
		entry->ip_idx = 0;
		ACL_SAFE_STRNCPY(entry->dns_ctx.ip[0], domain,
			sizeof(entry->dns_ctx.ip[0]));
		entry->dns_ctx.port[0] = port;
		entry->dns_ctx.ip_cnt = 1;
		entry->nslookup_notify_fn(entry, NSLOOKUP_OK);
		return;
	} else if (service->dns_handle) {
		inner_nslookup(service, entry, domain, port);
	} else if (service->dns_server) {
		thrpool_nslookup(service, entry, domain, port);
	} else {
		acl_msg_fatal("%s(%d): no dns lookup set", myname, __LINE__);
	}
}
