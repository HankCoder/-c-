#ifndef ACL_MSG_INCLUDE_H
#define ACL_MSG_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include <stdarg.h>
#include "acl_vstream.h"

#undef	USE_PRINTF_MACRO

/**
 * �ڽ�д��־����־�ļ�ǰ�ص��û��Զ���ĺ������ҽ���־��Ϣ���ݸ��ú�����
 * ֻ�е��û�ͨ�� acl_msg_pre_write �������ú����Ч
 * @param ctx {void*} �û����Զ������
 * @param fmt {const char*} ��ʽ����
 * @param ap {va_list} ��ʽ�����б�
 */
typedef void (*ACL_MSG_PRE_WRITE_FN)(void *ctx, const char *fmt, va_list ap);

/**
 * Ӧ��ͨ���˺������Ϳ����Զ�����־�򿪺�������Ӧ���ڴ���־ǰ����
 * acl_msg_register ע�����Զ���򿪺�������Ӧ�õ��� acl_msg_open
 * ʱ����ô˶������־��������־����������ȱʡ�ķ�������־�ļ�
 * @param file_name {const char*} �ش����Զ�����־�򿪺����Ĳ�������
 *  ����־�ļ��ش�
 * @param ctx {void*} Ӧ�ô��ݽ�ȥ�Ĳ���
 * @return {int} ����Զ������־�������� -1 �����ȱʡ����־�򿪺���
 */
typedef int (*ACL_MSG_OPEN_FN) (const char *file_name, void *ctx);

/**
 * Ӧ��ͨ���˺������Ϳ����Զ�����־�رպ�������Ӧ���ڴ���־ǰ����
 * acl_msg_register ע�����Զ���򿪺�������Ӧ�õ��� acl_msg_close
 * ʱ����ô˶���ر���־�����ر���־����������ȱʡ�ķ����ر���־�ļ�
 * @param ctx {void*} Ӧ�ô��ݽ�ȥ�Ĳ���
 */
typedef void (*ACL_MSG_CLOSE_FN) (void *ctx);

/**
 * Ӧ��ͨ���˺������Ϳ����Զ�����־��¼��������Ӧ���ڴ���־ǰ����
 * acl_msg_register ע�����Զ����¼��������Ӧ��д��־ʱ���ô��Զ���
 * ������¼��־��������ȱʡ����־��¼����
 * @param ctx {void*} Ӧ�ô��ݽ�ȥ�Ĳ���
 * @param fmt {const char*} ��ʽ����
 * @param ap {va_list} �����б�
 */
typedef void (*ACL_MSG_WRITE_FN) (void *ctx, const char *fmt, va_list ap);

/**
 * �ڴ���־ǰ���ô˺���ע��Ӧ���Լ�����־�򿪺�������־�رպ�������־��¼����
 * @param open_fn {ACL_MSG_OPEN_FN} �Զ�����־�򿪺���
 * @param close_fn {ACL_MSG_CLOSE_FN} �Զ�����־�رպ���
 * @param write_fn {ACL_MSG_WRITE_FN} �Զ�����־��¼����
 * @param ctx {void*} �Զ������
 */
ACL_API void acl_msg_register(ACL_MSG_OPEN_FN open_fn, ACL_MSG_CLOSE_FN close_fn,
        ACL_MSG_WRITE_FN write_fn, void *ctx);

/**
 * �� acl_msg_register ע���Զ��庯�����������ȱʡ����־������
 */
ACL_API void acl_msg_unregister(void);

/**
 * �ڴ���־ǰ���ô˺���ע��Ӧ�õ�˽�к������ڼ�¼��־ǰ���ȼ�¼��Ϣͨ��
 * ��ע��ĺ������ݸ�Ӧ��
 * @param pre_write {ACL_MSG_PRE_WRITE_FN} ��־��¼ǰ���õĺ���
 * @param ctx {void*} �Զ������
 */
ACL_API void acl_msg_pre_write(ACL_MSG_PRE_WRITE_FN pre_write, void *ctx);

/**
 * ȫ�ֱ�������ʾ���Լ���
 * @deprecated �����ò�����ֻ���ڲ�ʹ�ã��ⲿӦ�ò�Ӧ����
 */
extern ACL_API int acl_msg_verbose;

/**
 * ��δ���� acl_msg_open ��ʽ����־ʱ�������� acl_msg_info/error/fatal/warn
 * �Ĳ������Ƿ�������Ϣ�������׼�����Ļ�ϣ�ͨ���˺��������øÿ��أ��ÿ���
 * ��Ӱ���Ƿ���Ҫ����Ϣ������ն���Ļ����Ӱ���Ƿ�������ļ���
 * @param onoff {int} �� 0 ��ʾ�����������Ļ
 */
ACL_API void acl_msg_stdout_enable(int onoff);

/**
 * ������ acl_msg_error_xxx/acl_msg_warn_xxx �Ⱥ�����¼����򾯸����͵���־ʱ
 * �Ƿ���Ҫ��¼���ö�ջ�����ɸú�����������
 * @param onoff {int} �� 0 ��ʾ�����¼���ó���/������־�Ķ�ջ��ȱʡ����¼
 */
ACL_API void acl_msg_trace_enable(int onoff);

/**
 * ��־�򿪺���
 * @param log_file {const char*} ��־�����߼��ϣ��� "|" �ָ���������
 *  �����Ǳ����ļ���Զ���׽ӿڣ���:
 *  /tmp/test.log|UDP:127.0.0.1:12345|TCP:127.0.0.1:12345|UNIX:/tmp/test.sock
 *  ������Ҫ��������־ͬʱ���� /tmp/test.log, UDP:127.0.0.1:12345,
 *  TCP:127.0.0.1:12345 �� UNIX:/tmp/test.sock �ĸ���־����������
 * @param plog_pre {const char*} ��־��¼��Ϣǰ����ʾ��Ϣ�������ý���
 * @param info_pre {const char*} ��־��¼��Ϣǰ����ʾ��Ϣ
 */
ACL_API void acl_msg_open(const char *log_file, const char *info_pre);

/**
 * ��־�򿪺���
 * @param fp {ACL_VSTREAM *} ��־�ļ������
 * @param info_pre {const char*} ��־��¼��Ϣǰ����ʾ��Ϣ
 */
ACL_API void acl_msg_open2(ACL_VSTREAM *fp, const char *info_pre);

/**
 * �ر���־����
 */
ACL_API void acl_msg_close(void);

/**
 * ����¼��־��Ϣ����־�ļ�ʱ����Ҫ�������µ���־��¼����
 */

#ifndef	USE_PRINTF_MACRO

/**
 * һ�㼶����־��Ϣ��¼����
 * @param fmt {const char*} ������ʽ
 * @param ... �������
 */
ACL_API void ACL_PRINTF(1, 2) acl_msg_info(const char *fmt,...);

/**
 * ���漶����־��Ϣ��¼����
 * @param fmt {const char*} ������ʽ
 * @param ... �������
 */
ACL_API void ACL_PRINTF(1, 2) acl_msg_warn(const char *fmt,...);

/**
 * ���󼶱���־��Ϣ��¼����
 * @param fmt {const char*} ������ʽ
 * @param ... �������
 */
ACL_API void ACL_PRINTF(1, 2) acl_msg_error(const char *fmt,...);

/**
 * ����������־��Ϣ��¼����
 * @param fmt {const char*} ������ʽ
 * @param ... �������
 */
ACL_API void ACL_PRINTF(1, 2) acl_msg_fatal(const char *fmt,...);

/**
 * ����������־��Ϣ��¼����
 * @param status {int} ��ǰδ��
 * @param fmt {const char*} ������ʽ
 * @param ... �������
 */
ACL_API void ACL_PRINTF(2, 3)
	acl_msg_fatal_status(int status, const char *fmt,...);

/**
 * �ֻż�����־��Ϣ��¼����
 * @param fmt {const char*} ������ʽ
 * @param ... �������
 */
ACL_API void ACL_PRINTF(1, 2) acl_msg_panic(const char *fmt,...);

/**
 * һ�㼶����־��Ϣ��¼����
 * @param fmt {const char*} ������ʽ
 * @param ap {va_list} ����б�
 */
ACL_API void acl_msg_info2(const char *fmt, va_list ap);


/**
 * ���漶����־��Ϣ��¼����
 * @param fmt {const char*} ������ʽ
 * @param ap {va_list} ����б�
 */
ACL_API void acl_msg_warn2(const char *fmt, va_list ap);

/**
 * ���󼶱���־��Ϣ��¼����
 * @param fmt {const char*} ������ʽ
 * @param ap {va_list} ����б�
 */
ACL_API void acl_msg_error2(const char *fmt, va_list ap);


/**
 * ����������־��Ϣ��¼����
 * @param fmt {const char*} ������ʽ
 * @param ap {va_list} ����б�
 */
ACL_API void acl_msg_fatal2(const char *fmt, va_list ap);

/**
 * ����������־��Ϣ��¼����
 * @param status {int} ��ǰδ��
 * @param fmt {const char*} ������ʽ
 * @param ap {va_list} ����б�
 */
ACL_API void acl_msg_fatal_status2(int status, const char *fmt, va_list ap);

/**
 * �ֻż�����־��Ϣ��¼����
 * @param fmt {const char*} ������ʽ
 * @param ap {va_list} ����б�
 */
ACL_API void acl_msg_panic2(const char *fmt, va_list ap);
#else

/**
 * ����¼��־��Ϣ����׼���ʱ����Ҫ�������µ���־��¼����
 */

#include <stdio.h>

#undef	acl_msg_info
#undef	acl_msg_warn
#undef	acl_msg_error
#undef	acl_msg_fatal
#undef	acl_msg_panic

#define	acl_msg_info	acl_msg_printf
#define	acl_msg_warn	acl_msg_printf
#define	acl_msg_error	acl_msg_printf
#define	acl_msg_fatal	acl_msg_printf
#define	acl_msg_panic	acl_msg_printf

#endif

/**
 * �����ڱ�׼C�� strerror, ���ú����ǿ�ƽ̨�����̰߳�ȫ�ģ���ö�Ӧĳ������
 * �ŵĴ���������Ϣ
 * @param errnum {unsigned int} �����
 * @param buffer {char*} �洢����������Ϣ���ڴ滺����
 * @param size {int} buffer �������Ĵ�С
 * @return {const char*} ���صĵ�ַӦ�� buffer ��ͬ
 */
ACL_API const char *acl_strerror(unsigned int errnum, char *buffer, int size);

/**
 * ����ϴ�ϵͳ���ó���ʱ�Ĵ���������Ϣ
 * @param buffer {char*} �洢����������Ϣ���ڴ滺����
 * @param size {int} buffer �Ŀռ��С
 * @return {const char*} ���صĵ�ַӦ�� buffer ��ͬ
 */
ACL_API const char *acl_last_strerror(char *buffer, int size);

/**
 * ����ϴ�ϵͳ���ó���ʱ�Ĵ���������Ϣ���ú����ڲ��������ֲ߳̾��������������߳�
 * ��ȫ�ģ���ʹ����������Щ
 * @return {const char *} ���ش�����ʾ��Ϣ 
 */
ACL_API const char *acl_last_serror(void);

/**
 * ����ϴ�ϵͳ���ó���ʱ�Ĵ����
 * @return {int} �����
 */
ACL_API int acl_last_error(void);

/**
 * �ֹ����ô����
 * @param errnum {int} �����
 */
ACL_API void acl_set_error(int errnum);

/**
 * �����Ϣ����׼���
 * @param fmt {const char*} ��ʽ����
 * @param ... �������
 */
ACL_API void ACL_PRINTF(1, 2) acl_msg_printf(const char *fmt,...);

#ifdef  __cplusplus
}
#endif

#endif

