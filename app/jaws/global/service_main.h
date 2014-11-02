#ifndef	__SERVICE_MAIN_INCLUDE_H__
#define	__SERVICE_MAIN_INCLUDE_H__

#include "lib_acl.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* ȫ�ֱ��� */
extern ACL_MEM_SLICE *var_mem_slice;

/**
 * ��ʼ��������������ģ��������������øú���һ��
 * @param aio {ACL_AIO*} �첽��ܾ��
 */
extern void service_init(ACL_AIO *aio, ACL_FIFO *modules);

/**
 * �����˳�ʱ�Ļص�����
 */
extern void service_exit(void);

/**
 * Э�鴦�������
 * @param fd {ACL_SOCKET} �ͻ�����������
 * @param aio {ACL_AIO*} �첽��ܾ��
 */
extern int service_main(ACL_SOCKET fd, ACL_AIO *aio);

#ifdef	__cplusplus
}
#endif

#endif
