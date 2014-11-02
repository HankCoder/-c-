#ifndef ACL_ARGV_INCLUDE_H
#define ACL_ARGV_INCLUDE_H

# ifdef	__cplusplus
extern "C" {
# endif
#include "acl_define.h"
#include <stdarg.h>
#include "acl_slice.h"
#include "acl_iterator.h"

/**
 * External interface.
 */
typedef struct ACL_ARGV {
	int     len;			/**< number of array elements */
	int     argc;			/**< array elements in use */
	char  **argv;			/**< string array */

	/* ��Ӽ����� */

	/* ������β������ַ��� (�ڲ���̬�������ַ���) */
	void  (*push_back)(struct ACL_ARGV*, const char*);
	/* ������ͷ����Ӷ�̬���� (�ڲ���̬�������ַ���)*/
	void  (*push_front)(struct ACL_ARGV*, const char*);
	/* ��������β���ַ��� (���������� acl_myfree �ͷ�) */
	char *(*pop_back)(struct ACL_ARGV*);
	/* ��������ͷ���ַ��� (���������� acl_myfree �ͷ�) */
	char *(*pop_front)(struct ACL_ARGV*);

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	void *(*iter_head)(ACL_ITER*, struct ACL_ARGV*);
	/* ȡ��������һ������ */
	void *(*iter_next)(ACL_ITER*, struct ACL_ARGV*);
	/* ȡ������β���� */
	void *(*iter_tail)(ACL_ITER*, struct ACL_ARGV*);
	/* ȡ��������һ������ */
	void *(*iter_prev)(ACL_ITER*, struct ACL_ARGV*);

	/* private */
	ACL_SLICE_POOL *slice;
} ACL_ARGV;

/* in acl_argv.c */
/**
 * ����һ���ַ�����̬����
 * @param size {int} ��̬����ĳ�ʼ��С
 * @return {ACL_ARGV*}
 */
ACL_API ACL_ARGV *acl_argv_alloc(int size);

ACL_API ACL_ARGV *acl_argv_alloc2(int size, ACL_SLICE_POOL *slice);

/**
 * ���ַ�����̬���������һ������ַ��������һ��NULL�ַ�����ʾ����
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 * @param ... �ַ����б����һ��ΪNULL, ��ʽ�磺{s1}, {s2}, ..., NULL
 */
ACL_API void acl_argv_add(ACL_ARGV *argvp,...);

/**
 * ���ַ�����̬����������ַ����б�
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 * @param ap {va_list} �ɶ���ַ�����ɵı���б�
 */
ACL_API void acl_argv_addv(ACL_ARGV *argvp, va_list ap);

/**
 * ���ַ�����̬����������ֶγ��������Ƶ��ַ����б�
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 * @param ... һ���г������Ƶ��ַ����б���: {s1}, {len1}, {s2}, {len2}, ... NULL
 */
ACL_API void acl_argv_addn(ACL_ARGV *argvp,...);

/**
 * ���ַ�����̬����������ֶγ��������Ƶ��ַ����б�
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 * @param ap {va_list} һ���г������Ƶ��ַ�����ɵı���б�
 */
ACL_API void acl_argv_addnv(ACL_ARGV *argvp, va_list ap);

/**
 * �����ַ�����̬����Ľ���λ��
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 */
ACL_API void acl_argv_terminate(ACL_ARGV *argvp);

/**
 * �ͷ��ַ�����̬����
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 */
ACL_API ACL_ARGV *acl_argv_free(ACL_ARGV *argvp);

/**
 * ���������±�λ�÷������Ӧ���ַ���ָ��
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 * @param idx {int} �±�λ��
 * @return {char*} NULL: �±�Խ�磻!= NULL: �ַ���ָ��λ��
 */
ACL_API char *acl_argv_index(ACL_ARGV *argvp, int idx);

/**
 * ���ص�ǰ�ַ�����̬�������Ѿ���ŵ��ַ�������
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 * @return {int}
 */
ACL_API int acl_argv_size(ACL_ARGV *argvp);

/* in acl_argv_split.c */
/**
 * ����Դ�ַ������ָ��ַ�������һ���ַ�����̬����
 * @param str {const char*} Դ�ַ���
 * @param delim {const char*} �ָ��ַ���
 * @return {ACL_ARGV*}
 */
ACL_API ACL_ARGV *acl_argv_split(const char *str, const char *delim);

/**
 * ����Դ�ַ������ָ��ַ�������һ���ַ�����̬���飬ͬʱ��������ڴ��
 * ������Ϊ�ڴ������
 * @param str {const char*} Դ�ַ���
 * @param delim {const char*} �ָ��ַ���
 * @param slice {ACL_SLICE_POOL*} �ڴ�ض��󣬿���Ϊ�գ���Ϊ��ʱ�����
 *  ȱʡ���ڴ���䷽ʽ
 * @return {ACL_ARGV*}
 */
ACL_API ACL_ARGV *acl_argv_split3(const char *str, const char *delim,
	ACL_SLICE_POOL *slice);

/**
 * ����Դ�ַ������ָ��ַ�������һ���ַ�����̬����, ���޶����ָ�����
 * @param str {const char*} Դ�ַ���
 * @param delim {const char*} �ָ��ַ���
 * @param n {size_t} ���ָ�����
 * @return {ACL_ARGV*}
 */
ACL_API ACL_ARGV *acl_argv_splitn(const char *str, const char *delim, size_t n);

/**
 * ����Դ�ַ������ָ��ַ�������һ���ַ�����̬����, ���޶����ָ�������
 * ͬʱ�����ڴ�ض�����Ϊ�ڴ������
 * @param str {const char*} Դ�ַ���
 * @param delim {const char*} �ָ��ַ���
 * @param n {size_t} ���ָ�����
 * @param slice {ACL_SLICE_POOL*} �ڴ�ض��󣬿���Ϊ�գ���Ϊ��ʱ�����
 *  ȱʡ���ڴ���䷽ʽ
 * @return {ACL_ARGV*}
 */
ACL_API ACL_ARGV *acl_argv_splitn4(const char *str, const char *delim,
	size_t n, ACL_SLICE_POOL *slice);

/**
 * Դ�ַ������ָ����ֽ��������������һ���ַ�����̬����
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 * @param str {const char*} Դ�ַ���
 * @param delim {const char*} �ָ��ַ���
 * @return {ACL_ARGV*}
 */
ACL_API ACL_ARGV *acl_argv_split_append(ACL_ARGV *argvp, const char *str,
	const char *delim);

/**
 * Դ�ַ������ָ����ֽ��������������һ���ַ�����̬����, ���޶����ָ�����
 * @param argvp {ACL_ARGV*} �ַ�����̬����ָ��
 * @param str {const char*} Դ�ַ���
 * @param delim {const char*} �ָ��ַ���
 * @param n {size_t} ���ָ�����
 * @return {ACL_ARGV*}
 */
ACL_API ACL_ARGV *acl_argv_splitn_append(ACL_ARGV *argvp, const char *str,
	const char *delim, size_t n);

#define ACL_ARGV_END	((char *) 0)

# ifdef	__cplusplus
}
# endif

#endif

