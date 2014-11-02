#ifndef	ACL_SANE_SOCKET_INCLUDE_H
#define	ACL_SANE_SOCKET_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"

/**
 * ȡ���׽������ӶԷ��������ַ, ��ַ��ʽΪ: IP:PORT
 * @param sockfd {ACL_SOCKET} �����׽���
 * @param buf {char*} �洢��ַ�Ļ�����������Ϊ��
 * @param bsize {size_t} buf �ռ��С
 * @return {int} 0: ok; -1: error
 */
ACL_API int acl_getpeername(ACL_SOCKET sockfd, char *buf, size_t bsize);

/**
 * ȡ���׽������ӱ��ص������ַ, ��ַ��ʽΪ: IP:PORT
 * @param sockfd {ACL_SOCKET} �����׽���
 * @param buf {char*} �洢��ַ�Ļ�����������Ϊ��
 * @param bsize {size_t} buf �ռ��С
 * @return {int} 0: ok; -1: error
 */
ACL_API int acl_getsockname(ACL_SOCKET sockfd, char *buf, size_t bsize);

/**
 * ȡ���׽��ֵ�����
 * @param sockfd {ACL_SOCKET} �����׽���
 * @return {int} -1: ��ʾ���������Ƿ�����׽���; >= 0 ��ʾ�ɹ�����׽���
 *  ���ͣ�����ֵ�� AF_INET �� AF_UNIX(���� UNIX ƽ̨)
 */
ACL_API int acl_getsocktype(ACL_SOCKET sockfd);

#ifdef	__cplusplus
}
#endif

#endif

