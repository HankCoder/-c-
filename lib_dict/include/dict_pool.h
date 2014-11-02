#ifndef	__DICT_POOL_INCLUDE_H__
#define	__DICT_POOL_INCLUDE_H__

#include "lib_acl.h"
#include "dict.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct DICT_POOL DICT_POOL;

typedef struct DICT_POOL_DB DICT_POOL_DB;

/**
 * ��ʼ�������ܱ�����һ��
 */
DICT_API void dict_pool_init(void);

/**
 * ����һ���洢��
 * @param partions {const char*} �洢����������ַ�������
 * @param partions_size {int} partions �����ĸ���
 * @param dict_type {const char*} �洢���� (btree/hash/cdb)
 * @param dict_name {const char*} �洢����
 * @param dict_size {int} �ô洢�ر���Ϊ�����洢����
 * @return {DICT_POOL*} �洢�صĶ���ָ��, ��Ϊ NULL ���ʾ����ʧ��
 */
DICT_API DICT_POOL *dict_pool_new(const char **partions, int partions_size,
	const char *dict_type, const char *dict_path,
	const char *dict_name, int pool_size);

/**
 * �رղ��ͷ�һ���洢��
 * @param pool {DICT_POOL*} ĳ���洢�صĶ���ָ��
 */
DICT_API void dict_pool_free(DICT_POOL *pool);

/**
 * ���һ�� key/value ����һ���洢��
 * @param pool {DICT_POOL*} ĳ���洢�صĶ���ָ��
 * @param key {char*} ��ֵ��ַ
 * @param key_len {size_t} key �ĳ���
 * @param value {char*} ���ݵ�ַ
 * @param len {size_t} value ���ݵĳ��ȴ�С(�ֽ�)
 * @return {int} 0: OK, < 0: Error
 */
DICT_API int  dict_pool_set(DICT_POOL *pool, char *key, size_t key_len, char *value, size_t len);

/**
 * ��һ���洢���ж�ȡ����
 * @param pool {DICT_POOL*} ĳ���洢�صĶ���ָ��
 * @param key {char*} ��ֵ��ַ
 * @param key_len {size_t} key �ĳ���
 * @param size {size_t*} �洢���������ݶ���ĳ��ȴ�С(�ֽ�)
 * @param {char*} �����������ݵ�ַ����Ϊ��ʱ���ʾ�ö��󲻴���,
 * ע��������ص����ݵ�ַ��Ϊ�գ����������������÷������ݶ����
 *     �� acl_myfree/1 �ͷŵ��÷��صĶ�̬�ڴ��ַ
 */
DICT_API char *dict_pool_get(DICT_POOL *pool, char *key, size_t key_len, size_t *size);

/**
 * ��һ���洢����ɾ������
 * @param pool {DICT_POOL*} ĳ���洢�صĶ���ָ��
 * @param key {char*} ��ֵ��ַ
 * @param key_len {size_t} key �ĳ���
 * @return {int} 0: ok; != 0: error
 */
DICT_API int  dict_pool_del(DICT_POOL *pool, char *key, size_t key_len);

/**
 * �����洢�е���������
 * @param pool {DICT_POOL*} ĳ���洢�صĶ���ָ��
 * @param key {char**} �������ǿ���洢��ֵ���, �������ֵ��Ϊ�գ�
 *  ���ͷ� key ��Ҫ����: acl_myfree/1, ���������ڴ�й¶
 * @param key_size {size_t*} ������ǿ���洢��ֵ����ĳ���
 * @param val {char**} �������ǿ���洢���ݽ��, �������ֵ��Ϊ�գ�
 *  ���ͷ� val ��Ҫ����: acl_myfree/1, ���� ������ڴ�й¶
 * @param val_size {size_t*} �������ǿ���洢���ݽ���ĳ���
 * @return {int} 0: ��ʾ����ǿ�, != 0: ��ʾ���Ϊ��
 */
DICT_API int dict_pool_seq(DICT_POOL *pool, char **key, size_t *key_size,
        char **val, size_t *val_size);

/**
 * ���ô洢��������
 * @param pool {DICT_POOL*} ĳ���洢�صĶ���ָ��
 */
DICT_API void dict_pool_seq_reset(DICT_POOL *pool);

/**
 * ɾ����ǰ���ݱ��������ָ������
 * @param pool {DICT_POOL*} ĳ���洢�صĶ���ָ��
 * @return {int} 0: ɾ��OK, != 0: �����ڻ�ɾ��ʧ��
 */
DICT_API int dict_pool_seq_delcur(DICT_POOL *pool);

/**
 * ���ݽ�ֵ�Ӵ洢���л�øý����ڵĴ洢DB
 * @param pool {DICT_POOL*} ĳ���洢�صĶ���ָ��
 * @param key {char*} ��ֵ��ַ
 * @param key_len {size_t} key �ĳ���
 * @return {DICT_POOL_DB*} �洢DB
 */
DICT_API DICT_POOL_DB *dict_pool_db(DICT_POOL *pool, const char *key, size_t key_len);

/**
 * ���ĳ�洢DB���ļ�ȫ·��
 * @param pool {DICT_POOL*} ĳ���洢�صĶ���ָ��
 * @return {const char*} �洢�ļ���
 */
DICT_API const char *dict_pool_db_path(DICT_POOL_DB *db);

/**
 * ����ĳ���洢DB
 * @param {DICT_POOL_DB*} �洢DB
 */
DICT_API void dict_pool_db_lock(DICT_POOL_DB *db);

/**
 * ����ĳ���洢DB
 * @param {DICT_POOL_DB*} �洢DB
 */
DICT_API void dict_pool_db_unlock(DICT_POOL_DB *db);

/**
 * ��ӻ��޸�����
 * @param {DICT_POOL_DB*} �洢DB
 * @param key {char*} ��ֵ��ַ
 * @param key_len {size_t} key �ĳ���
 * @param value {char*} ֵ
 * @param len {size_t} value ����
 * @return {int} 0: ok; < 0: error
 */
DICT_API int  dict_pool_db_set(DICT_POOL_DB *db, char *key, size_t key_len, char *value, size_t len);

/**
 * �Ӵ洢DB�л��ĳ��������Ӧ��ֵ
 * @param {DICT_POOL_DB*} �洢DB
 * @param key {char*} ��ֵ��ַ
 * @param key_len {size_t} key �ĳ���
 * @param size {size_t*} �����ѯ����ǿ���洢���ؽ���ĳ���
 * @return {char*} ������Ӧ��ֵ, ��Ҫ�� acl_myfree()/1 �ͷ�
 */
DICT_API char *dict_pool_db_get(DICT_POOL_DB *db, char *key, size_t key_len, size_t *size);

/**
 * �Ӵ洢DB��ɾ��ĳ������Ӧ��ֵ
 * @param {DICT_POOL_DB*} �洢DB
 * @param key {char*} ��ֵ��ַ
 * @param key_len {size_t} key �ĳ���
 * @return {int} 0: ok; != 0: error
 */
DICT_API int dict_pool_db_del(DICT_POOL_DB *db, char *key, size_t key_len);

/**
 * ����֮ǰ�������Ĵ洢DB
 * @param {DICT_POOL_DB*} �洢DB
 */
DICT_API void dict_pool_db_seq_reset(DICT_POOL_DB *db);

/**
 * ����ĳ���洢DB
 * @param {DICT_POOL_DB*} �洢DB
 * @param key {char**} �洢���ĵ�ַ
 * @param key_size {size_t*} �洢�� key �����ݵĳ���
 * @param value {char**} �洢ֵ�ĵ�ַ
 * @param value_size {size_t*} �洢�� value �����ݵĳ���
 * @return {int} 0: ��ʾ����ǿ�, != 0: ��ʾ���Ϊ��
 */
DICT_API int dict_pool_db_seq(DICT_POOL_DB *db, char **key, size_t *key_size,
        char **val, size_t *val_size);

#ifdef	__cplusplus
}
#endif

#endif
