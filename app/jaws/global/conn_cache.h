#ifndef	__CONN_CACHE_INCLUDE_H__
#define	__CONN_CACHE_INCLUDE_H__

#include "lib_acl.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct CONN_CACHE {
	ACL_AIO *aio;
	ACL_HTABLE *cache;
	int   conn_limit;
	int   nset;
	int   nget;
	int   nclose;
} CONN_CACHE;

typedef struct CONN CONN;

/**
 * ���ӳس�ʼ������, �ú������ڳ����ʼ��ʱ������һ��
 * @param aio {ACL_AIO*} �첽������
 * @param conn_limit {int} ÿһ�����ӳص���������
 * @return {CONN_CACHE*} ����һ�����ӳصĻ������
 */
CONN_CACHE *conn_cache_create(ACL_AIO *aio, int conn_limit);

/**
 * �����ӳ������һ��������
 * @param cache {CONN_CAHCE*} �����ӻ������
 * @param timeout {int} �����ӵĳ�ʱʱ��
 * @param free_fn {void (*)(ACL_ASTREAM*, void*)} �ر�����ʱ�Ļص�����,
 *  ����ò����ǿգ��򵱸����ӱ��ر�ǰ���Զ����� free_fn
 * @param ctx {void*} free_fn �Ĳ���֮һ
 */
void conn_cache_push_stream(CONN_CACHE *cache, ACL_ASTREAM *stream,
	int timeout, void (*free_fn)(ACL_ASTREAM*, void*), void *ctx);

/**
 * �����ӳ�ȡ����Ӧĳ����ֵ�����Ӷ���
 * @param cache {CONN_CAHCE*} �����ӻ������
 * @param key {const char*} ��ѯ��ֵ���磺192.168.0.1:80
 * @return {CONN*} ���Ӷ���, ��Ϊ NULL ���ʾ������
 */
CONN *conn_cache_get_conn(CONN_CACHE *cache, const char *key);

/**
 * �����ӳ�ȡ����Ӧĳ����ֵ��������
 * @param cache {CONN_CAHCE*} �����ӻ������
 * @param key {const char*} ��ѯ��ֵ���磺192.168.0.1:80
 * @param ctx_pptr {void**} �������ǿգ���洢�û����Զ������
 * @return {ACL_ASTREAM*} ������
 */
ACL_ASTREAM *conn_cache_get_stream(CONN_CACHE *cache, const char *key, void **ctx_pptr);

/**
 * �����ӳ���ɾ����Ӧĳ����ֵ������
 * @param cache {CONN_CAHCE*} �����ӻ������
 * @param key {const char*} ��ѯ��ֵ���磺192.168.0.1:80
 */
void conn_cache_delete_key(CONN_CACHE *cache, const char *key);

/**
 * �����ӳ���ɾ��ĳ�����Ӷ����ҹر�������
 * @param conn {CONN*} ���Ӷ���
 */
void conn_cache_delete_conn(CONN *conn);

/**
 * �����ӳ���ɾ��ĳ����������ɾ����Ӧ�����Ӷ����ҹر�������
 * @param cache {CONN_CAHCE*} �����ӻ������
 * @param stream {ACL_ASTREAM*} ������
 */
void conn_cache_delete_stream(CONN_CACHE *cache, ACL_ASTREAM *stream);

#ifdef	__cplusplus
}
#endif

#endif
