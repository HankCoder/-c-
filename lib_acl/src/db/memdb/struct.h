#ifndef __MESTRUCT_INCLUDE_H__
#define __MESTRUCT_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include "stdlib/acl_array.h"
#include "stdlib/acl_htable.h"
#include "db/acl_mdb.h"
#include "stdlib/avl.h"
#include "stdlib/acl_slice.h"
#include "stdlib/acl_htable.h"
#include "stdlib/acl_binhash.h"
#include "ring.h"

/* �û���ѯ������� */
struct ACL_MDT_RES {
	ACL_ARRAY *a;			/* �� ACL_MDT_NOD �Ĳ�ѯ������ϴ洢�ڴ������� */
	int   ipos;			/* ��һ������Ԫ���ڶ�̬�����е�λ�� */
};

/* ���ݽ������ݽṹ����(24 byte) */
struct ACL_MDT_NOD {
	RING mdt_entry;			/* ���ӽ� ACL_MDT �е� node_head */
	RING ref_head;			/* ���ø����ݽ����������ý��ļ��� */
	void  *data;			/* �û�����������ڲ���ʾ, ����Ϊ�����û�
					 * ���Զ������������, ͨ��ǿ������ת������
					 * ͳһת��Ϊ void * ����, �ڽ�������ʱ�û�
					 * ��Ҫ���Զ������������е�ĳЩ�ֶ�Ϊ��ֵ
					 * ������������¼��������, �Ӷ������������ݿ�,
					 * ���ո� data �ֶ��еļ����� ACL_MDT_REC �е�
					 * key ��ͬ��ӳ�䵽 ACL_MDT_REC �е� table
					 * ������.
					 */

	unsigned int dlen:24;		/* data ���ݵĳ��� */
	unsigned int nrefer:8;		/* ���ø����ݽ��� ACL_MDT_REF ���� */
};

/* �������������¼���ݽṹ����, ������(ACL_MDT)�����������ֶ�(table)
 * ����ɵ�λΪ ACL_REC, �� ACL_MDT.table ����һϵ�� ACL_REC
 * ����������¼��ɵ�.
 * (12 byte)
 */
struct ACL_MDT_REC {
	RING ref_head;			/* ������ͬ��ֵ�� ACL_MDT_NOD_REF �ļ��� */
	ACL_MDT_IDX *idx;		/* ������������������ */
	const char *key;		/* ���������� */
	unsigned int   nrefer:8;	/* �ý�����ϵ� ACL_MDT_NODE_REF Ԫ���ܺ� */
};

/**
 * ���ý�㶨��(20 byte)
 */
struct ACL_MDT_REF {
	RING nod_entry;			/* �� ACL_MDT_NOD ���� */
	RING rec_entry;			/* �� ACL_MDT_REC ���� */
	ACL_MDT_NOD *node;		/* ���� ACL_MDT_NOD */
	ACL_MDT_REC *rec;		/* ���� ACL_MDT_REC */
};

/**
 * ��������ֶζ�������, ��������ÿ���������һ�������ֶζ���
 */
struct ACL_MDT_IDX {
	RING mdt_entry;			/* ���ӽ� ACL_MDT �е� idx_head ���� */
	char *name;			/* �ֶ����� */
	ACL_MDT *mdt;			/* ������������������������ */
	unsigned int flag;		/* �����ݱ��Լ����־ */
};

/**
 * �������Թ�ϣ��ʽ�洢
 */
typedef struct ACL_MDT_IDX_HASH {
	ACL_MDT_IDX idx;
	ACL_HTABLE *table;		/* ������������������ʶ���ݽ�����ļ��� */
} ACL_MDT_IDX_HASH;

/**
 * �������Թ�ϣ��ʽ�洢
 */
typedef struct ACL_MDT_IDX_BHASH {
	ACL_MDT_IDX idx;
	ACL_BINHASH *table;		/* ������������������ʶ���ݽ�����ļ��� */
} ACL_MDT_IDX_BHASH;

/**
 * ��������ƽ�ȶ�������ʽ�洢
 */
typedef struct ACL_MDT_IDX_AVL {
	ACL_MDT_IDX idx;
	avl_tree_t avl;
	ACL_SLICE *slice;		/* �ڴ����� */
} ACL_MDT_IDX_AVL;

/* ���������ݽṹ����, ÿ���������ڽ���ʱ���������ݽ��(ACL_NODE)�е�
 * �û�����(data)�е�ĳ�������ֶ�Ϊ��ֵ������.
 */
struct ACL_MDT {
	RING idx_head;			/* �����ֶζ������� */
	RING nod_head;			/* �������ݽ��ļ��� */

	char *name;			/* ���������� */
	int   node_cnt;			/* �������ݽ����ܺ� */
	int   error;			/* ������ʱ��¼����״̬�� */
	unsigned int tbl_flag;		/* ��־λ */

	ACL_SLICE *nod_slice;
	ACL_SLICE *rec_slice;
	ACL_SLICE *ref_slice;

	/* public */
	ACL_MDT_NOD *(*add)(ACL_MDT *mdt, void *data, unsigned int dlen,
	        const char *key_labels[], const char *keys[]);
	ACL_MDT_RES *(*get)(ACL_MDT *mdt, const char *key_label,
	        const char *key, int from, int limit);
	int (*del)(ACL_MDT *mdt, const char *key_label,
	        const char *key, void (*onfree_fn)(void *data, unsigned int dlen));
	int (*probe)(ACL_MDT *mdt, const char *key_label, const char *key);
	ACL_MDT_RES *(*list)(ACL_MDT *mdt, int from, int limit);
	int (*walk)(ACL_MDT *mdt, int (*walk_fn)(const void *data, unsigned int dlen),
		int from, int limit);

	void (*tbl_free)(ACL_MDT*);

	/* private */
	ACL_MDT_IDX *(*idx_create)(ACL_MDT *mdt, size_t init_capacity,
		const char *name, unsigned int flag);
	void (*idx_free)(ACL_MDT_IDX *idx);
	void (*idx_add)(ACL_MDT_IDX *idx, const char *key, ACL_MDT_REC *rec);
	void (*idx_del)(ACL_MDT_IDX *idx, const char *key);
	ACL_MDT_REC *(*idx_get)(ACL_MDT_IDX *idx, const char *key);
};

typedef struct ACL_MDT_HASH {
	ACL_MDT mdt;
} ACL_MDT_HASH;

typedef struct ACL_MDT_BHASH {
	ACL_MDT mdt;
} ACL_MDT_BHASH;

typedef struct ACL_MDT_AVL {
	ACL_MDT mdt;
} ACL_MDT_AVL;

/* �������ݿ����ݽṹ���� */
struct ACL_MDB {
	char   name[128];		/* ���ݿ����� */
	char   type[32];		/* ���ݿ�����: hash/avl */
	ACL_HTABLE *tbls;		/* ��������������ļ��� */
};

#ifdef __cplusplus
}
#endif

#endif

