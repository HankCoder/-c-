#ifndef	ACL_NETDB_INCLUDE_H
#define	ACL_NETDB_INCLUDE_H

#include "stdlib/acl_define.h"

#ifdef	ACL_UNIX
#include <netinet/in.h>
#endif

#include "stdlib/acl_array.h"

/**
 * ������ַ�ṹ
 */
typedef struct ACL_HOSTNAME ACL_HOST_INFO;
typedef struct ACL_HOSTNAME {
	char  ip[64];                   /**< the ip addr of the HOST */
	struct sockaddr_in saddr;	/**< ip addr in sockaddr_in */
	unsigned int ttl;               /**< the HOST's ip timeout(second) */
	int   hport;
	unsigned int nrefer;            /**< refer number to this HOST */
} ACL_HOSTNAME;

/**
 * DNS��ѯ�����
 */
typedef struct ACL_DNS_DB {
	ACL_ARRAY *h_db;
	int   size;
	char  name[256];

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	const ACL_HOST_INFO *(*iter_head)(ACL_ITER*, struct ACL_DNS_DB*);
	/* ȡ��������һ������ */
	const ACL_HOST_INFO *(*iter_next)(ACL_ITER*, struct ACL_DNS_DB*);
	/* ȡ������β���� */
	const ACL_HOST_INFO *(*iter_tail)(ACL_ITER*, struct ACL_DNS_DB*);
	/* ȡ��������һ������ */
	const ACL_HOST_INFO *(*iter_prev)(ACL_ITER*, struct ACL_DNS_DB*);
	/* ȡ�����������ĵ�ǰ������Ա�ṹ���� */
	const ACL_HOST_INFO *(*iter_info)(ACL_ITER*, struct ACL_DNS_DB*);
} ACL_DNS_DB;

/* in acl_netdb.c */

/**
 * �ӽ������ȡ��ĳ���±�λ�õ�������ַ�ṹ
 * @param h_dns_db {const ACL_DNS_DB*} DNS�����
 * @param i {int} �±�λ��
 * @return {const ACL_HOSTNAME*} ������Ӧ�±��������ַ�ṹ
 */
ACL_API const ACL_HOSTNAME *acl_netdb_index(const ACL_DNS_DB *h_dns_db, int i);

/**
 * �ӽ������ȡ��ĳ���±�λ�õ�����IP��ַ
 * @param h_dns_db {const ACL_DNS_DB*} DNS�����
 * @param i {int} �±�λ��
 * @return {const struct sockaddr_in*} IP��ַ�ṹ, NULL��ʾʧ��
 */
ACL_API const struct sockaddr_in *acl_netdb_index_saddr(ACL_DNS_DB *h_dns_db, int i);

/**
 * ��������еĶ�Ӧĳ���±��������ַ��������
 * @param h_dns_db {const ACL_DNS_DB*} DNS�����
 * @param i {int} �±�λ��
 * @param n {int} ��Ҫ���ӵ�����ֵ
 */
ACL_API void acl_netdb_refer_oper(ACL_DNS_DB *h_dns_db, int i, int n);

/**
 * ��������еĶ�Ӧĳ���±��������ַ���ü�1
 * @param h_dns_db {const ACL_DNS_DB*} DNS�����
 * @param i {int} �±�λ��
 */
ACL_API void acl_netdb_refer(ACL_DNS_DB *h_dns_db, int i);

/**
 * ��������еĶ�Ӧĳ���±��������ַ���ü�1
 * @param h_dns_db {const ACL_DNS_DB*} DNS�����
 * @param i {int} �±�λ��
 */
ACL_API void acl_netdb_unrefer(ACL_DNS_DB *h_dns_db, int i);

/**
 * ��������еĶ�Ӧĳ���±��IP��ַ�����ַ�����ʾ
 * @param h_dns_db {const ACL_DNS_DB*} DNS�����
 * @param i {int} �±�λ��
 * @return {const char*} ��õĽ����NULL ��ʾʧ��
 */
ACL_API const char *acl_netdb_index_ip(const ACL_DNS_DB *h_dns_db, int i);

/**
 * ȡ�ý������������ַ�ĸ���
 * @param h_dns_db {const ACL_DNS_DB*} DNS�����
 * @return {int} ������ַ���� > 0, -1 ��ʾ������������
 */
ACL_API int acl_netdb_size(const ACL_DNS_DB *h_dns_db);

/**
 *  �ͷŽ�����ڴ���Դ
 * @param h_dns_db {ACL_DNS_DB*} DNS�����
 */
ACL_API void acl_netdb_free(ACL_DNS_DB *h_dns_db);

/**
 * ������������һ����ѯ������Ľṹ������������DNS��ѯ
 * @param domain {const char*} Ҫ��ѯ������
 * @return {ACL_DNS_DB*} �����Ľ��������
 */
ACL_API ACL_DNS_DB *acl_netdb_new(const char *domain);

/**
 * �����������IP��ַ
 * @param h_dns_db {ACL_DNS_DB*} ��ѯ���������
 * @param ip {const char*} Ҫ��ӵ�IP��ַ
 */
ACL_API void acl_netdb_addip(ACL_DNS_DB *h_dns_db, const char *ip);

/**
 * �����������IP��ַ���˿ں�
 * @param h_dns_db {ACL_DNS_DB*} ��ѯ���������
 * @param ip {const char*} Ҫ��ӵ�IP��ַ
 * @param port {int} Ҫ��ӵĶ˿ں�
 */
ACL_API void acl_netdb_add_addr(ACL_DNS_DB *h_dns_db, const char *ip, int port);

/**
 * ��¡һ����ѯ���������
 * @param h_dns_db {const ACL_DNS_DB*} Դ���������
 * @return {ACL_DNS_DB*} �¿�¡�Ľ��������
 */
ACL_API ACL_DNS_DB *acl_netdb_clone(const ACL_DNS_DB *h_dns_db);

/**
 * ��ѯĳ��������IP��ַ��
 * @param name {const char*} ����
 * @param h_error {int*} �����ѯʧ�ܴ洢����ԭ��
 * @return {ACL_DNS_DB*} ��ѯ�����, ���ΪNULL���ѯʧ��, ���⣬��ʹ���ز�Ϊ�գ�
 *  Ҳ����Ҫͨ�� acl_netdb_size()/1 ��ý���������鳤��
 */
ACL_API ACL_DNS_DB *acl_gethostbyname(const char *name, int *h_error);

/**
 * ���ݴ���Ż�ó�����ʾ��Ϣ
 * @param errnum {int} �����
 * @return {const char*} ������Ϣ
 */ 
ACL_API const char *acl_netdb_strerror(int errnum);

/* in acl_netdb_cache.c */
/**
 * ��DNS��������ӻ�������
 * @param h_dns_db {const ACL_DNS_DB*} DNS��ѯ�����
 * @param timeout {int} �ý����������ĳ�ʱʱ�䣬��� <= 0, �����Ĭ�ϵ�ֵ��
 *  ��Ĭ��ֵ���� acl_netdb_cache_init()/2 �е�����ֵ, ��λΪ��
 */
ACL_API void acl_netdb_cache_push(const ACL_DNS_DB *h_dns_db, int timeout);

/**
 * ��DNS������ȡ��DNS��ѯ�����
 * @param name {const char*} ����
 * @return {ACL_DNS_DB*} DNS��ѯ�����
 */
ACL_API ACL_DNS_DB *acl_netdb_cache_lookup(const char *name);

/**
 * ��DNS������ɾ��ĳ��DNS��ѯ�����
 * @param name {const char*} ����
 */
ACL_API void acl_netdb_cache_del_host(const char *name);

/**
 * ��ʼ��DNS������
 * @param timeout {int} DNS�������Ĭ�ϻ���ʱ��(��)
 * @param thread_safe {int} �Ƿ���ҪDNS�������̰߳�ȫ, 0: ��ʾ����Ҫ,
 *  1: ��ʾ��Ҫ�̰߳�ȫ
 */
ACL_API void acl_netdb_cache_init(int timeout, int thread_safe);

#endif
