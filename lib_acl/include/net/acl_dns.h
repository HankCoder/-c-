#ifndef	ACL_DNS_INCLUDE_H
#define	ACL_DNS_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include "stdlib/acl_htable.h"
#include "stdlib/acl_cache2.h"
#ifdef  ACL_UNIX
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "event/acl_events.h"
#include "aio/acl_aio.h"
#include "acl_netdb.h"

/* DNS ��ѯʱ�Ĵ����붨�� */

#define	ACL_DNS_OK			0
#define	ACL_DNS_OK_CACHE		1
#define	ACL_DNS_ERR_FMT			-1
#define	ACL_DNS_ERR_SVR			-2
#define	ACL_DNS_ERR_NO_EXIST		-3
#define	ACL_DNS_ERR_NO_SUPPORT		-4
#define	ACL_DNS_ERR_DENY		-5
#define	ACL_DNS_ERR_YX			-6
#define	ACL_DNS_ERR_YXRR		-7
#define	ACL_DNS_ERR_NXRR		-8
#define	ACL_DNS_ERR_NO_AUTH		-9
#define	ACL_DNS_ERR_NOT_ZONE		-10
#define	ACL_DNS_ERR_UNPACK		-15
#define	ACL_DNS_ERR_TIMEOUT		-16
#define	ACL_DNS_ERR_EXIST		-17
#define	ACL_DNS_ERR_BUILD_REQ		-18

typedef struct ACL_DNS_ADDR {
	char  ip[64];			/* DNS ��������ַ */
	unsigned short port;		/* DNS �������˿� */
	struct sockaddr_in addr;	/* DNS ��ַ */
	int   addr_len;			/* addr ��С */
	int   mask_length;		/* DNS ������������������볤��(> 0 && < 32) */
	struct in_addr in;		/* addr �����ε�ַ */
} ACL_DNS_ADDR;

typedef struct ACL_DNS {
	ACL_AIO *aio;			/* �첽IO��� */
	unsigned short qid;		/* ���������ID��ʶ�� */
	ACL_ASTREAM *astream;		/* �첽�� */

	ACL_ARRAY *groups;		/* �������б� */
	ACL_ARRAY *dns_list;		/* DNS ��������ַ�б� */
	int   dns_idx;			/* ��ǰʹ�õ� dns_list �����±� */
	ACL_DNS_ADDR addr_from;		/* ��Դ DNS ��ַ */
	ACL_HTABLE *lookup_table;	/* ��ѯ����� */
	ACL_CACHE2 *dns_cache;		/* ���ڻ���DNS��ѯ��� */
	int   timeout;			/* ÿ�β�ѯ�ĳ�ʱʱ��ֵ(��) */
	int   retry_limit;		/* ��ѯ��ʱʱ���ԵĴ������� */
	unsigned int flag;		/* ��־λ */
#define	ACL_DNS_FLAG_ALLOC		(1 << 0)	/* ���첽����Ƕ�̬����� */
#define	ACL_DNS_FLAG_CHECK_DNS_IP	(1 << 1)	/* ���DNS��ַ�Ƿ�ƥ�� */
#define	ACL_DNS_FLAG_CHECK_DNS_NET	(1 << 2)	/* ���DNS�����Ƿ�ƥ�� */

	/* �ú���ָ���������⶯̬���ؿ�ķ��ʵ�ַ��һ������ */
	ACL_EVENT_NOTIFY_TIME lookup_timeout;
} ACL_DNS;

typedef struct ACL_DNS_REQ ACL_DNS_REQ;

/**
 * ��ʼ��DNS�첽��ѯ����ṹ
 * @param dns {ACL_DNS*} DNS�첽��ѯ���
 * @param aio {ACL_AIO*} �첽���
 * @param timeout {int} ÿ��DNS��ѯʱ�ĳ�ʱֵ
 */
ACL_API void acl_dns_init(ACL_DNS *dns, ACL_AIO *aio, int timeout);

/**
 * ����һ��DNS�첽��ѯ����ͬʱ���г�ʼ��
 * @param aio {ACL_AIO*} �첽���
 * @param timeout {int} ÿ��DNS��ѯʱ�ĳ�ʱֵ
 * @return {ACL_DNS*} DNS�첽��ѯ���
 */
ACL_API ACL_DNS *acl_dns_create(ACL_AIO *aio, int timeout);

/**
 * ��DNS�������
 * @param dns {ACL_DNS*} DNS�첽��ѯ���
 * @param limit {int} DNS ��������󻺴���Ŀ
 */
ACL_API void acl_dns_open_cache(ACL_DNS *dns, int limit);

/**
 * ���һ��DNS��������ַ
 * @param dns {ACL_DNS*} DNS�첽��ѯ���
 * @param dns_ip {const char*} DNS������IP��ַ
 * @param dns_port {unsigned short} DNS�������˿�
 * @param mask_length {int} DNS���������ڵ��������볤��(0 < && < 32)
 */
ACL_API void acl_dns_add_dns(ACL_DNS *dns, const char *dns_ip,
	unsigned short dns_port, int mask_length);
/**
 * �ر��첽��ѯ���ͬʱ�ͷ�������Դ
 * @param dns {ACL_DNS*} DNS�첽��ѯ���
 */
ACL_API void acl_dns_close(ACL_DNS *dns);

/**
 * ���ñ�־λ�����DNS��ԴIP��ַ�Ƿ���Ŀ���ַ��ͬ������ͬ����������
 * ���ݰ�����Ҫ��Ϊ�˷�ֹDNS��ѯʱ��UDP����
 * @param dns {ACL_DNS*} DNS�첽��ѯ���
 */
ACL_API void acl_dns_check_dns_ip(ACL_DNS *dns);

/**
 * ���ñ�־λ�����DNS��ԴIP���������Ƿ���Ŀ��������ͬ������ͬ����
 * ���������ݰ�����Ҫ��Ϊ�˷�ֹDNS��ѯʱ��UDP����
 * @param dns {ACL_DNS*} DNS�첽��ѯ���
 */
ACL_API void acl_dns_check_dns_net(ACL_DNS *dns);

/**
 * ����DNS��ѯ��ʱʱ���Դ���
 * @param dns {ACL_DNS*} DNS�첽��ѯ���
 * @param retry_limit {int} ���Դ���
 */
ACL_API void acl_dns_set_retry_limit(ACL_DNS *dns, int retry_limit);

/**
 * �첽��ѯһ��������Ӧ��A��¼IP��ַ����
 * @param dns {ACL_DNS*} DNS�첽��ѯ���
 * @param domain {const char*} ����
 * @param callback {void (*)(ACL_DNS_DB*, void*)} ��ѯ�ɹ���ʧ�ܵĻص�����,
 *  �����ظ� callback �� ACL_DNS_DB Ϊ�����ʾ��ѯʧ��, �ڶ�������Ϊ�û�����
 *  �Ĳ���, ����������Ϊ��ѯʧ��ʱ�Ĵ����
 * @param ctx {void*} callback �Ĳ���֮һ
 * @return {ACL_DNS_REQ*} ���ر���DNS��ѯ���¼�����, ��ΪNULL���ʾ����
 */
ACL_API ACL_DNS_REQ *acl_dns_lookup(ACL_DNS *dns, const char *domain,
	void (*callback)(ACL_DNS_DB *, void *, int), void *ctx);

/**
 * ��DNS��ѯ��������Ӿ�̬������Ϣ
 * @param dns {ACL_DNS*} DNS�첽��ѯ���
 * @param domain {const char*} ����
 * @param ip_list {const char*} IP��ַ�б��ָ���Ϊ ';'����: 192.168.0.1;192.168.0.2
 */
ACL_API void acl_dns_add_host(ACL_DNS *dns, const char *domain, const char *ip_list);

/**
 * ��DNS��ѯ��������Ӳ�ѯ��������Ϣ
 * @param dns {ACL_DNS*} DNS�첽��ѯ���
 * @param group {const char*} ������������: .test.com, �� a.test.com, b.test.com
 *  ������ .test.com ������
 * @param ip_list {const char*} ����ǿ�����þ�̬��ʽ���IP��ַ�б�
 * @param refer {const char*} ������Ĵ�������, �����ô�������������������ȥ��DNS��ѯ
 * @param excepts {ACL_ARGV*} ��Ȼ��Щ�������� group ����������ȴ���������������
 *  ��Ա����
 */
ACL_API void acl_dns_add_group(ACL_DNS *dns, const char *group, const char *refer,
		const char *ip_list, const char *excepts);
/**
 * ȡ��ĳ����ѯ�¼�����
 * @param handle {ACL_DNS_REQ*} ĳ��������ѯ�¼�
 */
ACL_API void acl_dns_cancel(ACL_DNS_REQ *handle);

/**
 * ���ݴ���ŵõ�����������Ϣ
 * @param errnum {int} DNS��ѯʱ���صĴ����
 * @return {const char*} ����������Ϣ
 */
ACL_API const char *acl_dns_serror(int errnum);

#ifdef	__cplusplus
}
#endif

#endif

