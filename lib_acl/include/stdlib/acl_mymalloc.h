/*
 * @file	mymalloc.h
 * @author	zsx
 * @date	2003-12-15
 * @version	1.0
 * @brief	���ļ�Ϊʹ��ACL��ʱ�ĸ߼��ڴ����ӿڣ��û�Ӧ����Ҫʹ�ô˽ӿ���
 *		�����ڴ�ķ��估�ͷ�, �û��������е��� acl_mem_hook.h �еĺ���
 *		�ӿ�ע���Լ����ڴ�������ͷŵȹ���ӿڣ������ٵ��� acl_myxxx ��
 *		�ĺ�ʱ���Զ��л����û��Լ����ڴ����ӿڵ�����
 */

#ifndef	ACL_MYMALLOC_INCLUDE_H
#define	ACL_MYMALLOC_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_malloc.h"

/**
 * ��̬�����ڴ�ĺ궨�壬����ʼ���·�����ڴ�ռ�
 * @param size {size_t} ���䳤��
 * @return {void *}
 */
#define acl_mymalloc(size) acl_malloc_glue(__FILE__, __LINE__, size)

/**
 * ��̬�����ڴ�ĺ궨�壬��ʼ���·�����ڴ�ռ�Ϊ��
 * @param nmemb {size_t} Ԫ�ظ���
 * @param size {size_t} ÿ��Ԫ�صĳ���
 * @return {void *}
 */
#define acl_mycalloc(nmemb, size) acl_calloc_glue(__FILE__, __LINE__, nmemb, size)

/**
 * ���¶�̬�����ڴ�ĺ궨��
 * @param ptr {void*} ԭ�ڴ��ַ
 * @param size {size_t} �·����ڴ�ʱҪ��ĳ���
 * @return {void *}
 */
#define acl_myrealloc(ptr, size) acl_realloc_glue(__FILE__, __LINE__, (ptr), size)

/**
 * ��̬�����ַ����궨��
 * @param str {const char*} Դ�ַ���
 * @return {char*} �µ��ַ��������� acl_myfree �ͷ�
 */
#define acl_mystrdup(str) acl_strdup_glue(__FILE__, __LINE__, (str))

/**
 * ��̬�����ַ����궨�壬���޶�����ڴ�ռ��С
 * @param str {const char*} Դ�ַ���
 * @param len {size_t} ���ַ�������ڴ�ռ��С����ֵ
 * @return {char*} �µ��ַ��������� acl_myfree �ͷ�
 */
#define acl_mystrndup(str, len) acl_strndup_glue(__FILE__, __LINE__, (str), len)

/**
 * ��̬�����ڴ�궨��
 * @param ptr {const void*} Դ�ڴ��ַ
 * @param len {size_t} Դ�ڴ��С
 * @return {void*} �µ��ַ��������� acl_myfree �ͷ�
 */
#define acl_mymemdup(ptr, len) acl_memdup_glue(__FILE__, __LINE__, (ptr), len)

/**
 * �ͷŶ�̬������ڴ�ռ�
 * @param _ptr_ {void*} ��̬�ڴ��ַ
 */
#define acl_myfree(_ptr_) do {  \
	acl_free_glue(__FILE__, __LINE__, (_ptr_));  \
	(_ptr_) = NULL;  \
} while (0)

/**
 * XXX: ��Ϊ�ú������ڻص������������޷����к궨��ת��, ���������ƴ˺���
 */
#define	acl_myfree_fn acl_free_fn_glue

#ifdef  __cplusplus
}
#endif

#endif

