
#ifndef _DEBUG_HTABLE_H_INCLUDED_
#define _DEBUG_HTABLE_H_INCLUDED_

#ifdef  __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"

/*--------------------------------------------------------------------------*/

/* Structure of one hash table entry. */

/**
 * ��ϣ�������Ͷ���
 * @param buffer ��Ҫ����ϣ���ַ���
 * @param len s �ĳ���
 */
typedef unsigned (*DEBUG_HASH_FN)(const void *buffer, size_t len);

/**
 * ��ϣ�����ṹ���, �����Ͷ����� htable.c ����Ϊ�˱����ڲ���Ա����
 */
typedef struct DEBUG_HTABLE	DEBUG_HTABLE;

/* ��ϣ����ÿһ����ϣ��Ĵ洢��Ϣ���� */
typedef struct DEBUG_HTABLE_INFO {
	char   *key;			/* lookup key */
	char   *value;			/* associated value */
	struct DEBUG_HTABLE_INFO *next;	/* colliding entry */
	struct DEBUG_HTABLE_INFO *prev;	/* colliding entry */
} DEBUG_HTABLE_INFO;

/**
 * ������ϣ��
 * @param size ��ϣ����
 * @return ������ϣ���ͷָ���Ϊ��(��ʱ��ʾ�������صĴ���, ��Ҫ���ڴ��������)
 */
DEBUG_HTABLE *debug_htable_create(int size);

/**
 * ����ϣ��������µ���
 * @param table ��ϣ��ָ��
 * @param key ��, �ں����ڲ��Ḵ�ƴ� key ��
 * @param value �û��Լ����ض�������(����������Ӳת������, ���Ǵ���������벻�ܶ�ջ����)
 * @return ������Ĺ�ϣ�����ָ��, == NULL: ��ʾ�ڲ��ַ����ڴ����, Ϊ���صĴ���
 */
DEBUG_HTABLE_INFO *debug_htable_enter(DEBUG_HTABLE *table, const char *key, char *value);

/**
 * �������� key ����Ѱĳһ�ض���ϣ��
 * @param table ��ϣ��ָ��
 * @param key ��
 * @return ��Ϊ��ָ��: ��ʾ�鵽�˶�Ӧ�� key ���Ĺ�ϣ��
 *         Ϊ��: ��ʾδ�鵽��Ӧ�� key ���Ĺ�ϣ��
 */
DEBUG_HTABLE_INFO *debug_htable_locate(DEBUG_HTABLE *table, const char *key);

/**
 * �������� key ����Ѱ�û���������
 * @param table ��ϣ��ָ��
 * @param key ��
 * @return ��Ϊ��: ��ʾ�鵽�˶�Ӧ�� key ����������, �û����Ը����û��Լ����������ͽ���ת��
 *         Ϊ��: ��ʾδ�鵽��Ӧ�� key ����������
 */
char *debug_htable_find(DEBUG_HTABLE *table, const char *key);

/**
 * ����:		���������� key ��ɾ��ĳһ��ϣ��
 * @param table ��ϣ��ָ��
 * @param key ��
 * @param free_fn ����ú���ָ�벻Ϊ�ղ����ҵ��˶�Ӧ�� key ����������, ���ȵ����û�
 *        ���ṩ������������һЩ��β����, Ȼ�����ͷŸù�ϣ��
 * @return 0: �ɹ�;  -1: δ�ҵ��� key ��
 */
int debug_htable_delete(DEBUG_HTABLE *table, const char *key, void (*free_fn) (char *));

/**
 * �ͷ�������ϣ��
 * @param table ��ϣ��ָ��
 * @param free_fn �����ָ�벻Ϊ����Թ�ϣ���е�ÿһ���ϣ�����øú�������β����, Ȼ�����ͷ�
 */
void debug_htable_free(DEBUG_HTABLE *table, void (*free_fn) (char *));

/**
 * ���ù�ϣ��, �ú������ͷŹ�ϣ���е�����������, �����³�ʼ��
 * @param table ��ϣ��ָ��
 * @param free_fn �����ָ�벻Ϊ����Թ�ϣ���е�ÿһ���ϣ�����øú�������β����, Ȼ�����ͷ�
 * @return �Ƿ����óɹ�. 0: OK; < 0: error.
 */
int debug_htable_reset(DEBUG_HTABLE *table, void (*free_fn) (char *));

/**
 * �Թ�ϣ���е�ÿһ���ϣ����д���
 * @param table ��ϣ��ָ��
 * @param walk_fn ����ÿһ���ϣ��ĺ���ָ��, ����Ϊ��
 * @param arg �û��Լ����͵�����
 */
void debug_htable_walk(DEBUG_HTABLE *table, void (*walk_fn) (DEBUG_HTABLE_INFO *, char *), char *arg);

/**
 * ����ϣ�������������ϳ�һ������
 * @param table ��ϣ��
 * @return ��Ϊ��: ����ָ��; Ϊ��: ��ʾ�ù�ϣ����û�й�ϣ��
 */
DEBUG_HTABLE_INFO **debug_htable_list(const DEBUG_HTABLE *table);

#ifdef  __cplusplus
}
#endif

#endif

