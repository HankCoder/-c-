
#ifndef	__SERVICE_MAIN_INCLUDE_H__
#define	__SERVICE_MAIN_INCLUDE_H__

#include "lib_acl.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* �����ļ��� */
/* in service_main.c */

extern int   var_cfg_debug_mem;
extern char *var_cfg_dbpath;
extern char *var_cfg_dbnames;
extern int   var_cfg_rw_timeout;
extern int   var_cfg_use_bdb;

extern ACL_CONFIG_BOOL_TABLE service_conf_bool_tab[];
extern ACL_CONFIG_INT_TABLE service_conf_int_tab[];
extern ACL_CONFIG_STR_TABLE service_conf_str_tab[];

/**
 * ��ʼ��������������ģ��������������øú���һ��
 * @param init_ctx {void*} �û��Զ�������ָ��, �����ڵ���
 *  app_main() ʱ����
 */
extern void service_init(void *init_ctx);

/**
 * �����˳�ʱ�Ļص�����
 * @param exist_ctx {void*} �û��Զ�������ָ��
 */
extern void service_exit(void *exit_ctx);

/**
 * Э�鴦�������
 * @param stream {ACL_VSTREAM*} �ͻ�������������
 * @param run_ctx {void*} �û��Զ�������ָ��
 */
extern int service_main(ACL_ASTREAM *astream, void *run_ctx);

#ifdef	__cplusplus
}
#endif

#endif
