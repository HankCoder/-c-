
#ifndef	__SERVICE_MAIN_INCLUDE_H__
#define	__SERVICE_MAIN_INCLUDE_H__

#include "lib_acl.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * ��ʼ��������������ģ��������������øú���һ��
 * @param init_ctx {void*} �û��Զ�������ָ��
 */
extern void service_init(void *init_ctx);

/**
 * �����˳�ʱ�Ļص�����
 * @param exist_ctx {void*} �û��Զ�������ָ��
 */
extern void service_exit(void *exit_ctx);

/**
 * Э�鴦�������
 * @param stream {ACL_VSTREAM*} �ͻ�������������
 * @param run_ctx {void*} �û��Զ�������ָ��
 */
extern int service_main(ACL_VSTREAM *stream, void *run_ctx);

#ifdef	__cplusplus
}
#endif

#endif
