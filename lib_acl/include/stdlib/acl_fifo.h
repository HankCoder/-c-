#ifndef ACL_FIFO_INCLUDE_H
#define ACL_FIFO_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_slice.h"
#include "acl_iterator.h"

typedef struct ACL_FIFO_INFO ACL_FIFO_INFO;
typedef struct ACL_FIFO_ITER ACL_FIFO_ITER;
typedef struct ACL_FIFO ACL_FIFO;

struct ACL_FIFO_INFO {
	void *data;     
	ACL_FIFO_INFO *prev;    
	ACL_FIFO_INFO *next;
};

struct ACL_FIFO_ITER {
	ACL_FIFO_INFO *ptr;
};

struct ACL_FIFO {
	ACL_FIFO_INFO *head;
	ACL_FIFO_INFO *tail;
	int   cnt;

	/* ��Ӽ����� */

	/* �����β����Ӷ�̬���� */
	void  (*push_back)(struct ACL_FIFO*, void*);
	/* �����ͷ����Ӷ�̬���� */
	void  (*push_front)(struct ACL_FIFO*, void*);
	/* ��������β����̬���� */
	void *(*pop_back)(struct ACL_FIFO*);
	/* ��������ͷ����̬���� */
	void *(*pop_front)(struct ACL_FIFO*);

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ACL_ITER*, struct ACL_FIFO*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ACL_ITER*, struct ACL_FIFO*);
	/* ȡ������β���� */
	void *(*iter_tail)(ACL_ITER*, struct ACL_FIFO*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ACL_ITER*, struct ACL_FIFO*);
	/* ȡ�����������ĵ�ǰ������Ա�ṹ���� */
	ACL_FIFO_INFO *(*iter_info)(ACL_ITER*, struct ACL_FIFO*);

	/* private */
	ACL_SLICE_POOL *slice;
};

/**
 * ��ʼ��һ���������У�Ӧ�ÿ�����ջ�Ϸ�����У�������øú������г�ʼ��
 * @param fifo {ACL_FIFO *}
 * @example:
 *   void test(void) {
 	ACL_FIFO fifo;

	acl_fifo_init(&fifo);
 *   }
 */
ACL_API void acl_fifo_init(ACL_FIFO *fifo);

/**
 * ���ڴ���з���һ�����ж���
 * @return {ACL_FIFO*}
 */
ACL_API ACL_FIFO *acl_fifo_new(void);

/**
 * ���ڴ���з���һ�����ж��󲢴��ڴ�ض�����Ϊ������
 * @param slice {ACL_SLICE_POOL*}
 * @return {ACL_FIFO*}
 */
ACL_API ACL_FIFO *acl_fifo_new1(ACL_SLICE_POOL *slice);

/**
 * �Ӷ�����ɾ��������ֵ��ͬ�Ķ���
 * @param fifo {ACL_FIFO*}
 * @param data {const void*}
 */
ACL_API int acl_fifo_delete(ACL_FIFO *fifo, const void *data);
ACL_API void acl_fifo_delete_info(ACL_FIFO *fifo, ACL_FIFO_INFO *info);

/**
 * �ͷ��Զѷ���Ķ��ж���
 * @param fifo {ACL_FIFO*}
 * @param free_fn {void (*)(void*)}, ����ú���ָ�벻Ϊ����
 *  �����ͷŶ����ж�̬����Ķ���
 */
ACL_API void acl_fifo_free(ACL_FIFO *fifo, void (*free_fn)(void *));
ACL_API void acl_fifo_free2(ACL_FIFO *fifo, void (*free_fn)(ACL_FIFO_INFO *));

/**
 * ����������һ����̬�Ѷ���
 * @param fifo {ACL_FIFO*}
 * @param data {void*} ��̬����
 * @return {ACL_FIFO_INFO*} ��� data �ǿ��򷵻ض����е�����Ӷ���, ���򷵻� NULL
 */
ACL_API ACL_FIFO_INFO *acl_fifo_push_back(ACL_FIFO *fifo, void *data);
#define acl_fifo_push	acl_fifo_push_back
ACL_API void acl_fifo_push_info_back(ACL_FIFO *fifo, ACL_FIFO_INFO *info);
#define acl_fifo_push_info	acl_fifo_push_info_back
ACL_API ACL_FIFO_INFO *acl_fifo_push_front(ACL_FIFO *fifo, void *data);

/**
 * �Ӷ��������Ƚ��ȳ���ʽ����һ����̬����, ͬʱ���ö���Ӷ�����ɾ��
 * @param fifo {ACL_FIFO*}
 * @return {void*}, ���Ϊ�գ����ʾ����Ϊ��
 */
ACL_API void *acl_fifo_pop_front(ACL_FIFO *fifo);
#define acl_fifo_pop	acl_fifo_pop_front
ACL_API ACL_FIFO_INFO *acl_fifo_pop_info(ACL_FIFO *fifo);

/**
 * �Ӷ������Ժ���ȳ���ʽ����һ����̬���� ͬʱ�ö���Ӷ�����ɾ��
 * @param fifo {ACL_FIFO*}
 * @return {void*}, ���Ϊ�գ����ʾ����Ϊ��
 */
ACL_API void *acl_fifo_pop_back(ACL_FIFO *fifo);

/**
 * ���ض�����ͷ���Ķ�̬����
 * @param fifo {ACL_FIFO*}
 * @return {void*}, ���Ϊ�գ����ʾ����Ϊ��
 */
ACL_API void *acl_fifo_head(ACL_FIFO *fifo);
ACL_API ACL_FIFO_INFO *acl_fifo_head_info(ACL_FIFO *fifo);

/**
 * ���ض�����β���Ķ�̬����
 * @param fifo {ACL_FIFO*}
 * @return {void*}, ���Ϊ�գ����ʾ����Ϊ��
 */
ACL_API void *acl_fifo_tail(ACL_FIFO *fifo);
ACL_API ACL_FIFO_INFO *acl_fifo_tail_info(ACL_FIFO *fifo);

/**
 * ���ض����ж�̬������ܸ���
 * @param fifo {ACL_FIFO*}
 * @return {int}, >= 0
 */
ACL_API int acl_fifo_size(ACL_FIFO *fifo);

/*--------------------  һЩ�����ݵĺ���� --------------------------------*/

/**
 * ��õ�ǰ iter �������Ķ����ַ
 * @param iter {ACL_FIFO_ITER}
 */
#define	ACL_FIFO_ITER_VALUE(iter)	((iter).ptr->data)
#define	acl_fifo_iter_value		ACL_FIFO_ITER_VALUE

/**
 * ���� ACL_FIFO
 * @param iter {ACL_FIFO_ITER}
 * @param fifo {ACL_FIFO}
 * @example:
        -- ���Ǳ�����֧�ֵı�����ʽ
	void test()
	{
		ACL_FIFO *fifo_ptr = acl_fifo_new();
		ACL_FIFO_ITER iter;
		char *data;
		int   i;

		for (i = 0; i < 10; i++) {
			data = acl_mymalloc(32);
			snprintf(data, 32, "data: %d", i);
			acl_fifo_push(fifo_ptr, data);
		}
		acl_fifo_foreach(iter, fifo_ptr) {
	                printf("%s\n", (char*) acl_fifo_iter_value(iter));
	        }

		acl_fifo_free(fifo_ptr, acl_myfree_fn);
	}

	-- ͨ������������ʽ
	void test2()
	{
		ACL_FIFO *fifo_ptr = acl_fifo_new();
		ACL_ITER iter;
		char *data;
		int   i;

		for (i = 0; i < 10; i++) {
			data = acl_mymalloc(32);
			snprintf(data, 32, "data: %d", i);
			acl_fifo_push(fifo_ptr, data);
		}
		acl_foreach(iter, fifo) {
			printf("%s\n", (char*) iter.data);
		}
		acl_fifo_free(fifo_ptr, acl_myfree_fn);
	}
 */
#define	ACL_FIFO_FOREACH(iter, fifo_ptr) \
	for ((iter).ptr = (fifo_ptr)->head; (iter).ptr; (iter).ptr = (iter).ptr->next)
#define	acl_fifo_foreach	ACL_FIFO_FOREACH

/**
 * ������� ACL_FIFO
 * @param iter {ACL_FIFO_ITER}
 * @param fifo {ACL_FIFO}
 * @example:
	void test()
	{
		ACL_FIFO fifo;
		ACL_FIFO_ITER iter;
		char *data;
		int   i;

		acl_fifo_init(&fifo);

		for (i = 0; i < 10; i++) {
			data = acl_mymalloc(32);
			snprintf(data, 32, "data: %d", i);
			acl_fifo_push(&fifo, data);
		}
		acl_fifo_foreach_reverse(iter, &fifo) {
	                printf("%s\n", (char*) iter.ptr->data);
	        }

		while (1) {
			data = acl_fifo_pop(&fifo);
			if (data == NULL)
				break;
		}
	}
 */
#define	ACL_FIFO_FOREACH_REVERSE(iter, fifo_ptr) \
	for ((iter).ptr = (fifo_ptr)->tail; (iter).ptr; (iter).ptr = (iter).ptr->prev)
#define	acl_fifo_foreach_reverse	ACL_FIFO_FOREACH_REVERSE

#ifdef __cplusplus
}
#endif

#endif

