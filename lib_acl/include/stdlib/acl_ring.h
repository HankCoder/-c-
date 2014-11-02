#ifndef	ACL_RING_INCLUDE_H
#define	ACL_RING_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include <stddef.h>

typedef struct ACL_RING ACL_RING;

/**
 * ���ݻ��ṹ���Ͷ���
 */
struct ACL_RING {
	ACL_RING   *succ;           /**< successor */
	ACL_RING   *pred;           /**< predecessor */

	ACL_RING   *parent;         /**< the header of all the rings */
	int   len;                  /**< the count in the ring */
};

typedef struct ACL_RING_ITER {
	ACL_RING *ptr;
} ACL_RING_ITER;

/**
 * ��ʼ�����ݻ�
 * @param ring {ACL_RING*} ���ݻ�
 */
ACL_API void acl_ring_init(ACL_RING *ring);

/**
 * ��õ�ǰ���ݻ���Ԫ�ظ���
 * @param ring {ACL_RING*} ���ݻ�
 * @return {int} ���ݻ���Ԫ�ظ���
 */
ACL_API int  acl_ring_size(const ACL_RING *ring);

/**
 * ��һ����Ԫ����ӽ�����ͷ��
 * @param ring {ACL_RING*} ���ݻ�
 * @param entry {ACL_RING*} �µ�Ԫ��
 */
ACL_API void acl_ring_prepend(ACL_RING *ring, ACL_RING *entry);

/**
 * ��һ����Ԫ����ӽ�����β��
 * @param ring {ACL_RING*} ���ݻ�
 * @param entry {ACL_RING*} �µ�Ԫ��
 */
ACL_API void acl_ring_append(ACL_RING *ring, ACL_RING *entry);

/**
 * ��һ����Ԫ�ش����ݻ���ɾ��
 * @param entry {ACL_RING*} ��Ԫ��
 */
ACL_API void acl_ring_detach(ACL_RING *entry);

/**
 * �ӻ��е���ͷ����Ԫ��
 * @param ring {ACL_RING*} ���ݻ�
 * @return {ACL_RING*} ͷ����Ԫ�أ�������ؿ����ʾ�����ݻ�Ϊ��
 */
ACL_API ACL_RING *acl_ring_pop_head(ACL_RING *ring);

/**
 * �ӻ��е���β����Ԫ��
 * @param ring {ACL_RING*} ���ݻ�
 * @return {ACL_RING*} β����Ԫ�أ�������ؿ����ʾ�����ݻ�Ϊ��
 */
ACL_API ACL_RING *acl_ring_pop_tail(ACL_RING *ring);

/*--------------------  һЩ�����ݵĺ���� --------------------------------*/

/**
 * ���ص�ǰ��Ԫ�ص���һ����Ԫ��
 */
#define ACL_RING_SUCC(c) ((c)->succ)
#define	acl_ring_succ	ACL_RING_SUCC

/**
 * ���ص�ǰ��Ԫ�ص�ǰһ����Ԫ��
 */
#define ACL_RING_PRED(c) ((c)->pred)
#define	acl_ring_pred	ACL_RING_PRED

/**
 * ����Ԫ��ָ��ת����Ӧ�õ��Զ������͵�ָ���ַ
 * @param ring_ptr {ACL_RING*} ��Ԫ��ָ��
 * @param app_type Ӧ���Զ�������
 * @param ring_member {ACL_RING*} ��Ԫ����Ӧ���Զ���ṹ�еĳ�Ա����
 * @return {app_type*} Ӧ���Զ���ṹ���͵Ķ����ַ
 */
#define ACL_RING_TO_APPL(ring_ptr, app_type, ring_member) \
    ((app_type *) (((char *) (ring_ptr)) - offsetof(app_type,ring_member)))

#define	acl_ring_to_appl	ACL_RING_TO_APPL

/**
 * ��ͷ����β���������ݻ��е����л�Ԫ��
 * @param iter {ACL_RING_ITER}
 * @param head_ptr {ACL_RING*} ���ݻ���ͷָ��
 * @example:
 	typedef struct {
		char  name[32];
		ACL_RING entry;
	} DUMMY;

	void test()
	{
		ACL_RING head;
		DUMMY *dummy;
		ACL_RING_ITER iter;
		int   i;

		acl_ring_init(&head);

		for (i = 0; i < 10; i++) {
			dummy = (DUMMY*) acl_mycalloc(1, sizeof(DUMMY));
			snprintf(dummy->name, sizeof(dummy->name), "dummy:%d", i);
			acl_ring_append(&head, &dummy->entry);
		}

		acl_ring_foreach(iter, &head) {
			dummy = acl_ring_to_appl(iter.ptr, DUMMY, entry);
			printf("name: %s\n", dummy->name);
		}

		while (1) {
			iter.ptr = acl_ring_pop_head(&head);
			if (iter.ptr == NULL)
				break;
			dummy = acl_ring_to_appl(iter.ptr, DUMMY, entry);
			acl_myfree(dummy);
		}
	}
 */
#define	ACL_RING_FOREACH(iter, head_ptr) \
        for ((iter).ptr = acl_ring_succ((head_ptr)); (iter).ptr != (head_ptr);  \
             (iter).ptr = acl_ring_succ((iter).ptr))

#define	acl_ring_foreach		ACL_RING_FOREACH

/**
 * ��β����ͷ���������ݻ��е����л�Ԫ��
 * @param iter {ACL_RING_ITER}
 * @param head_ptr {ACL_RING*} ���ݻ���ͷָ��
 */
#define	ACL_RING_FOREACH_REVERSE(iter, head_ptr) \
        for ((iter).ptr = acl_ring_pred((head_ptr)); (iter).ptr != (head_ptr);  \
             (iter).ptr = acl_ring_pred((iter).ptr))

#define	acl_ring_foreach_reverse	ACL_RING_FOREACH_REVERSE

/**
 * �������ݻ��е�һ����Ԫ��ָ��
 * @param head {ACL_RING*} ��ͷָ��
 * @return {ACL_RING*} NULL: ��Ϊ��
 */
#define ACL_RING_FIRST(head) \
	(acl_ring_succ(head) != (head) ? acl_ring_succ(head) : 0)

#define	acl_ring_first		ACL_RING_FIRST

/**
 * �������ݻ���ͷ��һ����Ԫ��ָ��ͬʱ����ת��Ӧ���Զ���ṹ���͵Ķ����ַ
 * @param head {ACL_RING*} ��ͷָ��
 * @param app_type Ӧ���Զ���ṹ����
 * @param ring_member {ACL_RING*} ��Ԫ����Ӧ���Զ���ṹ�еĳ�Ա����
 * @return {app_type*} Ӧ���Զ���ṹ���͵Ķ����ַ
 */
#define ACL_RING_FIRST_APPL(head, app_type, ring_member) \
	(acl_ring_succ(head) != (head) ? \
	 ACL_RING_TO_APPL(acl_ring_succ(head), app_type, ring_member) : 0)

#define	acl_ring_first_appl	ACL_RING_FIRST_APPL

/**
 * �������ݻ������һ����Ԫ��ָ��
 * @param head {ACL_RING*} ��ͷָ��
 * @return {ACL_RING*} NULL: ��Ϊ��
 */
#define ACL_RING_LAST(head) \
       (acl_ring_pred(head) != (head) ? acl_ring_pred(head) : 0)

#define	acl_ring_last		ACL_RING_LAST

/**
 * �������ݻ������һ����Ԫ��ָ��ͬʱ����ת��Ӧ���Զ���ṹ���͵Ķ����ַ
 * @param head {ACL_RING*} ��ͷָ��
 * @param app_type Ӧ���Զ���ṹ����
 * @param ring_member {ACL_RING*} ��Ԫ����Ӧ���Զ���ṹ�еĳ�Ա����
 * @return {app_type*} Ӧ���Զ���ṹ���͵Ķ����ַ
 */
#define ACL_RING_LAST_APPL(head, app_type, ring_member) \
       (acl_ring_pred(head) != (head) ? \
	ACL_RING_TO_APPL(acl_ring_pred(head), app_type, ring_member) : 0)

#define	acl_ring_last_appl	ACL_RING_LAST_APPL

/**
 * ��һ����Ԫ����ӽ�����β��
 * @param ring {ACL_RING*} ���ݻ�
 * @param entry {ACL_RING*} �µ�Ԫ��
 */
#define	ACL_RING_APPEND(ring_in, entry_in) do {  \
	ACL_RING *ring_ptr = (ring_in), *entry_ptr = (entry_in);  \
        entry_ptr->succ      = ring_ptr->succ;  \
        entry_ptr->pred      = ring_ptr;  \
        entry_ptr->parent    = ring_ptr->parent;  \
        ring_ptr->succ->pred = entry_ptr;  \
        ring_ptr->succ       = entry_ptr;  \
        ring_ptr->parent->len++;  \
} while (0)

/**
 * ��һ����Ԫ����ӽ�����ͷ��
 * @param ring {ACL_RING*} ���ݻ�
 * @param entry {ACL_RING*} �µ�Ԫ��
 */
#define	ACL_RING_PREPEND(ring_in, entry_in) do {  \
	ACL_RING *ring_ptr = (ring_in), *entry_ptr = (entry_in);  \
	entry_ptr->pred      = ring_ptr->pred;  \
	entry_ptr->succ      = ring_ptr;  \
	entry_ptr->parent    = ring_ptr->parent;  \
	ring_ptr->pred->succ = entry_ptr;  \
	ring_ptr->pred       = entry_ptr;  \
	ring_ptr->parent->len++;  \
} while (0)

/**
 * ��һ����Ԫ�ش����ݻ���ɾ��
 * @param entry {ACL_RING*} ��Ԫ��
 */
#define	ACL_RING_DETACH(entry_in) do {  \
	ACL_RING   *succ, *pred, *entry_ptr = (entry_in);  \
	succ = entry_ptr->succ;  \
	pred = entry_ptr->pred;  \
	if (succ != NULL && pred != NULL) {  \
		pred->succ = succ;  \
		succ->pred = pred;  \
		entry_ptr->parent->len--;  \
		entry_ptr->succ = entry_ptr->pred = NULL;  \
	}  \
} while (0)

#ifdef  __cplusplus
}
#endif

#endif

