
#ifndef	__ACL_DBPOOL_NULL_INCLUDE_H__
#define	__ACL_DBPOOL_NULL_INCLUDE_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include "db/acl_dbpool.h"

/* in acl_dbpool_null.c */
/**
 * ����һ�� null ���͵����ݿ����ӳ�
 * @param db_info ��¼���й�������������Ҫ����Ϣ
 * @return DB_POOL * ����һ�����õ����ӳؾ��
 */
extern ACL_DB_POOL *acl_dbpool_null_create(const ACL_DB_INFO *db_info);

#ifdef	__cplusplus
}
#endif

#endif
