#ifndef	ACL_MYLOG_INCLUDE_H
#define	ACL_MYLOG_INCLUDE_H

#include <stdarg.h>

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_vstream.h"

#ifdef	ACL_UNIX
#include <netinet/in.h>
#endif

typedef struct ACL_LOG ACL_LOG;

/**
 * ����ǰ��ʱ��ת������־��¼��ʽ��ʱ���ʽ
 * @param buf {char*} �ڴ�洢��
 * @param size {size_t} buf �Ŀռ��С
 */
ACL_API void acl_logtime_fmt(char *buf, size_t size);

/**
 * �����Ƿ��¼�߳�ID�ţ�Ĭ��������ǲ���¼��
 * @param onoff {int} �� 0 ��ʾ��¼�߳�ID�����򲻼�¼
 */
ACL_API void acl_log_add_tid(int onoff);

/**
 * ������־���ļ������
 * @param fp {ACL_VSTREAM *} �ļ������
 * @param plog_pre {const char*} ��־��¼��Ϣǰ����ʾ��Ϣ�������ý���
 */
ACL_API void acl_log_fp_set(ACL_VSTREAM *fp, const char *plog_pre);

/**
 * ����־�ļ�
 * @param recipients {const char*} ��־�������б��� "|" �ָ���������
 *  �����Ǳ����ļ���Զ���׽ӿڣ���:
 *  /tmp/test.log|UDP:127.0.0.1:12345|TCP:127.0.0.1:12345|UNIX:/tmp/test.sock
 *  ������Ҫ��������־ͬʱ���� /tmp/test.log, UDP:127.0.0.1:12345,
 *  TCP:127.0.0.1:12345 �� UNIX:/tmp/test.sock �ĸ���־����������
 * @param plog_pre {const char*} ��־��¼��Ϣǰ����ʾ��Ϣ�������ý���
 *  ����д��ֵ
 */
ACL_API int acl_open_log(const char *recipients, const char *plog_pre);

/**
 * д��־
 * @param fmt {const char*} ��ʽ����
 * @param ... ��������
 * @return {int} д����־�ļ���������
 */
ACL_API int ACL_PRINTF(1, 2) acl_write_to_log(const char *fmt, ...);

/**
 * д��־
 * @param info {const char*} ��־��Ϣ����ʾ��Ϣ
 * @param fmt {const char*} ��ʽ����
 * @param ap {va_list} �����б�
 * @return {int} д����־�ļ���������
 */
ACL_API int acl_write_to_log2(const char *info, const char *fmt, va_list ap);

/**
 * �ر���־�ļ����
 */
ACL_API void acl_close_log(void);

#ifdef  __cplusplus
}
#endif

#endif


