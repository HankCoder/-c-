#ifndef	ACL_TRACE_INCLUDE_H
#define	ACL_TRACE_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * ����ǰ�����Ķ�ջ�����ָ���ļ���
 * @param filepath {const char*} Ŀ���ļ���
 */
void acl_dump_strace(const char *filepath);
#define acl_dump_trace acl_dump_strace

/**
 * ����ǰ�����Ķ�ջ�������־��
 */
void acl_log_strace(void);

#ifdef	__cplusplus
}
#endif

#endif
