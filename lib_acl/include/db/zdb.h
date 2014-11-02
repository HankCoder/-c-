#ifndef	ACL_ZDB_INCLUDE_H
#define	ACL_ZDB_INCLUDE_H

#include "stdlib/acl_define.h"
#include "stdlib/acl_vstring.h"
#include "stdlib/acl_fhandle.h"

typedef struct ZDB ZDB;
typedef struct ZDB_KEY_HDR ZDB_KEY_HDR;
typedef struct ZDB_BLK ZDB_BLK;
typedef struct ZDB_BLK_OFF ZDB_BLK_OFF;
typedef struct ZDB_DAT_HDR ZDB_DAT_HDR;
typedef struct ZDB_KEY_STORE ZDB_KEY_STORE;
typedef struct ZDB_DAT_STORE ZDB_DAT_STORE;
typedef struct ZDB_IO ZDB_IO;

/* ƫ�����ĳ������� */
typedef acl_int64 zdb_off_t;

/* �����Ͷ��� */
typedef acl_int64 zdb_key_t;

/* ֵ�洢�п������ݿ�����ƫ�������� */
typedef int zdb_lnk_t;

/* ���̷�����Ϣ���� */
typedef struct ZDB_DISK {
	acl_int64 limit;	/* �÷������������� */
	acl_int64 count;	/* Ŀǰ�÷����ѷ������� */
	char *path;		/* ���̷���·�� */
	int   idisk;		/* �ô��̷����� */
	int   priority;		/* Ȩ�����ȼ� */
	int  *dat_ifiles;	/* ��ǰ���õ� dat_ifile ֵ�洢�ļ������� */
	int   dat_ifiles_size;	/* dat_ifiles �Ĵ�С */
} ZDB_DISK;

/* ���� zdb_open() ʱ�� ZDB ���ö��� */
typedef struct ZDB_CFG {
	const char *key_path;	/* ���洢���ڵĸ�Ŀ¼ */
	zdb_key_t key_begin;	/* ��ֵ����ʼֵ */
	zdb_key_t key_limit;	/* ÿ�����洢�ļ��м��������� */
	acl_int64 dat_limit;	/* ÿ��ֵ�洢�ļ������ݵ������� */

	int   key_cache_max;	/* ���ü��洢 IO ����ʱ����󻺴����ݿ���� */
	int   key_cache_timeout; /* ���ü��洢 IO ����ʱÿ�������Ĺ���ʱ�� */
	int   key_wback_max;	/* ���ü��洢 IO ����ʱд�������󻺴����ݿ���� */

	int   dat_nstep;	/* ֵ�洢���������ݿ�ʱ�Ĳ���ֵ */
	int   blk_dlen;		/* ֵ�洢�����ݿ������ݲ��ֵĳ���(�����û����ݳ���) */
	int   dat_cache_max;	/* ����ֵ�洢 IO ����ʱ����󻺴����ݿ���� */
	int   dat_cache_timeout; /* ����ֵ�洢 IO ����ʱÿ�������Ĺ���ʱ�� */
	int   dat_wback_max;	/* ����ֵ�洢 IO ����ʱд�������󻺴����ݿ���� */
} ZDB_CFG;

/* ZDB �ṹ���� */
struct ZDB {
	/* public */

	char *dbname;		/* ���ݿ��� */
	char *key_path;		/* ZDB ��key�洢·�� */
	unsigned int oflags;	/* ��ʱ�ı�־λ */
#define	ZDB_FLAG_LINK_BUSY	(1 << 0)  /* ��ֵ�洢�е�ռ�����ݿ��������� */
#define	ZDB_FLAG_OPEN_LOCK	(1 << 1)  /* �Լ���ģʽ�򿪴洢��� */
#define	ZDB_FLAG_CACHE_DAT	(1 << 2)  /* �Ƿ񻺴�ֵ�洢������ */
#define	ZDB_FLAG_CACHE_KEY	(1 << 3)  /* �Ƿ񻺴���洢������ */
#define	ZDB_FLAG_SLICE_KEY	(1 << 4)  /* ����ֵ�洢ʱ�Ƿ�����ڴ���Ƭ��ʽ */
#define	ZDB_FLAG_SLICE_DAT	(1 << 5)  /* ���ü��洢ʱ�Ƿ�����ڴ���Ƭ��ʽ */

	unsigned int status;	/* ״̬λ */
#define	ZDB_STAT_KEY_NEW	(1 << 0)  /* �¼� */

	/* private */

	zdb_key_t key_begin;	/* ���м��洢����ʼֵ */
	zdb_key_t key_limit;	/* ÿ�����洢�м��ĸ��������ֵ */
	int   key_cache_max;	/* ���ü��洢 IO ����ʱ����󻺴����ݿ���� */
	int   key_cache_timeout; /* ���ü��洢 IO ����ʱÿ�������Ĺ���ʱ�� */
	int   key_wback_max;	/* ���ü��洢 IO ����ʱд�������󻺴����ݿ���� */

	acl_int64 dat_limit;	/* ֵ�洢��ֵ����洢�������� */
	int   blk_dlen;		/* ֵ�洢�����ݿ���ÿ�������鵥Ԫ���� */
	int   dat_nstep;	/* ֵ�洢��ÿ���������ݿ�ĸ��� */
	int   dat_cache_max;	/* ����ֵ�洢 IO ����ʱ����󻺴����ݿ���� */
	int   dat_cache_timeout; /* ����ֵ�洢 IO ����ʱÿ�������Ĺ���ʱ�� */
	int   dat_wback_max;	/* ����ֵ�洢 IO ����ʱд�������󻺴����ݿ���� */

	ZDB_DISK *dat_disks;	/* ֵ�洢���̷������� */

	int   (*key_get)(ZDB*, zdb_key_t, ZDB_BLK_OFF*);
	int   (*key_set) (ZDB*, zdb_key_t, const ZDB_BLK_OFF*);

	ZDB_BLK *(*dat_get)(ZDB*, const ZDB_BLK_OFF*, zdb_key_t*, size_t*);
	int   (*dat_add)(ZDB*, zdb_key_t, const void*, int);
	int   (*dat_update)(ZDB*, zdb_key_t, const ZDB_BLK_OFF*,
			const void*, size_t);

	/* private */

	/* ����Ϊ��ʱ���� */

	ACL_VSTRING *path_tmp;	/* ��ʱ�õĴ洢�ļ���ȫ·��, ��ҪΪ�˲������� */
	int   blk_count_tmp;	/* ��ʱ�õĿ���, ��ҪΪ�˲������� */
	int   inode_tmp;	/* ��ʱ�õ����·���ţ���ҪΪ�˲������� */
};

/* xxx: Ϊ�˱�֤��ƽ̨�ԣ����½ṹ���嶼��4�ֽڶ���� */

#ifdef	ACL_SUNOS5
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/* ֵ�洢ͷ */
struct ZDB_DAT_HDR {
	acl_int64  limit;	/* ֵ�洢��ֵ����洢�������� */
	acl_int64  size;	/* ��ǰֵ�洢�ļ��Ѿ�����Ķ���洢�ĸ��� */
	acl_int64  count;	/* ��ǰֵ�洢�е����� */
	acl_int64  reserv1;	/* �����ֶ� */
	acl_int64  reserv2;	/* �����ֶ� */
	acl_int64  reserv3;	/* �����ֶ� */
	acl_int64  reserv4;	/* �����ֶ� */
	int   nstep;		/* ÿ������ʱֵ�洢�ļ���Сʱ�Ĳ���ֵ��С */
	int   blk_hdr_dlen;	/* ֵ�洢��ÿ�����ݵ�ͷ������: ZDB_BLK_HDR.sizeof */
	int   blk_dlen;		/* ֵ�洢�����ݿ���ÿ�������鵥Ԫ���� */
	int   blk_count;	/* ��ֵ�洢�е����ݿ��л�������� */

	/* ��һ�����п��λ�õ���ʵ����λ�õļ��㹫ʽΪ:
	 * zdb_off_t off_head = ZDB_DAT_HDR.sizeof +
	 *     ZDB_DAT_HDR.ihead_idle * (ZDB_DAT_HDR.blk_hdr_dlen +
	 *         ZDB_DAT_HDR.blk_dlen * ZDB_DAT_HDR.blk_count);
	 */
	zdb_lnk_t ihead_idle;

	/* ��һ�����ݿ��λ�õ���ʵ����λ�õļ��㹫ʽΪ:
	 * zdb_off_t off_head = ZDB_DAT_HDR.sizeof +
	 *     ZDB_DAT_HDR.ihead_busy * (ZDB_DAT_HDR.blk_hdr_dlen +
	 *         ZDB_DAT_HDR.blk_dlen * ZDB_DAT_HDR.blk_count);
	 */
	zdb_lnk_t ihead_busy;
	zdb_lnk_t itail_busy;
	int   dummy;		/* ��֤8�ֽڶ��� */
};

/* ���洢��ͷ */
struct ZDB_KEY_HDR {
	zdb_key_t  key_limit;	/* �ü��洢��������� */
	zdb_key_t  key_count;	/* ��ǰ���м������� */
	zdb_key_t  key_begin;	/* ������ʼֵ */
	acl_int64  reserv1;	/* �����ֶ� */
	acl_int64  reserv2;	/* �����ֶ� */
	acl_int64  reserv3;	/* �����ֶ� */
	acl_int64  reserv4;	/* �����ֶ� */
};

/* �����ݵ�ͷ�ṹ���� */
typedef struct ZDB_BLK_HDR {
	zdb_key_t  key;		/* ��Ӧ�ڼ��洢�еļ� */

	/* ��ʶ�����ݿ������λ��, У����, ͬʱ��֤�� 8 �ֽڶ��� */
	zdb_lnk_t  blk_ilnk;

	/* �� inext_idle ���������ݿ����ӳ�һ����������, ����ֵΪ -1 ���ʾ
	 * �ǿ��п�, ��һ�����п�λ��, ��һ�����п����ʵ����λ�õļ��㹫ʽΪ:
	 * zdb_off_t off_next = ZDB_BLK_HDR.sizeof +
	 *     ZDB_BLK_HDR.inext_idle * (ZDB_BLK_HDR.blk_hdr_dlen +
	 *         ZDB_DAT_HDR.blk_dlen * ZDB_DAT_HDR.blk_count);
	 */
	zdb_lnk_t  inext_idle;

#ifdef	ZDB_LINK_BUSY
	/* �� inext_busy/iprev_busy ��ռ�����ݿ����ӳ�һ��˫������, ����ֵΪ -1
	 * ���ʾ��ռ�ÿ�, ��һ��ռ�ÿ�λ��, ��һ��ռ�ÿ����ʵ����λ�õļ��㹫ʽΪ:
	 * zdb_off_t off_next = ZDB_BLK_HDR.sizeof +
	 *     ZDB_BLK_HDR.inext_busy * (ZDB_BLK_HDR.blk_hdr_dlen +
	 *         ZDB_DAT_HDR.blk_dlen * ZDB_DAT_HDR.blk_count);
	 * zdb_off_t off_prev = ZDB_BLK_HDR.sizeof +
	 *     ZDB_BLK_HDR.inext_prev * (ZDB_BLK_HDR.blk_hdr_dlen +
	 *         ZDB_DAT_HDR.blk_dlen * ZDB_DAT_HDR.blk_count);
	 */
	zdb_lnk_t  inext_busy;
	zdb_lnk_t  iprev_busy;
#endif
} ZDB_BLK_HDR;

/* ���洢�д洢��ֵ�洢��ƫ�������� */
struct ZDB_BLK_OFF {
	zdb_off_t offset;	/* ƫ������Ϣ */
	int   inode;		/* ·����Ϣ */
};

/* �����ݽṹ���� */
struct ZDB_BLK {
	ZDB_BLK_HDR hdr;
	char  dat[1];		/* ֵ�洢��ÿ�����ݵ������� */
};

#ifdef	ACL_SUNOS5
#pragma pack(0)  /* ����ȡ��4�ֽڶ������� */
#else
#pragma pack(pop)  /* ����ȡ��4�ֽڶ������� */
#endif

/* ͨ�ô洢����ṹ */
typedef struct ZDB_STORE {
	ACL_FHANDLE fhandle;	/* �ļ���� */
	ZDB *db;		/* ���ö��� */
	ZDB_IO *io;		/* IO ������� */
	int   cache_max;	/* ���ô洢 IO ����ʱ����󻺴����ݿ���� */
	int   cache_timeout;	/* ���ô洢 IO ����ʱÿ�������Ĺ���ʱ�� */
	int   wback_max;	/* д���������ݿ������ */

	unsigned int flag;	/* ��־λ */
#define	STORE_FLAG_KEY		(1 << 0)  /* ��ʾ�Ǽ��洢 */
#define	STORE_FLAG_DAT		(1 << 1)  /* ��ʾ��ֵ�洢 */
#define	STORE_FLAG_IO_SLICE	(1 << 2)  /* �Ƿ����� ZDB_IO ���ڴ���Ƭ���䷽ʽ */

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	const void *(*iter_head)(ACL_ITER*, struct ZDB_STORE*);
	/* ȡ��������һ������ */
	const void *(*iter_next)(ACL_ITER*, struct ZDB_STORE*);
} ZDB_STORE;

/* ���洢 */
struct ZDB_KEY_STORE {
	ZDB_STORE store;
	ZDB_KEY_HDR hdr;	/* ���洢ͷ */
};


/* ֵ�洢 */
struct ZDB_DAT_STORE {
	ZDB_STORE store;
	ZDB_DAT_HDR hdr;	/* ֵ�洢ͷ */
};

#ifdef	__cplusplus
extern "C" {
#endif

/*-------------------------  in zdb_test.c  ----------------------------------*/

/**
 * zdb ���Ժ���
 */
ACL_API void zdb_test(const char *cmd);

/*----------------------------   in zdb.c   ----------------------------------*/

/* in zdb.c */

/**
 * ����ʼ����ʱ��Ҫ��ʼ�� zdb
 */
ACL_API void zdb_init(void);

/**
 * �����˳�ǰ��Ҫ�ͷ� zdb �ڲ�һЩ��Դ
 */
ACL_API void zdb_end(void);

/**
 * ����ZDB�����Ϣ������
 * @param db {ZDB*}
 */
ACL_API void zdb_sync(ZDB *db);

/**
 * �򿪻򴴽�һ�� ZDB ���ݿ�
 * @param dbname {const char*} ZDB ���ݿ�����
 * @param oflags {unsigned int} �� ZDB ��ʱ�ı�־λ
 * @param cfg {const ZDB_CFG*} �� ZDB ʱ�����ö���
 * @return {ZDB*} ZDB ���ݿ���
 */
ACL_API ZDB *zdb_open(const char *dbname, unsigned int oflags, const ZDB_CFG *cfg);

/**
 * �ر� ZDB ���ݿ���
 * @param db {ZDB*} ZDB ���ݿ���
 */
ACL_API void zdb_close(ZDB *db);

/**
 * �� ZDB ���ݿ��в�����Ӧ��ֵ������
 * @param db {ZDB*} ZDB ���ݿ���
 * @param key {zdb_key_t} ��ֵ
 * @param size {size_t*} ����ָ��ǿ��Ҳ�ѯ���Ҳ�ǿ���洢��ѯ��������ݳ���,
 *  ���� ZDB_BLK.dat �д洢���ݵĳ���
 * @param blk_off_buf {ZDB_BLK_OFF*} ���ǿ���洢����ֵ��ƫ��λ��������
 * @return {ZDB_BLK*} NULL: δ�ҵ�; !NULL: ZDB_BLK->dat Ϊ�û����ݵĵ�ַ,
 *  ���� ZDB_BLK ��������� acl_myfree()/1 �����ͷ�
 * ע��:
 *  �� size ָ���ַ�ǿ�ʱ�������ߵ����ͱ����� size_t ���Ͷ��� int ���ͣ���Ϊ��
 *  64λ���� size_t Ϊ8���ֽڳ��ȣ��� int Ϊ4���ֽڳ��ȣ��ڲ��ڶ� size ��ַ��ֵ
 *  ʱ�����������Զ��� size_t ����ʵ�ռ䳤�ȸ�ֵ����������ߵ� size Ϊ int ���ͣ�
 *  ��ͻ���ֿռ�Խ�����󣬴Ӷ���ʹ�����������Ī����������⣬������ valgrind
 *  Ҳ�鲻�����������!
 */
ACL_API ZDB_BLK *zdb_lookup(ZDB *db, zdb_key_t key, size_t *size, ZDB_BLK_OFF *blk_off_buf);

/**
 * �� ZDB_BLK ��ȡ���û�����
 * @param b {ZDB_BLK*}
 * @return {void*}
 */
#define	zdb_blk_data(b)	((b)->dat)

/**
 * �ͷ��� zdb_lookup()/3 ���ص����ݿռ�
 * @param b {ZDB_BLK*}
 */
#define	zdb_blk_free(b)	acl_myfree((b))

/**
 * ��ӻ���� ZDB ���ݿ��е�����
 * @param db {ZDB*} ZDB ���ݿ���
 * @param key {zdb_key_t} ��ֵ
 * @param blk_off_saved {const ZDB_BLK_OFF*} �ϴε��� zdb_lookup()/4 ʱ�ķ��ؽ��,
 *  �Ӷ�ͨ���ظ����ò�ѯ��������Ч��, ����ڵ��� zdb_lookup()/4 ʱ���ؽ��Ϊ��,
 *  ����� zdb_update()/5 ʱ���뽫��ֵ�� NULL
 * @param dat {const void*} �� key ����Ӧ�����ݵ�ַ
 * @param len {size_t} dat ���ݳ���
 * @return {int} 0: δ���»����; -1: ����; 1: �ɹ�
 */
ACL_API int zdb_update(ZDB *db, zdb_key_t key, const ZDB_BLK_OFF *blk_off_saved,
	const void *dat, size_t len);

/*--------------------------------- in zdb_key.c -----------------------------*/

/**
 * ͬ�����洢ͷ������
 * @param store {ZDB_KEY_STORE*} ֵ�洢���
 * @return {int} -1: error, > 0 ok
 */
ACL_API int key_store_header_sync(ZDB_KEY_STORE *store);

/**
 * ���ݼ�ֵ�򿪼��洢
 * @param db {ZDB*}
 * @param key {zdb_key_t} ��ֵ
 * @return {ZDB_KEY_STORE*} !NULL: ok; NULL: error
 */
ACL_API ZDB_KEY_STORE *zdb_key_store_open(ZDB *db, zdb_key_t key);

/**
 * �����ļ����򿪼��洢
 * @param db {ZDB*}
 * @param filepath {const char*} �ļ���
 * @return {ZDB_KEY_STORE*} !NULL: ok; NULL: error
 */
ACL_API ZDB_KEY_STORE *zdb_key_store_open2(ZDB *db, const char *filepath);

/**
 * �رռ��洢
 * @param store {ZDB_KEY_STORE*} ���洢���
 */
ACL_API void zdb_key_store_close(ZDB_KEY_STORE *store);

/**
 * ���ü��洢�м�λ�õ�ֵλ��ֵ
 * @param db {ZDB*}
 * @param key {zdb_key_t}
 * @param blk_off {const ZDB_BLK_OFF*}
 * @return {int} 0: ok; -1: error
 */
ACL_API int zdb_key_set(ZDB *db, zdb_key_t key, const ZDB_BLK_OFF *blk_off);

/**
 * ���ݼ�ֵ�Ӽ��洢��ȡ�øü�����Ӧ����������λ��
 * @param db {ZDB*}
 * @param key {zdb_key_t} ��ֵ
 * @param blk_off {ZDB_BLK_OFF*} �洢����Ķ���
 * @return {int} 1: ��ʾ�鵽, 0: ��ʾδ�鵽, -1: ��ʾ����
 */
ACL_API int zdb_key_get(ZDB *db, zdb_key_t key, ZDB_BLK_OFF *blk_off);

/**
 * ��ѯ���洢ͷ��״̬
 * @param db {ZDB*}
 * @param filepath {const char*} ���洢�ļ�ȫ·��
 * @param key_hdr {ZDB_KEY_HDR*} ������ż��洢ͷ��Ϣ���ڴ��ַ
 * @return {int} 0: ok; -1: error
 */
ACL_API int zdb_key_status(ZDB *db, const char *filepath, ZDB_KEY_HDR *key_hdr);

/**
 * �������洢�еļ���״̬
 * @param db {ZDB*}
 * @param filepath {const char*} ���洢�ļ�ȫ·��
 * @param key_hdr {ZDB_KEY_HDR*} ���ǿ���������ż��洢ͷ��Ϣ���ڴ��ַ
 * @return {int} 0: ok; -1: error
 */
ACL_API int zdb_key_check3(ZDB *db, const char *filepath, ZDB_KEY_HDR *key_hdr);
ACL_API int zdb_key_check(ZDB_KEY_STORE *store, ZDB_KEY_HDR *key_hdr);

/**
 * ��ʼ�����洢
 * @param db {ZDB*}
 * @param key_begin {zdb_key_t} ��ʼ��ֵ
 * @param key_end {zdb_key_t} ������ֵ
 * @return {int} 0: ok; -1: error
 */
ACL_API int zdb_key_init(ZDB *db, zdb_key_t key_begin, zdb_key_t key_end);
/*--------------------------------- in zdb_dat.c -----------------------------*/

/**
 * ͬ��ֵ�洢ͷ������
 * @param store {ZDB_DAT_STORE*} ֵ�洢���
 * @return {int} -1: error, > 0 ok
 */
ACL_API int dat_store_header_sync(ZDB_DAT_STORE *store);

/**
 * �ر�ֵ�洢
 * @param store {ZDB_DAT_STORE*} ֵ�洢���
 */
ACL_API void zdb_dat_store_close(ZDB_DAT_STORE *store);

/**
 * �򿪻򴴽�ֵ�洢
 * @param filepath {const char*} ֵ�洢�ļ�ȫ·��
 * @return {ZDB_DAT_STORE*} !NULL: ok; NULL: error
 */
ACL_API ZDB_DAT_STORE *zdb_dat_store_open(ZDB *db, const char *filepath);

/**
 * ��ֵ�洢��ȡ�ö�Ӧ������
 * @param db {ZDB*}
 * @param blk_off {const ZDB_BLK_OFF*}
 * @param key {zdb_key_t*} �洢����ֵ
 * @param size {size_t*} �洢���ݵĳ���
 * @return {void*} ���ݵ�ַ�����ҵ���Ϊ�գ��Ҳ����������Ϊ��, ����Ϊ��,
 *  ����Ҫ�� zdb_blk_free()/1 �����ͷ�
 * ע��:
 *  �� size ָ���ַ�ǿ�ʱ�������ߵ����ͱ����� size_t ���Ͷ��� int ���ͣ���Ϊ��
 *  64λ���� size_t Ϊ8���ֽڳ��ȣ��� int Ϊ4���ֽڳ��ȣ��ڲ��ڶ� size ��ַ��ֵ
 *  ʱ�����������Զ��� size_t ����ʵ�ռ䳤�ȸ�ֵ����������ߵ� size Ϊ int ���ͣ�
 *  ��ͻ���ֿռ�Խ�����󣬴Ӷ���ʹ�����������Ī����������⣬������ valgrind
 *  Ҳ�鲻�����������!
 */
ACL_API ZDB_BLK *zdb_dat_get(ZDB *db, const ZDB_BLK_OFF *blk_off,
	zdb_key_t *key, size_t *size);

/**
 * ��ֵ�洢�����������
 * @param db {ZDB*}
 * @param key {zdb_key_t} ��ֵ
 * @param dat {const void*} ���ݵ�ַ
 * @param len {size_t} dat ���ݳ���
 * @return {int} 0: ����û�и���; 1: ���ݸ���; -1: ����
 */
ACL_API int zdb_dat_add(ZDB *db, zdb_key_t key, const void *dat, int len);

/**
 * ����ֵ�洢�е�������Ϣ
 * @param db {ZDB*}
 * @param key {zdb_key_t} ��
 * @param blk_off {const ZDB_BLK_OFF*} �洢�ڼ��洢����Ӧֵ��λ����Ϣ
 * @param dat {const void*} ���ݵ�ַ
 * @param len {size_t} dat ���ݳ���
 * @return {int} 0: ����û�и���; 1: ���ݸ���; -1: ����
 */
ACL_API int zdb_dat_update(ZDB *db, zdb_key_t key, const ZDB_BLK_OFF *blk_off,
	const void *dat, size_t len);

/**
 * ��ȡֵ�洢��ͷ��Ϣ
 * @param db {ZDB*}
 * @param filepath {const char*} ֵ�洢�ļ���
 * @param dat_hdr {ZDB_DAT_HDR*} �洢���
 * @retur {int} 0: ok; -1: error
 */
ACL_API int zdb_dat_stat(ZDB *db, const char *filepath, ZDB_DAT_HDR *dat_hdr);

/**
 * ���ֵ�洢�е����ݿ����ȷ��
 * @param db {ZDB*}
 * @param filepath {const char*} ֵ�洢�ļ���
 * @param dat_hdr {ZDB_DAT_HDR*} ���ǿ���洢ֵ�洢ͷ��Ϣ
 * @return {int} 0: ok; -1: error
 */
ACL_API int zdb_dat_check3(ZDB *db, const char *filepath, ZDB_DAT_HDR *dat_hdr);
ACL_API int zdb_dat_check(ZDB_DAT_STORE *store, ZDB_DAT_HDR *dat_hdr);

/*--------------------   in zdb_key_walk.c -----------------------------------*/
ACL_API int zdb_key_walk(ZDB *db, int (*walk_fn)(ZDB_KEY_STORE *store));

/*--------------------   in zdb_dat_walk.c -----------------------------------*/
ACL_API int zdb_dat_walk(ZDB *db, int (*walk_fn)(ZDB_DAT_STORE *store));

/*--------------------   in zdb_key_iter.c -----------------------------------*/

/*--------------------   in zdb_dat_iter.c -----------------------------------*/
/**
 * ����ֵ�洢�ĵ�����
 * @param store {ZDB_DAT_STORE*}
 * @param read_data {int} �Ƿ���Ҫ��ȡ���ݿ��е����ݲ���, !0: ��ʾ��ȡ���ݿ��е�
 *  ���ݲ���; 0: ����ȡ���ݿ��е�ͷ
 */
ACL_API void zdb_dat_iter_set(ZDB_DAT_STORE *store, int read_data);

#ifdef	__cplusplus
}
#endif

#endif
