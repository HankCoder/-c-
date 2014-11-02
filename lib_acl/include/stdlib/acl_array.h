#ifndef	ACL_ARRAY_INCLUDE_H
#define	ACL_ARRAY_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_iterator.h"

/**
 * ��̬�������Ͷ���
 */
typedef	struct ACL_ARRAY ACL_ARRAY;
struct ACL_ARRAY{
	int     capacity;	/**< items ����ռ��С */
	int     count;		/**< items �к���Ԫ�صĸ��� */
	void    **items;	/**< ��̬���� */

	/* ��Ӽ����� */

	/* ������β����Ӷ�̬���� */
	void  (*push_back)(struct ACL_ARRAY*, void*);
	/* ������ͷ����Ӷ�̬���� */
	void  (*push_front)(struct ACL_ARRAY*, void*);
	/* ��������β����̬���� */
	void *(*pop_back)(struct ACL_ARRAY*);
	/* ��������ͷ����̬���� */
	void *(*pop_front)(struct ACL_ARRAY*);

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ACL_ITER*, struct ACL_ARRAY*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ACL_ITER*, struct ACL_ARRAY*);
	/* ȡ������β���� */
	void *(*iter_tail)(ACL_ITER*, struct ACL_ARRAY*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ACL_ITER*, struct ACL_ARRAY*);
};

/**
 * ����һ����̬����
 * @param init_size {int} ��̬����ĳ�ʼ��С
 * @return {ACL_ARRAY*} ��̬����ָ��
 */
ACL_API ACL_ARRAY *acl_array_create(int init_size);

/**
 * �ͷŵ���̬�����ڵĳ�Ա�������������ͷŶ�̬�������
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ��
 */
ACL_API void acl_array_clean(ACL_ARRAY *a, void (*free_fn)(void *));

/**
 * �ͷŵ���̬�����ڵĳ�Ա���������ͷŶ�̬�������
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ��
 */
ACL_API void acl_array_free(ACL_ARRAY *a, void (*free_fn)(void *));
#define acl_array_destroy acl_array_free

/**
 * ��̬����β����Ӷ�̬��Ա����
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param obj {void*} ��̬��Ա����
 * @return {int} >=0: �ɹ�, ����ֵΪ��Ԫ���������е��±�λ�ã�-1: ʧ��
 */
ACL_API int acl_array_append(ACL_ARRAY *a, void *obj);

/**
 * ��̬����ͷ����Ӷ�̬��Ա����
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param obj {void*} ��̬��Ա����
 * @return {int} >=0: �ɹ�, ����ֵΪ��Ԫ���������е��±�λ�ã�-1: ʧ��
 */
ACL_API int acl_array_prepend(ACL_ARRAY *a, void *obj);

/**
 * ��̬������ָ��λ��ǰ��Ӷ�̬��Ա����(�ý�㼰�Ժ����н�㶼����һ��λ��)
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param position {int} ĳ��λ�ã�����Խ��
 * @param obj {void*} ��̬��Ա����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
ACL_API int acl_array_pred_insert(ACL_ARRAY *a, int position, void *obj);

/**
 * ��̬������ָ��λ�ú���Ӷ�̬��Ա����(�ý���Ժ����н�㶼����һ��λ��)
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param position {int} ĳ��λ�ã�����Խ��
 * @param obj {void*} ��̬��Ա����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
ACL_API int acl_array_succ_insert(ACL_ARRAY *a, int position, void *obj);
#define acl_array_insert acl_array_succ_insert

/**
 * �Ӷ�̬�����е�ָ��λ��ɾ��ĳ����̬����, ɾ����������Ԫ�ص��Ⱥ�˳�򱣳ֲ���,
 * �����ɾ��λ�����м�ĳ��λ�ã�Ϊ�˱�֤Ԫ�ص�˳���ԣ��ڲ�����ɾ��Ԫ�غ������Ԫ��
 * ��ǰ��һ��λ��
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param position {int} ĳ��λ�ã�����Խ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ�룬�����
 *  ָ��Ϊ�գ����ͷţ������ô˺��������ͷŶ�̬����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
ACL_API int acl_array_delete_idx(ACL_ARRAY *a, int position, void (*free_fn)(void *));

/**
 * �Ӷ�̬�����е�ָ��λ��ɾ��ĳ������ɾ����������Ԫ�ص��Ⱥ�˳���п��ܷ����˸ı�,
 * ��Ϊɾ������Զ�������������Ԫ��������λ�ô�
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param position {int} ĳ��λ�ã�����Խ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ�룬�����
 *  ָ��Ϊ�գ����ͷţ������ô˺��������ͷŶ�̬����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
ACL_API int acl_array_delete(ACL_ARRAY *a, int position, void (*free_fn)(void*));

/**
 * �Ӷ�̬������ɾ��ָ��ָ���ַ�Ķ�̬����, ɾ����������Ԫ�ص��Ⱥ�˳�򱣳ֲ���
 * �����ɾ��λ�����м�ĳ��λ�ã�Ϊ�˱�֤Ԫ�ص�˳�����ڲ�������ɾ��Ԫ�غ������Ԫ��
 * ��ǰ��һ��λ��
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param obj {void*} ��̬����ָ���ַ
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ�룬�����
 *  ָ��Ϊ�գ����ͷţ������ô˺��������ͷŶ�̬����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
ACL_API int acl_array_delete_obj(ACL_ARRAY *a, void *obj, void (*free_fn)(void *));

/**
 * �Ӷ�̬������ɾ��ĳ���±귶Χ�Ķ�̬����
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param ibegin {int} ��ʼ�±�λ��
 * @param iend {int} �����±�λ��
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ�룬�����
 *  ָ��Ϊ�գ����ͷţ������ô˺��������ͷŶ�̬����
 * @return {int} 0: �ɹ���-1: ʧ��
 */
ACL_API int acl_array_delete_range(ACL_ARRAY *a, int ibegin, int iend, void (*free_fn)(void*));

/**
 * �ƶ���̬�����еĶ���
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param ito {int} �ƶ���Ŀ���±�λ��
 * @param ifrom {int} �Ӵ��±�λ�ÿ�ʼ�ƶ�
 * @param free_fn {void (*)(void*)} �����ͷŶ�̬�����ڳ�Ա�������ͷź���ָ�룬�����
 *  ָ��Ϊ�գ����ͷţ������ô˺��������ͷŶ�̬�����ͷŵĶ�̬��������Ϊ
 *  [idx_obj_begin, idx_src_begin), Ϊһ�뿪�������
 * @return {int} 0: �ɹ���-1: ʧ��
 */
ACL_API int acl_array_mv_idx(ACL_ARRAY *a, int ito, int ifrom, void (*free_fn)(void *) );

/**
 * Ԥ�ȱ�֤��̬����Ŀռ䳤��
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param app_count {int} ��Ҫ��̬���������� app_count ������λ��
 * @return {int} 0: �ɹ���-1: ʧ��
 */
ACL_API int acl_array_pre_append(ACL_ARRAY *a, int app_count);

/**
 * �Ӷ�̬�����е�ĳ���±�λ��ȡ����̬����
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @param idx {int} �±�λ�ã�����Խ�磬���򷵻�-1
 * @return {void*} != NULL: �ɹ���== NULL: �����ڻ�ʧ��
 */
ACL_API void *acl_array_index(const ACL_ARRAY *a, int idx);

/**
 * ��õ�ǰ��̬�����ж�̬����ĸ���
 * @param a {ACL_ARRAY*} ��̬����ָ��
 * @return {int} ��̬�����ж�̬����ĸ���
 */
ACL_API int acl_array_size(const ACL_ARRAY *a);

#ifdef  __cplusplus
}
#endif

#endif

