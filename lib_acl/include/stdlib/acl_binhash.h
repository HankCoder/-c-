#ifndef ACL_BINHASH_INCLUDE_H
#define ACL_BINHASH_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_hash.h"			/* just for ACL_HASH_FN */
#include "acl_slice.h"
#include "acl_iterator.h"

typedef struct ACL_BINHASH ACL_BINHASH;
typedef struct ACL_BINHASH_INFO ACL_BINHASH_INFO;

/**     
 * Structure of one hash table.
 */
struct ACL_BINHASH {
	int     size;                   /**< length of entries array */
	int     used;                   /**< number of entries in table */
	unsigned int flag;              /**< the hash table's properties flag */
	int     status;                 /**< the hash tables' operation status */
	ACL_BINHASH_INFO **data;        /**< entries array, auto-resized */
	ACL_SLICE *slice;               /**< memory slice */
	ACL_HASH_FN hash_fn;            /**< hash function */

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ACL_ITER*, struct ACL_BINHASH*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ACL_ITER*, struct ACL_BINHASH*);
	/* ȡ������β���� */
	void *(*iter_tail)(ACL_ITER*, struct ACL_BINHASH*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ACL_ITER*, struct ACL_BINHASH*);
	/* ȡ�����������ĵ�ǰ������Ա�ṹ���� */
	ACL_BINHASH_INFO *(*iter_info)(ACL_ITER*, struct ACL_BINHASH*);
};

/**
 * Structure of one hash table entry.
 */
struct ACL_BINHASH_INFO {
	union {
		void *key;
		const void *c_key;
	} key;				/**
					 * ��ϣ��, ֻ�����������������Ϊ��������ϣ��ı�־λΪ
					 * ACL_BINHASH_FLAG_KEY_REUSE ʱ��Ҫ��������ļ��ռ�
					 */
	int     key_len;                /**< ��ϣ������ */
	void   *value;                  /**< ��ϣ������Ӧ���û����� */
	struct ACL_BINHASH_INFO *next;  /**< colliding entry */
	struct ACL_BINHASH_INFO *prev;  /**< colliding entry */
};

/**
 * ACL_BINHASH ����������
 */
typedef struct ACL_BINHASH_ITER {
	/* public */
	ACL_BINHASH_INFO *ptr;

	/* private */
	int  i;
	int  size;
	ACL_BINHASH_INFO **h;
} ACL_BINHASH_ITER;

/**
 * ����һ����ϣ��
 * @param size {int} ��ϣ��ĳ�ʼ����С
 * @param flag {unsigned int} ��ϣ�����Ա�־λ, ACL_BINHASH_FLAG_xxx
 * @return {ACL_BINHASH*} �´����Ĺ�ϣ��ָ��
 */
ACL_API ACL_BINHASH *acl_binhash_create(int size, unsigned int flag);
#define	ACL_BINHASH_FLAG_KEY_REUSE	(1 << 0)
#define	ACL_BINHASH_FLAG_SLICE_RTGC_OFF	(1 << 1)
#define	ACL_BINHASH_FLAG_SLICE1		(1 << 2)
#define	ACL_BINHASH_FLAG_SLICE2		(1 << 3)
#define	ACL_BINHASH_FLAG_SLICE3		(1 << 4)

/**
 * ���ϣ������Ӷ���
 * @param table {ACL_BINHASH*} ��ϣ��ָ��
 * @param key {const void*} ��ϣ��
 * @param key_len {int} key �ĳ���
 * @param value {void*} ��ֵ
 * @return {ACL_BINHASH_INFO*} �´����Ĺ�ϣ��Ŀָ��
 */
ACL_API ACL_BINHASH_INFO *acl_binhash_enter(ACL_BINHASH *table, const void *key, int key_len, void *value);

/**
 * �ӹ�ϣ���и��ݼ���ȡ�ö�Ӧ�Ĺ�ϣ��Ŀ
 * @param table {ACL_BINHASH*} ��ϣ��ָ��
 * @param key {const void*} ��ϣ��
 * @param key_len {int} key �ĳ���
 * @return {ACL_BINHASH_INFO*} ��ϣ��Ŀָ��
 */
ACL_API ACL_BINHASH_INFO *acl_binhash_locate(ACL_BINHASH *table, const void *key, int key_len);

/**
 * ��ѯĳ����ϣ���ļ�ֵ
 * @param table {ACL_BINHASH*} ��ϣ��ָ��
 * @param key {const void*} ��ϣ��
 * @param key_len {int} key �ĳ���
 * @return {void*} ��ϣ��ֵ
 */
ACL_API void *acl_binhash_find(ACL_BINHASH *table, const void *key, int key_len);

/**
 * ɾ��ĳ����ϣ��
 * @param table {ACL_BINHASH*} ��ϣ��ָ��
 * @param key {const void*} ��ϣ��
 * @param key_len {int} key �ĳ���
 * @param free_fn {void (*)(void*)} �����ͷŹ�ϣ��ֵ�ĺ���ָ�룬���Ϊ�������ڲ��ͷż�ֵ
 * @return {int} 0: ok, -1: error
 */
ACL_API int acl_binhash_delete(ACL_BINHASH *table, const void *key, int key_len, void (*free_fn) (void *));

/**
 * �ͷŹ�ϣ��
 * @param table {ACL_BINHASH*} ��ϣ��ָ��
 * @param free_fn {void (*)(void*)} �����Ϊ�գ����ô˺������ͷŹ�ϣ���ڵ����м�ֵ
 */
ACL_API void acl_binhash_free(ACL_BINHASH *table, void (*free_fn) (void *));

/**
 * ����������ϣ�������û������Ļص�����������ϣ���еļ�ֵ
 * @param table {ACL_BINHASH*} ��ϣ��ָ��
 * @param walk_fn {void (*)(ACL_BINHASH_INFO*, void*)} �ڱ�����ϣ���е�ÿ��Ԫ��ʱ�Ļص�����
 * @param arg {void*} �û����ݵĲ�������Ϊ������ walk_fn �д���
 */
ACL_API void acl_binhash_walk(ACL_BINHASH *table, void (*walk_fn) (ACL_BINHASH_INFO *, void *), void *arg);

/**
 * �г���ǰ��ϣ���е�����Ԫ�������б�
 * @param table {ACL_BINHASH*} ��ϣ��ָ��
 * @return {ACL_BINHASH_INFO*} ��ϣ��������Ԫ����ɵ�ACL_BINHASH_INFO����, 
 *  �������е����һ��ָ��Ϊ NULL
 */
ACL_API ACL_BINHASH_INFO **acl_binhash_list(ACL_BINHASH *table);

/**
 * ��ù�ϣ�����ʱ�ĳ����
 * @param table {ACL_BINHASH*} ��ϣ��ָ��
 * @return {int} �����
 */
ACL_API int acl_binhash_errno(ACL_BINHASH *table);
#define ACL_BINHASH_STAT_OK		0
#define ACL_BINHASH_STAT_INVAL		1
#define ACL_BINHASH_STAT_DUPLEX_KEY	2
#define	ACL_BINHASH_STAT_NO_KEY		3

/**
 * ���ع�ϣ��ǰ�������ռ��С
 * @param table ��ϣ��ָ��
 * @return ��ϣ��������ռ��С
 */
ACL_API int acl_binhash_size(const ACL_BINHASH *table);

/**
 * ��ǰ��ϣ���ж���ĸ���
 * @param table {ACL_BINHASH*} ��ϣ��ָ��
 * @return {int}
 */
ACL_API int acl_binhash_used(ACL_BINHASH *table);

ACL_API ACL_BINHASH_INFO **acl_binhash_data(ACL_BINHASH *table);
ACL_API const ACL_BINHASH_INFO *acl_binhash_iter_head(ACL_BINHASH *table, ACL_BINHASH_ITER *iter);
ACL_API const ACL_BINHASH_INFO *acl_binhash_iter_next(ACL_BINHASH_ITER *iter);
ACL_API const ACL_BINHASH_INFO *acl_binhash_iter_tail(ACL_BINHASH *table, ACL_BINHASH_ITER *iter);
ACL_API const ACL_BINHASH_INFO *acl_binhash_iter_prev(ACL_BINHASH_ITER *iter);

/*--------------------  һЩ�����ݵĺ���� --------------------------------*/

#define	ACL_BINHASH_ITER_KEY(iter)	((iter).ptr->key.c_key)
#define	acl_binhash_iter_key		ACL_BINHASH_ITER_KEY

#define	ACL_BINHASH_ITER_VALUE(iter)	((iter).ptr->value)
#define	acl_binhash_iter_value		ACL_BINHASH_ITER_VALUE

/**
 * ���� ACL_BINHASH
 * @param iter {ACL_BINHASH_ITER}
 * @param table_ptr {ACL_BINHASH *}
 * @example:
	void test()
	{
		ACL_BINHASH *table = acl_binhash_create(10, 0);
		ACL_BINHASH_ITER iter;
		char *value, key[32];
		int   i;

		for (i = 0; i < 100; i++) {
			value = (char*) acl_mystrdup("value");
			snprintf(key, sizeof(key), "key:%d", i);
			(void) acl_binhash_enter(table, key, strlen(key), value);
		}

		acl_binhash_foreach(iter, table) {
			printf("%s=%s\n", iter.ptr->key.c_key, iter.ptr->value);
			if (i == 50)
				break;
		}

		acl_binhash_free(table, acl_myfree_fn);
	}
 */
#if 0
#define	ACL_BINHASH_FOREACH(iter, table_ptr)  \
    if (table_ptr)  \
        for((iter).size = acl_binhash_size((table_ptr)), (iter).i = 0,  \
          (iter).h = acl_binhash_data((table_ptr)); (iter).i < (iter).size; (iter).i++)  \
            for ((iter).ptr = *(iter).h++; (iter).ptr; (iter).ptr = (iter).ptr->next)
#define	ACL_BINHASH_FOREACH_REVERSE(iter, table_ptr)  \
    if (table_ptr)  \
        for((iter).size = acl_binhash_size((table_ptr)), (iter).i = (iter).size - 1,  \
          (iter).h = acl_binhash_data((table_ptr)) + (iter).i; (iter).i >= 0; (iter).i--)  \
            for ((iter).ptr = *(iter).h--; (iter).ptr; (iter).ptr = (iter).ptr->next)
#else
#define	ACL_BINHASH_FOREACH(iter, table_ptr)  \
    if (table_ptr)  \
        for((void) acl_binhash_iter_head((table_ptr), &iter);  \
            (iter).ptr;  \
            (void) acl_binhash_iter_next(&iter))
#define	ACL_BINHASH_FOREACH_REVERSE(iter, table_ptr)  \
    if (table_ptr)  \
        for((void) acl_binhash_iter_tail((table_ptr), &iter);  \
            (iter).ptr;  \
            (void) acl_binhash_iter_prev(&iter))
#endif

#define	acl_binhash_foreach		ACL_BINHASH_FOREACH
#define	acl_binhash_foreach_reverse	ACL_BINHASH_FOREACH_REVERSE

#ifdef  __cplusplus
}
#endif

#endif

