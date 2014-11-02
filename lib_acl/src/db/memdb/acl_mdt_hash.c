#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_msg.h"
#include "stdlib/acl_mystring.h"

#endif

#include "struct.h"
#include "mdb_private.h"

/**
 * ��������
 */
static ACL_MDT_IDX *mdt_idx_create(ACL_MDT *mdt acl_unused, size_t init_capacity,
	const char *name, unsigned int flag)
{
	ACL_MDT_IDX_HASH *idx;
	unsigned int flag2 = 0;

	if (init_capacity < 128)
		init_capacity = 128;
	idx = (ACL_MDT_IDX_HASH*) acl_mycalloc(1, sizeof(ACL_MDT_IDX_HASH));

	if ((flag & ACL_MDT_FLAG_KMR))
		flag2 |= ACL_HTABLE_FLAG_KEY_REUSE;

	idx->table = acl_htable_create(init_capacity, flag2);
	idx->idx.name = acl_mystrdup(name);
	idx->idx.flag = flag;
	return ((ACL_MDT_IDX*) idx);
}

static void mdt_idx_free(ACL_MDT_IDX *idx)
{
	ACL_MDT_IDX_HASH *idx_hash = (ACL_MDT_IDX_HASH*) idx;

	acl_htable_free(idx_hash->table, NULL);
	acl_myfree(idx->name);
	acl_myfree(idx_hash);
}

/**
 * ��һ��������������µ��ֶ�
 * @param idx {ACL_MDT_IDX*} ������
 * @param key {const char*} ���ݱ������ֶ�ֵ
 * @param rec {ACL_MDT_REC*}
 * @return {ACL_HTABLE_INFO*}
 */
static void mdt_idx_add(ACL_MDT_IDX *idx, const char *key, ACL_MDT_REC *rec)
{
	const char *myname = "mdt_idx_add";
	ACL_MDT_IDX_HASH *idx_hash = (ACL_MDT_IDX_HASH*) idx;
	ACL_HTABLE_INFO *info;

	info = acl_htable_enter(idx_hash->table, key, (char*) rec);
	if (info == NULL)
		acl_msg_fatal("%s(%d): acl_htable_enter error, value(%s)",
			myname, __LINE__, key);
	else
		rec->key = info->key.c_key;
}

/**
 * �����ݱ�������в�ѯ��Ӧĳ��������ֵ�Ľ����
 * @param idx {ACL_MDT_IDX*} ������
 * @param key {const char*} ���ݱ������ֶ�ֵ
 * @return {ACL_MDT_REC*} ��Ӧĳ�������ֶ�ֵ�Ľ������
 */
static ACL_MDT_REC *mdt_idx_get(ACL_MDT_IDX *idx, const char *key)
{
	ACL_MDT_IDX_HASH *idx_hash = (ACL_MDT_IDX_HASH*) idx;
	ACL_MDT_REC *rec;

	rec = (ACL_MDT_REC*) acl_htable_find(idx_hash->table, key);
	return (rec);
}

/**
 * ��һ����������ɾ��������
 * @param idx {ACL_MDT_IDX*} ������
 * @param rec {ACL_MDT_REC*} �������ж�Ӧĳ�����Ľ��������
 * @param key_value {const char*} ���ݽ������ý������ü�ֵ
 */
static void mdt_idx_del(ACL_MDT_IDX *idx, const char *key)
{
	ACL_MDT_IDX_HASH *idx_hash = (ACL_MDT_IDX_HASH*) idx;

	/* idx->table ��ϣ����洢����: rec->key: rec �ԣ����Բ���Ҫ��
	 * ��ϣ���ڲ��ͷ� rec �ڴ棬��Ϊ������ʾ���ͷŸ���Դ
	 */
	acl_htable_delete(idx_hash->table, key, NULL);
}

/**
 * �ͷŹ�ϣģʽ�����ݱ�
 */
static void mdt_hash_free(ACL_MDT *mdt)
{
	ACL_MDT_HASH *mdt_hash = (ACL_MDT_HASH*) mdt;

	acl_myfree(mdt_hash);
}

ACL_MDT *acl_mdt_hash_create()
{
	ACL_MDT_HASH *mdt;

	mdt = (ACL_MDT_HASH *) acl_mycalloc(1, sizeof(ACL_MDT_HASH));
	mdt->mdt.tbl_free = mdt_hash_free;
	mdt->mdt.idx_create = mdt_idx_create;
	mdt->mdt.idx_free = mdt_idx_free;
	mdt->mdt.idx_add = mdt_idx_add;
	mdt->mdt.idx_get = mdt_idx_get;
	mdt->mdt.idx_del = mdt_idx_del;
	return ((ACL_MDT*) mdt);
}
