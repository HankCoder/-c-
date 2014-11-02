#ifndef ACL_BTREE_INCLUDE_H
#define ACL_BTREE_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "acl_define.h"

/**
 * �������ṹ���Ͷ���
 */
typedef struct ACL_BTREE ACL_BTREE;

/**
 * ����һ������������
 * @return {ACL_BTREE*} �´����Ķ���������
 */
ACL_API ACL_BTREE *acl_btree_create(void);

/**
 * �ͷ�һ������������
 * @param tree {ACL_BTREE*} ����������
 * @return {int} 0: �ɹ�; -1: ʧ��
 */
ACL_API int acl_btree_destroy(ACL_BTREE *tree);

/**
 * �Ӷ������в�ѯ
 * @param tree {ACL_BTREE*} ����������
 * @param key {unsigned int} ��ѯ��
 * @return {void*} ��ѯ���
 */
ACL_API void *acl_btree_find(ACL_BTREE *tree, unsigned int key);

/**
 * ������������
 * @param tree {ACL_BTREE*} ����������
 * @param key {unsigned int} ��
 * @param data {void*} ��̬����
 */
ACL_API int acl_btree_add(ACL_BTREE *tree, unsigned int key, void *data);

/**
 * �Ӷ�������ɾ��
 * @param tree {ACL_BTREE*} ����������
 * @param key {unsigned int} ��
 * @return {void*} ��ɾ���Ķ�̬�����ַ, ����������򷵻�NULL
 */
ACL_API void *acl_btree_remove(ACL_BTREE *tree, unsigned int key);

/**
 * ���ض���������С�ļ�
 * @param tree {ACL_BTREE*} ����������
 * @param key {unsigned int*} ��ָ�룬�洢���������Ϊ��
 * @return {int} 0: ��ʾ�ҵ���С��; -1: ��ʾ�����δ�ҵ���С��
 */
ACL_API int acl_btree_get_min_key(ACL_BTREE *tree, unsigned int *key);

/**
 * ���ض����������ļ�
 * @param tree {ACL_BTREE*} ����������
 * @param key {unsigned int*} ��ָ�룬�洢���������Ϊ��
 * @return {int} 0: ��ʾ�ҵ�����; -1: ��ʾ�����δ�ҵ�����
 */
ACL_API int acl_btree_get_max_key(ACL_BTREE *tree, unsigned int *key);

/**
 * �ɸ��������������ڶ������е���һ���ڽ���
 * @param tree {ACL_BTREE*} ����������
 * @param cur_key {unsigned int} ��ǰ������
 * @param next_key {unsigned int*} �洢�������ָ���ַ
 * @return {int} 0: ��ʾ�ҵ�; -1: ��ʾ�����δ�ҵ�
 */
ACL_API int acl_btree_get_next_key(ACL_BTREE *tree,
	unsigned int cur_key, unsigned int *next_key);

/**
 * ���㵱ǰ�����������
 * @param tree {ACL_BTREE*} ����������
 * @return {int} �����������
 */
ACL_API int acl_btree_depth(ACL_BTREE *tree);
ACL_API void acl_btree_dump(ACL_BTREE *b);

#ifdef __cplusplus
}
#endif

#endif

