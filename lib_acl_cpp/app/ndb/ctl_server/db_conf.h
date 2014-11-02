#pragma once

extern char* var_cfg_mysql_dbaddr;
extern char* var_cfg_mysql_dbname;
extern char* var_cfg_mysql_dbuser;
extern char* var_cfg_mysql_dbpass;

extern int   var_cfg_mysql_dbpool_limit;
extern int   var_cfg_mysql_dbpool_timeout;
extern int   var_cfg_mysql_dbpool_dbping;

/**
 * ����������
 * @param path {const char*} �����ļ�·��
 */
bool db_conf_load(const char* path);

/**
 * ж�������������ڴ�
 */
void db_conf_unload(void);
