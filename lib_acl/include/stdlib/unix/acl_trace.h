#ifndef	ACL_TRACE_INCLUDE_H
#define	ACL_TRACE_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * ����ǰ�����Ķ�ջ�����ָ���ļ���
 * @param filepath {const char*} Ŀ���ļ���
 */
void acl_trace_save(const char *filepath);

/**
 * ����ǰ�����Ķ�ջ�������־��
 */
void acl_trace_info(void);

#ifdef	__cplusplus
}
#endif

#endif
