#ifndef	ACL_IOSTUFF_INCLUDE_H
#define	ACL_IOSTUFF_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_vstream.h"

#define ACL_CLOSE_ON_EXEC   1  /**< ��־λ, ���� exec ���Զ��رմ򿪵������� */
#define ACL_PASS_ON_EXEC    0

#define ACL_BLOCKING        0  /**< ������д��־λ */
#define ACL_NON_BLOCKING    1  /**< ��������д��־λ */

/**
 * �����׽ӿ�Ϊ�����������
 * @param fd {ACL_SOCKET} SOCKET �׽���
 * @param on {int} �Ƿ����ø��׽���Ϊ������, ACL_BLOCKING �� ACL_NON_BLOCKING
 * @return {int} 0: �ɹ�; -1: ʧ��
 */
ACL_API int acl_non_blocking(ACL_SOCKET fd, int on);

/**
 * д�ȴ�������ֱ���׽��ֿ�д�������ʱ
 * @param fd {ACL_SOCKET} ������
 * @param timeout {int} ��ʱʱ�䣬��λΪ�룬��ֵ�������������Σ�
 *  > 0  : ��ʾ���ʱʱ���������
 *  == 0 : ��ʾ���ȴ�����������������
 *  < 0  : ʱ��ʾֱ�Ӹ��׽��ֿɶ������Ϊֹ
 * @return {int} 0: ��д; -1: ʧ�ܻ�ʱ
 */
ACL_API int acl_write_wait(ACL_SOCKET fd, int timeout);

/**
 * ���ȴ�������ֱ���׽��������ݿɶ��������ʱ
 * @param fd {ACL_SOCKET} ������
 * @param timeout {int} ��ʱʱ�䣬��λΪ�룬��ֵ�������������Σ�
 *  > 0  : ��ʾ���ʱʱ���������
 *  == 0 : ��ʾ���ȴ�����������������
 *  < 0  : ʱ��ʾֱ�Ӹ��׽��ֿɶ������Ϊֹ
 * @return {int} 0: �����ݿɶ��������ֳ���; -1: ʧ�ܻ�ʱ
 */
ACL_API int acl_read_wait(ACL_SOCKET fd, int timeout);

/**
 * ���뼶��˯��
 * @param delay {unsigned} ����ֵ
 */
ACL_API void acl_doze(unsigned delay);

/**
* ĳ���������Ƿ�ɶ�
* @param fd {ACL_SOCKET} ������
* @return {int} 0: ���ɶ�; != 0: �ɶ�
*/
ACL_API int acl_readable(ACL_SOCKET fd);

/**
 * ��ʱ������
 * @param fd {ACL_SOCKET} �����׽���
 * @param buf {void*} �洢��������Ϊ��
 * @param len {unsigned} buf �洢����С
 * @param timeout {int} ��ʱʱ�䣬��λΪ�룬��ֵ�������������Σ�
 *  > 0  : ��ʾ���ʱʱ���������
 *  == 0 : ��ʾ���ȴ�����������������
 *  < 0  : ʱ��ʾֱ�Ӹ��׽��ֿɶ������Ϊֹ
 * @return {int} > 0 ��������; -1: ����
*/
ACL_API int acl_timed_read(ACL_SOCKET fd, void *buf, unsigned len,
	int timeout, void *unused_context);

/**
 * ��ʱд����
 * @param fd {ACL_SOCKET} �����׽���
 * @param buf {void*} ���ݴ洢��������Ϊ��
 * @param len {unsigned} ���ݳ��ȴ�С
 * @param timeout {int} ��ʱʱ�䣬��λΪ�룬��ֵ�������������Σ�
 *  > 0  : ��ʾ���ʱʱ���������
 *  == 0 : ��ʾ���ȴ�����������������
 *  < 0  : ʱ��ʾֱ�Ӹ��׽��ֿɶ������Ϊֹ
 * @return {int} > 0 �ɹ�д�������; -1: ����
 */
ACL_API int acl_timed_write(ACL_SOCKET fd, void *buf, unsigned len,
	int timeout, void *unused_context);

/**
 * ����������ѭ��д�����ݣ�ֱ��д�ꡢ�����ʱΪֹ
 * @param fd {ACL_SOCKET} �����׽���
 * @param buf {void*} ���ݴ洢��������Ϊ��
 * @param len {unsigned} ���ݳ��ȴ�С
 * @param timeout {int} ��ʱʱ�䣬��λΪ��
 * @param {int} �ɹ�д��ĳ���
 */
ACL_API int acl_write_buf(ACL_SOCKET fd, const char *buf, int len, int timeout);

/**
 * ̽���׽�����ϵͳ�����������ݳ���
 * @param fd {ACL_SOCKET} ������
 * @return {int} ϵͳ���������ݳ���
 */
ACL_API int acl_peekfd(ACL_SOCKET fd);

/**
 * �����ܵ�
 * @param fds {ACL_FILE_HANDLE [2]} �洢���
 * @return {int} 0: ok; -1: error
 */
ACL_API int acl_pipe(ACL_FILE_HANDLE fds[2]);

/**
 * �رչܵ���
 * @param fds {ACL_FILE_HANDLE[2]} �ܵ���
 * @return {int} 0: ok; -1: error
 */
ACL_API int acl_pipe_close(ACL_FILE_HANDLE fds[2]);

/**
* ����һ���ܵ���
* @param fds {ACL_FILE_HANDLE[2]} �洢�����Ĺܵ��Ե�ַ������Ϊ��
* @return 0: ok; -1: error
*/
ACL_API int acl_duplex_pipe(ACL_FILE_HANDLE fds[2]);

#ifdef	ACL_UNIX
/**
 * �����ļ���������־λ�������� exec ����������Զ����ر�
 * @param fd {int} �ļ�������
 * @param on {int} ACL_CLOSE_ON_EXEC �� 0
 * @return {int} 0: ok; -1: error
 */
ACL_API int acl_close_on_exec(int fd, int on);

/**
 * ��ĳ���ļ���������ʼ�ر�֮������д򿪵��ļ�������
 * @param lowfd {int} ���ر������������ֵ
 * @return {int} 0: ok; -1: error
 */
ACL_API int acl_closefrom(int lowfd);

/**
 * �趨��ǰ���̿��Դ�����ļ�������ֵ
 * @param limit {int} �趨�����ֵ
 * @return {int} >=0: ok; -1: error
 */
ACL_API int acl_open_limit(int limit);

/**
 * �жϸ���ĳ���ļ��������Ƿ����׽���
 * @param fd {int} �ļ�������
 * @return {int} != 0: ��; 0: ��
 */
ACL_API int acl_issock(int fd);
#endif

#ifdef	__cplusplus
}
#endif

#endif
