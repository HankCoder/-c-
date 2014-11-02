
#ifndef __HTABLE_H_INCLUDED__
#define __HTABLE_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include "private.h"

#define	HAS_VALUE

/*--------------------------------------------------------------------------*/
/**
 * ��ϣ�������Ͷ���
 * @param buffer ��Ҫ����ϣ���ַ���
 * @param len s �ĳ���
 */
typedef unsigned (*HASH_FN)(const void *buffer, size_t len);

/**
 * ��ϣ�����ṹ���, �����Ͷ����� htable.c ����Ϊ�˱����ڲ���Ա����
 */
typedef struct HTABLE	HTABLE;

/**
 * ��ϣ����ÿһ����ϣ��Ĵ洢��Ϣ����
 */
#ifdef  PACK_STRUCT
#pragma pack(4)
#endif
typedef struct HTABLE_INFO {
	union {
		char   *key;
		const char *c_key;
	} key;				/**< lookup key */
#ifdef	HAS_VALUE
	char   *value;			/**< associated value */
#endif
	struct HTABLE_INFO *next;	/**< colliding entry */
	struct HTABLE_INFO *prev;	/**< colliding entry */
} HTABLE_INFO;
#ifdef  PACK_STRUCT
#pragma pack(0)
#endif

/**
 * ������ϣ��
 * @param size ��ϣ����
 * @param flag �� ACL_MDT_IDX �е� flag ��ͬ
 * @return ������ϣ���ͷָ���Ϊ��(��ʱ��ʾ�������صĴ���, ��Ҫ���ڴ��������)
 */
HTABLE *htable_create(int size, unsigned int flag, int use_slice);

/**
 * ���ù�ϣ��Ŀ��Ʋ���
 * @param table ��ϣ�������
 * @param name ���Ʋ����ı�γ�ʼֵ, name ���Ժ�Ŀ��Ʋ������¶���
 *  HTABLE_CTL_END: ��α������־
 *  HTABLE_CTL_RWLOCK: �Ƿ����ö�д������
 *  HTABLE_CTL_HASH_FN: �û��Զ���Ĺ�ϣֵ���㺯��
 */
void htable_ctl(HTABLE *table, int name, ...);
#define	HTABLE_CTL_END      0  /**< ���ƽ�����־ */
#define	HTABLE_CTL_HASH_FN  2  /**< ����˽�й�ϣ���� */

/**
 * �����һ�ι�ϣ��������ϣ���״̬
 * @param table ��ϣ��ָ��
 * @return {int} ������ϣ����״̬, �μ����µ� HTABLE_STAT_XXX
 */
int htable_last_errno(HTABLE *table);
#define	HTABLE_STAT_OK          0  /**< ״̬���� */
#define	HTABLE_STAT_INVAL       1  /**< ��Ч���� */
#define	HTABLE_STAT_DUPLEX_KEY  2  /**< �ظ��� */

/**
 * ���ù�ϣ��ĵ�ǰ״̬, error ȡֵ HTABLE_STAT_XXX
 * @param table ��ϣ��ָ��
 * @param error ���ù�ϣ��Ĵ���״̬
 */
void htable_set_errno(HTABLE *table, int error);

/**
 * ����ϣ��������µ���
 * @param table ��ϣ��ָ��
 * @param key ��, �ں����ڲ��Ḵ�ƴ� key ��
 * @param value �û��Լ����ض�������(����������Ӳת������, ���Ǵ���������벻�ܶ�ջ����)
 * @return ������Ĺ�ϣ�����ָ��, == NULL: ��ʾ�ڲ��ַ����ڴ����, Ϊ���صĴ���
 *  ע����������ʱ�ù�ϣ�������ڣ��򷵻��Ѿ����ڵĹ�ϣ�ʹ����Ӧ��ͨ������
 *  htable_last_errno() ���鿴�Ƿ��ظ����ͬһ����ֵ(HTABLE_STAT_DUPLEX_KEY)
 */
HTABLE_INFO *htable_enter(HTABLE *table, const char *key, char *value);

/**
 * �������� key ����Ѱĳһ�ض���ϣ��
 * @param table ��ϣ��ָ��
 * @param key ��
 * @return ��Ϊ��ָ��: ��ʾ�鵽�˶�Ӧ�� key ���Ĺ�ϣ��
 *         Ϊ��: ��ʾδ�鵽��Ӧ�� key ���Ĺ�ϣ��
 */
HTABLE_INFO *htable_locate(HTABLE *table, const char *key);

/**
 * �������� key ����Ѱ�û���������
 * @param table ��ϣ��ָ��
 * @param key ��
 * @return ��Ϊ��: ��ʾ�鵽�˶�Ӧ�� key ����������, �û����Ը����û��Լ���
 *  �������ͽ���ת��; Ϊ��: ��ʾδ�鵽��Ӧ�� key ����������
 */
char *htable_find(HTABLE *table, const char *key);

/**
 * ���������� key ��ɾ��ĳһ��ϣ��
 * @param table ��ϣ��ָ��
 * @param key ��
 * @param free_fn ����ú���ָ�벻Ϊ�ղ����ҵ��˶�Ӧ�� key ����������, ���ȵ����û�
 *        ���ṩ������������һЩ��β����, Ȼ�����ͷŸù�ϣ��
 * @return 0: �ɹ�;  -1: δ�ҵ��� key ��
 */
int htable_delete(HTABLE *table, const char *key, void (*free_fn) (char *));

/**
 * �ͷ�������ϣ��
 * @param table ��ϣ��ָ��
 * @param free_fn �����ָ�벻Ϊ����Թ�ϣ���е�ÿһ���ϣ�����øú�������β����, Ȼ�����ͷ�
 */
void htable_free(HTABLE *table, void (*free_fn) (char *));

/**
 * �Թ�ϣ���е�ÿһ���ϣ����д���
 * @param table ��ϣ��ָ��
 * @param walk_fn ����ÿһ���ϣ��ĺ���ָ��, ����Ϊ��
 * @param arg �û��Լ����͵�����
 */
void htable_walk(HTABLE *table, void (*walk_fn) (HTABLE_INFO *, char *), char *arg);

/**
 * ���ع�ϣ��ǰ�������ռ��С(��ֵӦ���ڹ�ϣ����Ԫ�ظ���)
 * @param table ��ϣ��ָ��
 * @return ��ϣ��������ռ��С
 */
int htable_capacity(const HTABLE *table);

/**
 * ���ع�ϣ��ǰ�Ĵ���������Ԫ�ظ���
 * @param table ��ϣ��ָ��
 * @return ��ϣ����Ԫ�ظ���
 */
int htable_used(const HTABLE *table);

/**
 * ����ϣ�������������ϳ�һ������
 * @param table ��ϣ��
 * @return ��Ϊ��: ����ָ��; Ϊ��: ��ʾ�ù�ϣ����û�й�ϣ��
 */
HTABLE_INFO **htable_list(const HTABLE *table);

void htable_list_free(HTABLE_INFO ** list);

/**
 * ��ʾ��ϣ���� key ���ķֲ�״̬
 * @param table ��ϣ��ָ��
 */
void htable_stat(const HTABLE *table);

#ifdef  __cplusplus
}
#endif

#endif

