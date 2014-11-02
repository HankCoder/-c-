#ifndef __BINHASH_H_INCLUDED_H__
#define __BINHASH_H_INCLUDED_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include "private.h"

typedef struct BINHASH BINHASH;

/**
 * Structure of one hash table entry.
 */
#ifdef  PACK_STRUCT
#pragma pack(4)
#endif
typedef struct BINHASH_INFO {
	union {
		char *key;
		const char *c_key;
	} key;                      /**< lookup key */
	int     key_len;            /**< key length */
	char   *value;              /**< associated value */
	struct BINHASH_INFO *next;  /**< colliding entry */
	struct BINHASH_INFO *prev;  /**< colliding entry */
} BINHASH_INFO;
#ifdef  PACK_STRUCT
#pragma pack(0)
#endif

/**
 * ����һ����ϣ��
 * @param size {int} ��ϣ��ĳ�ʼ����С
 * @param flag {unsigned int} �� ACL_MDT_IDX �е� flag ��ͬ
 * @return {BINHASH*} �´����Ĺ�ϣ��ָ��
 */
BINHASH *binhash_create(int size, unsigned int flag, int use_slice);

/**
 * ���ϣ������Ӷ���
 * @param table {BINHASH*} ��ϣ��ָ��
 * @param key {const char*} ��ϣ��
 * @param key_len {int} key �ĳ���
 * @param value {char*} ��ֵ
 * @return {BINHASH_INFO*} �´����Ĺ�ϣ��Ŀָ��
 */
BINHASH_INFO *binhash_enter(BINHASH *table, const char *key, int key_len, char *value);

/**
 * �ӹ�ϣ���и��ݼ���ȡ�ö�Ӧ�Ĺ�ϣ��Ŀ
 * @param table {BINHASH*} ��ϣ��ָ��
 * @param key {const char*} ��ϣ��
 * @param key_len {int} key �ĳ���
 * @return {BINHASH_INFO*} ��ϣ��Ŀָ��
 */
BINHASH_INFO *binhash_locate(BINHASH *table, const char *key, int key_len);

/**
 * ��ѯĳ����ϣ���ļ�ֵ
 * @param table {BINHASH*} ��ϣ��ָ��
 * @param key {const char*} ��ϣ��
 * @param key_len {int} key �ĳ���
 * @return {char*} ��ϣ��ֵ
 */
char *binhash_find(BINHASH *table, const char *key, int key_len);

/**
 * ɾ��ĳ����ϣ��
 * @param table {BINHASH*} ��ϣ��ָ��
 * @param key {const char*} ��ϣ��
 * @param key_len {int} key �ĳ���
 * @param free_fn {void (*)(char*)} �����ͷŹ�ϣ��ֵ�ĺ���ָ�룬���Ϊ�������ڲ��ͷż�ֵ
 */
void binhash_delete(BINHASH *table, const char *key, int key_len, void (*free_fn) (char *));

/**
 * �ͷŹ�ϣ��
 * @param table {BINHASH*} ��ϣ��ָ��
 * @param free_fn {void (*)(char*)} �����Ϊ�գ����ô˺������ͷŹ�ϣ���ڵ����м�ֵ
 */
void binhash_free(BINHASH *table, void (*free_fn) (char *));

/**
 * ����������ϣ�������û������Ļص�����������ϣ���еļ�ֵ
 * @param table {BINHASH*} ��ϣ��ָ��
 * @param walk_fn {void (*)(BINHASH_INFO*, char*)} �ڱ�����ϣ���е�ÿ��Ԫ��ʱ�Ļص�����
 * @param arg {char*} �û����ݵĲ�������Ϊ������ walk_fn �д���
 */
void binhash_walk(BINHASH *table, void (*walk_fn) (BINHASH_INFO *, char *), char *arg);

/**
 * �г���ǰ��ϣ���е�����Ԫ�������б�
 * @param table {BINHASH*} ��ϣ��ָ��
 * @return {BINHASH_INFO*} ��ϣ��������Ԫ����ɵ�BINHASH_INFO����, 
 *  �������е����һ��ָ��Ϊ NULL
 */
BINHASH_INFO **binhash_list(BINHASH *table);

#ifdef  __cplusplus
}
#endif

#endif

