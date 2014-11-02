#ifndef	ACL_MEMDB_INCLUDE_H
#define	ACL_MEMDB_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"

typedef struct ACL_MDT_NOD ACL_MDT_NOD;
typedef struct ACL_MDT_RES ACL_MDT_RES;
typedef struct ACL_MDT_REC ACL_MDT_REC;
typedef struct ACL_MDT_REF ACL_MDT_REF;
typedef struct ACL_MDT_IDX ACL_MDT_IDX;
typedef struct ACL_MDT ACL_MDT;
typedef struct ACL_MDB ACL_MDB;

/* ���ݱ�������ؼ��ֶε�Լ����־λ */
#define ACL_MDT_FLAG_NUL		(0)
#define ACL_MDT_FLAG_UNI		(1 << 0)	/**< ��ʾΨһ */
#define	ACL_MDT_FLAG_KMR		(1 << 1)	/**< ��ʾ���ü��ڴ� */
#define ACL_MDT_FLAG_DMR		(1 << 2)	/**< ��ʾ����ֵ�ڴ� */
#define	ACL_MDT_FLAG_SLICE1		(1 << 10)	/**< ���� ACL_SLICE_FLAG_GC1 */
#define	ACL_MDT_FLAG_SLICE2		(1 << 11)	/**< ���� ACL_SLICE_FLAG_GC2 */
#define	ACL_MDT_FLAG_SLICE3		(1 << 12)	/**< ���� ACL_SLICE_FLAG_GC3 */
#define	ACL_MDT_FLAG_SLICE_RTGC_OFF	(1 << 13)	/**< �ر��ڴ���Ƭ��ʵʱ�������չ��� */

/************************************************************************/
/*                          in acl_mdb.c                                */
/************************************************************************/

/**
 * ����һ�����ݿ���
 * @param dbname {const char*} ���ݿ���
 * @param dbtype {const char*} ���ݿ�����: hash/avl
 * @return {ACL_MDB*} ���ݿ���
 */
ACL_API ACL_MDB *acl_mdb_create(const char *dbname, const char *dbtype);

/**
 * �رղ��ͷ�һ���ڴ����ݿ�
 * @param {ACL_MDB*} ���ݿ���
 */
ACL_API void acl_mdb_free(ACL_MDB *mdb);

/**
 * �ڸ������ݿ��ϴ���һ�����ݱ�
 * @param mdb {ACL_MDB*} ���ݿ���
 * @param tbl_name {const char*} ����
 * @param tlb_flag {unsigned int} ������Ա�־λ
 * @param init_capacity {size_t} ���ݱ������ÿ�����������ڲ���ϣ��ĳ�ʼ������
 * @param key_labels {const char *[]} ���ݱ��ֶ�������
 * @param flags {unsigned int[]} �� key_labels ���Ӧ��Լ����־λ
 * @return {ACL_MDT*} �½������ݱ���
 */
ACL_API ACL_MDT *acl_mdb_tbl_create(ACL_MDB *mdb, const char *tbl_name,
	unsigned int tbl_flag, size_t init_capacity,
	const char *key_labels[], unsigned int flags[]);

/**
 * ���һ���µ����ݼ�¼
 * @param mdb {ACL_MDB*} ���ݿ���
 * @param tbl_name {const char*} ���ݱ���
 * @param data {void*} Ӧ��������
 * @param dlen {unsigned int} data �����ݴ�С
 * @param key_labels {const char *[]} ���ݱ��ֶ�������
 * @param keys {const char *[]} ���ݱ��ֶ�����Ӧֵ����
 * @return {ACL_MDB_NOD*} ����ӵ����������ݱ��д洢�ľ��
 */
ACL_API ACL_MDT_NOD *acl_mdb_add(ACL_MDB *mdb, const char *tbl_name,
	void *data, unsigned int dlen,
	const char *key_labels[], const char *keys[]);

/**
 * ̽�����ݱ��ж�Ӧ���ֶ�ֵ�Ƿ����
 * @param mdb {ACL_MDB*} ���ݿ���
 * @param tbl_name {const char*} ���ݱ���
 * @param key_label {const char*} ���ݱ������ֶ���
 * @param key {const char*} ���ݱ������ֶμ�ֵ
 * @return {int} 0: ������, != 0: ����
 */
ACL_API int acl_mdb_probe(ACL_MDB *mdb, const char *tbl_name,
	const char *key_label, const char *key);

/**
 * �����ݿ��в�ѯ���������Ľ����
 * @param mdb {ACL_MDB*} ���ݿ���
 * @param tbl_name {const char*} ���ݱ���
 * @param key_label {const char*} ���ݱ��е��ֶ���
 * @param key {const char*} ���ݱ��е��ֶ�ֵ
 * @param from {int} ��ѯ�Ľ��ϣ���ǴӸ�λ�ÿ�ʼ���д洢
 * @param limit {int} ��ѯ�Ľ�������ϣ������
 * @return {ACL_MDT_RES*} ��ѯ����������Ϊ���������ѯ���Ϊ�ջ����
 */
ACL_API ACL_MDT_RES *acl_mdb_find(ACL_MDB *mdb, const char *tbl_name,
	const char *key_label, const char *key, int from, int limit);

/**
 * �����ݿ����г�ĳ���ݱ���ĳ����Χ�Ľ����
 * @param mdb {ACL_MDB*} ���ݿ���
 * @param tbl_name {const char*} ���ݱ���
 * @param from {int} ��ѯ�Ľ��ϣ���ǴӸ�λ�ÿ�ʼ���д洢
 * @param limit {int} ��ѯ�Ľ�������ϣ������
 * @return {ACL_MDT_RES*} ��ѯ����������Ϊ���������ѯ���Ϊ�ջ����
 */
ACL_API ACL_MDT_RES *acl_mdb_list(ACL_MDB *mdb, const char *tbl_name,
	int from, int limit);

/**
 * �����ݿ���ɾ��һ�����ݼ�¼
 * @param mdb {ACL_MDB*} ���ݿ���
 * @param tbl_name {const char*} ���ݱ���
 * @param key_label {const char*} ���ݱ��ֶ���
 * @param key {const char*} ���ݱ��ֶ�ֵ
 * @param onfree_fn {void (*)(void*, unsigned int)}
  *	�ͷ��û��Ķ���ʱ���õ��ͷŻص�����
 * @return {int} ɾ����������
 */
ACL_API int acl_mdb_del(ACL_MDB *mdb, const char *tbl_name,
	const char *key_label, const char *key,
	void (*onfree_fn)(void*, unsigned int));

/**
 * �������ݿ���ĳ�����ݱ���������ݽ��
 * @param mdb {ACL_MDB*} ���ݿ���
 * @param tbl_name {const char*} ���ݱ���
 * @param walk_fn �����ص�����������ú������ط�0ֵ����ֹͣ����
 * @param from {int} ��ѯ�Ľ��ϣ���ǴӸ�λ�ÿ�ʼ���д洢
 * @param limit {int} ��ѯ�Ľ�������ϣ������
 * @return {int} ���������ݽ����ֵ
 */
ACL_API int acl_mdb_walk(ACL_MDB *mdb, const char *tbl_name,
	int (*walk_fn)(const void*, unsigned int),
	int from, int limit);

/**
 * ���ݿ���ĳ�����ݱ���Ԫ���ܸ���
 * @param mdb {ACL_MDB*} ���ݿ���
 * @param tbl_name {const char*} ���ݱ���
 * @return {int} >=0
 */
ACL_API int acl_mdb_cnt(ACL_MDB *mdb, const char *tbl_name);

/************************************************************************/
/*                          in acl_mdt.c                                */
/************************************************************************/

/**
 * ����һ�����ݱ�
 * @param dbtype {const char *} ������: hash/avl
 * @param tbl_name {const char*} ����
 * @param tlb_flag {unsigned int} ������Ա�־λ
 * @param init_capacity {size_t} ÿ���ڲ���ϣ��ĳ�ʼ������
 * @param key_labels {const char *[]} ���еĸ����ֶ������飬�����NULL����
 * @param flags {unsigned int[]} �� key_labels ���Ӧ��Լ����־λ
 * @return {ACL_MDT*} �½������ݱ�ľ��
 */
ACL_API ACL_MDT *acl_mdt_create(const char *dbtype, const char *tbl_name,
	unsigned int tbl_flag, size_t init_capacity,
	const char *key_labels[], unsigned int flags[]);

/**
 * �ͷ�һ���ڴ��
 * @param mdt {ACL_MDT*} �ڴ����ݱ���
 */
ACL_API void acl_mdt_free(ACL_MDT *mdt);

/**
 * �����ݱ������һ���µ����ݼ�¼
 * @param mdt {ACL_MDT*} ���ݱ���
 * @param data {void*} �û��Ķ�̬����, ������ ACL_MDT_FLAG_DMR ��־λ
 *  δ�����ã������ڲ�����һ�ݸö�̬����
 * @param dlen {unsigned int} data �����ݳ���
 * @param key_labels {const char*[]} ���ݱ�������ֶ������飬��NULL����
 * @param keys {const char*[]} ���ݱ�������ֶ�ֵ���飬��NULL����
 * @return {ACL_MDT_NOD*} ����ӵ����ݽ�����
 */
ACL_API ACL_MDT_NOD *acl_mdt_add(ACL_MDT *mdt, void *data,
	unsigned int dlen, const char *key_labels[], const char *keys[]);

/**
 * ̽�����ݱ��ж�Ӧ���ֶ�ֵ�Ƿ����
 * @param mdt {ACL_MDT*} ���ݱ���
 * @param key_label {const char*} ���ݱ������ֶ���
 * @param key {const char*} ���ݱ������ֶμ�ֵ
 * @return {int} 0: ������, != 0: ����
 */
ACL_API int acl_mdt_probe(ACL_MDT *mdt, const char *key_label, const char *key);

/**
 * �����ݱ��в�ѯĳ�����ݱ�������ֵ�Ľ������
 * @param mdt {ACL_MDT*} ���ݱ���
 * @param key_label {const char*} ���ݱ������ֶ���
 * @param key {const char*} ���ݱ������ֶμ�ֵ
 * @param from {int} ��ѯ�Ľ��ϣ���ǴӸ�λ�ÿ�ʼ���д洢
 * @param limit {int} ��ѯ�Ľ�������ϣ������
 * @return {ACL_MDT_REC*} ��Ӧĳ�������ֶ�ֵ�Ľ������
 */
ACL_API ACL_MDT_RES *acl_mdt_find(ACL_MDT *mdt, const char *key_label,
	const char *key, int from, int limit);

/**
 * �����ݱ���˳���г�ĳ����Χ�ڵ��������ݽ�㼯��
 * @param mdt {ACL_MDT*} ���ݱ���
 * @param from {int} ��ѯ�Ľ��ϣ���ǴӸ�λ�ÿ�ʼ���д洢
 * @param limit {int} ��ѯ�Ľ�������ϣ������
 * @return {ACL_MDT_REC*} ��Ӧĳ�������ֶ�ֵ�Ľ������
 */
ACL_API ACL_MDT_RES *acl_mdt_list(ACL_MDT *mdt, int from, int limit);

/**
 * �����ݱ���ɾ����Ӧĳ�������ֶμ�ֵ�Ľ������
 * @param mdt {ACL_MDT*} ���ݱ���
 * @param key_label {const char*} ���ݱ������ֶ���
 * @param key {const char*} ���ݱ������ֶμ�ֵ
 * @param onfree_fn {void (*)(void*, unsigned int}
 *	�û������ͷŶ�̬���ݵĻص�����
 * @return {int} ���ͷŵ����ݽ�����Ŀ
 */
ACL_API int acl_mdt_delete(ACL_MDT *mdt, const char *key_label,
	const char *key, void (*onfree_fn)(void*, unsigned int));

/**
 * �������ݱ���������ݽ�㣬���ص��û��Ĵ�����
 * @param mdt {ACL_MDT*} ���ݱ���
 * @param walk_fn �ص��������������0�����������ֹͣ����
 * @param from {int} ��ѯ�Ľ��ϣ���ǴӸ�λ�ÿ�ʼ���д洢
 * @param len {int} ��ѯ�Ľ�������ϣ������
 * @return {int} �����������ݳ���
 */
ACL_API int acl_mdt_walk(ACL_MDT *mdt, int (*walk_fn)(const void*, unsigned int),
	int from, int len);

/**
 * ������ѯ�Ľ�������л�ȡ��һ�����ݽ��
 * @param res {ACL_MDT_RES*} ���ݽ������
 * @return {void*} �û��Լ��ܹ�ʶ��Ķ�̬���ݣ�������NULL��ʾ������Ѿ�û������
 */
ACL_API const void *acl_mdt_fetch_row(ACL_MDT_RES *res);

/**
 * ��ò�ѯ������еļ�¼��
 * @param res {ACL_MDT_RES*} ���ݽ������
 * @return {int} 0: �����Ϊ��; > 0: �������Ϊ��
 */
ACL_API int acl_mdt_row_count(ACL_MDT_RES *res);

/**
 * �ͷŲ�ѯ�����̬�ڴ棬�������ͷ�ʵ�ʵ����ݽ��
 * @param res {ACL_MDT_RES*} ���ݽ������
 */
ACL_API void acl_mdt_res_free(ACL_MDT_RES *res);

/**
 * ���ݱ�������Ԫ�ص�����
 * @param mdt {ACL_MDT*} ���ݱ���
 * @return {int} >=0
 */
ACL_API int acl_mdt_cnt(ACL_MDT *mdt);

#ifdef	__cplusplus
}
#endif

#endif

