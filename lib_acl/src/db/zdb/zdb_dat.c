#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include <string.h>
#include <stdio.h>
#include "stdlib/acl_msg.h"
#include "stdlib/acl_debug.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_meter_time.h"
#include "stdlib/acl_sane_basename.h"
#include "thread/acl_pthread.h"
#include "db/zdb.h"

#endif

#include "zdb_private.h"

int dat_store_header_sync(ZDB_DAT_STORE *store)
{
	const char *myname = "dat_store_header_sync";
	int   ret;

	ret = ZDB_WRITE((ZDB_STORE*) store, &store->hdr, sizeof(store->hdr), 0);
	if (ret == -1)
		acl_msg_error("%s(%d): zdb_write to %s error(%s)", myname,
			__LINE__, STORE_PATH(&store->store), acl_last_serror());
	return (ret);
}

/**
 * �ͷ� ACL_VSTRING �ص�����
 * @param arg {void*} �ص�����, ��ת���� ACL_VSTRING ����
 */
static void free_vstring_fn(void *arg)
{
	ACL_VSTRING *s = (ACL_VSTRING*) arg;

	acl_vstring_free(s);
}

/**
 * ȷ�� ZDB.dat_ifiles ��������������
 * @param db {ZDB*}
 * @param size {int} ���������Ĵ�СҪ��
 */
static void dat_ifiles_space(ZDB *db, int idisk, int size)
{
	int   i;

	/* xxx: ֻ���Լ�1����Ϊ�±��Ǵ�1��ʼ��, �� C �����е����������Ǵ� 0 ��ʼ��,
	 * ����˵ db->dat_disks[0] �Ǹ��˷ѵ��� :( --- zsx
	 */
	size++;

	if (db->dat_disks[idisk].dat_ifiles == NULL) {
		if (size < 16)
			size = 16;
		db->dat_disks[idisk].dat_ifiles = (int*)
			acl_mycalloc(size, sizeof(int));
		db->dat_disks[idisk].dat_ifiles_size = size;
		return;
	}

	if (size < db->dat_disks[idisk].dat_ifiles_size)
		return;
	db->dat_disks[idisk].dat_ifiles = (int*)
		acl_myrealloc(db->dat_disks[idisk].dat_ifiles, size * sizeof(int));

	for (i = db->dat_disks[idisk].dat_ifiles_size; i < size; i++)
		db->dat_disks[idisk].dat_ifiles[i] = 0;
	db->dat_disks[idisk].dat_ifiles_size = size;
}

/**
 * ����ֵ�洢�����·����
 * @param db {ZDB*}
 * @param len {int} ���ݵĳ���
 * @return {int} >= 0: ok; -1: error
 */
static int dat_inode(ZDB *db, int len)
{
	const char *myname = "dat_inode";
	int  inode;

	/* �����ĳ��ȱ���Ϊ������������� */

	if (len % db->blk_dlen != 0) {
		acl_msg_error("%s(%d): len(%d) %% db->blk_dlen(%d) != 0, invalid",
			myname, __LINE__, len, db->blk_dlen);
		return (-1);
	}

	/* �������ݿ鳤�ȼ����������ĸ���, ͬʱҲ�Ǵ洢�ļ�����Ŀ¼�ı�ʶ�� */

	inode = len / db->blk_dlen;
	if (inode > DIR_LIMIT) {
		acl_msg_error("%s(%d): inode(%d) > %u, too large",
			myname, __LINE__, inode, DIR_LIMIT);
		return (-1);
	}

	return (inode);
}

/**
 * ���ֵ�洢���ļ�ȫ·��
 * @param db {ZDB*}
 * @param buf {ACL_VSTRING*} �洢����Ļ�����
 * @param inode {int} ·����
 * @param ifile {int} �ļ���
 * @return {ACL_VSTRING*} �����������ַ
 **/
static ACL_VSTRING *dat_filepath(ZDB *db, ACL_VSTRING *buf, int idisk, int inode, int ifile)
{
	static acl_pthread_key_t buf_key = ACL_TLS_OUT_OF_INDEXES;
	ACL_VSTRING *buf_safe;
	static ACL_VSTRING *__buf_unsafe = NULL;

	buf_safe = (ACL_VSTRING*) acl_pthread_tls_get(&buf_key);
	if (buf_safe == NULL) {
		if (buf_key == (acl_pthread_key_t) ACL_TLS_OUT_OF_INDEXES) {
			if (__buf_unsafe == NULL)
				__buf_unsafe = acl_vstring_alloc(256);
			buf_safe = __buf_unsafe;
		} else {
			buf_safe = acl_vstring_alloc(256);
			acl_pthread_tls_set(buf_key, buf_safe, free_vstring_fn);
		}
	}

	if (buf == NULL)
		buf = buf_safe;

#ifdef	INCLUDE_PATH
	acl_vstring_sprintf(buf, "%s/%d/%s_%d_%d.dat",
		db->dat_disks[idisk].path, inode, db->dbname, inode, ifile);
#else
	acl_vstring_sprintf(buf, "%s/%s_%d_%d.dat",
		db->dat_disks[idisk].path, db->dbname, inode, ifile);
#endif
	return (buf);
}

/**
 * ����ļ�������·��
 * @param db {ZDB*}
 * @param buf {ACL_VSTRING*} �洢����Ļ�����
 * @param inode {int} ·����
 * @return {ACL_VSTRING*} �����������ַ
 */
static ACL_VSTRING *dat_path(ZDB *db, ACL_VSTRING *buf, int idisk, int inode acl_unused)
{
	static acl_pthread_key_t buf_key = (acl_pthread_key_t) ACL_TLS_OUT_OF_INDEXES;
	ACL_VSTRING *buf_safe;
	static ACL_VSTRING *__buf_unsafe = NULL;

	buf_safe = (ACL_VSTRING*) acl_pthread_tls_get(&buf_key);
	if (buf_safe == NULL) {
		if (buf_key == (acl_pthread_key_t) ACL_TLS_OUT_OF_INDEXES) {
			if (__buf_unsafe == NULL)
				__buf_unsafe = acl_vstring_alloc(256);
			buf_safe = __buf_unsafe;
		} else {
			buf_safe = acl_vstring_alloc(256);
			acl_pthread_tls_set(buf_key, buf_safe, free_vstring_fn);
		}
	}

	if (buf == NULL)
		buf = buf_safe;

#ifdef  INCLUDE_PATH
	acl_vstring_sprintf(buf, "%s/%d", db->dat_disks[idisk].path, inode);
#else
	acl_vstring_sprintf(buf, "%s", db->dat_disks[idisk].path);
#endif

	return (buf);
}

#ifdef	ZDB_LINK_BUSY

/**
 * ���µ�ռ�ÿ����ӵ�ռ������
 * @param store {ZDB_DAT_STORE*}
 * @param blk {ZDB_BLK*}
 * @parem ilnk {zdb_lnk_t}
 * @return {int} 0: ok; -1: error
 */
static int busy_blk_link(ZDB_DAT_STORE *store, ZDB_BLK *blk, zdb_lnk_t ilnk)
{
	const char *myname = "busy_blk_link";
	ZDB_BLK *hdr_blk = NULL;
	zdb_off_t off;
	int  ret, blk_len;

#undef	RETURN
#define	RETURN(x) do {  \
	if (hdr_blk)  \
		zdb_blk_free(hdr_blk);  \
	return (x);  \
} while (0)
	
	if (store->hdr.itail_busy == -1) {
		if (store->hdr.ihead_busy != -1) {
			acl_msg_error("%s(%d): %s, ihead_busy(%d) != -1",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				store->hdr.ihead_busy);
			RETURN (-1);
		}
		store->hdr.itail_busy = ilnk;
		store->hdr.ihead_busy = ilnk;
		blk->hdr.inext_busy = -1;
		blk->hdr.iprev_busy = -1;
		RETURN (0);
	} else if (store->hdr.ihead_busy == -1) {
		acl_msg_error("%s(%d): %s, ihead_busy(%d) invalid", myname,
			__LINE__, STORE_PATH((ZDB_STORE*) store),
			store->hdr.ihead_busy);
		RETURN (-1);
	}

	off = BLK_HDR_OFF(store, store->hdr.ihead_busy);
	if (off <= 0) {
		acl_msg_error("%s(%d): %s, off(" ACL_FMT_I64D ") invalid",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store), off);
		RETURN (-1);
	}

	blk_len = BLK_LEN(store);
	hdr_blk = (ZDB_BLK*) acl_mymalloc(blk_len);
	ret = ZDB_READ((ZDB_STORE*) store, hdr_blk, blk_len, off);
	if (ret == -1) {
		acl_msg_error("%s(%d): zdb_read %s error(%s)", myname, __LINE__,
			STORE_PATH((ZDB_STORE*) store), acl_last_serror());
		RETURN (-1);
	}

	if (hdr_blk->hdr.blk_ilnk != store->hdr.ihead_busy) {
		acl_msg_error("%s(%d): %s, blk_ilnk(%d) != ihead_busy(%d)",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			hdr_blk->hdr.blk_ilnk, store->hdr.ihead_busy);
		RETURN (-1);
	}

	hdr_blk->hdr.iprev_busy = ilnk;
	blk->hdr.inext_busy = store->hdr.ihead_busy;
	blk->hdr.iprev_busy = -1;
	store->hdr.ihead_busy = ilnk;  /* ����ռ�ÿ���ͷ */

	/* ����ĳռ�ÿ��ͷ��Ϣ */
	ret = ZDB_WRITE((ZDB_STORE*) store, hdr_blk, blk_len, off);
	if (ret == -1) {
		acl_msg_error("%s(%d): zdb_write to %s error(%s)",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			acl_last_serror());
		RETURN (-1);
	}

	RETURN (0);
}

static int busy_blk_unlink(ZDB_DAT_STORE *store, ZDB_BLK *blk, zdb_lnk_t ilnk)
{
	const char *myname = "busy_blk_unlink";
	ZDB_BLK_HDR *blk_hdr = NULL;
	zdb_off_t off;
	int   ret;

#undef	RETURN
#define	RETURN(x) do {  \
	if (blk_hdr)  \
		acl_myfree(blk_hdr);  \
	return (x);  \
} while (0)

	if (ilnk < 0) {
		acl_msg_error("%s(%d): %s, ilnk(%d) invalid",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store), ilnk);
		RETURN (-1);
	}

	if (store->hdr.ihead_busy == -1) {
		acl_msg_error("%s(%d): ihead_busy(%d) invalid",
			myname, __LINE__, store->hdr.ihead_busy);
		RETURN (-1);
	} else if (store->hdr.itail_busy == -1) {
		acl_msg_error("%s(%d): itail_busy(%d) invalid",
			myname, __LINE__, store->hdr.itail_busy);
		RETURN (-1);
	}

	if (blk->hdr.iprev_busy == -1) {
		/* ��ռ�ÿ�Ӧ����ͷ���� */

		if (store->hdr.ihead_busy != ilnk) {
			acl_msg_error("%s(%d): %s, ihead_busy(%d) != ilnk(%d)",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				store->hdr.ihead_busy, ilnk);
			RETURN (-1);
		}
		if (store->hdr.itail_busy == ilnk) {
			/* ˵���� blk �����һ��ռ�ÿ��� */

			store->hdr.ihead_busy = store->hdr.itail_busy = -1;
			blk->hdr.inext_busy = blk->hdr.iprev_busy = -1;
			RETURN (0);
		} else if (blk->hdr.inext_busy < 0) {
			acl_msg_error("%s(%d): %s, inext_busy(%d) invalid",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				blk->hdr.inext_busy);
			RETURN (-1);
		}

		blk_hdr = (ZDB_BLK_HDR*) acl_mymalloc(sizeof(ZDB_BLK_HDR));
		off = BLK_HDR_OFF(store, blk->hdr.inext_busy);
		ret = ZDB_READ((ZDB_STORE*) store, blk_hdr, sizeof(ZDB_BLK_HDR), off);
		if (ret == -1) {
			acl_msg_error("%s(%d): zdb_read %s error(%s)",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				acl_last_serror());
			RETURN (-1);
		} else if (blk_hdr->blk_ilnk != blk->hdr.inext_busy) {
			acl_msg_error("%s(%d): blk_ilnk(%d) != inext_busy(%d)",
				myname, __LINE__, blk_hdr->blk_ilnk,
				blk->hdr.inext_busy);
			RETURN (-1);
		}

		blk_hdr->iprev_busy = -1;
		ret = ZDB_WRITE((ZDB_STORE*) store, blk_hdr, sizeof(ZDB_BLK_HDR), off);
		if (ret == -1) {
			acl_msg_error("%s(%d): zdb_write %s error(%s)",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				acl_last_serror());
			RETURN (-1);
		}

		store->hdr.ihead_busy = blk_hdr->blk_ilnk;
		RETURN (0);
	}

	if (blk->hdr.inext_busy == -1) {
		/* ˵����ռ�ÿ�Ӧ��Ϊβ���� */

		if (store->hdr.itail_busy != ilnk) {
			acl_msg_error("%s(%d): %s, itail_busy(%d) != ilnk(%d)",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				store->hdr.itail_busy, ilnk);
			RETURN (-1);
		}

		/* xxx: sanity check */
		if (store->hdr.ihead_busy == ilnk) {
			/* ˵���� blk �����һ��ռ�ÿ��� */

			store->hdr.ihead_busy = store->hdr.itail_busy = -1;
			blk->hdr.inext_busy = blk->hdr.iprev_busy = -1;
			RETURN (0);
		} else if (blk->hdr.iprev_busy < 0) {
			acl_msg_error("%s(%d): %s, iprev_busy(%d) invalid",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				blk->hdr.iprev_busy);
			RETURN (-1);
		}

		blk_hdr = (ZDB_BLK_HDR*) acl_mymalloc(sizeof(ZDB_BLK_HDR));
		off = BLK_HDR_OFF(store, blk->hdr.iprev_busy);
		ret = ZDB_READ((ZDB_STORE*) store, blk_hdr, sizeof(ZDB_BLK_HDR), off);
		if (ret == -1) {
			acl_msg_error("%s(%d): zdb_read %s error(%s)",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				acl_last_serror());
			RETURN (-1);
		} else if (blk_hdr->blk_ilnk != blk->hdr.iprev_busy) {
			acl_msg_error("%s(%d): blk_ilnk(%d) != iprev_busy(%d)",
				myname, __LINE__, blk_hdr->blk_ilnk,
				blk->hdr.iprev_busy);
			RETURN (-1);
		}

		blk_hdr->inext_busy = -1;
		ret = ZDB_WRITE((ZDB_STORE*) store, blk_hdr, sizeof(ZDB_BLK_HDR), off);
		if (ret == -1) {
			acl_msg_error("%s(%d): zdb_write %s error(%s)",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				acl_last_serror());
			RETURN (-1);
		}

		store->hdr.itail_busy = blk_hdr->blk_ilnk;
		RETURN (0);
	}

	/* ˵����ռ�ÿ�Ӧ��Ϊ�м�� */

	/* ���º�һ��ռ�ÿ��ͷ��Ϣ */

	blk_hdr = (ZDB_BLK_HDR*) acl_mymalloc(sizeof(ZDB_BLK_HDR));
	off = BLK_HDR_OFF(store, blk->hdr.inext_busy);
	ret = ZDB_READ((ZDB_STORE*) store, blk_hdr, sizeof(ZDB_BLK_HDR), off);
	if (ret == -1) {
		acl_msg_error("%s(%d): zdb_read %s error(%s)",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			acl_last_serror());
		RETURN (-1);
	} else if (blk_hdr->blk_ilnk != blk->hdr.inext_busy) {
		acl_msg_error("%s(%d): blk_ilnk(%d) != inext_busy(%d)",
			myname, __LINE__, blk_hdr->blk_ilnk,
			blk->hdr.inext_busy);
		RETURN (-1);
	}

	blk_hdr->iprev_busy = blk->hdr.iprev_busy;
	ret = ZDB_WRITE((ZDB_STORE*) store, blk_hdr, sizeof(ZDB_BLK_HDR), off);
	if (ret == -1) {
		acl_msg_error("%s(%d): zdb_write %s error(%s)",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			acl_last_serror());
		RETURN (-1);
	}

	/* ����ǰһ��ռ�ÿ��ͷ��Ϣ */

	off = BLK_HDR_OFF(store, blk->hdr.iprev_busy);
	ret = ZDB_READ((ZDB_STORE*)store, blk_hdr, sizeof(ZDB_BLK_HDR), off);
	if (ret == -1) {
		acl_msg_error("%s(%d): zdb_read %s error(%s)",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			acl_last_serror());
		RETURN (-1);
	} else if (blk_hdr->blk_ilnk != blk->hdr.iprev_busy) {
		acl_msg_error("%s(%d): blk_ilnk(%d) != iprev_busy(%d)",
			myname, __LINE__, blk_hdr->blk_ilnk,
			blk->hdr.iprev_busy);
		RETURN (-1);
	}

	blk_hdr->inext_busy = blk->hdr.inext_busy;
	ret = ZDB_WRITE((ZDB_STORE*) store, blk_hdr, sizeof(ZDB_BLK_HDR), off);
	if (ret == -1) {
		acl_msg_error("%s(%d): zdb_write %s error(%s)",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			acl_last_serror());
		RETURN (-1);
	}

	blk->hdr.inext_busy = blk->hdr.iprev_busy = 1;
	RETURN (0);
}

#endif  /* ZDB_LINK_BUSY */

/**
 * ���Ӳ���ʼ��ֵ�洢�����ݿ�
 * @param store {ZDB_DAT_STORE*} ֵ�洢���
 * @param count {acl_int64} ���ӵ����ݿ����
 * @return {int} 0: ok; -1: error
 */
static int dat_store_blk_add(ZDB_DAT_STORE *store, acl_int64 count)
{
	const char *myname = "dat_store_blk_add";
	ZDB_BLK *blk;
	acl_int64 i;
	int   ret;

	/* ��������Ƿ�Ϸ� */
	if (count <= 0) {
		acl_msg_error("%s(%d): count(" ACL_FMT_I64D ") invalid",
			myname, __LINE__, count);
		return (-1);
	}

	/* ����Ƿ��Ѿ��ﵽ�������Ƹ��� */
	if (store->hdr.size >= store->hdr.limit) {
		acl_msg_error("%s(%d): store(%s)'s size(" ACL_FMT_I64D
			") >= limit(" ACL_FMT_I64D ")",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			store->hdr.size, store->hdr.limit);
		return (-1);
	}

	count += store->hdr.size;  /* �� count ��Ϊ���ֵ */
	if (count > store->hdr.limit)
		count = store->hdr.limit;  /* ���뱣֤���ܳ������� */

	if ((((ZDB_STORE*) store)->db->oflags & ZDB_FLAG_CACHE_DAT) == 0) {
		/* ���ļ�ָ����β */
		if (acl_vstream_fseek(((ZDB_STORE*) store)->fhandle.fp, 0, SEEK_END) < 0) {
			acl_msg_error("%s(%d): fseek %s error(%s)",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				acl_last_serror());
			return (-1);
		}
	}

	/* ������ʵ�� ZDB_BLK �Ŀռ��С������һ���µ� ZDB_BLK ���� */
	blk = (ZDB_BLK*) acl_mycalloc(1, (size_t) BLK_LEN(store));

	/* ˳���ʼ��ֵ�洢�еĸ������ݿ�, ���γɵ����� */
	
	if ((((ZDB_STORE*) store)->db->oflags & ZDB_FLAG_CACHE_DAT) == 0) {
		for (i = store->hdr.size; i < count; i++) {
			blk->hdr.inext_idle = (zdb_lnk_t) i + 1;
			/* ���ﵽ���ֵʱ��ʾ��һ��λ����Ч */
			if (blk->hdr.inext_idle == count)
				blk->hdr.inext_idle = -1;
			blk->hdr.blk_ilnk = (zdb_lnk_t) i;  /* ��ʶ�Լ�������λ�ú�, У���� */
			blk->hdr.inext_idle = store->hdr.ihead_idle;  /* �������� */
			blk->hdr.key = -1;  /* ���еĳ�ʼ��Ϊ -1 */

#ifdef	ZDB_LINK_BUSY
			blk->hdr.inext_busy = -1;
			blk->hdr.iprev_busy = -1;
#endif

			store->hdr.ihead_idle = (zdb_lnk_t) i;  /* ����ֵ�洢ͷ��ͷ����ָ�� */

			ret = acl_vstream_buffed_writen(((ZDB_STORE*) store)->fhandle.fp,
					blk, (size_t) BLK_LEN(store));
			if (ret == ACL_VSTREAM_EOF) {
				acl_myfree(blk);
				acl_msg_error("%s(%d): write to %s error(%s)", myname,
					__LINE__, STORE_PATH((ZDB_STORE*) store),
					acl_last_serror());
				return (-1);
			}
		}

	} else {
		for (i = store->hdr.size; i < count; i++) {
			blk->hdr.inext_idle = (zdb_lnk_t) i + 1;
			/* ���ﵽ���ֵʱ��ʾ��һ��λ����Ч */
			if (blk->hdr.inext_idle == count)
				blk->hdr.inext_idle = -1;
			blk->hdr.blk_ilnk = (zdb_lnk_t) i;  /* ��ʶ�Լ�������λ�ú�, У���� */
			blk->hdr.inext_idle = store->hdr.ihead_idle;  /* �������� */
			blk->hdr.key = -1;  /* ���еĳ�ʼ��Ϊ -1 */

#ifdef	ZDB_LINK_BUSY
			blk->hdr.inext_busy = -1;
			blk->hdr.iprev_busy = -1;
#endif

			store->hdr.ihead_idle = (zdb_lnk_t) i;  /* ����ֵ�洢ͷ��ͷ����ָ�� */

			ret = ZDB_WRITE((ZDB_STORE*) store, blk, (size_t) BLK_HDR_LEN(store),
					BLK_HDR_OFF(store, i));  /* ֻ���ʼ����ͷ��Ϣ */
			if (ret == ACL_VSTREAM_EOF) {
				acl_myfree(blk);
				acl_msg_error("%s(%d): write to %s error(%s)", myname,
					__LINE__, STORE_PATH((ZDB_STORE*) store),
					acl_last_serror());
				return (-1);
			}
		}
	}

	acl_myfree(blk);

	if ((((ZDB_STORE*) store)->db->oflags & ZDB_FLAG_CACHE_DAT) == 0) {
		/* ˢ��д�Ļ����������� */
		if (acl_vstream_fflush(((ZDB_STORE*)store)->fhandle.fp) == ACL_VSTREAM_EOF) {
			acl_msg_error("%s(%d): fflush to %s error %s",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
				acl_last_serror());
			return (-1);
		}
	}

	store->hdr.size = count;  /* ����ֵ�洢�ܷ�������ݿ���� */
	return (0);
}

/**
 * ��ʼ��ֵ�洢
 * @param store {ZDB_DAT_STORE*} ֵ�洢���
 * @param db {ZDB*} ZDB ���
 * @return {int} 0: ok; -1: error
 */
static int dat_store_init(ZDB_DAT_STORE *store, ZDB *db)
{
	const char *myname = "dat_store_init";
	int   ret;

	/* ��ʼ��ֵ�洢��ͷ��Ϣ */

	store->hdr.limit = db->dat_limit;
	store->hdr.nstep = db->dat_nstep > 0 ? db->dat_nstep : (int) db->dat_limit;
	store->hdr.size = 0;
	store->hdr.count = 0;
	store->hdr.blk_hdr_dlen = sizeof(ZDB_BLK_HDR);
	store->hdr.blk_dlen = db->blk_dlen;
	store->hdr.blk_count = db->blk_count_tmp;
	store->hdr.ihead_idle = -1;  /* ָ��һ����λ�� */

#ifdef	ZDB_LINK_BUSY
	store->hdr.ihead_busy = -1;  /* û��ռ�����ݿ� */
	store->hdr.itail_busy = -1;  /* û��ռ�����ݿ� */
#endif

	if ((((ZDB_STORE*) store)->db->oflags & ZDB_FLAG_CACHE_DAT) == 0)
		ret = acl_vstream_buffed_writen(((ZDB_STORE*) store)->fhandle.fp,
			&store->hdr, sizeof(store->hdr));
	else
		ret = ZDB_WRITE((ZDB_STORE*) store, &store->hdr, sizeof(store->hdr), 0);

	if (ret == ACL_VSTREAM_EOF) {
		acl_msg_error("%s(%d): write header to %s error(%s)",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			acl_last_serror());
		return (-1);
	}
 
	return (0);
}

/**
 * ��ֵ�洢�ļ����ʱ�Ļص�����
 * @param fh {ACL_FHANDLE*} �ļ����
 * @param arg {void*} ����
 * @return {int} 0: ok; -1: error
 */
static int dat_store_on_open(ACL_FHANDLE *fh, void *arg)
{
	const char *myname = "dat_store_on_open";
	ZDB_DAT_STORE *store = (ZDB_DAT_STORE*) fh;
	ZDB *db = (ZDB*) arg;
	int   ret;

	if (fh->size != sizeof(ZDB_DAT_STORE))
		acl_msg_fatal("%s: fh->size(%d) != ZDB_DAT_STORE's size(%d)",
			myname, (int) fh->size, (int) sizeof(ZDB_DAT_STORE));
	((ZDB_STORE*) store)->db = db;
	((ZDB_STORE*) store)->flag = STORE_FLAG_DAT;
	if ((db->oflags & ZDB_FLAG_SLICE_DAT))
		((ZDB_STORE*) store)->flag |= STORE_FLAG_IO_SLICE;

	/* ��������ļ����ʼ�� */

	if (fh->fsize == 0) {
		acl_debug(ZDB_DBG_DAT, 1) ("%s: begin init %s ...",
			myname, STORE_PATH((ZDB_STORE*) store));
		if (dat_store_init(store, db) < 0) {
			acl_msg_error("%s(%d): %s, dat_store_init error",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store));
			return (-1);
		}
		acl_debug(ZDB_DBG_DAT, 1) ("%s: init %s ok",
			myname, STORE_PATH((ZDB_STORE*) store));

		if ((db->oflags & ZDB_FLAG_CACHE_DAT) != 0) {
			((ZDB_STORE*) store)->cache_max = db->dat_cache_max;
			((ZDB_STORE*) store)->cache_timeout = db->dat_cache_timeout;
			((ZDB_STORE*) store)->wback_max = db->dat_wback_max;
			zdb_io_cache_open((ZDB_STORE*) store, (size_t) BLK_LEN(store));
		}

		/* ��ʼ��Ӳ���ʼ��ֵ�洢�е����ݿ� */
		if (dat_store_blk_add(store, store->hdr.nstep) < 0) {
			acl_msg_error("%s(%d): dat_store_blk_add error",
				myname, __LINE__);
			return (-1);
		}
		zdb_dat_iter_set(store, 1);
		return (0);
	}

	/* ����Ǿ��ļ����ȡ�ļ�ͷ��Ϣ, ��Ӧ���м��� */

	if ((((ZDB_STORE*) store)->db->oflags & ZDB_FLAG_CACHE_DAT) == 0)
		ret = acl_vstream_readn(((ZDB_STORE*) store)->fhandle.fp,
				&store->hdr, sizeof(store->hdr));
	else
		ret = ZDB_READ((ZDB_STORE*) store, &store->hdr, sizeof(store->hdr), 0);

	if (ret == ACL_VSTREAM_EOF) {
		acl_msg_error("%s(%d): %s, read key header from %s error(%s)",
			myname, __LINE__, __FILE__, STORE_PATH((ZDB_STORE*) store), acl_last_serror());
		return (-1);
	}

	if ((db->oflags & ZDB_FLAG_CACHE_DAT) != 0) {
		((ZDB_STORE*) store)->cache_max = db->dat_cache_max;
		((ZDB_STORE*) store)->cache_timeout = db->dat_cache_timeout;
		((ZDB_STORE*) store)->wback_max = db->dat_wback_max;
		zdb_io_cache_open((ZDB_STORE*) store, (size_t) BLK_LEN(store));
	}

	zdb_dat_iter_set(store, 1);
	return (0);
}

/**
 * �ر�ֵ�洢�ļ����ʱ�Ļص�����
 * @param fh {ACL_FHANDLE*} �ļ����
 */
static void dat_store_on_close(ACL_FHANDLE *fh)
{
	const char *myname = "dat_store_on_close";
	ZDB_DAT_STORE *store = (ZDB_DAT_STORE*) fh;

	dat_store_header_sync(store);
	zdb_io_cache_close((ZDB_STORE*) store);
	acl_debug(ZDB_DBG_DAT, 2) ("%s(%d): sync header ok, close %s now",
		myname, __LINE__, STORE_PATH((ZDB_STORE*) store));
}

void zdb_dat_store_close(ZDB_DAT_STORE *store)
{
	acl_fhandle_close(&((ZDB_STORE*) store)->fhandle, 120);
}

ZDB_DAT_STORE *zdb_dat_store_open(ZDB *db, const char *filepath)
{
	const char *myname = "zdb_dat_store_open";
	ZDB_DAT_STORE *store;
	unsigned int oflags = ACL_FHANDLE_O_NOATIME /* | ACL_FHANDLE_O_DIRECT */;

	if ((db->oflags & ZDB_FLAG_OPEN_LOCK) != 0)
		oflags |= ACL_FHANDLE_O_MLOCK;

	acl_vstring_strcpy(db->path_tmp, filepath);
	store = (ZDB_DAT_STORE*) acl_fhandle_open(sizeof(ZDB_DAT_STORE), oflags,
			filepath, dat_store_on_open, db, dat_store_on_close);
	if (store == NULL)
		acl_msg_error("%s(%d): open file(%s) error(%s)",
			myname, __LINE__, filepath, acl_last_serror());
	return (store);
}

ZDB_BLK *zdb_dat_get(ZDB *db, const ZDB_BLK_OFF *blk_off, zdb_key_t *key, size_t *size)
{
	const char *myname = "zdb_dat_get";
	int   idisk, inode, ifile, ret, blk_dlen;
	ZDB_DAT_STORE *store;
	const ACL_VSTRING *path;
	zdb_lnk_t blk_ilnk;
	zdb_off_t off;
	ZDB_BLK *blk;

	idisk = (blk_off->inode >> DIR_BITS) & DISK_MASK;
	if (idisk < 0 || idisk > DISK_LIMIT) {
		acl_msg_error("%s(%d): idisk(%d) invalid",
			myname, __LINE__, idisk);
		return (NULL);
	}

	inode = blk_off->inode & DIR_MASK;
	if (inode < 0 || inode > DIR_LIMIT) {
		acl_msg_error("%s(%d): inode(%d) invalid",
			myname, __LINE__, inode);
		return (NULL);
	}

	/* �ж���ֵ�洢�е�ƫ��ֵ */
	if (blk_off->offset < 0) {
		acl_msg_error("%s(%d): blk_off_old(" ACL_FMT_I64D ") invalid",
			myname, __LINE__, blk_off->offset);
		return (NULL);
	}

	ifile = (int) blk_off->offset / (int) db->dat_limit;  /* �ļ����� */

	path = dat_filepath(db, NULL, idisk, inode, ifile);
	store = zdb_dat_store_open(db, STR(path));
	if (store == NULL) {
		acl_msg_error("%s(%d): open %s error(%s)",
			myname, __LINE__, STR(path), acl_last_serror());
		return (NULL);
	}

	/* �������ļ��е���ʵλ�� */

	blk_ilnk = (int) blk_off->offset - (zdb_lnk_t) db->dat_limit * ifile;  /* �ļ�λ������ֵ */
	off = BLK_HDR_OFF(store, blk_ilnk);

	/* ������ʵ�� ZDB_BLK �Ŀռ��С */

	blk_dlen = (int) BLK_LEN(store);

	/* ����һ�� ZDB_BLK ���� */

	blk = (ZDB_BLK*) acl_mycalloc(1, blk_dlen);

	ret = ZDB_READ((ZDB_STORE*) store, blk, blk_dlen, off);

	acl_debug(ZDB_DBG_GETD, 2)
		("%s(%d): %s, zdb_read(%d), blk_off: " ACL_FMT_I64D
		", idisk: %d, inode: %d,"
		 " ifile: %d, ilnk: %d, dat_limit: " ACL_FMT_I64D ", blk_dlen: %d",
		 myname, __LINE__, STORE_PATH((ZDB_STORE*) store), ret, off,
		 idisk, inode, ifile, blk_ilnk, db->dat_limit, blk_dlen);

	if (ret == -1) {
		acl_msg_error("%s(%d): zdb_read %s error(%s), blk_off("
			ACL_FMT_I64D "), blk_dlen(%d), idisk: %d, inode: %d, ifile: %d",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			acl_last_serror(), off, blk_dlen,
			idisk, inode, ifile);
		zdb_blk_free(blk);
		zdb_dat_store_close(store);  /* �ر�ֵ�洢 */
		return (NULL);
	}

	zdb_dat_store_close(store);  /* �ر�ֵ�洢 */

	if (key)
		*key = blk->hdr.key;
	if (size)
		*size = (size_t) (BLK_LEN(store) - BLK_HDR_LEN(store));
	return (blk);
}

int zdb_dat_add(ZDB *db, zdb_key_t key, const void *dat, int len)
{
	const char *myname = "zdb_dat_add";
	ZDB_DAT_STORE *store = NULL, *store_tmp;
	int   n, blk_dlen, idisk, inode, ifile;
	ZDB_BLK *blk = NULL;
	ZDB_BLK_OFF blk_off;
	zdb_off_t off;
	zdb_lnk_t ilnk;
	const ACL_VSTRING *path;

#undef	RETURN
#define	RETURN0(x) do { \
	return ((x)); \
} while (0)

#define	RETURN(x) do {  \
	if (store)  \
		zdb_dat_store_close(store);  \
	if (blk)  \
		zdb_blk_free(blk);  \
	return ((x));  \
} while (0)

	inode = dat_inode(db, len);
	if (inode < 0) {
		acl_msg_error("%s(%d): inode(%d) invalid, dat len(%d)",
			myname, __LINE__, inode, len);
		RETURN (-1);
	}

	/* xxx: ��Ϊÿ��Ŀ¼��ֵ���Ŀ¼��ֵ�洢�������е����ݿ������� */
	db->blk_count_tmp = inode;  /* xxx: ���ô���ʱ����ҪΪ�˲������� */

	/* ���Ա����ô洢Ŀ¼�����п��õĴ洢�ļ�: �п����Ǿ��ļ�, Ҳ�п��������ļ� */
	
	idisk = zdb_disk_select(db);
	if (idisk < 0) {
		acl_msg_error("%s(%d): no disk available", myname, __LINE__);
		RETURN (-1);
	}

	dat_ifiles_space(db, idisk, inode);  /* ȷ�� db->dat_ifiles ����ռ乻�� */
	ifile = db->dat_disks[idisk].dat_ifiles[inode];
	if (ifile < 0)
		ifile = 0;

	/* ѡ����õ�ֵ�洢��� */

	for (; ifile < ZDB_DAT_FILE_LIMIT; ifile++) {
		path = dat_filepath(db, NULL, idisk, inode, ifile);
		acl_debug(ZDB_DBG_ADDD, 2) ("%s(%d): path(%s)",
			myname, __LINE__, STR(path));
		store_tmp = zdb_dat_store_open(db, STR(path));
		if (store_tmp == NULL) {
			acl_msg_error("%s(%d): zdb_dat_store_open %s error(%s)",
				myname, __LINE__, STR(path), acl_last_serror());
			RETURN (-1);
		}

		/* �Ƿ��п��ÿ��п�? */

		if (store_tmp->hdr.limit <= store_tmp->hdr.count) {
			/* �ر������˵�ֵ�洢 */
			zdb_dat_store_close(store_tmp);
			continue;
		}

		/* �����Ҫ����ֵ�洢�ռ������� */
		if (store_tmp->hdr.size <= store_tmp->hdr.count) {
			n = dat_store_blk_add(store_tmp, store_tmp->hdr.nstep);
			if (n < 0) {
				acl_msg_error("%s(%d): add blk to %s error",
					myname, __LINE__,
					STORE_PATH((ZDB_STORE*) store_tmp));
				zdb_dat_store_close(store_tmp);
				RETURN (-1);
			}
		}

		store = store_tmp;
		db->dat_disks[idisk].dat_ifiles[inode] = ifile;  /* ������ļ������� */
		break;
	}

	if (store == NULL) {
		acl_msg_error("%s(%d): too many ifile(%d), ZDB_DAT_FILE_LIMIT(%d),"
			" idisk(%d), inode(%d), path(%s)", myname, __LINE__, ifile,
			ZDB_DAT_FILE_LIMIT, idisk, inode,
			acl_sane_dirname(NULL, STR(dat_path(db, NULL, idisk, inode))));
		RETURN (-1);
	}

	if (store->hdr.ihead_idle < 0) {
		acl_msg_error("%s(%d): %s, ihead_idle(%d) invalid, limit("
			ACL_FMT_I64D "), count(" ACL_FMT_I64D ")", myname,
			__LINE__, STORE_PATH((ZDB_STORE*) store),
			store->hdr.ihead_idle, store->hdr.limit,
			store->hdr.count);
		RETURN (-1);
	}

	/* ʹ�õ�һ�����п� */
	ilnk = store->hdr.ihead_idle;

	/* ������ʵ�� ZDB_BLK �Ŀռ��С */
	blk_dlen = (int) BLK_LEN(store);

	/* �����һ�����п������λ�� */
	off = BLK_HDR_OFF(store, ilnk);

	acl_debug(ZDB_DBG_ADDD, 2)
		("%s(%d): blk_count: %d, blk_dlen: %d, blk_off(" ACL_FMT_I64D
		"), blk_hdr_dlen: %d, len: %d", myname, __LINE__,
		 store->hdr.blk_count, store->hdr.blk_dlen,
		off, store->hdr.blk_hdr_dlen, len);

	/* ����һ�� ZDB_BLK ���� */
	blk = (ZDB_BLK*) acl_mymalloc(blk_dlen);
	if (blk == NULL)
		acl_msg_fatal("%s(%d): calloc error(%s)",
			myname, __LINE__, acl_last_serror());

	/* ��ȡ�ÿ������ݿ�ͷ��Ϣ */
	n = ZDB_READ((ZDB_STORE*) store, blk, (size_t) BLK_HDR_LEN(store), off);
	if (n == -1) {
		acl_msg_error("%s(%d): zdb_read %s error(%s), ihead_idle(%d),"
			" blk_dlen(%d), blk_off(" ACL_FMT_I64D "), blk_hdr_dlen(%d)",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			acl_last_serror(), ilnk, blk_dlen, off,
			store->hdr.blk_hdr_dlen);
		RETURN (-1);
	}

	/* xxx: �� blk ��ʵ���ǵ�һ�����п�, �μ����� ilnk ���γ� */

	blk->hdr.key = key;
	store->hdr.ihead_idle = blk->hdr.inext_idle;  /* ���¿��п���ͷ */
	blk->hdr.inext_idle = -1;  /* �ӿ��п����з��� */

#ifdef	ZDB_LINK_BUSY
	if ((db->oflags & ZDB_FLAG_LINK_BUSY) != 0) {
		if (busy_blk_link(store, blk, ilnk) < 0)
			RETURN (-1);
	}
#endif

	acl_debug(ZDB_DBG_ADDD, 2)
		("%s(%d): blk_off: " ACL_FMT_I64D ", inext: %d, key: "
		ACL_FMT_I64D, myname, __LINE__, off, ilnk, key);

	/* ���µ�ǰ�������ݿ���Ϣ */

	memcpy(blk->dat, dat, len);  /* ����Դ�������� */

	n = ZDB_WRITE((ZDB_STORE*) store, blk, blk_dlen, off);  /* �������ݿ���Ϣ */
	if (n == -1) {
		acl_msg_error("%s(%d): zdb_write to %s error(%s)",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			acl_last_serror());
		RETURN (-1);
	}

	/* ����ֵ�洢ͷ��Ϣ */

	store->hdr.count++;

	/* �� blk_off ӳ��Ϊ���洢�е�ֵ */

	blk_off.offset = ilnk + db->dat_limit * ifile;  /* �ۼ�֮ǰ���е��ļ��м�¼��ֵ����֮�� */
	if (blk_off.offset < 0) {
		acl_msg_error("%s(%d): blk_off.offset(" ACL_FMT_I64D
			") too large", myname, __LINE__, blk_off.offset);
		RETURN (-1);
	}

	acl_debug(ZDB_DBG_ADDD, 2)
		("%s(%d): blk_off: " ACL_FMT_I64D ", ihead_idle: %d, dat_limit: "
		ACL_FMT_I64D ", inode: %d, ifile: %d",
		myname, __LINE__, blk_off.offset, store->hdr.ihead_idle,
		db->dat_limit, inode, ifile);

	/* �� idisk �� inode ��ϴ洢 */
	blk_off.inode = (idisk << DIR_BITS) + inode;

	/* ���¼��洢����������ֵ */
	if (db->key_set(db, key, &blk_off) < 0) {
		acl_msg_error("%s(%d): set key store error(%s)",
			myname, __LINE__, acl_last_serror());
		RETURN (-1);
	}

	RETURN (1);
}

int zdb_dat_update(ZDB *db, zdb_key_t key, const ZDB_BLK_OFF *blk_off,
	const void *dat, size_t len)
{
	const char *myname = "zdb_dat_update";
	int   idisk, inode, inode_new, ifile, ret;
	const ACL_VSTRING *path;
	ZDB_DAT_STORE *store = NULL;
	zdb_lnk_t  ilnk;
	zdb_off_t  off;
	ZDB_BLK *blk = NULL;

#undef	RETURN
#define	RETURN(x) do {  \
	if (store)  \
		zdb_dat_store_close(store);  \
	if (blk)  \
		zdb_blk_free(blk);  \
	return (x);  \
} while (0)

	idisk = (blk_off->inode >> DIR_BITS) & DISK_MASK;
	if (idisk < 0 || idisk > DISK_LIMIT) {
		acl_msg_error("%s(%d): idisk(%d) invalid",
			myname, __LINE__, idisk);
		RETURN (-1);
	}

	inode = blk_off->inode & DIR_MASK;
	if (inode < 0 || inode > DIR_LIMIT) {
		acl_msg_error("%s(%d): inode(%d) invalid",
			myname, __LINE__, inode);
		RETURN (-1);
	}

	/* �ж���ֵ�洢�е�ƫ��ֵ */
	if (blk_off->offset < 0) {
		acl_msg_error("%s(%d): blk_off_old(" ACL_FMT_I64D
			") invalid", myname, __LINE__, blk_off->offset);
		RETURN (-1);
	}

	/* �����µ� inode ���·���� */
	inode_new = dat_inode(db, len);
	if (inode_new < 0) {
		acl_msg_error("%s(%d): inode(%d) invalid, dat len(%d)",
			myname, __LINE__, inode_new, (int) len);
		RETURN (-1);
	}

	ifile = (int) blk_off->offset / (int) db->dat_limit;  /* �ļ����� */

	/* �ļ�λ������ֵ */
	ilnk = (zdb_lnk_t) (blk_off->offset - db->dat_limit * ifile);

	path = dat_filepath(db, NULL, idisk, inode, ifile);
	store = zdb_dat_store_open(db, STR(path));
	if (store == NULL) {
		acl_msg_error("%s(%d): open %s error(%s)",
			myname, __LINE__, STR(path), acl_last_serror());
		RETURN (-1);
	}

	if (inode_new == inode) {
		/* ���¾�λ�õ�������Ϣ */

		/* ������ʵ�Ĵ洢λ����������ʼλ��ֵ */
		off = BLK_DAT_OFF(store, ilnk);

		/* д�������� */
		ret = ZDB_WRITE((ZDB_STORE*) store, dat, len, off);

		if (ret == -1) {
			acl_msg_error("%s(%d): zdb_write to %s error(%s)",
				myname, __LINE__, STR(path), acl_last_serror());
			RETURN (-1);
		}
		RETURN (1);
	}

	/* ��Ҫ�ƶ����ݿ�����������ֵ�洢�� */

	/* ��ֵ�洢�п϶�����ռ�����ݿ� */

	if (store->hdr.count <= 0) {
		acl_msg_error("%s(%d): %s, store->hdr.count(" ACL_FMT_I64D
			") <= 0, ihead_idle(%d), inode(%d, %d), inode_new(%d, %d),"
			" key(" ACL_FMT_I64D "), blk_off(" ACL_FMT_I64D ")",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			store->hdr.count, store->hdr.ihead_idle,
			idisk, inode, inode_new >> DISK_BITS,
			inode_new & DIR_MASK, key, blk_off->offset);
		store->hdr.count = 0;  /* xxx: reset to 0 */
		RETURN (-1);
	}

	/* ������ʵ�Ĵ洢λ�������ݿ�ͷ��ʼλ��ֵ */
	off = BLK_HDR_OFF(store, ilnk);

	/* ��Ҫ�ƶ�ֵ��λ����ʹ��λ�ñ�Ϊ���п� */

	blk = (ZDB_BLK*) acl_mymalloc((size_t) BLK_LEN(store));  /* �������ݿ� */

	/* ��ȡ��ռ�����ݿ��ͷ����Ϣ */
	ret = ZDB_READ((ZDB_STORE*) store, blk, (size_t) BLK_HDR_LEN(store), off);
	if (ret == -1) {
		acl_msg_error("%s(%d): zdb_read %s error(%s),"
			" blk_hdr_dlen(%d), blk_off(" ACL_FMT_I64D ")",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store),
			acl_last_serror(), (int) BLK_HDR_LEN(store), blk_off->offset);
		RETURN (-1);
	}

	blk->hdr.key = -1;
	blk->hdr.inext_idle = store->hdr.ihead_idle;  /* ��������ݿ������� */

#ifdef	ZDB_LINK_BUSY
	if ((db->oflags & ZDB_FLAG_LINK_BUSY) != 0) {
		if (busy_blk_unlink(store, blk, ilnk) < 0) {
			acl_msg_error("%s(%d): %s, unlink busy blk error",
				myname, __LINE__, STORE_PATH((ZDB_STORE*) store));
			RETURN (-1);
		}
	}
#endif

	/* ����ֵ�洢ͷ��Ϣ */

	store->hdr.ihead_idle = ilnk;
	store->hdr.count--;

	/* ����������ļ������� */

	dat_ifiles_space(db, idisk, inode);
	if (inode < db->dat_disks[idisk].dat_ifiles[inode])
		db->dat_disks[idisk].dat_ifiles[inode] = inode;

	/* ֻд���ݿ��ͷ����Ϣ, �������ݿ��Ϊ���п� */
	ret = ZDB_WRITE((ZDB_STORE*) store, blk, (size_t) BLK_HDR_LEN(store), off);
	if (ret == -1) {
		acl_msg_error("%s(%d): prwrite %s error(%s), blk_off("
			ACL_FMT_I64D ", " ACL_FMT_I64D ")",
			myname, __LINE__, STR(path),
			acl_last_serror(), blk_off->offset, off);
		RETURN (-1);
	}

	db->dat_disks[idisk].count--;  /* �����ڴ��̷����ķ��������� 1 */
	if (db->dat_disks[idisk].count < 0) {
		acl_msg_error("%s(%d): count(%lld) < 0 in %s",
			myname, __LINE__, db->dat_disks[idisk].count,
			db->dat_disks[idisk].path);
		RETURN (-1);
	}

	/* ����һ��ֵ�洢������¼�¼ */

	ret = zdb_dat_add(db, key, dat, len);
	RETURN (ret);
}

int zdb_dat_stat(ZDB *db, const char *filepath, ZDB_DAT_HDR *dat_hdr)
{
	const char *myname = "zdb_dat_stat";
	ZDB_DAT_STORE *store;

	store = zdb_dat_store_open(db, filepath);
	if (store == NULL) {
		acl_msg_error("%s(%d): open %s error(%s)",
			myname, __LINE__, filepath, acl_last_serror());
		return (-1);
	}

	memcpy(dat_hdr, &store->hdr, sizeof(ZDB_DAT_HDR));
	zdb_dat_store_close(store);
	return (0);
}

int zdb_dat_check(ZDB_DAT_STORE *store, ZDB_DAT_HDR *dat_hdr)
{
	const char *myname = "zdb_dat_check";
	acl_int64 nidle = 0, nused = 0;
	ZDB_BLK *blk;
	zdb_lnk_t ilnk;
	zdb_off_t blk_off;
	int   blk_len, ret, failed = 0;
	ACL_ITER iter;
	time_t begin;

	acl_msg_info("%s(%d): %s: checking ......",
		myname, __LINE__, STORE_PATH((ZDB_STORE*) store));
	acl_msg_info("%s(%d):\tHDR STATUS:", myname, __LINE__);
	acl_msg_info("%s(%d):\tlimit: " ACL_FMT_I64D ", size: "
		ACL_FMT_I64D ", count: " ACL_FMT_I64D,
		myname, __LINE__, store->hdr.limit,
		store->hdr.size, store->hdr.count);
	acl_msg_info("%s(%d):\tnstep: %d, blk_hdr_dlen: %d, blk_dlen: %d,"
		" blk_count: %d", myname, __LINE__, store->hdr.nstep,
		store->hdr.blk_hdr_dlen, store->hdr.blk_dlen,
		store->hdr.blk_count);
	acl_msg_info("%s(%d):\tihead_idle: %d, ihead_busy: %d",
		myname, __LINE__, store->hdr.ihead_idle, store->hdr.ihead_busy);

	if (dat_hdr)
		memcpy(dat_hdr, &store->hdr, sizeof(ZDB_DAT_HDR));

	/* ɨ�����б�ʹ�õ����ݿ��� */

	acl_msg_info("%s(%d):\tBegin check used blk ......", myname, __LINE__);

	time(&begin);

	/* ��������ݿ��ͷ */
	zdb_dat_iter_set(store, 1);
	acl_foreach(iter, (ZDB_STORE*) store) {
		nused++;
		if (nused > 0 && nused % 10000 == 0) {
			ZDB_BLK_HDR *blk_hdr = (ZDB_BLK_HDR*) iter.data;

			printf("\tnused: " ACL_FMT_I64D ", key: "
				ACL_FMT_I64D ", dlen: %d, ",
				nused, blk_hdr->key, iter.dlen);
			ACL_METER_TIME("-");
		}
	}

	if (nused != store->hdr.count) {
		acl_msg_error("%s(%d):\terror, nused(" ACL_FMT_I64D
			") != store->hdr.count(" ACL_FMT_I64D
			") for %s, please repair it!, time: %ld", myname, __LINE__,
			nused, store->hdr.count, STORE_PATH((ZDB_STORE*) store),
			(long) (time(NULL) - begin));
		failed = 1;
	} else {
		acl_msg_info("%s(%d):\tOk, check used blk over, busy blk: "
			ACL_FMT_I64D ", time: %ld",
			myname, __LINE__, store->hdr.count, time(NULL) - begin);
	}

	/* ����������ݿ�������� */
	zdb_dat_iter_set(store, 1);

	/* ��ʼɨ�����еĿ������ݿ��� */

	acl_msg_info("%s(%d):\tBegin check idle blk ......", myname, __LINE__);

	time(&begin);

	ret = 0;
	blk_len = (int) BLK_LEN(store);
	blk = (ZDB_BLK*) acl_mymalloc(blk_len);
	ilnk = store->hdr.ihead_idle;
	while (ilnk >= 0) {
		blk_off = BLK_HDR_OFF(store, ilnk);
		ret = ZDB_READ((ZDB_STORE*) store, blk, blk_len, blk_off);
		if (ret == -1) {
			acl_msg_error("%s(%d): zdb_read %s error(%s), blk_off("
				ACL_FMT_I64D ")", myname, __LINE__,
				STORE_PATH((ZDB_STORE*) store),
				acl_last_serror(), blk_off);
			failed = 1;
			break;
		}
		ilnk = blk->hdr.inext_idle;
		nidle++;
	}

	if (ret >= 0 && nidle == store->hdr.size - store->hdr.count)
		acl_msg_info("%s(%d):\tOk, check idle blk over , idle blk: "
			ACL_FMT_I64D ", time: %ld",
			myname, __LINE__, nidle, time(NULL) - begin);
	else {
		acl_msg_error("%s(%d):\tcheck idle blk error for %s, time: %ld",
			myname, __LINE__, STORE_PATH((ZDB_STORE*) store), time(NULL) - begin);
		acl_msg_error("%s(%d): \tret: %d, idle blk: " ACL_FMT_I64D
			", store->hdr.size - store->hdr.count: " ACL_FMT_I64D,
			myname, __LINE__, ret, nidle,
			store->hdr.size - store->hdr.count);
		failed = 1;
	}

	zdb_blk_free(blk);

	return (failed ? -1 : 0);
}

int zdb_dat_check3(ZDB *db, const char *filepath, ZDB_DAT_HDR *dat_hdr)
{
	const char *myname = "zdb_dat_check3";
	ZDB_DAT_STORE *store;
	int   ret;

	store = zdb_dat_store_open(db, filepath);
	if (store == NULL) {
		acl_msg_error("%s(%d): open %s error(%s)",
			myname, __LINE__, filepath, acl_last_serror());
		return (-1);
	}

	ret = zdb_dat_check(store, dat_hdr);
	zdb_dat_store_close(store);
	return (ret);
}
