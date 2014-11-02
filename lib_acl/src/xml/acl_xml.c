#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include <string.h>
#include <stdio.h>
#include "stdlib/acl_msg.h"
#include "stdlib/acl_mystring.h"
#include "stdlib/acl_define.h"
#include "stdlib/acl_mymalloc.h"
#include "xml/acl_xml.h"

#endif

#define	LEN	ACL_VSTRING_LEN
#define	STR	acl_vstring_str

ACL_XML_ATTR *acl_xml_attr_alloc(ACL_XML_NODE *node)
{
	ACL_XML_ATTR *attr;

	if (node->xml->slice)
		attr = (ACL_XML_ATTR*) acl_slice_pool_calloc(__FILE__, __LINE__,
				node->xml->slice, 1, sizeof(ACL_XML_ATTR));
	else
		attr = (ACL_XML_ATTR*) acl_mycalloc(1, sizeof(ACL_XML_ATTR));
	attr->node = node;
	attr->name = acl_vstring_alloc2(node->xml->slice, 16);
	attr->value = acl_vstring_alloc2(node->xml->slice, 16);
	attr->quote = 0;
	attr->backslash = 0;
	attr->part_word = 0;

	acl_array_append(node->attr_list, attr);
	return (attr);
}

void acl_xml_attr_free(ACL_XML_ATTR *attr)
{
	acl_vstring_free(attr->name);
	acl_vstring_free(attr->value);
	if (attr->node->xml->slice)
		acl_slice_pool_free(__FILE__, __LINE__, attr);
	else
		acl_myfree(attr);
}

static ACL_XML_NODE *node_iter_head(ACL_ITER *it, ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr;

	it->dlen = -1;
	it->key = NULL;
	it->klen = -1;

	it->i = 0;
	it->size = node->children.len;

	if ((ring_ptr = acl_ring_succ(&node->children)) == &node->children) {
		it->ptr = it->data = NULL;
		return (NULL);
	}
	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;
	return (it->ptr);
}

static ACL_XML_NODE *node_iter_next(ACL_ITER *it, ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr;
	struct ACL_XML_NODE *child;

	child = (struct ACL_XML_NODE*) it->data;
	if ((ring_ptr = acl_ring_succ(&child->node)) == &node->children) {
		it->ptr = it->data = NULL;
		return (NULL);
	}
	it->i++;
	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;
	return (it->ptr);
}

static ACL_XML_NODE *node_iter_tail(ACL_ITER *it, ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr;

	it->dlen = -1;
	it->key = NULL;
	it->klen = -1;

	it->i = 0;
	it->size = node->children.len;

	if ((ring_ptr = acl_ring_pred(&node->children)) == &node->children) {
		it->ptr = it->data = NULL;
		return (NULL);
	}

	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;
	return (it->ptr);
}

static ACL_XML_NODE *node_iter_prev(ACL_ITER *it, ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr;
	struct ACL_XML_NODE *child;

	child = (struct ACL_XML_NODE*) it->data;
	if ((ring_ptr = acl_ring_pred(&child->node)) == &node->children) {
		it->ptr = it->data = NULL;
		return (NULL);
	}
	it->i++;
	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;
	return (it->ptr);
}

static void acl_xml_node_reset(ACL_XML_NODE *node)
{
	ACL_VSTRING_RESET(node->ltag);
	ACL_VSTRING_RESET(node->rtag);
	ACL_VSTRING_RESET(node->text);
	ACL_VSTRING_TERMINATE(node->ltag);
	ACL_VSTRING_TERMINATE(node->rtag);
	ACL_VSTRING_TERMINATE(node->text);

	node->id = NULL;

	if (node->attr_list)
		acl_array_clean(node->attr_list,
			(void (*)(void*)) acl_xml_attr_free);
	node->parent = NULL;
	acl_ring_init(&node->children);
	node->depth = 0;

	acl_ring_init(&node->node);
	node->curr_attr = NULL;
	node->quote = 0;
	node->last_ch = 0;
	node->nlt = 0;
	node->meta[0] = 0;
	node->flag = 0;
	node->status = ACL_XML_S_NXT;
}

ACL_XML_NODE *acl_xml_node_alloc(ACL_XML *xml)
{
	ACL_XML_NODE *node;

	if (xml->node_cache) {
		node = (ACL_XML_NODE*)
			xml->node_cache->pop_back(xml->node_cache);
		if (node) {
			acl_xml_node_reset(node);
			node->xml = xml;
			xml->node_cnt++;
			return (node);
		}
	}

	if (xml->slice)
		node = (ACL_XML_NODE*) acl_slice_pool_calloc(__FILE__, __LINE__,
				xml->slice, 1, sizeof(ACL_XML_NODE));
	else
		node = (ACL_XML_NODE*) acl_mycalloc(1, sizeof(ACL_XML_NODE));
	acl_ring_init(&node->children);
	acl_ring_init(&node->node);

	node->xml = xml;
	node->status = ACL_XML_S_NXT;
	node->ltag = acl_vstring_alloc2(xml->slice, 16);
	node->rtag = acl_vstring_alloc2(xml->slice, 16);
	node->text = acl_vstring_alloc2(xml->slice, 16);
	node->attr_list = acl_array_create(5);
	xml->node_cnt++;

	node->iter_head = node_iter_head;
	node->iter_next = node_iter_next;
	node->iter_tail = node_iter_tail;
	node->iter_prev = node_iter_prev;
	return (node);
}

static void acl_xml_node_free(ACL_XML_NODE *node)
{
	acl_vstring_free(node->ltag);
	acl_vstring_free(node->rtag);
	acl_vstring_free(node->text);
	acl_ring_detach(&node->node);
	acl_array_free(node->attr_list, (void (*)(void*)) acl_xml_attr_free);
	if (node->xml->slice)
		acl_slice_pool_free(__FILE__, __LINE__, node);
	else
		acl_myfree(node);
}

int acl_xml_node_delete(ACL_XML_NODE *node)
{
	ACL_RING *next;
	ACL_XML_NODE *node_next;
	int   n = 1;

	while ((next = acl_ring_pop_head(&node->children)) != NULL) {
		node_next = acl_ring_to_appl(next, ACL_XML_NODE, node);
		n += acl_xml_node_delete(node_next);
	}

	node->xml->node_cnt--;
	if (node->id != NULL)
		acl_htable_delete(node->xml->id_table, STR(node->id), NULL);
	if (node->xml->node_cache &&
		acl_array_size(node->xml->node_cache) < node->xml->max_cache)
	{
		node->xml->node_cache->push_back(node->xml->node_cache, node);
		acl_ring_detach(&node->node);
	} else
		acl_xml_node_free(node);
	return (n);
}

void acl_xml_node_append(ACL_XML_NODE *node1, ACL_XML_NODE *node2)
{
	/*
	if (node1->parent)
		acl_xml_node_add_child(node1->parent, node2);
	else
	*/
		acl_ring_append(&node1->node, &node2->node);
	node2->parent = node1->parent;
}

void acl_xml_node_add_child(ACL_XML_NODE *parent, ACL_XML_NODE *child)
{
	acl_ring_prepend(&parent->children, &child->node);
	child->parent = parent;
}

ACL_XML_NODE *acl_xml_node_parent(ACL_XML_NODE *node)
{
	return (node->parent);
}

ACL_XML_NODE *acl_xml_node_next(ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr = acl_ring_succ(&node->node);
	ACL_XML_NODE *parent;

	if (ring_ptr == &node->node)
		return (NULL);
	parent = node->parent;
	acl_assert(parent != NULL);
	if (ring_ptr == &parent->children)
		return (NULL);
	return (acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node));
}

ACL_XML_NODE *acl_xml_node_prev(ACL_XML_NODE *node)
{
	ACL_RING *ring_ptr = acl_ring_succ(&node->node);
	ACL_XML_NODE *parent;

	if (ring_ptr == &node->node)
		return (NULL);
	parent = node->parent;
	acl_assert(parent != NULL);
	if (ring_ptr == &parent->children)
		return (NULL);

	return (acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node));
}

static ACL_XML_NODE *xml_iter_head(ACL_ITER *it, ACL_XML *xml)
{
	ACL_RING *ring_ptr;

	it->dlen = -1;
	it->key = NULL;
	it->klen = -1;

	it->i = 0;
	it->size = xml->node_cnt;

	ring_ptr = acl_ring_succ(&xml->root->children);
	if (ring_ptr== &xml->root->children) {
		it->ptr = it->data = NULL;
		return (NULL);
	}
	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;
	return (it->ptr);
}

static ACL_XML_NODE *xml_iter_next(ACL_ITER *it, ACL_XML *xml)
{
	ACL_RING *ring_ptr;
	struct ACL_XML_NODE *node, *parent;

	node = (struct ACL_XML_NODE*) it->data;

	/* �ȱ�����ǰ�����ӽ�� */

	ring_ptr = acl_ring_succ(&node->children);
	if (ring_ptr != &node->children) {
		it->i++;
		it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
		it->data = it->ptr;
		return (it->ptr);
	}

	/* ��ǰ�����ӽ�������ϣ��ٱ�����ǰ�����ֵܽ�� */

	parent = acl_xml_node_parent(node);
	ring_ptr = acl_ring_succ(&node->node);
	if (ring_ptr != &parent->children) {
		it->i++;
		it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
		it->data = it->ptr;
		return (it->ptr);
	}

	/* ��ǰ�����ֵܽ�������ϣ���������ǰ���ĸ������ֵܽ�� */

	do {
		if (parent == xml->root)
			break;
		ring_ptr = acl_ring_succ(&parent->node);
		parent = acl_xml_node_parent(parent);
		if (parent == NULL)
			acl_msg_fatal("%s(%d): parent null", __FILE__, __LINE__);

		if (ring_ptr != &parent->children) {
			it->i++;
			it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
			it->data = it->ptr;
			return (it->ptr);
		}
	} while (ring_ptr != &xml->root->children);

	/* ���������н�� */

	it->ptr = it->data = NULL;
	return (NULL);
}

static ACL_XML_NODE *xml_iter_tail(ACL_ITER *it, ACL_XML *xml)
{
	ACL_RING *ring_ptr;

	it->dlen = -1;
	it->key = NULL;
	it->klen = -1;

	it->i = 0;
	it->size = xml->node_cnt;

	ring_ptr = acl_ring_pred(&xml->root->children);
	if (ring_ptr== &xml->root->children) {
		it->ptr = it->data = NULL;
		return (NULL);
	}
	it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
	it->data = it->ptr;
	return (it->ptr);
}

static ACL_XML_NODE *xml_iter_prev(ACL_ITER *it, ACL_XML *xml)
{
	ACL_RING *ring_ptr;
	struct ACL_XML_NODE *node, *parent;

	node = (struct ACL_XML_NODE*) it->data;

	/* �ȱ�����ǰ�����ӽ�� */

	ring_ptr = acl_ring_pred(&node->children);
	if (ring_ptr != &node->children) {
		it->i++;
		it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
		it->data = it->ptr;
		return (it->ptr);
	}

	/* ��ǰ�����ӽ�������ϣ��ٱ�����ǰ�����ֵܽ�� */

	parent = acl_xml_node_parent(node);
	ring_ptr = acl_ring_pred(&node->node);
	if (ring_ptr != &parent->children) {
		it->i++;
		it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
		it->data = it->ptr;
		return (it->ptr);
	}

	/* ��ǰ�����ֵܽ�������ϣ���������ǰ���ĸ������ֵܽ�� */

	do {
		if (parent == xml->root)
			break;
		ring_ptr = acl_ring_pred(&parent->node);
		parent = acl_xml_node_parent(parent);
		if (parent == NULL)
			acl_msg_fatal("%s(%d): parent null", __FILE__, __LINE__);

		if (ring_ptr != &parent->children) {
			it->i++;
			it->ptr = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);
			it->data = it->ptr;
			return (it->ptr);
		}
	} while (ring_ptr != &xml->root->children);

	/* ���������н�� */

	it->ptr = it->data = NULL;
	return (NULL);
}

ACL_XML *acl_xml_alloc()
{
	return (acl_xml_alloc1(NULL));
}

ACL_XML *acl_xml_alloc1(ACL_SLICE_POOL *slice)
{
	ACL_XML *xml;

	if (slice) {
		xml = (ACL_XML*) acl_slice_pool_calloc(__FILE__, __LINE__,
				slice, 1, sizeof(ACL_XML));
		xml->slice = slice;
	} else {
		xml = (ACL_XML*) acl_mycalloc(1, sizeof(ACL_XML));
		xml->slice = NULL;
	}

	xml->root = acl_xml_node_alloc(xml);
	xml->id_table = acl_htable_create3(10, 0, slice);

	xml->iter_head = xml_iter_head;
	xml->iter_next = xml_iter_next;
	xml->iter_tail = xml_iter_tail;
	xml->iter_prev = xml_iter_prev;
	return (xml);
}

void acl_xml_foreach_init(ACL_XML *xml, ACL_XML_NODE *node)
{
	xml->root = node;
	xml->iter_head = xml_iter_head;
	xml->iter_next = xml_iter_next;
	xml->iter_tail = xml_iter_tail;
	xml->iter_prev = xml_iter_prev;
}

void acl_xml_slash(ACL_XML *xml, int ignore)
{
	if (ignore)
		xml->flag |= ACL_XML_FLAG_IGNORE_SLASH;
}

void acl_xml_cache(ACL_XML *xml, int max_cache)
{
	if (xml->node_cache != NULL) {
		acl_array_free(xml->node_cache,
			(void (*)(void*)) acl_xml_node_free);
		xml->node_cache = NULL;
		xml->max_cache = 0;
	}
	if (max_cache > 0) {
		xml->node_cache = acl_array_create(max_cache);
		xml->max_cache = max_cache;
	}
}

void acl_xml_cache_free(ACL_XML *xml)
{
	if (xml->node_cache != NULL) {
		acl_array_free(xml->node_cache,
			(void (*)(void*)) acl_xml_node_free);
		xml->node_cache = NULL;
		xml->max_cache = 0;
	}
}

int acl_xml_free(ACL_XML *xml)
{
	ACL_RING *next;
	ACL_XML_NODE *node;
	int   n = 1;

	while ((next = acl_ring_pop_head(&xml->root->children)) != NULL) {
		node = acl_ring_to_appl(next, ACL_XML_NODE, node);
		n += acl_xml_node_delete(node);
	}

	acl_xml_node_free(xml->root);
	xml->node_cnt--;
	acl_assert(xml->node_cnt == 0);
	acl_htable_free(xml->id_table, NULL);
	if (xml->node_cache != NULL)
		acl_array_free(xml->node_cache,
			(void (*)(void*)) acl_xml_node_free);
	if (xml->slice)
		acl_slice_pool_free(__FILE__, __LINE__, xml);
	else
		acl_myfree(xml);
	return (n);
}

void acl_xml_reset(ACL_XML *xml)
{
	const char *myname = "acl_xml_reset";
	ACL_RING *next;
	ACL_XML_NODE *node;

	while ((next = acl_ring_pop_head(&xml->root->children)) != NULL) {
		node = acl_ring_to_appl(next, ACL_XML_NODE, node);
		(void) acl_xml_node_delete(node);
	}

	/* ��Ϊ�������һ�����㣬���Բ���Ҫ�ͷţ�����ڵ���
	 * acl_xml_free ʱ���ͷ�
	 */ 
	acl_ring_detach(&xml->root->node);
	acl_xml_node_reset(xml->root);

	if (xml->node_cnt != 1)
		acl_msg_fatal("%s(%d): node_cnt(%d) invalid",
			myname, __LINE__, xml->node_cnt);

	acl_htable_reset(xml->id_table, NULL);
	xml->curr_node = NULL;
}

int acl_xml_is_closure(ACL_XML *xml)
{
	ACL_RING *ring_ptr;
	ACL_XML_NODE *node;

	/* ��� xml->root �������һ��һ���ӽ�� */
	ring_ptr = acl_ring_succ(&xml->root->children);

	if (ring_ptr == &xml->root->children) {
		/* ˵��û����ʵ�ӽ�� */
		return (0);
	}

	node = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);

	if ((node->flag & ACL_XML_F_SELF_CL)) {
		/* ˵���ý�����ԱպϽ�� */
		return (1);
	}

	if (node->status == ACL_XML_S_RGT) {
		return (1);
	}

	/* ˵�����һ��һ���ӽ�㻹δ������� */
	return (0);
}

int acl_xml_is_complete(ACL_XML *xml, const char *tag)
{
	ACL_RING *ring_ptr;
	ACL_XML_NODE *node;

	/* ��� xml->root �������һ��һ���ӽ�� */
	ring_ptr = acl_ring_succ(&xml->root->children);

	if (ring_ptr == &xml->root->children) {
		/* ˵��û����ʵ�ӽ�� */
		return (0);
	}

	node = acl_ring_to_appl(ring_ptr, ACL_XML_NODE, node);

	if ((node->flag & ACL_XML_F_SELF_CL)) {
		/* ˵���ý�����ԱպϽ�� */
		return (1);
	}

	if (node->status != ACL_XML_S_RGT) {
		/* ˵�����һ��һ���ӽ�㻹δ������� */
		return (0);
	}

	if (strcasecmp(STR(node->rtag), tag) == 0) {
		return (1);
	}

	/* ˵�� xml �е����һ�������������ǩ��ƥ�� */
	return (0);
}
