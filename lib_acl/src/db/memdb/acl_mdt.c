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

#include "db/acl_mdb.h"

#endif

#include "struct.h"
#include "mdb_private.h"

/*---------------------------------------------------------------------------*/

/**
 * �����ݱ��в�ѯĳ����������
 */
static ACL_MDT_IDX *mdt_idx(const ACL_MDT *mdt, const char *key_label)
{
	RING *iter;
	ACL_MDT_IDX *idx;

	FOREACH_RING_ENTRY(iter, &mdt->idx_head) {
		idx = RING_TO_APPL(iter, ACL_MDT_IDX, mdt_entry);
		if (strcasecmp(idx->name, key_label) == 0)
			return (idx);
	}

	return (NULL);
}

/* ��������������ĸ��������ֶ� */

static  void mdt_idx_create(ACL_MDT *mdt, size_t init_capacity,
	const char *key_labels[], unsigned int idx_flags[])
{
	ACL_MDT_IDX *idx;
	const char *ptr;
	int   i = 0;

	while ((ptr = key_labels[i]) != NULL) {
		idx = mdt->idx_create(mdt, init_capacity, ptr, idx_flags[i]);
		idx->mdt = mdt;
		ring_prepend(&mdt->idx_head, &idx->mdt_entry);
		i++;
	}
}

/* �������е����� */

static void mdt_idx_free(ACL_MDT *mdt)
{
	RING *iter, *iter_tmp;
	ACL_MDT_IDX *idx;

	for (iter = ring_succ(&mdt->idx_head); iter != &mdt->idx_head;) {
		iter_tmp = ring_succ(iter);
		idx = RING_TO_APPL(iter, ACL_MDT_IDX, mdt_entry);
		mdt->idx_free(idx);
		iter = iter_tmp;
	}
}

/**
 * �ͷ�ĳ�����ݽ�㣬ͬʱ�����û����ͷŻص����ͷ��û�������
 * @param mdt {ACL_MDT*} ���ݱ���
 * @param node {ACL_MDT_NODE*} ĳ�����ݽ��ָ��
 * @param onfree_fn ���ͷ�ĳ�����ݽ��ǰ���øú���
 */
static void mdt_node_free(ACL_MDT *mdt, ACL_MDT_NOD *node,
	void (*onfree_fn)(void*, unsigned int))
{
	if (onfree_fn && node->data)
		onfree_fn(node->data, node->dlen);

	ring_detach(&node->mdt_entry);
	if ((mdt->tbl_flag & ACL_MDT_FLAG_DMR) == 0 && node->data)
		acl_myfree(node->data);
	if (mdt->nod_slice)
		acl_slice_free2(mdt->nod_slice, node);
	else
		acl_myfree(node);
	mdt->node_cnt--;
}

/**
 * ����һ���µ���������
 * @param data {void*} �û��Ķ�̬����ָ��
 * @return {ACL_MDT_NOD*} �������µ����ݽ��
 */
static ACL_MDT_NOD *mdt_node_new(ACL_MDT *mdt, void *data, unsigned int dlen)
{
	ACL_MDT_NOD *node;
	void *pdata;

	if (mdt->nod_slice)
		node = (ACL_MDT_NOD*) acl_slice_alloc(mdt->nod_slice);
	else
		node = (ACL_MDT_NOD*) acl_mymalloc(sizeof(ACL_MDT_NOD));

	if ((mdt->tbl_flag & ACL_MDT_FLAG_DMR) == 0) {
		pdata = acl_mymalloc(dlen);
		memcpy(pdata, data, dlen);
		node->data = pdata;
	} else {
		node->data = data;
	}
	node->dlen = dlen;
	node->nrefer = 0;
	ring_init(&node->ref_head);

	/* �����ݽ�����������ݽ��˫������ */
	ring_prepend(&mdt->nod_head, &node->mdt_entry);
	mdt->node_cnt++;
	return (node);
}

/**
 * ���ݼ� key ����һ���µĽ�����϶���
 */
static ACL_MDT_REC *mdt_rec_new(ACL_MDT *mdt)
{
	ACL_MDT_REC *rec;

	if (mdt->rec_slice)
		rec = (ACL_MDT_REC*) acl_slice_calloc(mdt->rec_slice);
	else
		rec = (ACL_MDT_REC*) acl_mycalloc(1, sizeof(ACL_MDT_REC));

	ring_init(&rec->ref_head);
	rec->nrefer = 0;

	return (rec);
}

/**
 * �ͷ�һ�����������
 * @param rec {ACL_MDT_REC*}
 */
static void mdt_rec_free(ACL_MDT *mdt, ACL_MDT_REC *rec)
{
	if (mdt->rec_slice)
		acl_slice_free2(mdt->rec_slice, rec);
	else
		acl_myfree(rec);
}

/**
 * �ͷ�һ�����ý�㣬�������ý�������õ����ݽ��������ϵĸ�������
 * ����Ϊ0ʱ����Զ��ͷ����ݽ���������
 * @param ref {ACL_MDT_REF*} ĳ�����ݽ������ĳ���������Ӧ�����ý��
 */
static void mdt_ref_del(ACL_MDT *mdt, ACL_MDT_REF *ref)
{
	ring_detach(&ref->nod_entry);
	ring_detach(&ref->rec_entry);
	ref->node->nrefer--;
	ref->rec->nrefer--;
	if (mdt->ref_slice)
		acl_slice_free2(mdt->ref_slice, ref);
	else
		acl_myfree(ref);
}

/**
 * ����һ���µ����ý�㲢���� �� node, rec ���й���
 * @param ref {ACL_MDT_REF*}
 * @parma node {ACL_MDT_NOD*}
 * @param rec {ACL_MDT_REC*}
 */ 
static void mdt_ref_add(ACL_MDT *mdt, ACL_MDT_NOD *node, ACL_MDT_REC *rec)
{
	ACL_MDT_REF *ref;

	if (mdt->ref_slice)
		ref = (ACL_MDT_REF *) acl_slice_calloc(mdt->ref_slice);
	else
		ref = (ACL_MDT_REF *) acl_mycalloc(1, sizeof(ACL_MDT_REF));

	/* �����ý�������ݽ����й��� */
	ref->node = node;
	node->nrefer++;
	/* �����ý���������ݽ������ý�㼯���� */
	ring_prepend(&node->ref_head, &ref->nod_entry);

	/* �����ý�������������й��� */
	ref->rec = rec;
	rec->nrefer++;  /* �����ü�����1 */
	/* �����ý�����������ϵ����ý�㼯���� */
	ring_prepend(&rec->ref_head, &ref->rec_entry);
}

/**
 * �����������ݣ�ͬʱ���������
 * @param mdt {ACL_MDT*} ��
 * @param data {void*} ����ָ��
 * @param dlen {unsigned int} data ���ݴ�С
 * @param key_lables {const char*[]} �����ؼ��ֶα�ʶ
 * @param keys {const char*[]} �����ؼ���
 * @return {ACL_MDT_NOD*} �´����ı���
 */
static ACL_MDT_NOD *mdt_add(ACL_MDT *mdt, void *data, unsigned int dlen,
	const char *key_labels[], const char *keys[])
{
	const char *myname = "mdt_add";
	ACL_MDT_IDX *idx;
	ACL_MDT_NOD *node;
	ACL_MDT_REC *rec;
	int   i;

	/* �Ƚ��и��������ֶε�Լ���Լ�� */

	node = mdt_node_new(mdt, data, dlen);  /* �������ݽ���� */

	i = 0;
	while (key_labels[i] != NULL && keys[i] != NULL) {
		idx = mdt_idx(mdt, key_labels[i]);
		if (idx == NULL) {
			acl_msg_error("%s(%d): key_label(%s) no exist in table(%s)",
				myname, __LINE__, key_labels[i], mdt->name);
			return (NULL);
		}
		/* �ӱ������л�ö�Ӧ�ڼ���������¼����� */
		rec = mdt->idx_get(idx, keys[i]);
		if (rec == NULL) {
			/* �����µļ�¼����� */
			rec = mdt_rec_new(mdt);
			/* ����������Ӧ�������� */
			rec->idx = idx;
			/* �ڱ������������µ����������� */
			mdt->idx_add(idx, keys[i], rec);
		} else if (idx->flag & ACL_MDT_FLAG_UNI) {
			acl_msg_warn("%s(%d): duplex key, key_label(%s), key(%s)",
				myname, __LINE__, key_labels[i], keys[i]);
			mdt_node_free(mdt, node, NULL);
			return (NULL);
		}

		/* �����µ����ý�㲢���������ݽ�㼰������϶�����й��� */
		mdt_ref_add(mdt, node, rec);
		i++;
	}
	return (node);
}

/**
 * ̽��ĳ�����ڱ��������Ƿ����
 * @param mdt {ACL_MDT*}
 * @param key_label {const char*}
 * @param key {const char*}
 * @return {int} 0: ������; 1: ����
 */
static int mdt_probe(ACL_MDT *mdt, const char *key_label, const char *key)
{
	const char *myname = "mdt_probe";
	ACL_MDT_REC *rec;
	ACL_MDT_IDX *idx;

	idx = mdt_idx(mdt, key_label);
	if (idx == NULL) {
		acl_msg_warn("%s: key_lable(%s) no exist in %s",
			myname, key_label, mdt->name);
		return (0);
	}

	rec = mdt->idx_get(idx, key);
	if (rec == NULL)
		return (0);
	return (1);
}

/**
 * �ڱ������и���ĳ��������ֵ��ѯ�����
 * @param mdt {ACL_MDT*}
 * @param key_label {const char*}
 * @param key {const char*}
 * @param from {int} ��ʼλ��
 * @param limit {int} ��������
 * @return {ACL_MDT_RES*} ��ѯ�����, NULL: ���Ϊ��, != NULL: ������ǿ�
 */
static ACL_MDT_RES *mdt_get(ACL_MDT *mdt, const char *key_label,
	const char *key, int from, int limit)
{
	const char *myname = "mdt_get";
	ACL_MDT_RES *res;
	ACL_MDT_REC *rec;
	ACL_MDT_REF *ref;
	ACL_MDT_IDX *idx;
	RING *ref_iter;
	void *data;
	int   i, n;

	idx = mdt_idx(mdt, key_label);
	if (idx == NULL) {
		acl_msg_warn("%s: key_lable(%s) no exist in %s",
			myname, key_label, mdt->name);
		return (NULL);
	}

	rec = mdt->idx_get(idx, key);
	if (rec == NULL)
		return (NULL);

	res = (ACL_MDT_RES*) acl_mycalloc(1, sizeof(ACL_MDT_RES));
	res->a = acl_array_create(100);
	res->ipos = 0;

	i = 0;
	n = 0;
	FOREACH_RING_FORWARD(ref_iter, &rec->ref_head) {
		if (from >= 0 && i++ < from)
			continue;
		ref = RING_TO_APPL(ref_iter, ACL_MDT_REF, rec_entry);
		data = acl_mymalloc(ref->node->dlen);
		memcpy(data, ref->node->data, ref->node->dlen);
		if (acl_array_append(res->a, data) < 0)
			acl_msg_fatal("%s(%d): add array error(%s)",
				myname, __LINE__, acl_last_serror());
		if (limit > 0 && ++n >= limit)
			break;
	}
	if (acl_array_size(res->a) == 0) {
		acl_array_destroy(res->a, NULL);
		acl_myfree(res);
		return (NULL);
	}
	return (res);
}

/**
 * ��������ֵ�ӱ���ɾ����Ӧ�ڸ�����ֵ�Ľ����
 * @param mdt {ACL_MDT*}
 * @param key_label {const char*}
 * @param key {const char*}
 * @param onfree_fn {void (*)(void*, unsigned int} �ص�����
 * @return {int} ��ɾ���Ľ�����н��ĸ���. -1: ����, >= 0: ok
 */
static int mdt_del(ACL_MDT *mdt, const char *key_label,
	const char *key, void (*onfree_fn)(void*, unsigned int))
{
	const char *myname = "mdt_del";
	ACL_MDT_REC *rec, *rec2;
	ACL_MDT_REF *ref, *ref2;
	ACL_MDT_NOD *node;
	RING *iter, *iter2;
	ACL_MDT_IDX *idx;
	int   n = 0;

	idx = mdt_idx(mdt, key_label);
	if (idx == NULL) {
		acl_msg_warn("%s: key_label(%s) not exist in %s",
			myname, key_label, mdt->name);
		return (-1);
	}

	/* �������ֶεļ���صļ�¼������� */
	rec = mdt->idx_get(idx, key);
	if (rec == NULL)
		return (0);

	/* �����ý�������е��������ý�㣬�ҳ������������ݽ�� */
	for (iter = ring_succ(&rec->ref_head); iter && rec->nrefer > 0;) {

		ref = RING_TO_APPL(iter, ACL_MDT_REF, rec_entry);
		iter = ring_succ(iter);

		/* ���������ڸ����ݽ����������ý��, ��ɾ����Щ���ý�� */
		node = ref->node;  /* ���ݽ�� */

		/* ���������ݽ��������������ý�� */
		for (iter2 = ring_succ(&node->ref_head);
			iter2 != NULL && node->nrefer > 0;) {

			ref2 = RING_TO_APPL(iter2, ACL_MDT_REF, nod_entry);
			rec2 = ref2->rec;
			iter2 = ring_succ(iter2);

			mdt_ref_del(mdt, ref2);  /* ɾ������ */

			/* �������ѭ��δ���֮ǰ����Ҫ��ֹ rec ����ǰ�ͷ�! */
			if (rec2->nrefer == 0 && rec2 != rec) {
				mdt->idx_del(rec2->idx, rec2->key);
				mdt_rec_free(mdt, rec2);
			}
		}

		if (node->nrefer != 0)
			acl_msg_fatal("%s(%d): node->nrefer(%d) != 0,"
				" tbl_name(%s), key_label(%s), key(%s)",
				myname, __LINE__, node->nrefer,
				mdt->name, key_label, key);
		mdt_node_free(mdt, node, onfree_fn);  /* ɾ�����ݽ�� */

		n++;
	}

	if (rec->nrefer != 0)
		acl_msg_fatal("%s(%d): rec->nrefer(%d) != 0,"
			" tbl_name(%s), key_label(%s), key(%s)",
			myname, __LINE__, rec->nrefer,
			mdt->name, key_label, key);
	/* �ӵ�ǰ������ɾ�� */
	mdt->idx_del(rec->idx, rec->key);
	mdt_rec_free(mdt, rec);

	/* ������ɾ�������ý���� */
	return (n);
}

/**
 * �ӱ����г�һЩ�����
 * @param mdt {ACL_MDT*}
 * @param from {int} ��ʼλ��
 * @param limit {int} ��������
 * @return {ACL_MDT_RES*} �����, NULL: ��, != NULL: �ǿ�
 */
static ACL_MDT_RES *mdt_list(ACL_MDT *mdt, int from, int limit)
{
	const char *myname = "mdt_list";
	ACL_MDT_RES *res;
	ACL_MDT_NOD *node;
	RING *nod_iter;
	void *data;
	int   n = 0, i = 0;

	if (mdt == NULL)
		return (NULL);

	res = (ACL_MDT_RES*) acl_mycalloc(1, sizeof(ACL_MDT_RES));
	res->a = acl_array_create(limit > 0 ? limit : 100);
	res->ipos = 0;

	FOREACH_RING_FORWARD(nod_iter, &mdt->nod_head) {
		if (from >= 0 && i++ < from)
			continue;
		n++;
		node = RING_TO_APPL(nod_iter, ACL_MDT_NOD, mdt_entry);
		data = acl_mymalloc(node->dlen);
		memcpy(data, node->data, node->dlen);
		if (acl_array_append(res->a, data) < 0) {
			acl_msg_fatal("%s(%d): add array", myname, __LINE__);
			break;
		}
		if (limit > 0 && n >= limit)
			break;
	}

	return (res);
}

/**
 * �������е����ݶ���
 * @param mdt {ACL_MDT*} ��
 * @param walk_fn {int (*)(const void*, unsigned int)} ���������Ļص�����
 * @param from {int} ��ʼ���������ʼλ��
 * @param limit {int} ����������������
 * @return {int} �Ѿ��������ı����ľ���
 */
static int mdt_walk(ACL_MDT *mdt, int (*walk_fn)(const void*, unsigned int),
	int from, int limit)
{
	ACL_MDT_NOD *node;
	RING *iter;
	int   n = 0, i = 0;

	if (walk_fn == NULL || mdt == NULL)
		return (0);

	FOREACH_RING_ENTRY(iter, &mdt->nod_head) {
		if (from >= 0 && i++ < from)
			continue;
		node = RING_TO_APPL(iter, ACL_MDT_NOD, mdt_entry);
		n++;
		if (walk_fn(node->data, node->dlen))
			break;
		if (limit > 0 && n >= limit)
			break;
	}

	return (n);
}

/* ɾ���������ݽ�� */

static void mdt_nodes_free(ACL_MDT *mdt)
{
	ACL_MDT_NOD *node;
	ACL_MDT_REC *rec;
	ACL_MDT_REF *ref;
	RING *iter, *tmp, *iter2;

	for (iter = ring_succ(&mdt->nod_head); iter != &mdt->nod_head; ) {
		tmp = ring_succ(iter);
		node = RING_TO_APPL(iter, ACL_MDT_NOD, mdt_entry);
		for (iter2 = ring_succ(&node->ref_head); iter2 != &node->ref_head;) {
			ref = RING_TO_APPL(iter2, ACL_MDT_REF, nod_entry);
			rec = ref->rec;
			iter2 = ring_succ(iter2);
			mdt_ref_del(mdt, ref);
			if (rec->nrefer == 0) {
				mdt_rec_free(mdt, rec);
			}
		}
		mdt_node_free(mdt, node, NULL);
		iter = tmp;
	}
}

/*---------------------------------------------------------------------------*/

ACL_MDT *acl_mdt_create(const char *dbtype, const char *tbl_name,
	unsigned int tbl_flag, size_t init_capacity,
	const char *key_labels[], unsigned int idx_flags[])
{
	const char *myname = "acl_mdt_create";
	ACL_MDT *mdt;
	unsigned int rtgc_flag = 0;

	if (strcasecmp(dbtype, "hash") == 0) {
		mdt = acl_mdt_hash_create();
	} else if (strcasecmp(dbtype, "binhash") == 0) {
		mdt = acl_mdt_binhash_create();
	} else if (strcasecmp(dbtype, "avl") == 0) {
		mdt = acl_mdt_avl_create();
	} else {
		acl_msg_error("%s(%d): dbtype(%s)", myname, __LINE__, dbtype);
		return (NULL);
	}

	mdt->name = acl_mystrdup(tbl_name);
	mdt->node_cnt = 0;
	mdt->tbl_flag = tbl_flag;

	ring_init(&mdt->nod_head);
	ring_init(&mdt->idx_head);
	mdt_idx_create(mdt, init_capacity, key_labels, idx_flags);

	mdt->add = mdt_add;
	mdt->get = mdt_get;
	mdt->del = mdt_del;
	mdt->probe = mdt_probe;
	mdt->list = mdt_list;
	mdt->walk = mdt_walk;

	if ((tbl_flag & ACL_MDT_FLAG_SLICE_RTGC_OFF))
		rtgc_flag = ACL_MDT_FLAG_SLICE_RTGC_OFF;

	if ((tbl_flag & ACL_MDT_FLAG_SLICE1)) {
		mdt->nod_slice = acl_slice_create("nod_slice", 0,
			sizeof(ACL_MDT_NOD), ACL_SLICE_FLAG_GC1 | rtgc_flag);
		mdt->rec_slice = acl_slice_create("rec_slice", 0,
			sizeof(ACL_MDT_REC), ACL_SLICE_FLAG_GC1 | rtgc_flag);
		mdt->ref_slice = acl_slice_create("ref_slice", 0,
			sizeof(ACL_MDT_REF), ACL_SLICE_FLAG_GC1 | rtgc_flag);
	} else 	if ((tbl_flag & ACL_MDT_FLAG_SLICE2)) {
		mdt->nod_slice = acl_slice_create("nod_slice", 0,
			sizeof(ACL_MDT_NOD), ACL_SLICE_FLAG_GC2 | rtgc_flag);
		mdt->rec_slice = acl_slice_create("rec_slice", 0,
			sizeof(ACL_MDT_REC), ACL_SLICE_FLAG_GC2 | rtgc_flag);
		mdt->ref_slice = acl_slice_create("ref_slice", 0,
			sizeof(ACL_MDT_REF), ACL_SLICE_FLAG_GC2 | rtgc_flag);
	} else 	if ((tbl_flag & ACL_MDT_FLAG_SLICE3)) {
		mdt->nod_slice = acl_slice_create("nod_slice", 0,
			sizeof(ACL_MDT_NOD), ACL_SLICE_FLAG_GC3 | rtgc_flag);
		mdt->rec_slice = acl_slice_create("rec_slice", 0,
			sizeof(ACL_MDT_REC), ACL_SLICE_FLAG_GC3 | rtgc_flag);
		mdt->ref_slice = acl_slice_create("ref_slice", 0,
			sizeof(ACL_MDT_REF), ACL_SLICE_FLAG_GC3 | rtgc_flag);
	}
	return (mdt);
}

void acl_mdt_free(ACL_MDT *mdt)
{
	acl_myfree(mdt->name);
	mdt_idx_free(mdt);
	mdt_nodes_free(mdt);
	if (mdt->nod_slice)
		acl_slice_destroy(mdt->nod_slice);
	if (mdt->rec_slice)
		acl_slice_destroy(mdt->rec_slice);
	if (mdt->ref_slice)
		acl_slice_destroy(mdt->ref_slice);
	mdt->tbl_free(mdt);
}

ACL_MDT_NOD *acl_mdt_add(ACL_MDT *mdt, void *data, unsigned int dlen,
	const char *key_labels[], const char *keys[])
{
	return (mdt->add(mdt, data, dlen, key_labels, keys));
}

ACL_MDT_RES *acl_mdt_find(ACL_MDT *mdt, const char *key_label,
	const char *key, int from, int limit)
{
	return (mdt->get(mdt, key_label, key, from, limit));
}

int acl_mdt_probe(ACL_MDT *mdt, const char *key_label, const char *key)
{
	return (mdt->probe(mdt, key_label, key));
}

ACL_MDT_RES *acl_mdt_list(ACL_MDT *mdt, int from, int limit)
{
	return (mdt->list(mdt, from, limit));
}

int acl_mdt_delete(ACL_MDT *mdt, const char *key_label,
	const char *key, void (*onfree_fn)(void*, unsigned int))
{
	return (mdt->del(mdt, key_label, key, onfree_fn));
}

int acl_mdt_walk(ACL_MDT *mdt, int (*walk_fn)(const void*, unsigned int),
	int from, int limit)
{
	return (mdt->walk(mdt, walk_fn, from, limit));
}

const void *acl_mdt_fetch_row(ACL_MDT_RES *res)
{
	const char *myname = "acl_mdt_fetch_row";
	void *ptr;
	int   n;

	if (res == NULL || res->a == NULL || res->ipos < 0) {
		acl_msg_error("%s(%d): input error", myname, __LINE__);
		return (NULL);
	}

	n = acl_array_size(res->a);
	if (res->ipos >= n)
		return (NULL);
	ptr = (void *) acl_array_index(res->a, res->ipos++);
	return (ptr);
}

int acl_mdt_row_count(ACL_MDT_RES *res)
{
	if (res == NULL || res->a == NULL || res->ipos < 0)
		return (0);
	return (acl_array_size(res->a));
}

void acl_mdt_res_free(ACL_MDT_RES *res)
{
	if (res->a)
		acl_array_destroy(res->a, acl_myfree_fn);
	acl_myfree(res);
}

int acl_mdt_cnt(ACL_MDT *mdt)
{
	if (mdt == NULL)
		return (0);

	return (mdt->node_cnt);
}
