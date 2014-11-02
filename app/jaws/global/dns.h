
#ifndef __DNS_SERVER_INCLUDE_H__
#define __DNS_SERVER_INCLUDE_H__

#include "lib_acl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USE_THREAD_POOL			/* ��Ҳ�ѯDNSģ������̳߳ط�ʽ */

/* ��ѯDNS����Ϣ���Ͷ��� */
#define DNS_MSG_LOOKUP		ACL_MSGIO_USER + 100
#define DNS_MSG_LOOKUP_RESULT	ACL_MSGIO_USER + 101

#define DNS_IP_MAX		5

typedef struct DNS_CTX DNS_CTX;
typedef struct DNS_SERVER DNS_SERVER;
typedef struct DNS_CACHE DNS_CACHE;

typedef void (*DNS_CALLBACK)(const DNS_CTX *);

struct DNS_CTX {
	/* public */
	char  domain_key[256];		/* ����(Сд) */
	char  ip[DNS_IP_MAX][64];	/* IP��ַ���� */
	int   port[DNS_IP_MAX];		/* port ���� */
	int   ip_cnt;			/* IP��ַ�����е���ЧIP��ַ���� */
	time_t begin;			/* ��ʼ��ѯʱ��ʱ��� */

	/* public */
	DNS_CALLBACK callback;		/* ��ѯ���ص��õ��û��Ļص����� */
	void *context;			/* �û����ݵĲ��� */

	/* private */
	DNS_SERVER *dns;		/* DNS���������ڲ����� */
	ACL_MSGIO  *mio;		/* IO��Ϣ������ڲ����� */
	ACL_DNS_DB *dns_db;		/* ��ʱ���Դ���DNS��ѯ������ڲ����� */
};

struct DNS_SERVER {
	ACL_AIO *aio;			/* �첽ͨ�ſ�� */
	acl_pthread_pool_t *wq;		/* �̳߳ؾ�� */
	ACL_MSGIO *listener;		/* ��Ϣ�����߾�� */
	ACL_MSGIO *mio;			/* ��Ϣ��� */
	char  addr[256];

	DNS_CACHE *dns_cache;		/* DNS������ */
	acl_pthread_mutex_t lock;	/* �߳��� */
};

/* in dns_server.c */
DNS_SERVER *dns_server_create(ACL_AIO *aio, int timeout);
void dns_server_close(DNS_SERVER *dns);
void dns_server_static_add(DNS_SERVER *dns, const char *map, const char *delim, int def_port);
int dns_server_lookup(DNS_SERVER *dns, const DNS_CTX *ctx);

/* in dns_cache.c */
void dns_cache_push_one(DNS_CACHE *dns_cache, const ACL_DNS_DB *dns_db, int timeout);
void dns_cache_push(DNS_CACHE *dns_cache, const ACL_DNS_DB *dns_db);
void dns_cache_push2(DNS_CACHE *dns_cache, const DNS_CTX *dns_ctx);

ACL_DNS_DB *dns_cache_lookup(DNS_CACHE *dns_cache, const char *name);
void dns_cache_del_host(DNS_CACHE *dns_cache, const char *name);
DNS_CACHE *dns_cache_create(int timeout, int thread_safe);

/* in dns_hosts.c */
void dns_hosts_load(ACL_DNS *dns_handle, const char *filename);

#ifdef __cplusplus
}
#endif
#endif
