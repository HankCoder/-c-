#ifndef	__SERVICE_VAR_INCLUDE_H__
#define	__SERVICE_VAR_INCLUDE_H__

#include "lib_acl.h"

/*------------- �ַ��������� ----------------*/

extern ACL_CFG_STR_TABLE var_conf_str_tab[];

extern char *var_cfg_debug_msg;
extern char *var_cfg_smtpd_addr;
extern char *var_cfg_mail_from;
extern char *var_cfg_mail_to;
extern char *var_cfg_auth_user;
extern char *var_cfg_auth_pass;
extern char *var_cfg_warn_mail;
extern char *var_cfg_smtp_helo;
extern char *var_cfg_sms_addr;
extern char *var_cfg_host_ip;

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
extern int  var_cfg_smtp_notify_cache_timeout;
extern int  var_cfg_sms_notify_cache_timeout;
extern int  var_cfg_work_week_min;
extern int  var_cfg_work_week_max;
extern int  var_cfg_work_hour_min;
extern int  var_cfg_work_hour_max;

/*----------------- ���������ȫ�ֱ��� ------*/
extern ACL_ARGV *var_recipients;
extern ACL_ARGV *var_ccs;
extern ACL_ARGV *var_bccs;

void service_var_init(void);
void service_var_end(void);

#endif
