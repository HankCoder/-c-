#ifndef ACL_ALLOCATOR_INCLUDE_H
#define ACL_ALLOCATOR_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	ACL_PREPARE_COMPILE
#include "acl_define.h"
#include <stdlib.h>
#endif

typedef enum {
	ACL_MEM_TYPE_NONE,
	ACL_MEM_TYPE_8_BUF,
	ACL_MEM_TYPE_16_BUF,
	ACL_MEM_TYPE_32_BUF,
	ACL_MEM_TYPE_64_BUF,
	ACL_MEM_TYPE_128_BUF,
	ACL_MEM_TYPE_256_BUF,
	ACL_MEM_TYPE_512_BUF,
	ACL_MEM_TYPE_1K_BUF,
	ACL_MEM_TYPE_2K_BUF,
	ACL_MEM_TYPE_4K_BUF,
	ACL_MEM_TYPE_8K_BUF,
	ACL_MEM_TYPE_16K_BUF,
	ACL_MEM_TYPE_32K_BUF,
	ACL_MEM_TYPE_64K_BUF,
	ACL_MEM_TYPE_128K_BUF,
	ACL_MEM_TYPE_256K_BUF,
	ACL_MEM_TYPE_512K_BUF,
	ACL_MEM_TYPE_1M_BUF,
	ACL_MEM_TYPE_VSTRING,
	ACL_MEM_TYPE_MAX
} acl_mem_type;

typedef struct ACL_MEM_POOL ACL_MEM_POOL;
typedef struct ACL_ALLOCATOR ACL_ALLOCATOR;

/* in acl_mpool.c */
/**
 * ����һ���ڴ����ض���
 * @param mem_limit {size_t} �ڴ�ص�����ڴ棬��λΪ�ֽ�
 * @return {ACL_ALLOCATOR *} �ڴ����ض���ָ��
 */
ACL_API ACL_ALLOCATOR *acl_allocator_create(size_t mem_limit);

/**
 * �����ڴ����ص�һЩ����
 * @param name {int} �����б�ĵ�һ������
 * ���÷�ʽ���£�
 * acl_allocator_ctl(ACL_ALLOCATOR_CTL_MIN_SIZE, 128,
 *		ACL_ALLOCATOR_CTL_MAX_SIZE, 1024,
 *		ACL_ALLOCATOR_CTL_END);
 */
ACL_API void acl_allocator_ctl(int name, ...);

#define ACL_ALLOCATOR_CTL_END		0    /**< ������� */
#define ACL_ALLOCATOR_CTL_MIN_SIZE	1    /**< ������С�ֽ��� */
#define ACL_ALLOCATOR_CTL_MAX_SIZE	2    /**< ��������ֽ��� */

/**
 * �����ڴ����ص�������С
 * @param allocator {ACL_ALLOCATOR*}
 * @param mem_limit {size_t} �ڴ�ص����ֵ����λΪ�ֽ�
 */
ACL_API void acl_allocator_config(ACL_ALLOCATOR *allocator, size_t mem_limit);

/**
 * �ͷ��ڴ����ض�������������ڴ�
 * @param allocator {ACL_ALLOCATOR*}
 */
ACL_API void acl_allocator_free(ACL_ALLOCATOR *allocator);

/**
 * ���һ���µ��ڴ��������
 * @param allocator {ACL_ALLOCATOR*}
 * @param label {const char*} ���ڴ�������͵�������Ϣ
 * @param obj_size {size_t} ÿ�����ڴ����͵Ĵ�С����λΪ�ֽ�
 * @param type {acl_mem_type} �ڴ�����
 * @param after_alloc_fn {void (*)(void*, void*)} �����ڴ�ɹ�����õĺ���������Ϊ��
 * @param before_free_fn {void (*)(void*, void*)} �ͷ��ڴ�ǰ�ص��ĺ���������Ϊ��
 * @param pool_ctx {void*} Ӧ���Լ���˽�ж������ after_alloc_fn �� before_free_fn
 *        ��Ϊ�գ���ص�ʱ���˲���ֱ�Ӵ��ݸ�Ӧ��
 * @return {ACL_MEM_POOL*} ���ڴ������������Ӧ�Ķ���
 */
ACL_API ACL_MEM_POOL *acl_allocator_pool_add(ACL_ALLOCATOR *allocator,
					const char *label,
					size_t obj_size,
					acl_mem_type type,
					void (*after_alloc_fn)(void *obj, void *pool_ctx),
					void (*before_free_fn)(void *obj, void *pool_ctx),
					void *pool_ctx);

/**
 * ���ڴ��������Ƴ�ĳ���ڴ��������
 * @param allocator {ACL_ALLOCATOR*}
 * @param pool {ACL_MEM_POOL*} �� acl_allocatore_pool_add ���صĶ���
 */
ACL_API void acl_allocator_pool_remove(ACL_ALLOCATOR *allocator, ACL_MEM_POOL *pool);

/**
 * ̽��ĳ�ַ��������Ƿ�������ڴ����ص��ڴ����������
 * @param allocator {ACL_ALLOCATOR*}
 * @param type {acl_mem_type} �ڴ�����
 * @return {int}, 0: ��!= 0: ��
 */
ACL_API int acl_allocator_pool_ifused(ACL_ALLOCATOR *allocator, acl_mem_type type);

/**
 * ĳ�ַ������͵��ڴ����ǰ��ʹ�õĸ���
 * @param allocator {ACL_ALLOCATOR*}
 * @param type {acl_mem_type} �ڴ�����
 * @return {int} ��ǰ���ڱ�ʹ�õ�ĳ���ڴ�������͵��ڴ�������
 */
ACL_API int acl_allocator_pool_inuse_count(ACL_ALLOCATOR *allocator, acl_mem_type type);

/**
 * ĳ�ַ���������������ڴ��е�ǰ���ڱ�ʹ�õ��ڴ��С
 * @param allocator {ACL_ALLOCATOR*}
 * @param type {acl_mem_type} �ڴ�����
 * @return {int} ĳ�ַ���������������ڴ��е�ǰ���ڱ�ʹ�õ��ڴ��С����λΪ�ֽ�
 */
ACL_API int acl_allocator_pool_inuse_size(ACL_ALLOCATOR *allocator, acl_mem_type type);

/**
 * �ڴ������ܹ�����������ڱ�ʹ�õ��ڴ�Ĵ�С
 * @param allocator {ACL_ALLOCATOR*}
 * @return {int} �ڴ��С����λ���ֽ�
 */
ACL_API int acl_allocator_pool_total_allocated(ACL_ALLOCATOR *allocator);

/**
 * ����ĳ���ڴ����͵��ڴ�
 * @param allocator {ACL_ALLOCATOR*}
 * @param type {acl_mem_type} �ڴ�����
 * @return {void*} �·�����ڴ�ĵ�ַ
 */
ACL_API void *acl_allocator_mem_alloc(ACL_ALLOCATOR *allocator, acl_mem_type type);

/**
 * �ͷ�ĳ���ڴ����͵��ڴ�ռ�
 * @param allocator {ACL_ALLOCATOR*}
 * @param type {acl_mem_type} �ڴ�����
 * @param obj {void*} ���ͷŵ��ڴ���󣬲���Ϊ��
 */
ACL_API void acl_allocator_mem_free(ACL_ALLOCATOR *allocator, acl_mem_type type, void *obj);

/**
 * ������Ҫ����ڴ��С���Զ������ڴ��������ƥ�䣬���ҵ���ƥ������ͣ�������ڴ��
 * ���ڴ������ԣ�����ֱ�ӵ��� acl_mymalloc �����ڴ����
 * @param filename {const char*} ���ñ������ĵ�ǰ�ļ���
 * @param line {int} ���ñ������ĵ�ǰ�ļ��к�
 * @param allocator {ACL_ALLOCATOR*}
 * @param size {size_t} ��������������ڴ��С
 * @return {void*} �·�����ڴ�ĵ�ַ
 */
ACL_API void *acl_allocator_membuf_alloc(const char *filename, int line,
	ACL_ALLOCATOR *allocator, size_t size);

/**
 * ������������ڴ��С�����·����ڴ�ռ䣬���ҵ���ƥ������ͣ�������ڴ��
 * �ڴ������ԣ�����ֱ������ acl_mymalloc �����ڴ����
 * @param filename {const char*} ���ñ������ĵ�ǰ�ļ���
 * @param line {int} ���ñ������ĵ�ǰ�ļ��к�
 * @param allocator {ACL_ALLOCATOR*}
 * @param oldbuf {void*} ԭ��������ڴ�
 * @param size {size_t} ����������ڴ��С
 * @return {void*} �·�����ڴ�ĵ�ַ
 */
ACL_API void *acl_allocator_membuf_realloc(const char *filename, int line,
	ACL_ALLOCATOR *allocator, void *oldbuf, size_t size);

/**
 * �ͷ��ڴ�, ������ҵ��ô�С���ڴ��������ڴ�������ͣ�����л��壬����ֱ������
 * acl_myfree �����ͷ�
 * @param filename {const char*} ���ñ������ĵ�ǰ�ļ���
 * @param line {int} ���ñ������ĵ�ǰ�ļ��к�
 * @param allocator {ACL_ALLOCATOR*}
 * @param buf {void*} �ڴ��ַ
 */
ACL_API void acl_allocator_membuf_free(const char *filename, int line,
	ACL_ALLOCATOR *allocator, void *buf);

#ifdef __cplusplus
}
#endif

#endif
