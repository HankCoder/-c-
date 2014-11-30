#ifndef ACL_MALLOC_INCLUDE_H
#define ACL_MALLOC_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "acl_define.h"

/*
 * Memory alignment of memory allocator results.
 * By default we align for doubles.
 */

#ifndef ALIGN_TYPE
# if defined(__hpux) && defined(__ia64)
#  define ALIGN_TYPE	__float80
# elif defined(__ia64__)
#  define ALIGN_TYPE	long double
# else
#  define ALIGN_TYPE	double
# endif
#endif

ACL_API void acl_memory_debug_start(void);
ACL_API void acl_memory_debug_stop(void);
ACL_API void acl_memory_debug_stack(int onoff);
ACL_API void acl_memory_stat(void);
ACL_API void acl_memory_alloc_stat(void);

/**
 * �򿪶�̬�ڴ�ع���
 * @param max_size {size_t} �ڴ�����ռ��С����λΪ�ֽ�
 * @param use_mutex {int} �ڴ���ڲ��Ƿ���û�����������Ƕ��̳߳�����Ӧ��
 *  ���� use_mutex Ϊ��0ֵ
 */
ACL_API void acl_mempool_open(size_t max_size, int use_mutex);

/**
 * �ر��ڴ�ع���
 */
ACL_API void acl_mempool_close(void);

/**
 * ���ڴ�ش򿪺󣬿���ͨ���˺��������ڴ��״̬
 */
ACL_API void acl_mempool_ctl(int name, ...);
#define	ACL_MEMPOOL_CTL_END         0  /**< ������־ */
#define	ACL_MEMPOOL_CTL_MUTEX       1  /**< �����ڴ���Ƿ���� */
#define	ACL_MEMPOOL_CTL_DISABLE     2  /**< �Ƿ�ر��ڴ�� */

/**
 * ��ǰ�ڴ���Ѿ�������ڴ��С
 * @return {int} �Ѿ�������ڴ��С
 */
ACL_API int acl_mempool_total_allocated(void);

/*---------------- ACL����ȱʡ���ڴ���䡢�ͷŵȹ���ӿ� ---------------------*/

/**
 * ��õ�ǰ�ڴ�ָ���һЩ״̬��Ϣ������ڴ��ʵ�ʴ�С���������С
 * @param filename {const char*} ���øú������ļ���������Ϊ��
 * @param line {int} ���øú�������Դ�ļ��е�����
 * @param ptr {void*} ��̬������ڴ��ⲿ��ַ
 * @param len {size_t*} �洢���ڴ���ⲿ���ô�С
 * @param read_len {size*} �洢���ڴ��ʵ�ʴ�С(��Ϊ�ڲ���һЩ�����ֽ�)
 */
ACL_API void acl_default_memstat(const char *filename, int line,
        void *ptr, size_t *len, size_t *real_len);

/**
 * �����ڴ������󱨾�ֵ�����������߷�����ڴ��С�ﵽ�˱���ֵ���ڲ����Զ�
 * ��¼������־��ͬʱ�����ö�ջ��ӡ����־�У��ڲ�ȱ��ֵ�� 100000000
 * @param len {size_t} ��󱨾�ֵ����ֵ���� > 0
 */
ACL_API void acl_default_set_memlimit(size_t len);

/**
 * ��õ�ǰ�����õ��ڴ������󱨾�ֵ��С(�ڲ�ȱʡֵ�� 100000000)
 * @return {size_t}
 */
ACL_API size_t acl_default_get_memlimit(void);

/**
 * ACL����ȱʡ���ڴ�������ӿ�, �����ڴ浫������ʼ���������ڴ������
 * �����ڱ�׼���е� malloc
 * @param filename {const char*} ���øú������ļ���������Ϊ��
 * @param line {int} ���øú�������Դ�ļ��е�����
 * @param size {size_t} ��Ҫ���ڴ��С
 * @return {void*} ����Ŀ��õ�ַ, �������ʧ�ܣ����ڲ����Զ�coredump
 *   ��Ҫ���� acl_default_free �ͷ�
 */
ACL_API void *acl_default_malloc(const char *filename, int line, size_t size);

/**
 * ACL����ȱʡ���ڴ�������ӿ�, �����ڴ沢��ʼ���������ڴ������Ϊ0
 * �����ڱ�׼���е� calloc
 * @param filename {const char*} ���øú������ļ���������Ϊ��
 * @param line {int} ���øú�������Դ�ļ��е�����
 * @param nmemb {size_t} �ڴ��ĸ���
 * @param size {size_t} ÿ���ڴ��Ĵ�С
 * @return {void*} ����Ŀ��õ�ַ, �������ʧ�ܣ����ڲ����Զ�coredump
 *   ��Ҫ���� acl_default_free �ͷ�
 */
ACL_API void *acl_default_calloc(const char *filename, int line, size_t nmemb, size_t size);

/**
 * ACL����ȱʡ���ڴ�������ӿ�, �����ڱ�׼��� realloc
 * @param filename {const char*} ���øú������ļ���������Ϊ��
 * @param line {int} ���øú�������Դ�ļ��е�����
 * @param ptr {void*} ֮ǰ��ACL����������ڴ��ַ
 * @param size {size_t} ��Ҫ���ڴ��С
 * @return {void*} ����Ŀ��õ�ַ, �������ʧ�ܣ����ڲ����Զ�coredump
 *   ��Ҫ���� acl_default_free �ͷ�
 */
ACL_API void *acl_default_realloc(const char *filename, int line, void *ptr, size_t size);

/**
 * �����ַ����������ڱ�׼���е� strdup
 * @param filename {const char*} ���øú������ļ���������Ϊ��
 * @param line {int} ���øú�������Դ�ļ��е�����
 * @param str {const char*} Դ�ַ�����ַ
 * @return {char*} �¸��Ƶ��ַ�����ַ����Ҫ���� acl_default_free �ͷ�
 */
ACL_API char *acl_default_strdup(const char *filename, int line, const char *str);

/**
 * �����ַ���������������ַ������ȣ������ڱ�׼���е� strndup
 * @param filename {const char*} ���øú������ļ���������Ϊ��
 * @param line {int} ���øú�������Դ�ļ��е�����
 * @param str {const char*} Դ�ַ�����ַ
 * @param len {size_t} �������ַ�������󳤶�ֵ
 * @return {char*} �¸��Ƶ��ַ�����ַ����Ҫ���� acl_default_free �ͷ�
 */
ACL_API char *acl_default_strndup(const char *filename, int line, const char *str, size_t len);

/**
 * �����ڴ�����
 * @param filename {const char*} ���øú������ļ���������Ϊ��
 * @param line {int} ���øú�������Դ�ļ��е�����
 * @param ptr {const void*} Դ�ڴ��ַ
 * @param len {size_t} Դ�ڴ�����ĳ���
 * @return {void*} �¸��Ƶ��ڴ��ַ 
 */
ACL_API void *acl_default_memdup(const char *filename, int line, const void *ptr, size_t len);

/**
 * �ͷ��� acl_devault_xxx ��������ڴ涯̬�ڴ�
 * @param filename {const char*} ���øú������ļ���������Ϊ��
 * @param line {int} ���øú�������Դ�ļ��е�����
 */
ACL_API void  acl_default_free(const char *filename, int line, void *ptr);

/*------- acl_mymalloc.h �ڴ����ӿ��еĺ������ʹ�õ��ڴ�������ӿ� ------*/
/* �ú����ӿڼ���ʵ�ǵ������������ڴ�����������ڴ�ķ������ͷŵȹ�������ģ���
 * �ṩ�˸߼�����õ��ⲿʹ�ýӿڣ������û�������
 */

ACL_API void *acl_malloc_glue(const char *filename, int line, size_t size);
ACL_API void *acl_calloc_glue(const char *filename, int line, size_t nmemb, size_t size);
ACL_API void *acl_realloc_glue(const char *filename, int line, void *ptr, size_t size);
ACL_API char *acl_strdup_glue(const char *filename, int line, const char *str);
ACL_API char *acl_strndup_glue(const char *filename, int line, const char *str, size_t len);
ACL_API void *acl_memdup_glue(const char *filename, int line, const void *ptr, size_t len);
ACL_API void  acl_free_glue(const char *filename, int line, void *ptr);
ACL_API void  acl_free_fn_glue(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
