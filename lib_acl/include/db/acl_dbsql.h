#ifndef ACL_DBSQL_INCLUDE_H
#define ACL_DBSQL_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include "acl_dbpool.h"

#ifndef	ACL_DB_ATOU
#define ACL_DB_ATOU(_str_) (_str_ ? strtoul(_str_, (char **) NULL, 10) : 0)
#endif

/**
 * ���ݿ��ѯ��䣬�����û������ select SQL �������ݿ����ѯ���
 * @param handle {ACL_DB_HANDLE*} ���ݿ����Ӿ��������Ϊ��
 * @param sql {const char*} select ��ѯ��䣬����Ϊ��
 * @param error {int*} �������ֵ NULL �Ҹñ����ǿ�ָ�����ָ��ĵ�ַ��
 *  �������Ĵ���ţ�����Ųμ� acl_dberr.h
 * @return {ACL_SQL_RES*} ��ѯ������������ѯʧ�ܻ��ѯ���Ϊ�գ��򷵻�
 *  NULL�����򷵻� ACL_SQL_RES ����(�����ý����Ҫ���� acl_dbsql_free_result
 *  �ͷ�)��ʾ����
 *
 *  ACL_DB_HANDLE* handle = ...;
 *  ACL_SQL_RES* res = acl_dbsql_select(...);
 *  ACL_ITER iter;
 *  if (res)
 *  {
 *    acl_foreach(iter, res)
 *    {
 *      const char **my_row = (const char**) iter.data;
 *      printf("first item: %s\n", my_row[0]);
 *      ...
 *    }
 *    acl_dbsql_free_result(handle, res);
 *  }
 *
 */
ACL_API ACL_SQL_RES *acl_dbsql_select(ACL_DB_HANDLE *handle,
	const char *sql, int *error);

/**
 * �ͷ��� acl_dbsql_select ���صĽ��������
 * @param handle {ACL_DB_HANDLE*} ���ݿ����Ӿ��������Ϊ��
 * @param res {ACL_SQL_RES*} acl_dbsql_select ���صĽ�����󣬲���Ϊ��
 */
ACL_API void acl_dbsql_free_result(ACL_DB_HANDLE *handle, ACL_SQL_RES *res);

/**
 * �Իص��ķ�ʽ��ѯ���ݿ������з��������Ľ��������ѯ�������ͨ���û�����
 * �Ļص��������ظ��û�
 * @param handle {ACL_DB_HANDLE*} ���ݿ����Ӿ��������Ϊ��
 * @param sql {const char*} select ��ѯ��䣬����Ϊ��
 * @param error {int*} �������ֵ -1 �Ҹñ����ǿ�ָ�����ָ��ĵ�ַ��
 *  �������Ĵ���ţ�����Ųμ� acl_dberr.h
 * @param walk_fn {int (*)(const void**, void*)}���û����õĲ�ѯ����ص�������
 *  �ǿգ�ÿ��һ�����������Ľ�����ص��øûص������������ѯ���Ϊ���������
 *  �Զ��ص���θûص����������е� result_row ��һ������ָ�룬�û��������Լ�
 *  �Ļص��������� result_row[i] ��ȡ���Լ���Ҫ���������(������ select ���
 *  �е���ƥ��)
 * @param arg {void*} �û��Զ���Ĳ������ò������Զ����ݸ� walk_fn �ص�������
 *  ��Ϊ walk_fn �����һ����������
 * @return {int} ��ѯ���������������� -1 ���ʾ��ѯ���ʧ�ܣ�0 ��ʾû�з���
 *  ��ѯ�����Ľ����> 0 ��ʾ��ѯ�Ľ������
 */
ACL_API int acl_dbsql_results(ACL_DB_HANDLE *handle, const char *sql, int *error,
	int (*walk_fn)(const void** result_row, void *arg), void *arg);

/**
 * �Իص��ķ�ʽ�����ݿ��в�ѯһ����¼����ѯ���ͨ���û����õĻص��������ظ��û�
 * @param handle {ACL_DB_HANDLE*} ���ݿ����Ӿ��������Ϊ��
 * @param sql {const char*} select ��ѯ��䣬����Ϊ��
 * @param error {int*} �������ֵ -1 �Ҹñ����ǿ�ָ�����ָ��ĵ�ַ��
 *  �������Ĵ���ţ�����Ųμ� acl_dberr.h
 * @param walk_fn {int (*)(const void**, void*)}���û����õĲ�ѯ����ص�������
 *  �ǿգ����鵽һ�����������Ľ��ʱ��ص��øûص��������� acl_dbsql_results ��
 *  ͬ���ûص��������ֻ�ᱻ����һ�Σ����е� result_row ��һ������ָ�룬�û���
 *  �����Լ��Ļص��������� result_row[i] ��ȡ���Լ���Ҫ���������(������ select
 *  ����е���ƥ��)
 * @param arg {void*} �û��Զ���Ĳ������ò������Զ����ݸ� walk_fn �ص�������
 *  ��Ϊ walk_fn �����һ����������
 * @return {int} ����ֵֻ������״̬��-1 ��ʾ��ѯ���ʧ�ܣ�0 ��ʾ��ѯ���Ϊ�գ�
 *  1 ��ʾ�鵽һ�������������� -1 �� *error ��¼��ʧ��ԭ�򣬲μ� acl_dberr.h
 */
ACL_API int acl_dbsql_result(ACL_DB_HANDLE *handle, const char *sql, int *error,
	int (*walk_fn)(const void** result_row, void *arg), void *arg);

/**
 * �������ݿ����ݣ�update, insert, delete ���޸����ݿ�Ĳ�������ʹ�øú���
 * @param handle {ACL_DB_HANDLE*} ���ݿ����Ӿ��������Ϊ��
 * @param sql {const char*} ���ݿ��޸���䣬����Ϊ��
 * @param error {int*} �������ֵ -1 �Ҹñ����ǿ�ָ�����ָ��ĵ�ַ��
 *  �������Ĵ���ţ�����Ųμ� acl_dberr.h
 * @return {int} ����ֵֻ������״̬��-1 ��ʾʧ��(��� error ָ��ǿ������м�¼��
 *  ����ԭ�򣬴���Ųμ�: acl_dberr.h)��0 ��ʾ���³ɹ�������δӰ�����ݿ�ԭʼ��Ϣ
 *  (ԭ���Ǳ���Ϣ�����ݿ�ԭ��Ϣ��ͬ)��> 0 ��ʾ���������ݿ������ݴ洢����
 */
ACL_API int acl_dbsql_update(ACL_DB_HANDLE *handle, const char *sql, int *error);

#ifdef __cplusplus
}
#endif

#endif

