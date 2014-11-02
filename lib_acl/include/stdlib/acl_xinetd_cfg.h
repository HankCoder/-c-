#ifndef	ACL_XINETD_CFG_INCLUDE_H
#define	ACL_XINETD_CFG_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_array.h"

/**
 * �����ļ�����������Ͷ���
 */
typedef struct ACL_XINETD_CFG_PARSER ACL_XINETD_CFG_PARSER;

/**
 * ����: �������Ҫ������ �������
 * @param xcp: �ṹָ��, ����Ϊ��
 * @param name: ������ı�����
 * @return �����ļ��������������
 */
ACL_API const char *acl_xinetd_cfg_get(const ACL_XINETD_CFG_PARSER *xcp, const char *name);

/**
 * ����: �������Ҫ������������飬����һ����������Ӧ�������ֵʱ����
 * @param xcp: �ṹָ��, ����Ϊ��
 * @param name: ������ı�����
 * @return �����ļ�������������ݶ�̬����
 */
ACL_API const ACL_ARRAY *acl_xinetd_cfg_get_ex(const ACL_XINETD_CFG_PARSER *xcp, const char *name);

/**
 * ����: �������ļ��л�ö�Ӧ����ĳһ������ֵλ�õ�����
 * @param xcp: �ṹָ��, ����Ϊ��
 * @param idx: ����λ��ֵ
 * @param ppname: ָ��ָ��ĵ�ַ�ı���
 * @param ppvalue: ָ��ָ��ĵ�ַ�ı���
 * @return  0: OK, -1: ERR
 */
ACL_API int acl_xinetd_cfg_index(const ACL_XINETD_CFG_PARSER *xcp,
			int idx,
			char **ppname,
			char **ppvalue);

/**
 * ����: �����ļ��������������
 * @param xcp: �ṹָ��, ����Ϊ��
 * @return �����ļ��������������
 */
ACL_API int acl_xinetd_cfg_size(const ACL_XINETD_CFG_PARSER *xcp);

/**
 * ����: �ͷ��ɽṹָ����ָ����ڴ�ռ�
 * @param xcp: �ṹָ��
 */
ACL_API void acl_xinetd_cfg_free(ACL_XINETD_CFG_PARSER *xcp);

/**
 * ����: ��ȡ�����ļ������н���
 * @param pathname: �����ļ����ļ���
 * @return �Ѿ������������ļ��Ľṹָ��
 */
ACL_API ACL_XINETD_CFG_PARSER *acl_xinetd_cfg_load(const char *pathname);

/**
 * ������������ṹ
 */
typedef struct ACL_CFG_INT_TABLE {
	const char *name;
	int  defval;
	int *target;
	int  min;
	int  max;
} ACL_CFG_INT_TABLE;

/**
 * 64 λ������������ṹ
 */
typedef struct ACL_CFG_INT64_TABLE {
	const char *name;
	acl_int64  defval;
	acl_int64 *target;
	acl_int64  min;
	acl_int64  max;
} ACL_CFG_INT64_TABLE;

/**
 * �ַ�����������ṹ
 */
typedef struct ACL_CFG_STR_TABLE {
	const char *name;
	const char *defval;
	char **target;
} ACL_CFG_STR_TABLE;

/**
 * ��������������ṹ
 */
typedef struct ACL_CFG_BOOL_TABLE {
	const char *name;
	int   defval;
	int  *target;
} ACL_CFG_BOOL_TABLE;

/* in acl_xinetd_params.c */

/**
 * �������ļ��������ж�ȡ�������͵ı�
 * @param cfg {ACL_XINETD_CFG_PARSER*} ��Ϊ��ʱ����Ĭ��ֵ���и�ֵ
 * @param table {ACL_CFG_INT_TABLE*}
 */
ACL_API void acl_xinetd_params_int_table(ACL_XINETD_CFG_PARSER *cfg,
	ACL_CFG_INT_TABLE *table);

/**
 * �������ļ��������ж�ȡ 64 λ�������͵ı�
 * @param cfg {ACL_XINETD_CFG_PARSER*} ��Ϊ��ʱ����Ĭ��ֵ���и�ֵ
 * @param table {ACL_CFG_INT64_TABLE*}
 */
ACL_API void acl_xinetd_params_int64_table(ACL_XINETD_CFG_PARSER *cfg,
	ACL_CFG_INT64_TABLE *table);

/**
* �������ļ��������ж�ȡ�ַ������͵ı�
* @param cfg {ACL_XINETD_CFG_PARSER*} ��Ϊ��ʱ����Ĭ��ֵ���и�ֵ
* @param table {ACL_CFG_STR_TABLE*}
*/
ACL_API void  acl_xinetd_params_str_table(ACL_XINETD_CFG_PARSER *cfg,
	ACL_CFG_STR_TABLE *table);

/**
* �������ļ��������ж�ȡBOOL���͵ı�
* @param cfg {ACL_XINETD_CFG_PARSER*} ��Ϊ��ʱ����Ĭ��ֵ���и�ֵ
* @param table {ACL_CFG_BOOL_TABLE*}
*/
ACL_API void  acl_xinetd_params_bool_table(ACL_XINETD_CFG_PARSER *cfg,
	ACL_CFG_BOOL_TABLE *table);

#ifdef	__cplusplus
}
#endif

#endif

