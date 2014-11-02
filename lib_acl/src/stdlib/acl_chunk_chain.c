#include "StdAfx.h"
#ifndef	ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_msg.h"
#include "stdlib/acl_ring.h"
#include "stdlib/acl_vstring.h"
#include "stdlib/acl_chunk_chain.h"

#endif

typedef struct ACL_CHUNK {
	acl_int64  from;
	acl_int64  dlen;
	char *buf;

	ACL_RING   entry;          /* linkage */
} ACL_CHUNK;

struct ACL_CHAIN {
	acl_int64 from_next;       /* ��ǰ������������λ�� */
	acl_int64 off_begin;       /* ƫ������ʼλ�� */

	ACL_RING     ring;         /* ��� DBUF ���ݿ���ɵ������� */
	ACL_VSTRING  sbuf;
};

static ACL_CHUNK *acl_chunk_new(const void *buf, acl_int64 from, int dlen)
{
	ACL_CHUNK *chunk = (ACL_CHUNK*) acl_mycalloc(1, sizeof(ACL_CHUNK));

	chunk->from = from;
	chunk->dlen  = dlen;
	chunk->buf   = (char *) acl_mymemdup(buf, dlen);
	return (chunk);
}

static void acl_chunk_free(ACL_CHUNK *chunk)
{
	acl_myfree(chunk->buf);
	acl_myfree(chunk);
}

static void acl_chunk_merge(ACL_CHUNK *chunk, const char *pdata,
	acl_int64 from, int dlen)
{
	char *tmpbuf, *ptr;
	acl_int64 to = from + dlen, n, chunk_to = chunk->from + chunk->dlen;

	/* sanity check: ���뱣֤�������ݱ��������ݿ������ݽ��� */

	if (from < chunk->from) {
		if (to < chunk->from)
			return;

		/* from < chunk->from <= to */

		if (to < chunk_to) {
			/* from < chunk->from <= to < chunk_to */
			n = chunk->from - from;
			ptr = tmpbuf = acl_mymalloc((int) (chunk->dlen + n));
			memcpy(ptr, pdata, dlen);
			ptr += dlen;
			n = to - chunk->from;
			pdata = chunk->buf + n;
			memcpy(ptr, pdata, (int) (chunk->dlen - n));

			acl_myfree(chunk->buf);
			chunk->buf = tmpbuf;
			chunk->dlen += chunk->from - from;
			chunk->from = from;
		} else {
			/* from < chunk->from < chunk_to <= to */
			acl_myfree(chunk->buf);
			chunk->buf = (char*) acl_mymemdup(pdata, dlen);
			chunk->from = from;
			chunk->dlen = dlen;
		}
	} else if (from <= chunk_to) {
		/* chunk->from <= from <= chunk_to */
		if (to > chunk_to) {
			/* chunk->from <= from <= chunk_to < to */
			n = to - chunk_to ;
			ptr = tmpbuf = acl_mymalloc((int) (chunk->dlen + n));
			memcpy(ptr, chunk->buf, (int) chunk->dlen);
			ptr += chunk->dlen;
			pdata += chunk_to - from;
			n = to - chunk_to;
			memcpy(ptr, pdata, (int) n);

			acl_myfree(chunk->buf);
			chunk->buf = tmpbuf;
			chunk->dlen += n;
			/* chunk->from no changed */
		}
		/* else: chunk->from <= from <= to <= chunk_to, do nothing */
	}
	/* else: chunk->from <= chunk_to < from <= to, do nothing */
}

ACL_CHAIN *acl_chain_new(size_t init_size, acl_int64 off_begin)
{
	ACL_CHAIN *chain;
	
	chain = (ACL_CHAIN*) acl_mycalloc(1, sizeof(ACL_CHAIN));
	acl_vstring_init(&chain->sbuf, init_size);
	acl_ring_init(&chain->ring);
	chain->off_begin = off_begin;
	chain->from_next = off_begin;
	return (chain);
}

void acl_chain_free(ACL_CHAIN *chain)
{
	ACL_RING *ring_iter;
	ACL_CHUNK *chunk;

	while ((ring_iter = acl_ring_pop_head(&chain->ring))) {
		chunk = ACL_RING_TO_APPL(ring_iter, ACL_CHUNK, entry);
		acl_chunk_free(chunk);
	}
	acl_vstring_free_buf(&chain->sbuf);
	acl_myfree(chain);
}

void acl_chain_set_from_next(ACL_CHAIN *chain, acl_int64 from_next)
{
	chain->from_next = from_next;
	ACL_VSTRING_RESET(&chain->sbuf);
}

void acl_chain_reset(ACL_CHAIN *chain, acl_int64 off_begin)
{
	chain->off_begin = off_begin;
	chain->from_next = off_begin;
	ACL_VSTRING_RESET(&chain->sbuf);
}

acl_int64 acl_chain_from_next(ACL_CHAIN *chain)
{
	return (chain->from_next);
}

acl_int64 acl_chain_off_begin(ACL_CHAIN *chain)
{
	return (chain->off_begin);
}

const char *acl_chain_data(ACL_CHAIN *chain)
{
	if (ACL_VSTRING_LEN(&chain->sbuf) == 0)
		return (NULL);

	return (acl_vstring_str(&chain->sbuf));
}

int acl_chain_data_len(ACL_CHAIN *chain)
{
	return (ACL_VSTRING_LEN(&chain->sbuf));
}

int acl_chain_size(ACL_CHAIN *chain)
{
	ACL_RING_ITER iter;
	int   n = 0;

	acl_ring_foreach(iter, &chain->ring) {
		n++;
	}

	return (n);
}

int acl_chain_chunk_data_len(ACL_CHAIN *chain)
{
	ACL_RING_ITER iter;
	ACL_CHUNK *chunk_iter;
	int   n = 0;

	acl_ring_foreach(iter, &chain->ring) {
		chunk_iter = ACL_RING_TO_APPL(iter.ptr, ACL_CHUNK, entry);
		n += (int) chunk_iter->dlen;
	}

	return (n);
}

void acl_chain_add(ACL_CHAIN *chain, const void *data,
	acl_int64 from, int dlen)
{
	const char *myname = "acl_chain_add";
	const char *pdata = (const char*) data;
	ACL_CHUNK *chunk_iter, *chunk, *chunk_saved;
	ACL_RING *ring_ptr, *ring_next;
	ACL_RING_ITER ring_iter;
	acl_int64 to, n, chunk_to;

	to = from + dlen;
	/* ���˵��ظ����� */
	if (to < chain->from_next) {
		acl_msg_warn("%s(%d): past data, to(" ACL_FMT_I64D ") < from_next("
			ACL_FMT_I64D "), from=" ACL_FMT_I64D ", dlen=%d",
			myname, __LINE__, to, chain->from_next, from, dlen);
		return;
	}

	/* make sure: from >= chain->from_next */
	if (from < chain->from_next) {
		n = chain->from_next - from;
		dlen -= (int) n;
		pdata += n;
		from = chain->from_next;
	}

	if (from == chain->from_next) {
		/* �����������ݿ鿽�����������ݻ������� */
		acl_vstring_memcat(&chain->sbuf, pdata, dlen);
		chain->from_next += dlen;

		while ((chunk_iter = ACL_RING_FIRST_APPL(&chain->ring, ACL_CHUNK, entry)) != NULL) {
			acl_int64 to_first;

			/* ������������ӿ����˳�ѭ�� */
			if (to < chunk_iter->from)
				break;

			/* �����������ݿ����� */

			/* ȥ���ý������ݿ�����ӽ�� */
			(void) acl_ring_pop_head(&chain->ring);

			to_first = chunk_iter->from + chunk_iter->dlen;

			/* ��������ݿ���������ݿ飬��ȥ���ظ��ľ����ݿ� */
			if (to >= to_first) {
				acl_chunk_free(chunk_iter);
				continue;
			}

			/* ���ݿ齻����� */

			/* from <= to < to_first
			 * chunk_iter->from <= from <= to < to_first
			 * or:
			 * from <= chunk_iter->from <= to < to_first
			 */
			n = to_first - to;
			pdata = chunk_iter->buf + chunk_iter->dlen - n;
			acl_vstring_memcat(&chain->sbuf, pdata, (int) n);
			chain->from_next += n;

			acl_chunk_free(chunk_iter);
			break;
		}

		return;
	}

	/* ����ݿ鲻��������������� */

	/* chain->from_next < from <= to*/
	chunk_iter = ACL_RING_FIRST_APPL(&chain->ring, ACL_CHUNK, entry);
	if (chunk_iter == NULL) {
		/* ˵���ǵ�һ�����ݿ� */
		chunk = acl_chunk_new(data, from, dlen);
		acl_ring_append(&chain->ring, &chunk->entry);
		return;
	}

	chunk_saved = NULL;

	acl_ring_foreach(ring_iter, &chain->ring) {
		chunk_iter = ACL_RING_TO_APPL(ring_iter.ptr, ACL_CHUNK, entry);
		chunk_to = chunk_iter->from + chunk_iter->dlen;

		/* chunk_iter->from <= to */
		if (from <= chunk_iter->from) {
			if (to < chunk_iter->from) {
				/* ˵�������ݿ����������е����ݿ��޽��� */
				/* from <= to < chunk_iter->from <= chunk_to */
				chunk = acl_chunk_new(data, from, dlen);
				/* ǰ�����ݿ��� */
				acl_ring_prepend(&chunk_iter->entry, &chunk->entry);
				return;
			}
			/* from <= chunk_iter->from <= to */
			if (to <= chunk_to) {
				/* from <= chunk_iter->from <= to <= chunk_to */
				acl_chunk_merge(chunk_iter, pdata, from, dlen);
				return;
			}
			/* else: from <= chunk_iter->from <= chunk_to < to */
			/* �������ݿ��ܴ����ڶ�����ݽ���� */
			chunk_saved = chunk_iter;
			break;
		} else if (from <= chunk_to) {
			/* chunk_iter->from < from <= chunk_to */
			if (to <= chunk_to) {
				/* chunk_iter->from <= from <= to <= chunk_to */
				acl_chunk_merge(chunk_iter, pdata, from, dlen);
				return;
			}
			/* else: chunk_iter->from < from <= chunk_to < to */
			/* �������ݿ��ܴ����ڶ�����ݽ���� */
			chunk_saved = chunk_iter;
			break;
		}
		/* else: from > chunk_to, continue */
	}

	if (chunk_saved == NULL) {
		/* ˵�������ݿ����������е����ݿ��޽��� */
		/* from <= to < chunk_iter->from <= chunk_to */
		chunk = acl_chunk_new(data, from, dlen);
		/* ǰ�����ݿ��� */
		acl_ring_prepend(&chain->ring, &chunk->entry);

		return;
	}

	/* from <= chunk_iter->from <= chunk_to < to
	 * or:
	 * chunk_iter->from < from <= chunk_to < to
	 */
	acl_chunk_merge(chunk_saved, pdata, from, dlen);
	ring_next = acl_ring_succ(ring_iter.ptr);
	while (ring_next != &chain->ring) {
		chunk = ACL_RING_TO_APPL(ring_next, ACL_CHUNK, entry);
		if (to < chunk->from)
			break;
		/* to >= chunk->from */
		chunk_to = chunk->from + chunk->dlen;
		if (to >= chunk_to) {
			/* from < chunk->from <= chunk_to <= to
			 * or:
			 * chunk_iter->from < from < chunk_to <= to
			 */
			ring_ptr = ring_next;
			ring_next = acl_ring_succ(ring_next);
			acl_ring_detach(ring_ptr);
			acl_chunk_free(chunk);
			continue;
		}
		/* from <= chunk->from < to < chunk_to
		 * or:
		 * chunk_iter->from < from < to < chunk_to
		 */
		acl_chunk_merge(chunk_saved, chunk->buf, chunk->from, (int) chunk->dlen);
		ring_ptr = ring_next;
		ring_next = acl_ring_succ(ring_next);
		acl_ring_detach(ring_ptr);
		acl_chunk_free(chunk);
		break;
	}
}

void acl_chain_list(ACL_CHAIN *chain)
{
	ACL_RING_ITER ring_iter;
	ACL_CHUNK *chunk;
	acl_int64 chunk_to;

	if (ACL_VSTRING_LEN(&chain->sbuf) > 0)
		printf("from=" ACL_FMT_I64D ", next to=" ACL_FMT_I64D " , dlen=%d\r\n",
			chain->off_begin, chain->from_next, (int) ACL_VSTRING_LEN(&chain->sbuf));

	acl_ring_foreach(ring_iter, &chain->ring) {
		chunk = ACL_RING_TO_APPL(ring_iter.ptr, ACL_CHUNK, entry);
		chunk_to = chunk->from + chunk->dlen;
		printf("from=" ACL_FMT_I64D ", next to=" ACL_FMT_I64D ", dlen=%d\r\n",
			chunk->from, chunk_to, (int) chunk->dlen);
	}
}
