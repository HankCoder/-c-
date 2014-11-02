#ifndef	ACL_SLICE_INCLUDE_H
#define	ACL_SLICE_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "acl_define.h"

#define	ACL_SLICE_FLAG_OFF		(0)
#define	ACL_SLICE_FLAG_GC1		(1 << 0)  /**< �ռ��ʡ, �� gc ���ܲ� */
#define	ACL_SLICE_FLAG_GC2		(1 << 1)  /**< �ռ��е�, gc �ȽϺ� */
#define	ACL_SLICE_FLAG_GC3		(1 << 2)  /**< �ռ����, gc ֻ��˳��ʱ��� */
#define	ACL_SLICE_FLAG_RTGC_OFF		(1 << 10) /**< �ر�ʵʱ�ڴ��ͷ� */
#define	ACL_SLICE_FLAG_LP64_ALIGN	(1 << 11) /**< �Ƿ����64λƽ̨��Ҫ��8�ֽڶ��� */

/**
 * �ڴ��Ƭ�ص�״̬�ṹ
 */
typedef struct ACL_SLICE_STAT {
	int   nslots;           /**< total slice count free in slots */
	int   islots;           /**< current position of free slots slice */
	int   page_nslots;      /**< count slice of each page */
	int   page_size;        /**< length of each malloc */
	int   slice_length;	/**< length of each slice from user's set */
	int   slice_size;       /**< length of each slice really allocated */
	int   nbuf;             /**< count of MEM_BUF allocated */
	acl_uint64 length;      /**< total size of all MEM_BUF's buf */
	acl_uint64 used_length; /**< total size of used */
	unsigned int flag;	/**< same as the ACL_SLICE's flag been set when created */
} ACL_SLICE_STAT;

typedef struct ACL_SLICE ACL_SLICE;

/**
 * �����ڴ�Ƭ�ض���
 * @param name {const char*} ��ʶ���ƣ��Ա��ڵ���
 * @param page_size {int} �����ڴ�ʱ�ķ����ڴ�ҳ��С
 * @param slice_size {int} ÿ���̶������ڴ�Ƭ�Ĵ�С
 * @param flag {unsigned int} ��־λ���μ�������ACL_SLICE_FLAG_xxx
 * @return {ACL_SLICE*} �ڴ�Ƭ�ض�����
 */
ACL_API ACL_SLICE *acl_slice_create(const char *name, int page_size,
	int slice_size, unsigned int flag);

/**
 * ����һ���ڴ�Ƭ�ض���
 * @param slice {ACL_SLICE*} �ڴ�Ƭ�ض���
 */
ACL_API void acl_slice_destroy(ACL_SLICE *slice);

/**
 * ���ڴ�Ƭ�����ж��ٸ��ڴ�Ƭ���ڱ�ʹ��
 * @param slice {ACL_SLICE*} �ڴ�Ƭ�ض���
 * @return {int} >= 0, ���ڱ�ʹ�õ��ڴ�Ƭ����
 */
ACL_API int acl_slice_used(ACL_SLICE *slice);

/**
 * ����һ���ڴ�Ƭ
 * @param slice {ACL_SLICE*} �ڴ�Ƭ�ض���
 * @return {void*} �ڴ�Ƭ��ַ
 */
ACL_API void *acl_slice_alloc(ACL_SLICE *slice);

/**
 * ����һ���ڴ�Ƭ���ҽ��ڴ�Ƭ���ݳ�ʼ��Ϊ0
 * @param slice {ACL_SLICE*} �ڴ�Ƭ�ض���
 * @return {void*} �ڴ�Ƭ��ַ
 */
ACL_API void *acl_slice_calloc(ACL_SLICE *slice);

/**
 * �ͷ�һ���ڴ�Ƭ
 * @param slice {ACL_SLICE*} �ڴ�Ƭ�ض���
 * @param ptr {void*} �ڴ�Ƭ��ַ, �������� acl_slice_alloc/acl_slice_calloc ������
 */
ACL_API void acl_slice_free2(ACL_SLICE *slice, void *ptr);

/**
 * �ͷ�һ���ڴ�Ƭ
 * @param ptr {void*} �ڴ�Ƭ��ַ, �������� acl_slice_alloc/acl_slice_calloc ������
 */
ACL_API void acl_slice_free(void *ptr);

/**
 * �鿴�ڴ�Ƭ�صĵ�ǰ״̬
 * @param slice {ACL_SLICE*} �ڴ�Ƭ�ض���
 * @param sbuf {ACL_SLICE_STAT*} �洢���, ����Ϊ��
 */
ACL_API void acl_slice_stat(ACL_SLICE *slice, ACL_SLICE_STAT *sbuf);

/**
 * �ֹ����ڴ�Ƭ�ز��õ��ڴ�����ͷ�
 * @param slice {ACL_SLICE*} �ڴ�Ƭ�ض���
 * @param {int} �Ƿ��Ѿ��������ڴ�Ƭ�ͷ����, 0: ��; 1: ��
 */
ACL_API int acl_slice_gc(ACL_SLICE *slice);

/*----------------------------------------------------------------------------*/

typedef struct ACL_SLICE_POOL {
	ACL_SLICE **slices;		/* the slice array */
	int   base;			/* the base byte size */
	int   nslice;			/* the max number of base size */
	unsigned int slice_flag;	/* flag: ACL_SLICE_FLAG_GC2[3] | ACL_SLICE_FLAG_RTGC_OFF */
} ACL_SLICE_POOL;

ACL_API void acl_slice_pool_init(ACL_SLICE_POOL *asp);
ACL_API ACL_SLICE_POOL *acl_slice_pool_create(int base, int nslice,
	unsigned int slice_flag);
ACL_API void acl_slice_pool_destroy(ACL_SLICE_POOL *asp);
ACL_API int acl_slice_pool_used(ACL_SLICE_POOL *asp);
ACL_API void acl_slice_pool_clean(ACL_SLICE_POOL *asp);
ACL_API void acl_slice_pool_reset(ACL_SLICE_POOL *asp);
ACL_API void acl_slice_pool_free(const char *filename, int line, void *buf);
ACL_API void acl_slice_pool_gc(ACL_SLICE_POOL *asp);
ACL_API void *acl_slice_pool_alloc(const char *filename, int line,
	ACL_SLICE_POOL *asp, size_t size);
ACL_API void *acl_slice_pool_calloc(const char *filename, int line,
	ACL_SLICE_POOL *asp, size_t nmemb, size_t size);
ACL_API void *acl_slice_pool_realloc(const char *filename, int line,
	ACL_SLICE_POOL *asp, void *ptr, size_t size);
ACL_API void *acl_slice_pool_memdup(const char *filename, int line,
	ACL_SLICE_POOL *asp, const void *ptr, size_t len);
ACL_API char *acl_slice_pool_strdup(const char *filename, int line,
	ACL_SLICE_POOL *asp, const char *str);
ACL_API char *acl_slice_pool_strndup(const char *filename, int line,
	ACL_SLICE_POOL *asp, const char *str, size_t len);

#ifdef	__cplusplus
}
#endif

#endif
