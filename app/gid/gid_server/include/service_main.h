
#ifndef	__SERVICE_MAIN_INCLUDE_H__
#define	__SERVICE_MAIN_INCLUDE_H__

#include "lib_acl.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * ��ʼ��������������ģ��������������øú���һ��
 * @param ctx {void*} �û��Զ�������ָ��
 */
extern void service_init(void *ctx);
extern void service_exit(void *ctx);

/**
 * Э�鴦�������
 * @param stream {ACL_VSTREAM*} �ͻ�������������
 * @param ctx {void*} �û��Զ�������ָ��
 */
extern int service_main(ACL_VSTREAM *stream, void *ctx);

#ifdef	__cplusplus
}
#endif

#endif
