#ifndef ACL_TCP_CTL_INCLUDE_H
#define ACL_TCP_CTL_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"

#define ACL_SOCKET_RBUF_SIZE	204800  /**< ȱʡ����������С */
#define ACL_SOCKET_WBUF_SIZE	204800  /**< ȱ��д��������С */

/**
 * ���� TCP �׽��ֵĶ���������С
 * @param fd {ACL_SOCKET} �׽���
 * @param size {int} ���������ô�С
 */
ACL_API void acl_tcp_set_rcvbuf(ACL_SOCKET fd, int size);

/**
 * ���� TCP �׽��ֵ�д��������С
 * @param fd {ACL_SOCKET} �׽���
 * @param size {int} ���������ô�С
 */
ACL_API void acl_tcp_set_sndbuf(ACL_SOCKET fd, int size);

/**
 * ��ȡ TCP �׽��ֵĶ���������С
 * @param fd {ACL_SOCKET} �׽���
 * @return {int} ��������С
 */
ACL_API int  acl_tcp_get_rcvbuf(ACL_SOCKET fd);

/**
 * ��ȡ TCP �׽��ֵ�д��������С
 * @param fd {ACL_SOCKET} �׽���
 * @return {int} ��������С
 */
ACL_API int  acl_tcp_get_sndbuf(ACL_SOCKET fd);

/**
 * �� TCP �׽��ֵ� nodelay ����
 * @param fd {ACL_SOCKET} �׽���
 */
ACL_API void acl_tcp_set_nodelay(ACL_SOCKET fd);

/**
 * ���� TCP �׽��ֵ� nodelay ����
 * @param fd {ACL_SOCKET} �׽���
 * @param onoff {int} 1 ��ʾ�򿪣�0 ��ʾ�ر�
 */
ACL_API void acl_tcp_nodelay(ACL_SOCKET fd, int onoff);

/**
 * ��� TCP �׽����Ƿ������� nodelay ѡ��
 * @param fd {ACL_SOCKET} �׽���
 * @return {int} 1 ��ʾ�򿪣�0 ��ʾ�ر�
 */
ACL_API int acl_get_tcp_nodelay(ACL_SOCKET fd);

/**
 * ���� TCP �׽��ֵ� SO_LINGER ѡ��
 * @param fd {ACL_SOCKET} �׽���
 * @param onoff {int} �Ƿ����� SO_LINGER ѡ��
 * @param timeout {int} ��SO_LINGER��ʱ��ȡ�� timed_wait ��ʱ�䣬��λΪ��
 */
ACL_API void acl_tcp_so_linger(ACL_SOCKET fd, int onoff, int timeout);

/**
 * ��� TCP �׽��ֵ� linger ֵ
 * @param fd {ACL_SOCKET} �׽���
 * @return {int} ���� -1 ��ʾδ���� linger ѡ����ڲ�����>= 0 ��ʾ������ linger
 *  ѡ���Ҹ�ֵ��ʾ�׽��ֹرպ�� TCP �������ں���ά�� TIME_WAIT ״̬�Ķ���ʱ��(��)
 */
ACL_API int acl_get_tcp_solinger(ACL_SOCKET fd);

/**
 * ���ü����׽��ֵ��ӳٽ��չ��ܣ������ͻ���������������ʱ�Ž������ӷ���
 * ��Ӧ�ã�Ŀǰ�ù��ܽ�֧�� Linux
 * @param fd {ACL_SOCKET} �׽���
 * @param timeout {int} ����ͻ��������ڹ涨��ʱ����δ�������ݣ�Ҳ�������ӷ���
 *  ��Ӧ��
 */
ACL_API void acl_tcp_defer_accept(ACL_SOCKET fd, int timeout);

#ifdef __cplusplus
}
#endif

#endif

