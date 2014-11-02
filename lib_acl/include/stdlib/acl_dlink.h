#ifndef ACL_DLINK_INCLUDE
#define ACL_DLINK_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_array.h"
#include "acl_iterator.h"

/**
 * ���ֿ�������Ԫ�����Ͷ���
 */
typedef	struct {
	acl_uint64 begin;
	acl_uint64 end;
	void *pnode;
} ACL_DITEM;

/**
 * ���ֿ����������Ͷ���
 */
typedef	struct ACL_DLINK {
	ACL_ARRAY *parray;
	void *call_back_data;

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ACL_ITER*, struct ACL_DLINK*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ACL_ITER*, struct ACL_DLINK*);
	/* ȡ������β���� */
	void *(*iter_tail)(ACL_ITER*, struct ACL_DLINK*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ACL_ITER*, struct ACL_DLINK*);
} ACL_DLINK;

/**
 * ����һ�����ֿ�����������
 * @param nsize {int} ��ʼ�����С
 * @return {ACL_DLINK*} ���ֿ�����������
 */
ACL_API ACL_DLINK *acl_dlink_create(int nsize);

/**
 * �ͷ�һ�����ֿ�����������
 * @param plink {ACL_DLINK*} ���ֿ�����������ָ��
 */
ACL_API void acl_dlink_free(ACL_DLINK *plink);

/**
 * ���ݶ��ֿ�����Ԫ�ز������Ƿ������ڶ��ֿ���������
 * @param plink {ACL_DLINK*} ���ֿ�����������ָ��
 * @param pitem {ACL_DITEM*} ���ݿ�Ԫ��
 * @return {ACL_DITEM*} ���ݿ�Ԫ��
 */
ACL_API ACL_DITEM *acl_dlink_lookup_by_item(const ACL_DLINK *plink,
	ACL_DITEM *pitem);

/**
 * ��������Ԫ�ز��Ҷ��ֿ��������е�����Ԫ��
 * @param plink {const ACL_DLINK*} ���ֿ�����������ָ��
 * @param pitem {ACL_DITEM*} ���ݿ�Ԫ��
 * @param pidx {int*} �洢��ѯ����Ԫ�ؽ���ڶ����������е��±�λ��
 * @return {ACL_DITEM*} ���ݿ�Ԫ��
 */
ACL_API ACL_DITEM *acl_dlink_lookup2_by_item(const ACL_DLINK *plink,
	ACL_DITEM *pitem, int *pidx);

/**
 * �Ӷ��ֿ��������в�ѯĳ��ֵ����Ӧ�����ݿ�Ԫ�ص�ַ
 * @param plink {const ACL_DLINK*} ���ֿ�����������ָ��
 * @param n {acl_uint64} ��ѯֵ
 * @return {ACL_DITEM*} ���ݿ�Ԫ��
 */
ACL_API ACL_DITEM *acl_dlink_lookup(const ACL_DLINK *plink, acl_uint64 n);

/**
 * �Ӷ��ֿ��������в�ѯĳ��ֵ����Ӧ�����ݿ�Ԫ�ص�ַ����¼���±�λ��
 * @param plink {const ACL_DLINK*} ���ֿ�����������ָ��
 * @param n {acl_uint64} ��ѯֵ
 * @param pidx {int*} �洢��ѯ����Ԫ�ؽ���ڶ����������е��±�λ��
 * @return {ACL_DITEM*} ���ݿ�Ԫ��
 */
ACL_API ACL_DITEM *acl_dlink_lookup2(const ACL_DLINK *plink,
	acl_uint64 n, int *pidx);

/**
 * �Ӷ��ֿ��������в�ѯĳ����Χ�����ݿ�Ԫ�ص�ַ����¼���±�λ��
 * @param plink {const ACL_DLINK*} ���ֿ�����������ָ��
 * @param begin {acl_uint64} ��ѯ��Χ����ʼλ��ֵ
 * @param end {acl_uint64} ��ѯ��Χ�Ľ���λ��ֵ
 * @param pidx {int*} �洢��ѯ����Ԫ�ؽ���ڶ����������е��±�λ��
 * @return {ACL_DITEM*} ���ݿ�Ԫ��
 */
ACL_API ACL_DITEM *acl_dlink_lookup_range(const ACL_DLINK *plink,
	acl_uint64 begin, acl_uint64 end, int *pidx);

/**
 * �Ӷ��ֿ��������в�ѯ��һ������ĳ������ֵ�����ݿ�Ԫ�ز���¼�±�λ��
 * @param plink {const ACL_DLINK*} ���ֿ�����������ָ��
 * @param off {acl_uint64} �����Ƚ�ֵ
 * @param pidx {int*} �洢��ѯ����Ԫ�ؽ���ڶ����������е��±�λ��
 * @return {ACL_DITEM*} ���ݿ�Ԫ��
 */
ACL_API ACL_DITEM *acl_dlink_lookup_larger(const ACL_DLINK *plink,
	acl_uint64 off, int *pidx);

/**
 * �Ӷ��ֿ��������в�ѯ��һ��С��ĳ������ֵ�����ݿ�Ԫ�ز���¼�±�λ��
 * @param plink {const ACL_DLINK*} ���ֿ�����������ָ��
 * @param off {acl_uint64} �����Ƚ�ֵ
 * @param pidx {int*} �洢��ѯ����Ԫ�ؽ���ڶ����������е��±�λ��
 * @return {ACL_DITEM*} ���ݿ�Ԫ��
 */
ACL_API ACL_DITEM *acl_dlink_lookup_lower(const ACL_DLINK *plink,
	acl_uint64 off, int *pidx);

/**
 * ����ֿ��������������ʼ���������ݿ�
 * @param plink {ACL_DLINK*} ���ֿ�����������ָ��
 * @param begin {acl_uint64} ������ʼλ��ֵ
 * @param end {acl_uint64} ��������λ��ֵ
 * @return {ACL_DITEM*} �´��������ݿ�Ԫ��
 */
ACL_API ACL_DITEM *acl_dlink_insert(ACL_DLINK *plink,
	acl_uint64 begin, acl_uint64 end);

/**
 * �Ӷ��ֿ���������ɾ������ĳ������ֵ�����ݿ�Ԫ��
 * @param plink {ACL_DLINK*} ���ֿ�����������ָ��
 * @param n {acl_uint64} ����λ��ֵ
 * @return {int} 0����ʾOK��-1: ��ʾ��������Ƿ��򲻴���
 */
ACL_API int acl_dlink_delete(ACL_DLINK *plink, acl_uint64 n);

/**
 * �������ݿ�Ԫ�شӶ��ֿ���������ɾ�������ݿ�Ԫ��
 * @param plink {ACL_DLINK*} ���ֿ�����������ָ��
 * @param pitem {ACL_DITEM*} ���ݿ�Ԫ��
 * @return {int} 0����ʾOK��-1: ��ʾ��������Ƿ�
 */
ACL_API int acl_dlink_delete_by_item(ACL_DLINK *plink, ACL_DITEM *pitem);

/**
 * ����ͬ acl_dlink_insert
 * @deprecated �˺�������Ҳ�����ṩ
 */
ACL_API ACL_DITEM *acl_dlink_modify(ACL_DLINK *plink,
	acl_uint64 begin, acl_uint64 end);

/**
 * �Ӷ�����������ɾ��ĳ����ֵ��Χ�����ݿ鼯��, 
 * ɾ�����п��ܻ����ڲ������µ����ݿ�Ԫ��
 * @param plink {ACL_DLINK*} ���ֿ�����������ָ��
 * @param begin {acl_uint64} ��Ҫɾ����Χ����ʼλ��
 * @param end {acl_uint64} ��Ҫɾ����Χ�Ľ���λ��
 * @return {int} 0����ʾOK��-1: ��ʾ��������Ƿ�
 */
ACL_API int acl_dlink_delete_range(ACL_DLINK *plink,
	acl_uint64 begin, acl_uint64 end);

/**
 * ����ĳ�±�λ�õ����ݿ�Ԫ�ص�ַ
 * @param plink {const ACL_DLINK*} ���ֿ�����������ָ��
 * @param idx {int} �±�λ��
 * @return {ACL_DITEM*} NULL: �±�Խ��; != NULL: ���ݿ�Ԫ�ص�ַ
 */
ACL_API ACL_DITEM *acl_dlink_index(const ACL_DLINK *plink, int idx);

/**
 * ��õ�ǰ�������������������ݿ�ĸ����ܺ�
 * @param plink {const ACL_DLINK*} ���ֿ�����������ָ��
 * @return {int} ���ݿ�ĸ���
 */
ACL_API int acl_dlink_size(const ACL_DLINK *plink);

/**
 * (������)��ӡ�������������������ݿ����ʼ������λ�õ���Ϣ
 * @param plink {const ACL_DLINK*} ���ֿ�����������ָ��
 * @return {int} 0����ʾOK��-1: ��ʾ��������Ƿ�
 */
ACL_API int acl_dlink_list(const ACL_DLINK *plink);

#ifdef __cplusplus
}
#endif
#endif
