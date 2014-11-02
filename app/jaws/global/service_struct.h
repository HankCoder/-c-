#ifndef __SERVICE_STRUCT_INCLUDE_H__
#define __SERVICE_STRUCT_INCLUDE_H__

#include "lib_acl.h"
#include "conn_cache.h"
#include "dns.h"

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SERVICE SERVICE;
typedef struct CLIENT_ENTRY CLIENT_ENTRY;

/* ��̬����غ������Ͷ��� */

/* �����ʼ���������Ͷ��� */
typedef void (*module_service_init_fn)(ACL_DLL_ENV *dll_env, const char *cfgdir);
/* ���񴴽��������Ͷ��� */
typedef SERVICE *(*module_service_create_fn)(void);
/* ������ں������Ͷ��� */
typedef void (*module_service_main_fn)(SERVICE *service, ACL_ASTREAM *stream);

typedef struct {
	module_service_init_fn mod_init;
	module_service_create_fn mod_create;
	module_service_main_fn mod_main;
} MODULE_SERVICE;

struct SERVICE {
	char  name[256];			/* �������� */
	ACL_AIO *aio;				/* �첽IO��� */
	acl_pthread_pool_t *wq;			/* Ϊ�˼����ϵ����ģ������Ҫ���̳߳ؾ�� */
	ACL_ASTREAM *sstream;			/* �����׽ӿ� */
	int   conn_timeout;			/* Ĭ�ϵ����ӳ�ʱʱ�� */
	int   rw_timeout;			/* Ĭ�ϵ�IO��ʱʱ�� */
	ACL_DNS *dns_handle;			/* ����ֱ�ӷ���DNSЭ�鷽ʽ��ѯ */
	DNS_SERVER *dns_server;			/* DNS��ѯ��� */
	ACL_HTABLE *dns_table;			/* DNS��ѯ�����ϣ�� */
	char  local_addr[256];			/* �����ı��ص�ַ */
	char **bind_ip_list;			/* �����ӷ�����ʱ����󶨱�����IP��ַ�б� */
	int   bind_ip_index;			/* ��ǰ��Ҫ�󶨵�IP */

	CONN_CACHE *conn_cache;			/* ���������ӳض��� */
	MODULE_SERVICE *module;			/* �����ķ���ģ�� */
};

typedef struct DNS_RING {
	char  domain_key[256];
	ACL_RING ring;
	int   nrefer;
} DNS_RING;

struct CLIENT_ENTRY {
	SERVICE *service;			/* ָ���첽������ */
	ACL_ASTREAM *client;			/* �����ڿͻ��˵������� */
	ACL_ASTREAM *server;			/* ����������˵������� */

	int   nrefer;				/* ���ü��� */

	ACL_RING dns_entry;			/* DNS��ѯ���ĳ������� */
	char  domain_key[256];			/* ����(Сд) */
	char  domain_addr[64];			/* ��������Ӧ��һ��IP��ַ */

	DNS_CTX dns_ctx;			/* �������������صĵ�ַ��Ϣ */
	int   ip_idx;				/* ��ǰ�����õ�IP��ַ������ */
	int   ip_ntry;				/* ���� IP ���� */
	int   server_port;			/* �����PORT */
	char  client_ip[32];			/* �ͻ���IP��ַ */
	int   client_port;			/* �����PORT */
	const char *dns_errmsg;

	struct {
		time_t begin;			/* �Ự��ʼʱ��� */
		time_t stamp;			/* ��̬�ı��ʱ��� */

		time_t dns_lookup;		/* DNS��ѯʱ��(����Ϊ��λ) */
		time_t connect;			/* ���ӷ�������ʱ��(����Ϊ��λ) */
	} tm;

	int   flag_has_replied;

	int   flag_conn_reuse;			/* �ظ��������ӳ��е����� */
	int   nretry_on_error;			/* �������Դ��� */
#define MAX_RETRIED	10

	void (*free_fn)(CLIENT_ENTRY*);
	void (*nslookup_notify_fn)(CLIENT_ENTRY*, int);
#define	NSLOOKUP_OK	0
#define	NSLOOKUP_ERR	-1

	int  (*connect_notify_fn)(CLIENT_ENTRY*);
	void (*connect_timeout_fn)(CLIENT_ENTRY*);
	void (*connect_error_fn)(CLIENT_ENTRY*);
};

#ifdef __cplusplus
}
#endif

#endif
