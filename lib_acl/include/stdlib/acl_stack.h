#ifndef	ACL_STACK_INCLUDE_H
#define	ACL_STACK_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_iterator.h"

/* ˵�����ú������ڲ����õ��ڴ����δ�����ڴ�ط�ʽ */

typedef struct ACL_STACK ACL_STACK;

/**
 * ջ���Ͷ���
 */
struct ACL_STACK {
	int   capacity;
	int   count;
	void **items;

	/* ��Ӽ����� */

	/* ��ջβ����Ӷ�̬���� */
	void  (*push_back)(struct ACL_STACK*, void*);
	/* ��ջͷ����Ӷ�̬���� */
	void  (*push_front)(struct ACL_STACK*, void*);
	/* ����ջβ����̬���� */
	void *(*pop_back)(struct ACL_STACK*);
	/* ����ջͷ����̬���� */
	void *(*pop_front)(struct ACL_STACK*);

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ACL_ITER*, struct ACL_STACK*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ACL_ITER*, struct ACL_STACK*);
	/* ȡ������β���� */
	void *(*iter_tail)(ACL_ITER*, struct ACL_STACK*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ACL_ITER*, struct ACL_STACK*);
};

/**
 * ����ջ�ռ��С
 * @param s {ACL_STACK*} ������ջ��������
 * @param count {int} ���ӵĿռ��С
 */
ACL_API void acl_stack_space(ACL_STACK *s, int count);

/**
 * ����һ��ջ��������
 * @param init_size {int} ջ�ĳ�ʼ���ռ��С������ > 0
 * @return {ACL_STACK*} �´�����ջ��������
 */
ACL_API ACL_STACK *acl_stack_create(int init_size);

/**
 * ���ջ��Ķ��󣬵�������ջ��������
 * @param s {ACL_STACK*} ������ջ��������
 * @param free_fn {void (*)(void*)} �����Ϊ�գ�����ô˺����ص�ջ���ÿһ������
 */
ACL_API void acl_stack_clean(ACL_STACK *s, void (*free_fn)(void *));

/**
 * ���ջ������Ķ�������ջ����
 * @param s {ACL_STACK*} ������ջ��������
 * @param free_fn {void (*)(void*)} �����Ϊ�գ�����ô˺����ص�ջ���ÿһ������
 */
ACL_API void acl_stack_destroy(ACL_STACK *s, void (*free_fn)(void *));

/**
 * ��ջ����β������µĶ���
 * @param s {ACL_STACK*} ������ջ��������
 * @param obj {void*}
 */
ACL_API void acl_stack_append(ACL_STACK *s, void *obj);
#define	acl_stack_push	acl_stack_append

/**
 * ��ջ������ͷ������µĶ���
 * @param s {ACL_STACK*} ������ջ��������
 * @param obj {void*}
 * ע���˲�����Ч��Ҫ�� acl_stack_append �ͣ���Ϊ��������Ҫ�ƶ����еĶ���λ��
 */
ACL_API void acl_stack_prepend(ACL_STACK *s, void *obj);

/**
 * ��ջ������ɾ��ĳ������
 * @param s {ACL_STACK*} ������ջ��������
 * @param position {int} ջ������λ��
 * @param free_fn {void (*)(void*)} �����Ϊ�գ����ô˺����ص���ɾ������
 */
ACL_API void acl_stack_delete(ACL_STACK *s, int position, void (*free_fn)(void *));

/**
 * @param s {ACL_STACK*} ������ջ��������
 * @param obj {void*} ��ɾ������ĵ�ַ
 * @param free_fn {void (*)(void*)} �����Ϊ�գ����ô˺����ص���ɾ������
 */
ACL_API void acl_stack_delete_obj(ACL_STACK *s, void *obj, void (*free_fn)(void *));

/**
 * ����ջ������ĳ��λ�õĶ����ַ
 * @param s {ACL_STACK*} ������ջ��������
 * @param position {int} ջ�����е�λ��
 * @return {void*} != NULL: ok; == NULL: error�򲻴���
 */
ACL_API void *acl_stack_index(ACL_STACK *s, int position);

/**
 * ����ջ�����е�ǰ�Ķ������
 * @param s {ACL_STACK*} ������ջ��������
 * @return {int} �������
 */
ACL_API int acl_stack_size(const ACL_STACK *s);

/**
 * ����ջ��β���Ķ����ַ, ͬʱ���ö����ջ���Ƴ�
 * @param s {ACL_STACK*} ������ջ��������
 * @return {void*} �����ַ, == NULL ��ʾ��ǰ����Ϊ��
 */
ACL_API void *acl_stack_pop(ACL_STACK *s);

/**
 * ����ջ�������ӵĶ����ַ, �������ö����ջ���Ƴ�
 * @param s {ACL_STACK*} ������ջ��������
 * @return {void*} �����ַ, == NULL ��ʾ��ǰ����Ϊ��
 */
ACL_API void *acl_stack_top(ACL_STACK *s);

#ifdef	__cplusplus
}
#endif

#endif
