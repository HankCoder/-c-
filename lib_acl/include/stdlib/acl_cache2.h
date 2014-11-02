#ifndef	ACL_CACHE2_INCLUDE_H
#define	ACL_CACHE2_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "acl_define.h"
#include <time.h>

/**
 * ������д洢�Ļ������
 */
typedef struct ACL_CACHE2_INFO {
	char *key;		/**< ��ֵ */
	void *value;		/**< �û���̬���� */
	int   nrefer;		/**< ���ü��� */
	time_t when_timeout;	/**< ����ʱ��� */
} ACL_CACHE2_INFO;

/**
 * �����
 */
typedef struct ACL_CACHE2 { 
	int   max_size;		/**< �����������С����ֵ */
	int   size;		/**< ��ǰ������еĻ��������� */

	/**< �ͷ��û���̬������ͷŻص����� */
	void  (*free_fn)(const ACL_CACHE2_INFO*, void *);

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ACL_ITER*, struct ACL_CACHE2*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ACL_ITER*, struct ACL_CACHE2*);
	/* ȡ������β���� */
	void *(*iter_tail)(ACL_ITER*, struct ACL_CACHE2*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ACL_ITER*, struct ACL_CACHE2*);
	/* ȡ�����������ĵ�ǰ������Ա�ṹ���� */
	ACL_CACHE2_INFO *(*iter_info)(ACL_ITER*, struct ACL_CACHE2*);
} ACL_CACHE2;

/**
 * ����һ������أ�������ÿ������������󻺴�ʱ�����û���صĿռ���������
 * @param max_size {int} �û���ص���������
 * @param free_fn {void (*)(void*)} �û������ͷŻ������ĺ���
 * @return {ACL_CACHE2*} ����ض�����
 */
ACL_API ACL_CACHE2 *acl_cache2_create(int max_size,
	void (*free_fn)(const ACL_CACHE2_INFO*, void*));

/**
 * �ͷ�һ�������
 * @param cache2 {ACL_CACHE2*} ����ض�����
 */
ACL_API void acl_cache2_free(ACL_CACHE2 *cache2);

/**
 * �򻺴������ӱ�����Ķ���
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param key {const char*} �������Ľ�ֵ
 * @param value {void*} ��̬�������
 * @param timeout {int} ÿ���������Ļ���ʱ��
 * @return {ACL_CACHE2_INFO*} ��������������Ľṹ�������е� value ���û��Ķ�����ͬ,
 *   ������� NULL ���ʾ���ʧ�ܣ�ʧ��ԭ��Ϊ�������̫���������ͬ��ֵ�Ķ������
 *   �����ü�����0; ������ط� NULL ���ʾ��ӳɹ��������ͬһ��ֵ���ظ���ӣ�����
 *   �µ������滻�ɵ����ݣ��Ҿ����ݵ����ͷź��������ͷ�
 */
ACL_API ACL_CACHE2_INFO *acl_cache2_enter(ACL_CACHE2 *cache2,
	const char *key, void *value, int timeout);

/**
 * �ӻ�����в���ĳ��������Ķ���
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param key {const char*} ��ѯ��
 * @return {void*} ��������û�����ĵ�ַ��ΪNULLʱ��ʾδ�ҵ�
 */
ACL_API void *acl_cache2_find(ACL_CACHE2 *cache2, const char *key);

/**
 * �ӻ�����в���ĳ��������Ķ����������Ļ�����Ϣ����
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param key {const char*} ��ѯ��
 * @return {ACL_CACHE2_INFO*} ������Ϣ�����ַ��ΪNULLʱ��ʾδ�ҵ�
 */
ACL_API ACL_CACHE2_INFO *acl_cache2_locate(ACL_CACHE2 *cache2, const char *key);

/**
 * �ӻ������ɾ��ĳ���������
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param info {ACL_CACHE2_INFO*} �û������������Ļ�����Ϣ����
 * @return {int} 0: ��ʾɾ���ɹ�; -1: ��ʾ�ö�������ü�����0��ö��󲻴���
 */
ACL_API int acl_cache2_delete(ACL_CACHE2 *cache2, ACL_CACHE2_INFO *info);

/**
 * �ӻ������ɾ��ĳ���������
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param key {const char*} ��ֵ
 * @return {int} 0: ��ʾɾ���ɹ�; -1: ��ʾ�ö�������ü�����0��ö��󲻴���
 */
ACL_API int acl_cache2_delete2(ACL_CACHE2 *cache2, const char *key);

/**
 * ʹ������еĹ��ڶ����Զ�ɾ��
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @return {int} >= 0: ���Զ�ɾ���Ļ������ĸ���
 */
ACL_API int acl_cache2_timeout(ACL_CACHE2 *cache2);

/**
 * ʹĳ���������Ļ���ʱ��ӳ�
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param info {ACL_CACHE2_INFO*} �������
 * @param timeout {int} ����ʱ��(��)
 */
ACL_API void acl_cache2_update2(ACL_CACHE2 *cache2, ACL_CACHE2_INFO *info, int timeout);

/**
 * ʹĳ���������Ļ���ʱ��ӳ�
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param key {const char*} ��ֵ
 * @param timeout {int} ����ʱ��(��)
 */
ACL_API void acl_cache2_update(ACL_CACHE2 *cache2, const char *key, int timeout);

/**
 * ����ĳ�����������ü�������ֹ����ǰɾ��
 * @param info {ACL_CACHE2_INFO*} �û������������Ļ�����Ϣ����
 */
ACL_API void acl_cache2_refer(ACL_CACHE2_INFO *info);

/**
 * ����ĳ�����������ü�������ֹ����ǰɾ��
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param key {const char*}
 */
ACL_API void acl_cache2_refer2(ACL_CACHE2 *cache2, const char *key);

/**
 * ����ĳ�����������ü���
 * @param info {ACL_CACHE2_INFO*} �û������������Ļ�����Ϣ����
 */
ACL_API void acl_cache2_unrefer(ACL_CACHE2_INFO *info);

/**
 * ����ĳ�����������ü���
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param key {const char*}
 */
ACL_API void acl_cache2_unrefer2(ACL_CACHE2 *cache2, const char *key);

/**
 * ��������ض����ڶ��߳�ʱ��
 * @param cache2 {ACL_CACHE2*} ����ض�����
 */
ACL_API void acl_cache2_lock(ACL_CACHE2 *cache2);

/**
 * ��������ض����ڶ��߳�ʱ��
 * @param cache2 {ACL_CACHE2*} ����ض�����
 */
ACL_API void acl_cache2_unlock(ACL_CACHE2 *cache2);

/**
 * ���������е����ж���
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param walk_fn {void (*)(ACL_CACHE2_INFO*, void*)} �����ص�����
 * @param arg {void *} walk_fn()/2 �еĵڶ�������
 */
ACL_API void acl_cache2_walk(ACL_CACHE2 *cache2,
	void (*walk_fn)(ACL_CACHE2_INFO *, void *), void *arg);

/**
 * ��ջ�����еĻ���������ĳ�����������Ȼ�ڱ������ҷ�ǿ����ɾ�����򽫲��ᱻ���
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @param force {int} �����0����ʹĳ�������������ü�����0Ҳ�ᱻɾ��
 * @return {int} ������Ļ���������
 */
ACL_API int acl_cache2_clean(ACL_CACHE2 *cache2, int force);

/**
 * ��ǰ������л������ĸ���
 * @param cache2 {ACL_CACHE2*} ����ض�����
 * @return {int} ������Ķ������
 */
ACL_API int acl_cache2_size(ACL_CACHE2 *cache2);

#ifdef	__cplusplus
}
#endif

#endif
