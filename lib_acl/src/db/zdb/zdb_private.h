#ifndef	__ZDB_PRIVATE_INCLUDE_H__
#define	__ZDB_PRIVATE_INCLUDE_H__

#include "db/zdb.h"

#define	ZDB_DBG_BASE		500
#define	ZDB_DBG_GETK		(ZDB_DBG_BASE)
#define	ZDB_DBG_GETD		(ZDB_DBG_BASE + 1)
#define	ZDB_DBG_ADDD		(ZDB_DBG_BASE + 2)
#define	ZDB_DBG_KEY		(ZDB_DBG_BASE + 3)
#define	ZDB_DBG_DAT		(ZDB_DBG_BASE + 4)

#define	ZDB_KEY_LIMIT		(unsigned int) -1
#define	ZDB_DAT_FILE_LIMIT	10000	/* ֵ�洢Ŀ¼��ÿ��Ŀ¼�������ļ����� */
#define	ZDB_KEY_DIR_LIMIT	10	/* ���洢Ŀ¼���� */
#define	ZDB_DAT_DIR_LIMIT	10	/* ֵ�洢Ŀ¼���� */

typedef const void *(*STORE_ITER)(ACL_ITER*, ZDB_STORE*);

/*--------------------------- һЩ�򵥷���ĺ궨�� ---------------------------*/

#define	DISK_BITS	8
#define	DISK_MASK	0xff
#define	DISK_LIMIT	(1 << 8)

#define	DIR_BITS	24
#define	DIR_MASK	0xffffff
#define	DIR_LIMIT	(1 << 24)

/**
 * ȡ�� ACL_VSTRING �����е����ݵ�ַ
 * @param x {ACL_VSTRING*}
 * @return {char*}
 */
#ifndef	STR
#define	STR(x)	acl_vstring_str((x))
#endif

/**
 * ȡ�� ACL_VSTRING ���������ݳ���
 * @param x {ACL_VSTRING*}
 * @return {size_t} ���ݳ���
 */
#ifndef	LEN
#define	LEN(x)	ACL_VSTRING_LEN((x))
#endif

/**
 * ȡ�� ACL_VSTREAM �������ļ�·��
 * @param x {ACL_VSTREAM*}
 * @return {char*} �ļ�·��
 */
#define	PATH(x)	ACL_VSTREAM_PATH((x))

/**
 * ȡ�ô洢���ļ�������ļ�·��
 * @param s {ZDB_KEY_STORE* || ZDB_DAT_STORE*}
 * @return {char*} �ļ�·��
 */
#define	STORE_PATH(s)	PATH((s)->fhandle.fp)

/**
 * ȡ�ô洢���ļ�������
 * @param s {ZDB_KEY_STORE* || ZDB_DAT_STORE*}
 * @return {ACL_FILE_HANDLE} �ļ�������
 */
#define	STORE_FILE(s)	ACL_VSTREAM_FILE((s)->fhandle.fp)

/**
 * ��ô洢�ļ��� ACL_VSTREAM ��
 * @param s {ZDB_KEY_STORE* || ZDB_DAT_STORE*}
 * @return {ACL_VSTREAM*}
 */
#define	STORE_STREAM(s)	((s)->fhandle.fp)

/**
 * ���ݼ�ֵȡ�ü��洢�����·����
 * @param z {ZDB*}
 * @param k {zdb_key_t}
 * @return {int} ���·����
 */
#define	KEY_INODE(z, k)	(int) ((k) / (z)->key_limit)

/**
 * �ɼ�ֵ��������ֵȡ�øü�ֵ������
 * @param z {ZDB*}
 * @param k {zdb_key_t}
 * @return {zdb_key_t} ����
 */
#define	KEY_MOD(z, k) ((k) % (z)->key_limit)

/**
 * ���ݼ�ֵȡ�øü��ڼ��洢�ļ��е�ƫ��λ��
 * @param z {ZDB*}
 * @param k {zdb_key_t}
 * @return {zdb_off_t}
 */
#define	KEY_OFF(z, k)  (  \
	((k) - KEY_INODE((z), (k)) * (zdb_off_t) (z)->key_limit) \
	* (zdb_off_t) sizeof(ZDB_BLK_OFF)  \
	+ (zdb_off_t) sizeof(ZDB_KEY_HDR)  )

/**
 * ���ݿ�ͷ������
 * @param s {ZDB_DAT_STORE*}
 * @return {int}
 */
#define	BLK_HDR_LEN(s)	((zdb_off_t) (s)->hdr.blk_hdr_dlen)

/**
 * ���ݿ鳤��(��������ͷ����)
 * @param s {ZDB_DAT_STORE*}
 * @return {int}
 */
#define BLK_LEN(s)  (  \
	BLK_HDR_LEN((s)) +  \
	(zdb_off_t) (s)->hdr.blk_dlen * (zdb_off_t) (s)->hdr.blk_count  )

/**
 * �������ݿ�������ͷ��ֵ�洢�е�ƫ��λ��
 * @param s {ZDB_DAT_STORE*}
 * @param x {zdb_lnk_t} �ļ��洢λ��������
 * @return {zdb_off_t}
 */
#define	BLK_HDR_OFF(s, x)  (  \
	(zdb_off_t) sizeof(ZDB_DAT_HDR) + BLK_LEN((s)) * (zdb_off_t) ((x))  )

/**
 * �������ݿ������ݲ�����ֵ�洢�е�ƫ��λ��
 * @param s {ZDB_DAT_STORE*}
 * @param x {zdb_lnk_t} �ļ��洢λ��������
 * @return {zdb_off_t}
 */
#define	BLK_DAT_OFF(s, x)  \
	(BLK_HDR_OFF(s, x) + (zdb_off_t) sizeof(ZDB_BLK_HDR))

#ifdef	__cplusplus
extern "C" {
#endif

/*----------------------------   in zdb.c   ----------------------------------*/

/**
 * �Ӵ��̷�����ѡ����ʵķ����ڵ�
 * @param db {ZDB*}
 */
int zdb_disk_select(ZDB *db);

/*--------------------------------- in zdb_io.c ------------------------------*/

#undef	DEBUG_ZDB_RW

#ifdef	DEBUG_ZDB_RW
#define	ZDB_WRITE(s, buf, len, off)  (  \
    acl_msg_info("%s(%d), %s: call zdb_write(%s, %ld, %lld)\n",  \
	__FILE__, __LINE__, __FUNCTION__, STORE_PATH((s)),  \
	(size_t) len, (zdb_off_t) off), 1 ?  \
    zdb_write((s), (buf), (len), (off)) : ACL_VSTREAM_EOF  )

#define	ZDB_READ(s, buf, len, off)  (  \
    acl_msg_info("%s(%d), %s: call zdb_read(%s, %ld, %lld)\n",  \
	__FILE__, __LINE__, __FUNCTION__, STORE_PATH((s)),  \
	(size_t) len, (zdb_off_t) off), 1 ?  \
    zdb_read((s), (buf), (len), (off)) : ACL_VSTREAM_EOF  )
#else
#define	ZDB_WRITE	zdb_write
#define	ZDB_READ	zdb_read
#endif

int zdb_io_cache_sync(ZDB_STORE *store);
void zdb_io_cache_open(ZDB_STORE *store, size_t blk_len);
void zdb_io_cache_close(ZDB_STORE *store);

/**
 * ��װ�� pwrite64 ��д�ӿ�
 * @param store {ZDB_STORE*} �ļ����
 * @param buf {const void*} ���ݵ�ַ
 * @param len {size_t} ���ݳ���
 * @param off {zdb_off_t} �ļ��е�λ��ƫ����
 * @return {int} > 0: ok; -1: error
 */
int zdb_write(ZDB_STORE *store, const void *buf, size_t len, zdb_off_t off);

/**
 * ��װ�� pread64 �Ķ��ӿ�
 * @param store {ZDB_STORE*} �ļ����
 * @param buf {const void*} ��������ַ
 * @param len {size_t} ����������
 * @param off {zdb_off_t} �ļ��е�λ��ƫ����
 * @return {int} > 0: ok; -1: error
 */
int zdb_read(ZDB_STORE *store, void *buf, size_t size, zdb_off_t off);

#ifdef	__cplusplus
}
#endif

#endif
