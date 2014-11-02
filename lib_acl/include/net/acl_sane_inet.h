#ifndef	ACL_SANE_INET_INCLUDE_H
#define	ACL_SANE_INET_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#ifdef	ACL_UNIX
#include <netinet/in.h>
#endif

/**
 * ��IP��ַת�����ַ�����ʽ
 * @param src {const unsigned char*} struct in_addr in.s_addr �������ڴ��ʾ
 * @param dst {char *} �洢ת�����
 * @param size {size_t} dst �Ŀռ��С
 * @return {const char*} NULL: error; !NULL: ok
 */
ACL_API const char *acl_inet_ntop4(const unsigned char *src, char *dst, size_t size);

/**
 * ��IP��ַת�����ַ�����ʽ
 * @param in {struct in_addr}
 * @param dst {char *} �洢ת�����
 * @param size {size_t} dst �Ŀռ��С
 * @return {const char*} NULL: error; !NULL: ok
 */
ACL_API const char *acl_inet_ntoa(struct in_addr in, char *dst, size_t size);

/**
 * �жϸ������ַ����Ƿ�����ȷ�� ip ��ַ
 * @param ip {const char *ip}
 * @return {int} 0: ��; -1: ��
 */
ACL_API int acl_is_ip(const char *ip);

/**
 * �ж������� ip ��ַ�Ƿ���� xxx.xxx.xxx.xxx ��ʽ
 * @param addr {const char*} IP ��ַ
 * @return {int} 1: ����, 0: ������
 */
ACL_API int acl_ipv4_valid(const char *addr);

/**
 * �ж������� ip ��ַ�Ƿ���� xxx.xxx.xxx.xxx:port ��ʽ
 * @param addr {const char*} IP:PORT ��ַ
 * @return {int} 1: ����, 0: ������
 */
ACL_API int acl_ipv4_addr_valid(const char *addr);

#ifdef	__cplusplus
}
#endif

#endif

