#ifndef	__SERVICE_VAR_INCLUDE_H__
#define	__SERVICE_VAR_INCLUDE_H__

#include "lib_acl.h"

/*------------- �ַ��������� ----------------*/

extern ACL_CFG_STR_TABLE var_conf_str_tab[];

/* ��־���������Ϣ */
extern char *var_cfg_debug_msg;

/*-------------- ����ֵ������ ---------------*/

extern ACL_CFG_BOOL_TABLE var_conf_bool_tab[];

/* �Ƿ������־������Ϣ */
extern int var_cfg_debug_enable;

/* �Ƿ���ͻ��˱��ֳ����� */
extern int var_cfg_keep_alive;

/*-------------- ���������� -----------------*/

extern ACL_CFG_INT_TABLE var_conf_int_tab[];

/* ÿ����ͻ���ͨ��ʱ������ʱʱ��(��) */
extern int  var_cfg_io_timeout;

#endif
