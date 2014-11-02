#ifndef ACL_VSTRING_INCLUDE_H
#define ACL_VSTRING_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include <stdarg.h>
#include "acl_vbuf.h"
#include "acl_slice.h"

/**
 * ��װ�� ACL_VBUF��ACL_VSTRING �ṹ���Ͷ���
 */
typedef struct ACL_VSTRING {
    ACL_VBUF  vbuf;
    int       maxlen;
    ACL_SLICE_POOL *slice;
} ACL_VSTRING;

/**
 * ��ʼ�� ACL_VSTRING �ṹ��ָ��ȱʡ�Ļ�������С�����û����Լ��ĺ�����
 * �ڲ��� ACL_VSTRING str ��ʽ���Ƕ�̬���䷽ʽ��ʹ��ʱ��Ҫ�ô˺������г�ʼ��,
 * ���⣬������ acl_vstring_free_buf ��ʽ���ͷ��ɸú���������ڲ�������
 * @param vp {ACL_VSTRING*} �����ַ������Ϊ��
 * @param len {size_t} ��ʼʱ��������С
 */
ACL_API void acl_vstring_init(ACL_VSTRING *vp, size_t len);

/**
 * ���� acl_vstring_init ��ʼ�� ACL_VSTRING ����ʱ��Ҫ���ô˺����ͷŻ������ڴ�
 * @param vp {ACL_VSTRING*} �����ַ������Ϊ��
 */
ACL_API void acl_vstring_free_buf(ACL_VSTRING *vp);

/**
 * ��̬����һ�� ACL_VSTRING ����ָ���ڲ��������ĳ�ʼ����С
 * @param len {size_t} ��ʼʱ��������С
 * @return {ACL_VSTRING*} �·���� ACL_VSTRING ����
 */
ACL_API ACL_VSTRING *acl_vstring_alloc(size_t len);

/**
 * ��̬����һ�� ACL_VSTRING ����ָ���ڲ��������ĳ�ʼ����С��
 * ͬʱָ���ڴ�ض����Ż��ڴ����
 * @param slice {ACL_SLICE_POOL*} ��Ƭ�ڴ�ع������
 * @param len {size_t} ��ʼʱ��������С
 * @return {ACL_VSTRING*} �·���� ACL_VSTRING ����
 */
ACL_API ACL_VSTRING *acl_vstring_alloc2(ACL_SLICE_POOL *slice, size_t len);

/**
 * ���� ACL_VSTRING ���������, Ŀǰ�ú����Ĺ��ܻ���������
 * @param vp {ACL_VSTRING*}
 * @param ... �� ACL_VSTRING_CTL_XXX ��ʾ�Ŀ��Ʋ�����������־Ϊ
 *  ACL_VSTRING_CTL_END
 */
ACL_API void acl_vstring_ctl(ACL_VSTRING *vp,...);

#define ACL_VSTRING_CTL_MAXLEN      1
#define ACL_VSTRING_CTL_END         0

/**
 * ���������ڵ����ݽض���ָ�����ȣ�ͬʱ��֤������������ '\0' ��β
 * @param vp {ACL_VSTRING*}
 * @param len {size_t} �ض̺�ĳ���
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_truncate(ACL_VSTRING *vp, size_t len);

/**
 * �ͷ��� acl_vstring_alloc ��̬����� ACL_VSTRING ����
 * @param vp {ACL_VSTRING*}
 * @return {ACL_VSTRING*} ��ԶΪ NULL�����Բ��ع��ķ���ֵ
 */
ACL_API ACL_VSTRING *acl_vstring_free(ACL_VSTRING *vp);

/**
 * �����ַ���
 * @param vp {ACL_VSTRING*}
 * @param src {const char*} Դ�ַ���
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_strcpy(ACL_VSTRING *vp, const char *src);

/**
 * �����ַ����������ó����涨��������
 * @param vp {ACL_VSTRING*}
 * @param src {const char*} Դ�ַ���
 * @param len {size_t} �涨��������
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_strncpy(ACL_VSTRING *vp,
		const char *src, size_t len);

/**
 * ���ӿ����ַ���
 * @param vp {ACL_VSTRING*}
 * @param src {const char*} Դ�ַ���
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_strcat(ACL_VSTRING *vp, const char *src);

/**
 * ���ӿ����ַ����������ó����涨��������
 * @param vp {ACL_VSTRING*}
 * @param src {const char*} Դ�ַ���
 * @param len {size_t} �涨��������
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_strncat(ACL_VSTRING *vp,
		const char *src, size_t len);

/**
 * �����ڴ������ݣ�ͬʱ��֤Ŀ�껺����β���� '\0'
 * @param vp {ACL_VSTRING*}
 * @param src {const char*} Դ���ݵ�ַ
 * @param len {size_t} Դ���ݳ���
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_memcpy(ACL_VSTRING *vp,
		const char *src, size_t len);

/**
 * �ƶ��ڴ�������, Դ������Ŀ���ַ���ڴ���������ͬһ���ڴ���Ҳ���Բ���
 * ͬһ���ڴ������ú�����֤Ŀ���ַβ���� '\0' ��β
 * @param vp {ACL_VSTRING*}
 * @param src {const char*} Դ���ݵ�ַ
 * @param len {size_t} Դ���ݳ���
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_memmove(ACL_VSTRING *vp,
		const char *src, size_t len);

/**
 * �����ڴ������ú�����֤Ŀ�껺������ '\0' ��β
 * @param vp {ACL_VSTRING*}
 * @param src {const char*} Դ���ݵ�ַ
 * @param len {size_t} Դ���ݳ���
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_memcat(ACL_VSTRING *vp,
		const char *src, size_t len);

/**
 * ����ĳ���ַ�
 * @param vp {ACL_VSTRING*}
 * @param ch {int} Ҫ���ҵ��ַ�
 * @return {char*} Ŀ���ַ�����λ�õĵ�ַ, ���δ�鵽�򷵻� NULL, 
 *  ע���÷��ص�ַ�ǲ��ܱ������ͷŵģ���Ϊ���� ACL_VSTRING ����ͳһ���й���
 */
ACL_API char *acl_vstring_memchr(ACL_VSTRING *vp, int ch);

/**
 * ����ĳ���ַ������ַ�����Сд����
 * @param vp {ACL_VSTRING*}
 * @param needle {const char*} Ҫ���ҵ��ַ�
 * @return {char*} Ŀ���ַ�����λ�õĵ�ַ, ���δ�鵽�򷵻� NULL, 
 *  ע���÷��ص�ַ�ǲ��ܱ������ͷŵģ���Ϊ���� ACL_VSTRING ����ͳһ���й���
 */
ACL_API char *acl_vstring_strstr(ACL_VSTRING *vp, const char *needle);

/**
 * ����ĳ���ַ����������ַ�����Сд
 * @param vp {ACL_VSTRING*}
 * @param needle {const char*} Ҫ���ҵ��ַ�
 * @return {char*} Ŀ���ַ�����λ�õĵ�ַ, ���δ�鵽�򷵻� NULL, 
 *  ע���÷��ص�ַ�ǲ��ܱ������ͷŵģ���Ϊ���� ACL_VSTRING ����ͳһ���й���
 */
ACL_API char *acl_vstring_strcasestr(ACL_VSTRING *vp, const char *needle);

/**
 * �Ӻ���ǰ�����ַ������ַ�����Сд����
 * @param vp {ACL_VSTRING*}
 * @param needle {const char*} Ҫ���ҵ��ַ�
 * @return {char*} Ŀ���ַ�����λ�õĵ�ַ, ���δ�鵽�򷵻� NULL, 
 *  ע���÷��ص�ַ�ǲ��ܱ������ͷŵģ���Ϊ���� ACL_VSTRING ����ͳһ���й���
 */
ACL_API char *acl_vstring_rstrstr(ACL_VSTRING *vp, const char *needle);

/**
 * �Ӻ���ǰ�����ַ������ַ�����Сд������
 * @param vp {ACL_VSTRING*}
 * @param needle {const char*} Ҫ���ҵ��ַ�
 * @return {char*} Ŀ���ַ�����λ�õĵ�ַ, ���δ�鵽�򷵻� NULL,
 *  ע���÷��ص�ַ�ǲ��ܱ������ͷŵģ���Ϊ���� ACL_VSTRING ����ͳһ���й���
 */
ACL_API char *acl_vstring_rstrcasestr(ACL_VSTRING *vp, const char *needle);

/**
 * �򻺳�����ĳ��ָ��λ�ú�������ݣ�ͬʱ��֤Ŀ�껺���������� '\0' ��β
 * @param vp {ACL_VSTRING*}
 * @param start {size_t} ָ����λ��
 * @param buf {const char*} ���ݵ�ַ
 * @param len {size_t} ���ݳ���
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_insert(ACL_VSTRING *vp, size_t start,
		const char *buf, size_t len);

/**
 * �򻺳�����ͷ��������ݣ�ͬʱ��֤Ŀ�껺���������� '\0' ��β
 * @param vp {ACL_VSTRING*}
 * @param buf {const char*} ���ݵ�ַ
 * @param len {size_t} ���ݳ���
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_prepend(ACL_VSTRING *vp,
		const char *buf, size_t len);

/**
 * �򻺳�������ʽ��ʽ�������
 * @param vp {ACL_VSTRING*}
 * @param format {const char*} ��ʽ���ַ���
 * @param ... �������
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *ACL_PRINTF(2, 3) acl_vstring_sprintf(ACL_VSTRING *vp,
		const char *format,...);

/**
 * �Ը��ӷ�ʽ�򻺳�������ʽ��ʽ�������
 * @param vp {ACL_VSTRING*}
 * @param format {const char*} ��ʽ���ַ���
 * @param ... �������
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *ACL_PRINTF(2, 3) acl_vstring_sprintf_append(
		ACL_VSTRING *vp, const char *format,...);

/**
 * �����������ڵ�������ͬʱ�� ACL_VSTRING �����ͷţ��û���Ҫ��������
 * acl_myfree ���ͷŷ��ص��������ڴ�
 * @param vp {ACL_VSTRING*}
 * @return {char*} ��������ַ��������ֵ��Ϊ NULL ʱ�û���Ҫ��������
 *  acl_myfree ���ͷŸõ�ַ�����������ڴ�й©
 */
ACL_API char *acl_vstring_export(ACL_VSTRING *vp);

/**
 * ���û��Ĵ洢�ַ����Ķ�̬�����ڴ������벢�����µ� ACL_VSTRING ����
 * @param str {char*} �ⲿ��̬����Ĵ洢�ַ������ڴ��ַ
 * @return {ACL_VSTRING*} �·���� ACL_VSTRING ����
 */
ACL_API ACL_VSTRING *acl_vstring_import(char *str);

/**
 * ����̬�ڴ����� ACL_VSTRING ճ��
 * ע��vp �����ǵ��� acl_vstring_alloc �����ģ����Ҳ��ܵ��� acl_vstring_init
 *     ���й���ʼ��, vp �������� acl_mymalloc ������λ��ջ�ϵ�һ������
 *    ����: ACL_VSTRING v��
 * @param vp {ACL_VSTRING*} ���� acl_mymalloc ���ɻ���һ��ջ����, ����
 *  acl_mymalloc ��ʽ���ɵ�ʱӦ��ͨ�� acl_myfree �ͷ���
 * @param buf {void*} �û����ݵ��ڴ���, ������ջ����
 * @param len {size_t} buf �ڴ����ĳ���
 */
ACL_API void acl_vstring_glue(ACL_VSTRING *vp, void *buf, size_t len);

/**
 * ȡ��ĳ��λ�õ��ַ�
 * @param vp {ACL_VSTRING*}
 * @param len {size_t} λ�ã������ֵԽ�磬�����ڲ��� fatal
 * @return {char} ���ҵ��ַ�
 */
ACL_API char acl_vstring_charat(ACL_VSTRING *vp, size_t len);

/**
 * ���涨��ʽ�������
 * @param vp {ACL_VSTRING*}
 * @param format {const char*}
 * @param ap {va_list}
 * @return {ACL_VSTRING*} �� vp ��ͬ
 * @see acl_vstring_sprintf
 */
ACL_API ACL_VSTRING *acl_vstring_vsprintf(ACL_VSTRING *vp,
		const char *format, va_list ap);

/**
 * ���涨��ʽ��β���������
 * @param vp {ACL_VSTRING*}
 * @param format {const char*}
 * @param ap {va_list}
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_vsprintf_append(ACL_VSTRING *vp,
		const char *format, va_list ap);

/**
 * ���涨��ʽ��ͷ���������
 * @param vp {ACL_VSTRING*}
 * @param format {const char*}
 * @param ... �������
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *ACL_PRINTF(2, 3) acl_vstring_sprintf_prepend(
		ACL_VSTRING *vp, const char *format, ...);

/**
 * ��Դ���л��һ������(���� "\r\n" �� "\n")��ͬʱ��ʣ�����ݻ�������, ���
 * δ��������У���ֻ����Դ��
 * @param vp {ACL_VSTRING*} �ַ������ݻ�����
 * @param src {const char**} Դ�ַ���ָ���ַ, ���غ�ָ���ַ�ƶ�����һλ��
 * @param dlen {size_t} Դ�ַ������ݳ���
 * @return {const ACL_VSTRING*} NULL, ��ʾδ�ҵ� "\r\n" �� "\n"�����Ὣʣ��
 *  �����ݿ������������ڣ�Ӧ����Ҫͨ�� ACL_VSTRING_LEN �жϻ��������Ƿ���
 *  ���ݣ�!NULL����ʾ����������
 *  ע�����������к�Ӧ�õ��� ACL_VSTRING_RESET(vp) �����������
 */
ACL_API const ACL_VSTRING *acl_buffer_gets_nonl(ACL_VSTRING *vp,
		const char **src, size_t dlen);

/**
 * ��Դ���л��һ������(���� "\r\n" �� "\n")��ͬʱ��ʣ�����ݻ�������, 
 * ���δ��������У���ֻ����Դ��
 * @param vp {ACL_VSTRING*} �ַ������ݻ�����
 * @param src {const char**} Դ�ַ���ָ���ַ, ���غ�ָ���ַ�ƶ�����һλ��
 * @param dlen {size_t} Դ�ַ������ݳ���
 * @return {const ACL_VSTRING*} NULL, ��ʾδ�ҵ� "\r\n" �� "\n"�����Ὣʣ��
 *  �����ݿ������������ڣ�Ӧ����Ҫͨ�� ACL_VSTRING_LEN �жϻ��������Ƿ���
 *  ���ݣ�!NULL����ʾ����������
 *  ע�����������к�Ӧ�õ��� ACL_VSTRING_RESET(vp) �����������
 */
ACL_API const ACL_VSTRING *acl_buffer_gets(ACL_VSTRING *vp,
		const char **src, size_t dlen);

 /*
  * Macros. Unsafe macros have UPPERCASE names.
  */
#define ACL_VSTRING_SPACE(vp, len) ((vp)->vbuf.space(&(vp)->vbuf, len))

/**
 * ȡ�õ�ǰ ACL_VSTRING ���ݴ洢��ַ
 * @param vp {ACL_VSTRING*}
 * @return {char*}
 */
#define acl_vstring_str(vp) ((char *) (vp)->vbuf.data)

/**
 * ȡ�õ�ǰ ACL_VSTRING ���洢�����ݵĳ���
 * @param vp {ACL_VSTRING*}
 * @return {int}
 */
#define ACL_VSTRING_LEN(vp) (size_t) ((vp)->vbuf.ptr - (vp)->vbuf.data)

/**
 * ȡ�õ�ǰ ACL_VSTRING �ڲ����������ܴ�С
 * @param vp {ACL_VSTRING*}
 * @return {int}
 */
#define	ACL_VSTRING_SIZE(vp) ((vp)->vbuf.len)

/**
 * ȡ�õ�ǰ ACL_VSTRING ������ƫ��ָ��λ��
 * @param vp {ACL_VSTRING*}
 * @return {char*}
 */
#define acl_vstring_end(vp) ((char *) (vp)->vbuf.ptr)

/**
 * �� ACL_VSTRING ������ƫ��ָ��λ���� 0
 * @param vp {ACL_VSTRING*}
 */
#define ACL_VSTRING_TERMINATE(vp) { \
	if ((vp)->vbuf.cnt <= 0) \
		ACL_VSTRING_SPACE((vp),1); \
	*(vp)->vbuf.ptr = 0; \
}

/**
 * ���� ACL_VSTRING �ڲ�������ָ���ַ��ʼλ�ã������Ὣβ�������� 0��Ӧ�ÿ���
 * ͨ������ ACL_VSTRING_TERMINATE ����������β���� 0
 * @param vp {ACL_VSTRING*}
 */
#define ACL_VSTRING_RESET(vp) {	\
	(vp)->vbuf.ptr = (vp)->vbuf.data; \
	(vp)->vbuf.cnt = (vp)->vbuf.len; \
}

/**
 * ���һ���ַ��� ACL_VSTRING ������
 * @param vp {ACL_VSTRING*}
 * @param ch {int} �ַ�
 */
#define	ACL_VSTRING_ADDCH(vp, ch) ACL_VBUF_PUT(&(vp)->vbuf, ch)

/**
 * �ƶ�����ƫ��ָ�����ڲ�������β��
 * @param vp {ACL_VSTRING*}
 */
#define ACL_VSTRING_SKIP(vp) { \
	while ((vp)->vbuf.cnt > 0 && *(vp)->vbuf.ptr) \
		(vp)->vbuf.ptr++, (vp)->vbuf.cnt--; \
}

/**
 * ��ǰ ACL_VSTRING �л��ж������ݿ���
 * @param vp {ACL_VSTRING*}
 */
#define acl_vstring_avail(vp) ((vp)->vbuf.cnt)

 /**
  * The following macro is not part of the public interface, because it can
  * really screw up a buffer by positioning past allocated memory.
  */
#define ACL_VSTRING_AT_OFFSET(vp, offset) { \
	(vp)->vbuf.ptr = (vp)->vbuf.data + (offset); \
	(vp)->vbuf.cnt = (vp)->vbuf.len - (offset); \
}

#ifdef  __cplusplus
}
#endif

#endif
