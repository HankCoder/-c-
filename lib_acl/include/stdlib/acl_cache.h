#ifndef	ACL_CACHE_INCLUDE_H
#define	ACL_CACHE_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "acl_define.h"
#include "acl_htable.h"
#include "acl_ring.h"
#include <time.h>

/**
 * ������д洢�Ļ������
 */
typedef struct ACL_CACHE_INFO {
	char *key;		/**< ��ֵ */
	void *value;		/**< �û���̬���� */
	int   nrefer;		/**< ���ü��� */
	time_t when_timeout;	/**< ����ʱ��� */
	ACL_RING entry;		/**< �ڲ���������Ա */
} ACL_CACHE_INFO;

/**
 * �����
 */
typedef struct ACL_CACHE { 
	ACL_HTABLE *table;	/**< ��ϣ�� */
	ACL_RING ring;		/**< ����ɾ���Ķ������������ */
	int   max_size;		/**< �����������С����ֵ */
	int   size;		/**< ��ǰ������еĻ��������� */
	int   timeout;		/**< ÿ��������������ʱ��(��) */

	/**< �ͷ��û���̬������ͷŻص����� */
	void  (*free_fn)(const ACL_CACHE_INFO*, void *);
	acl_pthread_mutex_t lock;	/**< ������� */
	ACL_SLICE *slice;		/**< �ڴ���Ƭ���� */

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ACL_ITER*, struct ACL_CACHE*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ACL_ITER*, struct ACL_CACHE*);
	/* ȡ������β���� */
	void *(*iter_tail)(ACL_ITER*, struct ACL_CACHE*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ACL_ITER*, struct ACL_CACHE*);
	/* ȡ�����������ĵ�ǰ������Ա�ṹ���� */
	ACL_CACHE_INFO *(*iter_info)(ACL_ITER*, struct ACL_CACHE*);
} ACL_CACHE;

/**
 * ����һ������أ�������ÿ������������󻺴�ʱ�����û���صĿռ���������
 * @param max_size {int} �û���ص���������
 * @param timeout {int} ÿ���������Ļ���ʱ��
 * @param free_fn {void (*)(void*)} �û������ͷŻ������ĺ���
 * @return {ACL_CACHE*} ����ض�����
 */
ACL_API ACL_CACHE *acl_cache_create(int max_size, int timeout,
	void (*free_fn)(const ACL_CACHE_INFO*, void*));

/**
 * �ͷ�һ������أ����Զ����� acl_cache_create()/3 �е��ͷź����ͷŻ������
 * @param cache {ACL_CACHE*} ����ض�����
 */
ACL_API void acl_cache_free(ACL_CACHE *cache);

/**
 * �򻺴������ӱ�����Ķ���
 * @param cache {ACL_CACHE*} ����ض�����
 * @param key {const char*} �������Ľ�ֵ
 * @param value {void*} ��̬�������
 * @return {ACL_CACHE_INFO*} ��������������Ľṹ�������е� value ���û��Ķ�����ͬ,
 *   ������� NULL ���ʾ���ʧ�ܣ�ʧ��ԭ��Ϊ�������̫���������ͬ��ֵ�Ķ������
 *   �����ü�����0; ������ط� NULL ���ʾ��ӳɹ��������ͬһ��ֵ���ظ���ӣ�����
 *   �µ������滻�ɵ����ݣ��Ҿ����ݵ����ͷź��������ͷ�
 */
ACL_API ACL_CACHE_INFO *acl_cache_enter(ACL_CACHE *cache, const char *key, void *value);

/**
 * �ӻ�����в���ĳ��������Ķ���
 * @param cache {ACL_CACHE*} ����ض�����
 * @param key {const char*} ��ѯ��
 * @return {void*} ��������û�����ĵ�ַ��ΪNULLʱ��ʾδ�ҵ�
 */
ACL_API void *acl_cache_find(ACL_CACHE *cache, const char *key);

/**
 * �ӻ�����в���ĳ��������Ķ����������Ļ�����Ϣ����
 * @param cache {ACL_CACHE*} ����ض�����
 * @param key {const char*} ��ѯ��
 * @return {ACL_CACHE_INFO*} ������Ϣ�����ַ��ΪNULLʱ��ʾδ�ҵ�
 */
ACL_API ACL_CACHE_INFO *acl_cache_locate(ACL_CACHE *cache, const char *key);

/**
 * �ӻ������ɾ��ĳ���������
 * @param cache {ACL_CACHE*} ����ض�����
 * @param info {ACL_CACHE_INFO*} �û������������Ļ�����Ϣ����
 * @return {int} 0: ��ʾɾ���ɹ�; -1: ��ʾ�ö�������ü�����0��ö��󲻴���
 */
ACL_API int acl_cache_delete(ACL_CACHE *cache, ACL_CACHE_INFO *info);

/**
 * �ӻ������ɾ��ĳ���������
 * @param cache {ACL_CACHE*} ����ض�����
 * @param key {const char*} ��ֵ
 * @return {int} 0: ��ʾɾ���ɹ�; -1: ��ʾ�ö�������ü�����0��ö��󲻴���
 */
ACL_API int acl_cache_delete2(ACL_CACHE *cache, const char *key);

/**
 * ʹ������еĹ��ڶ����Զ�ɾ��
 * @param cache {ACL_CACHE*} ����ض�����
 * @return {int} >= 0: ���Զ�ɾ���Ļ������ĸ���
 */
ACL_API int acl_cache_timeout(ACL_CACHE *cache);

/**
 * ʹĳ���������Ļ���ʱ��ӳ�
 * @param cache {ACL_CACHE*} ����ض�����
 * @param info {ACL_CACHE_INFO*} �������
 * @param timeout {int} ����ʱ��(��)
 */
ACL_API void acl_cache_update2(ACL_CACHE *cache, ACL_CACHE_INFO *info, int timeout);

/**
 * ʹĳ���������Ļ���ʱ��ӳ�
 * @param cache {ACL_CACHE*} ����ض�����
 * @param key {const char*} ��ֵ
 * @param timeout {int} ����ʱ��(��)
 */
ACL_API void acl_cache_update(ACL_CACHE *cache, const char *key, int timeout);

/**
 * ����ĳ�����������ü�������ֹ����ǰɾ��
 * @param info {ACL_CACHE_INFO*} �û������������Ļ�����Ϣ����
 */
ACL_API void acl_cache_refer(ACL_CACHE_INFO *info);

/**
 * ����ĳ�����������ü�������ֹ����ǰɾ��
 * @param cache {ACL_CACHE*} ����ض�����
 * @param key {const char*}
 */
ACL_API void acl_cache_refer2(ACL_CACHE *cache, const char *key);

/**
 * ����ĳ�����������ü���
 * @param info {ACL_CACHE_INFO*} �û������������Ļ�����Ϣ����
 */
ACL_API void acl_cache_unrefer(ACL_CACHE_INFO *info);

/**
 * ����ĳ�����������ü���
 * @param cache {ACL_CACHE*} ����ض�����
 * @param key {const char*}
 */
ACL_API void acl_cache_unrefer2(ACL_CACHE *cache, const char *key);

/**
 * ��������ض����ڶ��߳�ʱ��
 * @param cache {ACL_CACHE*} ����ض�����
 */
ACL_API void acl_cache_lock(ACL_CACHE *cache);

/**
 * ��������ض����ڶ��߳�ʱ��
 * @param cache {ACL_CACHE*} ����ض�����
 */
ACL_API void acl_cache_unlock(ACL_CACHE *cache);

/**
 * ���������е����ж���
 * @param cache {ACL_CACHE*} ����ض�����
 * @param walk_fn {void (*)(ACL_CACHE_INFO*, void*)} �����ص�����
 * @param arg {void *} walk_fn()/2 �еĵڶ�������
 */
ACL_API void acl_cache_walk(ACL_CACHE *cache, void (*walk_fn)(ACL_CACHE_INFO *, void *), void *arg);

/**
 * ��ջ�����еĻ���������ĳ�����������Ȼ�ڱ������ҷ�ǿ����ɾ�����򽫲��ᱻ���
 * @param cache {ACL_CACHE*} ����ض�����
 * @param force {int} �����0����ʹĳ�������������ü�����0Ҳ�ᱻɾ��
 * @return {int} ������Ļ���������
 */
ACL_API int acl_cache_clean(ACL_CACHE *cache, int force);

/**
 * ��ǰ������л������ĸ���
 * @param cache {ACL_CACHE*} ����ض�����
 * @return {int} ������Ķ������
 */
ACL_API int acl_cache_size(ACL_CACHE *cache);

#ifdef	__cplusplus
}
#endif

#endif
