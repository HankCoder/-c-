#ifndef	ACL_IFCONF_INCLUDE_H
#define	ACL_IFCONF_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"

typedef struct ACL_IFADDR {
	char *name;		/* �ӿ����� */
#ifdef WIN32
	char *desc;		/* �ӿ����� */
#endif
	char  ip[32];		/* ���ַ�����ʾ��IP��ַ */
	unsigned int addr;	/* �����ֽ���� 32 λ IP ��ַ */
} ACL_IFADDR;

typedef struct ACL_IFCONF {
	ACL_IFADDR *addrs;	/* ACL_IFADDR ���� */
	int  length;		/* ACL_IFADDR ���鳤�� */

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	const ACL_IFADDR *(*iter_head)(ACL_ITER*, struct ACL_IFCONF*);
	/* ȡ��������һ������ */
	const ACL_IFADDR *(*iter_next)(ACL_ITER*, struct ACL_IFCONF*);
	/* ȡ������β���� */
	const ACL_IFADDR *(*iter_tail)(ACL_ITER*, struct ACL_IFCONF*);
	/* ȡ��������һ������ */
	const ACL_IFADDR *(*iter_prev)(ACL_ITER*, struct ACL_IFCONF*);
} ACL_IFCONF;

/**
 * ������������������ַ������ӿ�����
 * @return {ACL_IFCONF*}
 */
ACL_API ACL_IFCONF *acl_get_ifaddrs(void);

/**
 * �ͷ��� acl_get_ifaddrs() ���ص� ACL_IFCONF �ڴ�
 * @param ifconf {ACL_IFCONF*}
 */
ACL_API void acl_free_ifaddrs(ACL_IFCONF *ifconf);

#ifdef	__cplusplus
}
#endif

#endif
