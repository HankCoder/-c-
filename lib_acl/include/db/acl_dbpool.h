#ifndef	ACL_DBPOOL_INCLUDE_H
#define	ACL_DBPOOL_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include <time.h>
#include "stdlib/acl_stdlib.h"

#define	ACL_DB_DEBUG_MEM	(1<<0)

typedef struct ACL_DB_HANDLE ACL_DB_HANDLE;
typedef struct ACL_SQL_RES ACL_SQL_RES;
typedef struct ACL_DB_POOL ACL_DB_POOL;

typedef struct ACL_DB_INFO {
	int   db_max;		/* ���ӳ���������� */
	char  db_addr[256];	/* ���ݿ�����ַ */
	char  db_name[256];	/* ���ݿ����� */
	char  db_user[256];	/* ���ݿ��ʺ� */
	char  db_pass[256];	/* �ʺ����� */
	unsigned long db_flags;	/* (mysql) ���ӱ�־λ */
	int   ping_inter;	/* ̽�����ݿ����ӵ�ʱ���� */
	int   timeout_inter;	/* ���ݿ����ӵĿ��г�ʱʱ�� */
	int   auto_commit;	/* (mysql) �Ƿ������Զ��ύ���� */
	int   conn_timeout;	/* (mysql/null) ���ӳ�ʱʱ�� */
	int   rw_timeout;	/* (mysql/null) IO��д��ʱʱ�� */
	int   buf_size;		/* (null) IO��������С */
	int   debug_flag;	/* ���Ա�־λ */

	/* ����ʵ�������ݿ�֮ǰ/������û����õĻص�����, ���������Ϊ NULL,
	 * ��� db_before_connect/db_after_connect ���� < 0 ��ᵼ��
	 * acl_dbpool_peek ���� NULL
	 */
	int  (*db_before_connect)(ACL_DB_HANDLE* db_handle, void *ctx);
	int  (*db_after_connect)(ACL_DB_HANDLE* db_handle, void *ctx);

	void *ctx;		/* db_before_connect/db_after_connect ����֮һ */
} ACL_DB_INFO;

struct ACL_DB_HANDLE {
#define	ACL_DBH_STATUS_NULL	0
#define	ACL_DBH_STATUS_READY	1
#define	ACL_DBH_STATUS_INUSE	2
	int    status;
	time_t timeout;
	time_t ping;

	ACL_DB_POOL *parent;

	int (*sql_results)(ACL_DB_HANDLE *handle, const char *sql, int  *error,
		int (*walk_fn)(const void** result_row, void *arg), void *arg);
	int (*sql_result)(ACL_DB_HANDLE *handle, const char *sql, int  *error,
		int (*callback)(const void** result_row, void *arg), void *arg);
	int (*sql_update)(ACL_DB_HANDLE *handle, const char *sql, int  *error);

	ACL_SQL_RES *(*sql_select)(ACL_DB_HANDLE *handle, const char *sql, int *error);
	void (*free_result)(ACL_SQL_RES *res);
};

struct ACL_SQL_RES {
	void *res;
	int   num;

        /* for acl_iterator */

	/* ȡ������ͷ���� */
	const void *(*iter_head)(ACL_ITER*, struct ACL_SQL_RES*);
	/* ȡ��������һ������ */
	const void *(*iter_next)(ACL_ITER*, struct ACL_SQL_RES*);
};

struct ACL_DB_POOL {
	ACL_DB_INFO db_info;

	ACL_DB_HANDLE *(*dbh_peek)(ACL_DB_POOL *);
	void  (*dbh_check)(ACL_DB_POOL *);
	void  (*dbh_release)(ACL_DB_HANDLE *);
	void *(*dbh_export)(ACL_DB_HANDLE *);
	void  (*dbh_close)(ACL_DB_HANDLE *);
	int   (*dbh_ping)(ACL_DB_HANDLE *);

	void  (*destroy)(ACL_DB_POOL *);

	int  db_max;
	int  db_ready;
	int  db_inuse;
};

/*----------------------------------------------------------------------------*/
#define	ACL_DB_AUTO_COMMIT(_db_pool_) (_db_pool_ ? _db_pool_->db_info.auto_commit : 0)

/* in acl_dbpool.c */
/**
 * ����һ�����ݿ����ӳ�
 * @param db_type {const char*} ���ݿ�������, Ŀǰ��֧�� mysql
 * @param db_info {const ACL_DB_INFO*} ��¼���й�������������Ҫ����Ϣ
 * @return {ACL_DB_POOL*} һ�����ݿ����ӳ�
 */
ACL_API ACL_DB_POOL *acl_dbpool_create(const char *db_type, const ACL_DB_INFO *db_info);

/**
 * ����һ�����ݿ����ӳ�
 * @param db_pool ���ݿ����ӳؾ��
 */
ACL_API void acl_dbpool_destroy(ACL_DB_POOL *db_pool);

/**
 * �����ӳ��л�ȡһ�����Ӿ��
 * @param db_pool {ACL_DB_POOL*} ���ݿ����ӳؾ��
 * @return {ACL_DB_HANDLE*} ���ݿ����Ӿ�������Ϊ�����ʾ��������ӳ�����
 */
ACL_API ACL_DB_HANDLE *acl_dbpool_peek(ACL_DB_POOL *db_pool);

/**
 * �ֹ�������ӳص�ÿ������?һ�����ӳ��ڲ��ᶨ�ڼ��ÿ�����ӣ�
 * Ҳ����ͨ���˺����ֹ�����ǿ�Ƽ��
 * @param db_pool {ACL_DB_POOL*} ���ݿ����ӳؾ��
 */
ACL_API void acl_dbpool_check(ACL_DB_POOL *db_pool);

/**
 * �����ݿ����Ӿ���ͷŸ����ݿ����ӳ�
 * @param db_handle {ACL_DB_HANDLE*} ���ݿ����Ӿ��
 */
ACL_API void acl_dbpool_release(ACL_DB_HANDLE *db_handle);
/**
 * �����ݿ�����ת��Ϊʵ�ʵ����ݿ����Ӿ��
 * @param db_handle {ACL_DB_HANDLE*} ���ݿ����Ӿ��
 * @return void * ʹ������Ҫ����ǿ��ת��Ϊ�Լ����õ����ݿ���������
 */
ACL_API void *acl_dbpool_export(ACL_DB_HANDLE *db_handle);
/**
 * ��ʹ�����Լ���⵽�����ݿ����ӳ���ʱ������ͨ���˽ӿ�ǿ�йرո�����
 * @param db_handle {ACL_DB_HANDLE*} ���ݿ����Ӿ��
 */
ACL_API void acl_dbpool_close(ACL_DB_HANDLE *db_handle);

/**
 * �������ӳصĶ�ʱPING����������������ô�ֵ���ڲ�����ȱʡ��ʽ
 * @param db_pool {ACL_DB_POOL*} ���ݿ����ӳؾ��
 * @param ping_fn {int (*)(ACL_DB_HANDLE*)} ̽������״̬�ĺ���ָ��
 */
ACL_API void acl_dbpool_set_ping(ACL_DB_POOL *db_pool, int (*ping_fn)(ACL_DB_HANDLE*));
/*----------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif

#endif
