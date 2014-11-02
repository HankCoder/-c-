#ifndef ACL_MEM_HOOK_INCLUDE_H
#define ACL_MEM_HOOK_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "acl_define.h"

/**
 * �����ڴ���䡢�ͷŵ�ע�ắ������ACL�ڲ������ͷ��ڴ�ʱ�������Щע��ĺ���
 * �ڵ��ô˺�������ע��ʱ���뱣֤�⼸������ָ��������ǿ�
 * @param malloc_hook {void *(*)(const char* fname, int lineno, size_t)}
 * @param calloc_hook {void *(*)(const char* fname, int lineno, size_t, size_t)}
 * @param realloc_hook {void *(*)(const char* fname, int lineno, void *, size_t)}
 * @param strdup_hook {void *(*)(const char* fname, int lineno, const char*)}
 * @param strndup_hook {void *(*)(const char* fname, int lineno, const char*, size_t)}
 * @param memdup_hook {void *(*)(const char* fname, int lineno, const void *, size_t)}
 * @param free_hook {void (*)(const char* fname, int lineno, void*)}
 */
ACL_API void acl_mem_hook(void *(*malloc_hook)(const char*, int, size_t),
		void *(*calloc_hook)(const char*, int, size_t, size_t),
		void *(*realloc_hook)(const char*, int, void*, size_t),
		char *(*strdup_hook)(const char*, int, const char*),
		char *(*strndup_hook)(const char*, int, const char*, size_t),
		void *(*memdup_hook)(const char*, int, const void*, size_t),
		void  (*free_hook)(const char*, int, void*));

/**
 * ȡ��֮ǰ���õ��ڴ湴�Ӻ������ָ�Ϊȱʡ״̬
 */
ACL_API void acl_mem_unhook(void);

#ifdef __cplusplus
}
#endif

#endif
