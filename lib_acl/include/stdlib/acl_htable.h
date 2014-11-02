#ifndef ACL_HTABLE_INCLUDE_H
#define ACL_HTABLE_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "thread/acl_thread.h"
#include "acl_hash.h"			/* just for ACL_HASH_FN */
#include "acl_slice.h"
#include "acl_iterator.h"

/*--------------------------------------------------------------------------*/
typedef struct ACL_HTABLE	ACL_HTABLE;
typedef struct ACL_HTABLE_INFO 	ACL_HTABLE_INFO;

/**
 * ��ϣ�����ṹ���
 */
struct ACL_HTABLE {
	int     size;                   /* length of entries array */
	int     init_size;              /* length of initial entryies array */
	int     used;                   /* number of entries in table */
	ACL_HTABLE_INFO **data;         /* entries array, auto-resized */
	unsigned int flag;              /* properties flag */
	int     status;                 /* the operator's status on the htable */

	ACL_HASH_FN  hash_fn;           /* hash function */
	ACL_SLICE_POOL *slice;
	acl_pthread_mutex_t *rwlock;

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ACL_ITER*, struct ACL_HTABLE*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ACL_ITER*, struct ACL_HTABLE*);
	/* ȡ������β���� */
	void *(*iter_tail)(ACL_ITER*, struct ACL_HTABLE*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ACL_ITER*, struct ACL_HTABLE*);
	/* ȡ�����������ĵ�ǰ������Ա�ṹ���� */
	ACL_HTABLE_INFO *(*iter_info)(ACL_ITER*, struct ACL_HTABLE*);
};

/**
 * ��ϣ����ÿһ����ϣ��Ĵ洢��Ϣ����
 */
struct ACL_HTABLE_INFO {
	union {
		char *key;
		const char *c_key;
	} key;				/**
					 * ��ϣ��, ֻ�����������������Ϊ��������ϣ��ı�־λΪ
					 * ACL_BINHASH_FLAG_KEY_REUSE ʱ��Ҫ��������ļ��ռ�
					 */
	void   *value;			/**< associated value */
	struct ACL_HTABLE_INFO *next;	/**< colliding entry */
	struct ACL_HTABLE_INFO *prev;	/**< colliding entry */
};

/**
 * ACL_HTABLE ����������
 */
typedef struct ACL_HTABLE_ITER {
	/* public */
	ACL_HTABLE_INFO *ptr;

	/* private */
	int  i;
	int  size;
	ACL_HTABLE_INFO **h;
} ACL_HTABLE_ITER;

/**
 * ������ϣ��
 * @param size ��ϣ����
 * @param flag {unsigned int} ��ϣ�����Ա�־λ, ACL_BINHASH_FLAG_xxx
 * @return ������ϣ���ͷָ���Ϊ��(��ʱ��ʾ�������صĴ���, ��Ҫ���ڴ��������)
 */
ACL_API ACL_HTABLE *acl_htable_create(int size, unsigned int flag);
#define	ACL_HTABLE_FLAG_KEY_REUSE	(1 << 0)  /* ����µĶ���ʱ�Ƿ�ֱ�Ӹ��ü���ַ */
#define	ACL_HTABLE_FLAG_USE_LOCK	(1 << 1)  /* �Ƿ�������ö��̻߳��ⷽʽ */
#define	ACL_HTABLE_FLAG_MSLOOK		(1 << 2)  /* ÿ�β�ѯʱ�Ƿ񽫲�ѯ������������ͷ */
#define	ACL_HTABLE_FLAG_KEY_LOWER	(1 << 3)  /* ͳһ����ת��ΪСд���Ӷ�ʵ�ּ���ѯ�����ִ�Сд�Ĺ��� */

ACL_API ACL_HTABLE *acl_htable_create3(int size, unsigned int flag, ACL_SLICE_POOL *slice);

/**
 * ���ù�ϣ��Ŀ��Ʋ���
 * @param table ��ϣ�������
 * @param name ���Ʋ����ı�γ�ʼֵ, name ���Ժ�Ŀ��Ʋ������¶���
 *  ACL_HTABLE_CTL_END: ��α������־
 *  ACL_HTABLE_CTL_RWLOCK: �Ƿ����ö�д������
 *  ACL_HTABLE_CTL_HASH_FN: �û��Զ���Ĺ�ϣֵ���㺯��
 */
ACL_API void acl_htable_ctl(ACL_HTABLE *table, int name, ...);
#define	ACL_HTABLE_CTL_END      0  /**< ���ƽ�����־ */
#define	ACL_HTABLE_CTL_RWLOCK   1  /**< �Ƿ���� */
#define	ACL_HTABLE_CTL_HASH_FN  2  /**< ����˽�й�ϣ���� */

/**
 * �����һ�ι�ϣ��������ϣ���״̬
 * @param table ��ϣ��ָ��
 * @return {int} ������ϣ����״̬, �μ����µ� ACL_HTABLE_STAT_XXX
 */
ACL_API int acl_htable_errno(ACL_HTABLE *table);
#define	ACL_HTABLE_STAT_OK          0  /**< ״̬���� */
#define	ACL_HTABLE_STAT_INVAL       1  /**< ��Ч���� */
#define	ACL_HTABLE_STAT_DUPLEX_KEY  2  /**< �ظ��� */

/**
 * ���ù�ϣ��ĵ�ǰ״̬, error ȡֵ ACL_HTABLE_STAT_XXX
 * @param table ��ϣ��ָ��
 * @param error ���ù�ϣ��Ĵ���״̬
 */
ACL_API void acl_htable_set_errno(ACL_HTABLE *table, int error);

/**
 * ����ϣ��������µ���
 * @param table ��ϣ��ָ��
 * @param key ��, �ں����ڲ��Ḵ�ƴ� key ��
 * @param value �û��Լ����ض�������(����������Ӳת������, ���Ǵ���������벻�ܶ�ջ����)
 * @return ������Ĺ�ϣ�����ָ��, == NULL: ��ʾ�ڲ��ַ����ڴ����, Ϊ���صĴ���
 *  ע����������ʱ�ù�ϣ�������ڣ��򷵻��Ѿ����ڵĹ�ϣ�ʹ����Ӧ��ͨ������
 *  acl_htable_last_errno() ���鿴�Ƿ��ظ����ͬһ����ֵ(ACL_HTABLE_STAT_DUPLEX_KEY)
 */
ACL_API ACL_HTABLE_INFO *acl_htable_enter(ACL_HTABLE *table, const char *key, void *value);

/**
 * ����ϣ��������µ��������߳�ͬʱ���д˲���ʱ�������ڲ����Զ���֤�������
 * @param table ��ϣ��ָ��
 * @param key ��, �ں����ڲ��Ḵ�ƴ� key ��
 * @param value �û��Լ����ض�������(����������Ӳת������, ���Ǵ���������벻�ܶ�ջ����)
 * @param callback ����ú���ָ�벻Ϊ�գ�����ӳɹ������øú���
 * @param arg callback �Ĳ���֮һ
 * @return {int} 0 ��ʾ ��ӳɹ���-1 ��ʾ���ʧ��
 *  ע����������ʱ�ù�ϣ�������ڣ��򷵻��Ѿ����ڵĹ�ϣ�ʹ����Ӧ��ͨ������
 *  acl_htable_last_errno() ���鿴�Ƿ��ظ����ͬһ����ֵ(ACL_HTABLE_STAT_DUPLEX_KEY)
 */
ACL_API int acl_htable_enter_r(ACL_HTABLE *table, const char *key, void *value,
		void (*callback)(ACL_HTABLE_INFO *ht, void *arg), void *arg);

/**
 * �������� key ����Ѱĳһ�ض���ϣ��
 * @param table ��ϣ��ָ��
 * @param key ��
 * @return ��Ϊ��ָ��: ��ʾ�鵽�˶�Ӧ�� key ���Ĺ�ϣ��
 *         Ϊ��: ��ʾδ�鵽��Ӧ�� key ���Ĺ�ϣ��
 */
ACL_API ACL_HTABLE_INFO *acl_htable_locate(ACL_HTABLE *table, const char *key);

/**
 * �������� key ����Ѱĳһ�ض���ϣ�������߳�ͬʱ���д˲���ʱ��
 * �����ڲ����Զ���֤�������
 * @param table ��ϣ��ָ��
 * @param key ��
 * @param callback �鵽��Ҫ��ļ�ֵ�������ָ��ǿ������֮
 * @param arg callback ����֮һ
 * @return ��Ϊ��ָ��: ��ʾ�鵽�˶�Ӧ�� key ���Ĺ�ϣ��
 *         Ϊ��: ��ʾδ�鵽��Ӧ�� key ���Ĺ�ϣ��
 */
ACL_API int acl_htable_locate_r(ACL_HTABLE *table, const char *key,
		void (*callback)(ACL_HTABLE_INFO *ht, void *arg), void *arg);

/**
 * �������� key ����Ѱ�û���������
 * @param table ��ϣ��ָ��
 * @param key ��
 * @return ��Ϊ��: ��ʾ�鵽�˶�Ӧ�� key ����������, �û����Ը����û��Լ���
 *  �������ͽ���ת��; Ϊ��: ��ʾδ�鵽��Ӧ�� key ����������
 */
ACL_API void *acl_htable_find(ACL_HTABLE *table, const char *key);

/**
 * �������� key ����Ѱ�û���������, ������߳�ͬʱ���д˲���ʱ��
 * �����ڲ����Զ���֤�������
 * @param table ��ϣ��ָ��
 * @param key ��
 * @param callback ���鵽��Ҫ��ļ�ֵ������ú���ָ�벻Ϊ�������֮
 * @param arg callback �Ĳ���֮һ
 * @return ��Ϊ��: ��ʾ�鵽�˶�Ӧ�� key ����������, �û����Ը����û��Լ���
 *  �������ͽ���ת��; Ϊ��: ��ʾδ�鵽��Ӧ�� key ����������
 */
ACL_API int  acl_htable_find_r(ACL_HTABLE *table, const char *key,
		void (*callback)(void *value, void *arg), void *arg);

/**
 * ���������� key ��ɾ��ĳһ��ϣ��
 * @param table ��ϣ��ָ��
 * @param key ��
 * @param free_fn ����ú���ָ�벻Ϊ�ղ����ҵ��˶�Ӧ�� key ����������, ���ȵ����û�
 *        ���ṩ������������һЩ��β����, Ȼ�����ͷŸù�ϣ��
 * @return 0: �ɹ�;  -1: δ�ҵ��� key ��
 */
ACL_API int acl_htable_delete(ACL_HTABLE *table, const char *key, void (*free_fn) (void *));
#define	acl_htable_delete_r	acl_htable_delete

/**
 * �ͷ�������ϣ��
 * @param table ��ϣ��ָ��
 * @param free_fn �����ָ�벻Ϊ����Թ�ϣ���е�ÿһ���ϣ�����øú�������β����, Ȼ�����ͷ�
 */
ACL_API void acl_htable_free(ACL_HTABLE *table, void (*free_fn) (void *));

/**
 * ���ù�ϣ��, �ú������ͷŹ�ϣ���е�����������, �����³�ʼ��
 * @param table ��ϣ��ָ��
 * @param free_fn �����ָ�벻Ϊ����Թ�ϣ���е�ÿһ���ϣ�����øú�������β����, Ȼ�����ͷ�
 * @return �Ƿ����óɹ�. 0: OK; < 0: error.
 */
ACL_API int acl_htable_reset(ACL_HTABLE *table, void (*free_fn) (void *));
#define	acl_htable_reset_r	acl_htable_reset

/**
 * �Թ�ϣ���е�ÿһ���ϣ����д���
 * @param table ��ϣ��ָ��
 * @param walk_fn ����ÿһ���ϣ��ĺ���ָ��, ����Ϊ��
 * @param arg �û��Լ����͵�����
 */
ACL_API void acl_htable_walk(ACL_HTABLE *table, void (*walk_fn) (ACL_HTABLE_INFO *, void *), void *arg);
#define	acl_htable_walk_r	acl_htable_walk

/**
 * ���ع�ϣ��ǰ�������ռ��С
 * @param table ��ϣ��ָ��
 * @return ��ϣ��������ռ��С
 */
ACL_API int acl_htable_size(const ACL_HTABLE *table);

/**
 * ���ع�ϣ��ǰ�Ĵ���������Ԫ�ظ���
 * @param table ��ϣ��ָ��
 * @return ��ϣ����Ԫ�ظ���
 */
ACL_API int acl_htable_used(const ACL_HTABLE *table);

/**
 * ����ϣ�������������ϳ�һ������
 * @param table ��ϣ��
 * @return ��Ϊ��: ����ָ��; Ϊ��: ��ʾ�ù�ϣ����û�й�ϣ��
 */
ACL_API ACL_HTABLE_INFO **acl_htable_list(const ACL_HTABLE *table);

/**
 * ��ʾ��ϣ���� key ���ķֲ�״̬
 * @param table ��ϣ��ָ��
 */
ACL_API void acl_htable_stat(const ACL_HTABLE *table);
#define	acl_htable_stat_r	acl_htable_stat

ACL_API ACL_HTABLE_INFO **acl_htable_data(ACL_HTABLE *table);
ACL_API const ACL_HTABLE_INFO *acl_htable_iter_head(ACL_HTABLE *table, ACL_HTABLE_ITER *iter);
ACL_API const ACL_HTABLE_INFO *acl_htable_iter_next(ACL_HTABLE_ITER *iter);
ACL_API const ACL_HTABLE_INFO *acl_htable_iter_tail(ACL_HTABLE *table, ACL_HTABLE_ITER *iter);
ACL_API const ACL_HTABLE_INFO *acl_htable_iter_prev(ACL_HTABLE_ITER *iter);

/*--------------------  һЩ�����ݵĺ���� --------------------------------*/

#define	ACL_HTABLE_ITER_KEY(iter)	((iter).ptr->key.c_key)
#define	acl_htable_iter_key		ACL_HTABLE_ITER_KEY

#define	ACL_HTABLE_ITER_VALUE(iter)	((iter).ptr->value)
#define	acl_htable_iter_value		ACL_HTABLE_ITER_VALUE

/**
 * ���� ACL_HTABLE
 * @param iter {ACL_HTABLE_ITER}
 * @param table_ptr {ACL_HTABLE *}
 * @example:
	void test()
	{
		ACL_HTABLE *table = acl_htable_create(10, 0);
		ACL_HTABLE_ITER iter;
		char *value, key[32];
		int   i;

		for (i = 0; i < 100; i++) {
			value = (char*) acl_mystrdup("value");
			snprintf(key, sizeof(key), "key:%d", i);
			(void) acl_htable_enter(table, key, value);
		}

		acl_htable_foreach(iter, table) {
			printf("%s=%s\n", acl_htable_iter_key(iter), acl_htable_iter_value(iter));
			if (i == 50)
				break;
		}
		acl_htable_free(table, acl_myfree_fn);
	}
 */

#if 0
#define	ACL_HTABLE_FOREACH(iter, table_ptr)  \
    if (table_ptr)  \
        for((iter).size = acl_htable_size((table_ptr)), (iter).i = 0,  \
          (iter).h = acl_htable_data((table_ptr)); (iter).i < (iter).size; (iter).i++)  \
            for ((iter).ptr = *(iter).h++; (iter).ptr; (iter).ptr = (iter).ptr->next)
#define	ACL_HTABLE_FOREACH_REVERSE(iter, table_ptr)  \
    if (table_ptr)  \
        for((iter).size = acl_htable_size((table_ptr)), (iter).i = (iter).size - 1,  \
          (iter).h = acl_htable_data((table_ptr)) + (iter).i; (iter).i >= 0; (iter).i--)  \
            for ((iter).ptr = *(iter).h--; (iter).ptr; (iter).ptr = (iter).ptr->next)
#else
#define	ACL_HTABLE_FOREACH(iter, table_ptr)  \
    if (table_ptr)  \
        for((void) acl_htable_iter_head((table_ptr), &iter);  \
            (iter).ptr;  \
            (void) acl_htable_iter_next(&iter))
#define	ACL_HTABLE_FOREACH_REVERSE(iter, table_ptr)  \
    if (table_ptr)  \
        for((void) acl_htable_iter_tail((table_ptr), &iter);  \
            (iter).ptr;  \
            (void) acl_htable_iter_prev(&iter))
#endif

#define	acl_htable_foreach		ACL_HTABLE_FOREACH
#define	acl_htable_foreach_reverse	ACL_HTABLE_FOREACH_REVERSE

#ifdef  __cplusplus
}
#endif

#endif

