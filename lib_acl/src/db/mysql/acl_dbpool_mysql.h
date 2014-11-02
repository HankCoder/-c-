
#ifndef	__ACL_DBPOOL_MYSQL_INCLUDE_H__
#define	__ACL_DBPOOL_MYSQL_INCLUDE_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include "db/acl_dbpool.h"

#ifdef	HAS_MYSQL

/* in acl_dbpool_mysql.c */

/**
 * ����һ�� mysql ���͵����ݿ����ӳ�
 * @param db_info ��¼���й�������������Ҫ����Ϣ
 * @return DB_POOL * ����һ�����õ����ӳؾ��
 */
ACL_DB_POOL *acl_dbpool_mysql_create(const ACL_DB_INFO *db_info);

/**
 * �����ٴ� mysql ����, ������´򿪳ɹ�, ��ͬʱ�رվ�����, �����
 * ʧ��, ����ԭ������ handle ��
 * @param handle {ACL_DB_HANDLE*}
 * @return {int} 0: ��ʾ���´򿪳ɹ�
 */
int sane_mysql_reopen(ACL_DB_HANDLE *handle);

#endif /* HAS_MYSQL */

#ifdef	__cplusplus
}
#endif

#endif
