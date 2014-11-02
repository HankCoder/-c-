#ifndef	__PRIVATE_ARRAY_INCLUDE_H_
#define	__PRIVATE_ARRAY_INCLUDE_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include "stdlib/acl_iterator.h"
#include "stdlib/acl_array.h"

/**
 * ����һ����̬����
 * @param init_size {int} ��̬����ĳ�ʼ��С
 * @return {ACL_ARRAY*} ��̬����ָ��
 */
ACL_ARRAY *private_array_create(int init_size);

/**
 * �ͷŵ���̬�����ڵĳ�Ա�������������ͷŶ�̬�������
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ��
 */
void private_array_clean(ACL_ARRAY *a, void (*free_fn)(void *));

/**
 * �ͷŵ���̬�����ڵĳ�Ա���������ͷŶ�̬�������
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ��
 */
void private_array_destroy(ACL_ARRAY *a, void (*free_fn)(void *));

/**
 * ��̬����β����Ӷ�̬��Ա����
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param obj {void*} ��̬��Ա����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
int private_array_push(ACL_ARRAY *a, void *obj);

/**
 * �Ӷ�̬�����е�����βһ������
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @return {void*}, NULL ��ʾ����Ϊ��
 */
void* private_array_pop(ACL_ARRAY *a);

/**
 * �Ӷ�̬�����е�ָ��λ��ɾ��ĳ������ɾ����������Ԫ�ص��Ⱥ�˳���п��ܷ����˸ı�,
 * ��Ϊɾ������Զ�������������Ԫ��������λ�ô�
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param position {int} ĳ��λ�ã�����Խ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ�룬�����
 *  ָ��Ϊ�գ����ͷţ������ô˺��������ͷŶ�̬����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
int private_array_delete(ACL_ARRAY *a, int idx, void (*free_fn)(void*));

int private_array_delete_obj(ACL_ARRAY *a, void *obj, void (*free_fn)(void*));

/**
 * �Ӷ�̬�����е�ĳ���±�λ��ȡ����̬����
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param idx {int} �±�λ�ã�����Խ�磬���򷵻�-1
 * @return {void*} != NULL: �ɹ���== NULL: �����ڻ�ʧ��
 */
void *private_array_index(const ACL_ARRAY *a, int idx);

/**
 * ��õ�ǰ��̬�����ж�̬����ĸ���
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @return {int} ��̬�����ж�̬����ĸ���
 */
int private_array_size(const ACL_ARRAY *a);

void private_array_grow(ACL_ARRAY *a, int min_capacity);

#define	PRIVATE_ARRAY_PUSH(a, ptr) do  \
{  \
	if ((a)->count >= (a)->capacity)  \
		private_array_grow((a), (a)->count + 16);  \
	(a)->items[(a)->count++] = (ptr);  \
} while (0)

#define	PRIVATE_ARRAY_POP(a, ptr) do  \
{  \
	if ((a)->count > 0) {  \
		(a)->count--;  \
		(ptr) = (a)->items[(a)->count];  \
		(a)->items[(a)->count] = NULL;  \
	} else {  \
		(ptr) = NULL;  \
	}  \
} while (0)


#ifdef  __cplusplus
}
#endif

#endif

