#ifndef ACL_RES_INCLUDE_H
#define ACL_RES_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include "acl_netdb.h"
#include <time.h>

/**
 * DNS���ؽ���Ĵ洢�ṹ
 */
typedef struct ACL_RES {
	char dns_ip[64];                /**< DNS��IP��ַ */
	unsigned short dns_port;        /**< DNS��Port */
	unsigned short cur_qid;         /**< �ڲ����������ݰ��ı�ʶ */
	time_t tm_spent;                /**< ��ѯʱ��ķ�(��) */
	int   errnum;
#define ACL_RES_ERR_SEND	-100    /**< д���� */
#define ACL_RES_ERR_READ	-101    /**< ������ */
#define ACL_RES_ERR_RTMO	-102    /**< ����ʱ */
#define ACL_RES_ERR_NULL	-103    /**< �ս�� */
#define ACL_RES_ERR_CONN	-104    /**< TCP��ʽʱ����ʧ�� */

	int transfer;                   /**< TCP/UDP ����ģʽ */
#define ACL_RES_USE_UDP		0       /**< UDP ����ģʽ */
#define ACL_RES_USE_TCP		1       /**< TCP ����ģʽ */

	int   conn_timeout;             /**< TCP ����ʱ�����ӳ�ʱʱ��, Ĭ��Ϊ10�� */
	int   rw_timeout;               /**< TCP/UDP �����IO��ʱʱ��, Ĭ��Ϊ10�� */
} ACL_RES;

/**
 * ����һ��DNS��ѯ����
 * @param dns_ip {const char*} DNS��IP��ַ
 * @param dns_port {unsigned short} DNS��Port
 * @return {ACL_RES*} �´����Ĳ�ѯ����
 */
ACL_API ACL_RES *acl_res_new(const char *dns_ip, unsigned short dns_port);

/**
 * �ͷ�һ��DNS��ѯ����
 * @param res {ACL_RES*} DNS��ѯ����
 */
ACL_API void acl_res_free(ACL_RES *res);

/**
 * ��ѯĳ��������IP��ַ
 * @param res {ACL_RES*} DNS��ѯ����
 * @param domain {const char*} Ҫ��ѯ������
 * @return {ACL_DNS_DB*} ��ѯ�Ľ����
 */
ACL_API ACL_DNS_DB *acl_res_lookup(ACL_RES *res, const char *domain);

/**
 * ���ݴ���Ż�ò�ѯʧ�ܵ�ԭ��
 * @param errnum {int} �����
 * @return {const char*} ������Ϣ
 */
ACL_API const char *acl_res_strerror(int errnum);

/**
 * ��õ�ǰ��ѯ�Ĵ�����Ϣ
 * @param res {ACL_RES*} DNS��ѯ����
 * @return {const char*} ������Ϣ
 */
ACL_API const char *acl_res_errmsg(const ACL_RES *res);

#ifdef __cplusplus
}
#endif

#endif

