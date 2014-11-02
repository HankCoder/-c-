#ifndef	ACL_FILE_INCLUDE_H
#define	ACL_FILE_INCLUDE_H

#include "acl_define.h"
#include "acl_vstream.h"
#include <stdarg.h>

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * �ļ������Ͷ���
 */
typedef struct ACL_FILE {
	ACL_VSTREAM *fp;	/**< ��ָ�� */
	unsigned int status;	/**< �ļ���״̬ */
#define	ACL_FILE_EOF		(1 << 0)
	int   errnum;		/**< �ļ����ĳ������� */
} ACL_FILE;

#define	ACL_FPATH(fp)	ACL_VSTREAM_PATH((fp)->fp)
#define	ACL_FSTREAM(fp)	((fp)->fp)

/**
 * �򿪻򴴽�һ���ļ����ж�д����
 * @param filenme {const char*} �ļ���
 * @param mode {const char*} �򿪱�־.
 *  r or rb: ֻ����ʽ���Ѿ������ļ�
 *  w or wb: ֻд��ʽ���Ѵ����ļ�����ջ�ֻд��ʽ�������ļ�
 *  a or ab: β������ֻд��ʽ���Ѵ����ļ��򴴽����ļ�
 *  r+ or rb+: �Զ�д��ʽ���Ѵ����ļ�
 *  w+ or wb+: �Զ�д��ʽ���Ѵ����ļ�����ջ򴴽����ļ�
 *  a+ or ab+: ��β�����Ӷ�д��ʽ���Ѵ����ļ��򴴽����ļ�
 */
ACL_API ACL_FILE *acl_fopen(const char *filename, const char *mode);

/**
 * �ر�һ���ļ���
 * @param fp {ACL_FILE*} �ļ���
 */
ACL_API int acl_fclose(ACL_FILE *fp);

/**
 * ����ļ����Ĵ����
 * @param fp {ACL_FILE*} �ļ���
 */
ACL_API void acl_clearerr(ACL_FILE *fp);

/**
 * �ж��Ƿ񵽴��ļ�β��
 * @param fp {ACL_FILE*} �ļ���
 * @return {int} 0: ��; !0: ��
 */
ACL_API int acl_feof(ACL_FILE *fp);

/**
 * ���ļ����ж�ȡһЩ�̶����ȵ����ݿ�
 * @param buf {void*} �ڴ滺������ַ
 * @param size {size_t} ÿ�����ݿ鳤��
 * @param nitems {size_t} ���ݿ����
 * @param fp {ACL_FILE*} �ļ���
 * @return {size_t} ���ݿ����, �������򷵻� EOF
 */
ACL_API size_t acl_fread(void *buf, size_t size, size_t nitems, ACL_FILE *fp);

/**
 * ���ļ����ж�ȡһ������
 * @param buf {char*} ��������ַ
 * @param size {int} buf �ռ��С
 * @param fp {ACL_FILE*} �ļ���
 * @return {char*} NULL: δ��������������; !NULL: ��������������
 */
ACL_API char *acl_fgets(char *buf, int size, ACL_FILE *fp);

/**
 * ���ļ����ж�ȡһ�����ݣ������ص�����β�������� "\r\n"
 * @param buf {char*} ��������ַ
 * @param size {int} buf �ռ��С
 * @param fp {ACL_FILE*} �ļ���
 * @return {char*} NULL: δ��������������; !NULL: ��������������
 */
ACL_API char *acl_fgets_nonl(char *buf, int size, ACL_FILE *fp);

/**
 * ���ļ����ж�ȡһ���ַ�
 * @param fp {ACL_FILE*} �ļ���
 * @return {int} EOF: �����ļ�β�����; !EOF: ��ȷ����һ���ַ��� ASCII ��
 */
ACL_API int acl_fgetc(ACL_FILE *fp);
#define	acl_getc	acl_fgetc

/**
 * �ӱ�׼�����ж�ȡһ������
 * @param buf {char*} ��������ַ
 * @param size {int} buf �ռ��С
 * @return {char*} NULL: �����������; !NULL: Ӧ�� buf ��ͬ��ַ
 */
ACL_API char *acl_gets(char *buf, size_t size);

/**
 * �ӱ�׼�����ж�ȡһ������, ������β�������� "\r\n"
 * @param buf {char*} ��������ַ
 * @param size {int} buf �ռ��С
 * @return {char*} NULL: �����������; !NULL: Ӧ�� buf ��ͬ��ַ
 */
ACL_API char *acl_gets_nonl(char *buf, size_t size);

/**
 * �ӱ�׼�����ж�ȡһ���ַ�
 * @return {int} EOF: �����ļ�β�����; !EOF: ��ȷ����һ���ַ��� ASCII ��
 */
ACL_API int acl_getchar(void);

/**
 * ���ļ�����д���θ�ʽ����
 * @param fp {ACL_FILE*} �ļ������
 * @param fmt {const char*} ��θ�ʽ
 * @param ... ���
 * @return {size_t} ���ݳ���, �������򷵻� EOF
 */
ACL_API int ACL_PRINTF(2, 3) acl_fprintf(ACL_FILE *fp, const char *fmt, ...);

/**
 * ���ļ�����д���θ�ʽ����
 * @param fp {ACL_FILE*} �ļ������
 * @param fmt {const char*} ��θ�ʽ
 * @param ap {va_list} ����б�
 * @return {size_t} ���ݳ���, �������򷵻� EOF
 */
ACL_API int acl_vfprintf(ACL_FILE *fp, const char *fmt, va_list ap);

/**
 * ���ļ�����д��һЩ�̶����ȵ����ݿ�
 * @param ptr {const void*} ���ݵ�ַ
 * @param size {size_t} ÿ�����ݿ鳤��
 * @param nitems {size_t} ���ݿ����
 * @param fp {ACL_FILE*} �ļ���ָ��
 * @return {size_t} ���ݿ����, �������򷵻� EOF
 */
ACL_API size_t acl_fwrite(const void *ptr, size_t size, size_t nitems, ACL_FILE *fp);

/**
 * ���ļ�����д�����ݲ��Զ���β����� "\r\n"
 * @param s {const char*} �ַ�����ַ
 * @param fp {ACL_FILE*} �ļ���ָ��
 * @return {int} д���������(���� "\r\n"), �������򷵻� EOF
 */
ACL_API int acl_fputs(const char *s, ACL_FILE *fp);

/**
 * ���׼�������д���θ�ʽ����
 * @param fmt {const char*} ��θ�ʽ
 * @param ... ���
 * @return {size_t} ���ݳ���, �������򷵻� EOF
 */
ACL_API int ACL_PRINTF(1, 2) acl_printf(const char *fmt, ...);

/**
 * ���׼�������д���θ�ʽ����
 * @param fmt {const char*} ��θ�ʽ
 * @param ap {va_list} ����б�
 * @return {size_t} ���ݳ���, �������򷵻� EOF
 */
ACL_API int acl_vprintf(const char *fmt, va_list ap);

/**
 * ���ļ�����д��һ���ֽ�
 * @param c {int} һ������ ASCII ��
 * @param fp {ACL_FILE*} �ļ���ָ��
 * @return {int} д���������, �������򷵻� EOF
 */
ACL_API int acl_putc(int c, ACL_FILE *fp);
#define	acl_fputc	acl_putc

/**
 * ���׼�����д�����ݲ��Զ���β����� "\r\n"
 * @param s {const char*} �ַ�����ַ
 * @return {int} д���������(���� "\r\n"), �������򷵻� EOF
 */
ACL_API int acl_puts(const char *s);

/**
 * ���ļ�����д��һ���ֽ�
 * @param c {int} һ������ ASCII ��
 * @return {int} д���������, �������򷵻� EOF
 */
ACL_API int acl_putchar(int c);

/**
 * ��λ�ļ�λ��
 * @param fp {ACL_FILE*} �ļ���
 * @param offset {acl_off_t} ƫ��λ��
 * @param whence {int} ƫ�Ʒ���, SEEK_SET, SEEK_CUR, SEEK_END
 * @return ret {acl_off_t}, ret >= 0: ��ȷ, ret < 0: ����
 */
ACL_API acl_off_t acl_fseek(ACL_FILE *fp, acl_off_t offset, int whence);

/**
 * ��õ�ǰ�ļ�ָ�����ļ��е�λ��
 * @param fp {ACL_FILE*} �ļ����
 * @return {acl_off_t} ����ֵ -1 ��ʾ����
 */
ACL_API acl_off_t acl_ftell(ACL_FILE *fp);

#ifdef	__cplusplus
}
#endif

#endif
