#ifndef	ACL_SYS_PATCH_INCLUDE_H
#define	ACL_SYS_PATCH_INCLUDE_H

# ifdef	__cplusplus
extern "C" {
# endif

#include "acl_define.h"
#include "acl_vstream.h"

#ifdef	WIN32
struct iovec {
	void *iov_base;   /**< Starting address */
	size_t iov_len;   /**< Number of bytes */
};

#ifdef	HAVE_NO_TIMEVAL
struct timeval {
	long tv_sec;      /**< seconds */
	long tv_usec;     /**< microseconds */
};
#endif

struct timezone {
	int tz_minuteswest; /**< minutes W of Greenwich */
	int tz_dsttime;     /**< type of dst correction */
};

/**
 * ˯�߼���
 * @param sec {int} ˯�ߵ�����
 */
ACL_API void sleep(int sec);

/**
 * ��õ�ǰʱ��
 * @param tv {struct timeval*} �洢��ǰʱ����
 * @param tz {struct timezone*} ʱ��
 */
ACL_API int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif  /* WIN32 */
#ifdef	ACL_UNIX
# include <sys/uio.h>
#endif

/**
 * �׽��ֳ�ʼ��������WIN32ƽ̨����Ҫ����WSAStartup����ʼ��SOCKET��
 * ������UNIXƽ̨����Ҫͨ�� signal(SIGPIPE, SIG_IGN) �������ź�
 * @return {int} 0: OK; -1: error
 */
ACL_API int acl_socket_init(void);

/**
 * �����˳�ǰ���ô˺����ͷ�ȫ���׽�����Դ����WIN32����Ч��
 * @return {int} 0: OK; -1: error
 */
ACL_API int acl_socket_end(void);

/**
 * �ر��׽���
 * @param fd {ACL_SOCKET} �׽���
 * @return {int} 0: OK; -1: error
 */
ACL_API int acl_socket_close(ACL_SOCKET fd);

/**
 * ���׽��ֶ�����
 * @param fd {ACL_SOCKET} �����׽���
 * @param buf {void*} �ڴ滺������ַ
 * @param size {size_t} buf ��������С
 * @param timeout {size_t} ����ʱʱ��(��)
 * @param fp {ACL_VSTREAM*} ������, ����Ϊ��
 * @param arg {void*} �û����ѵĲ������ڻص���ʽʱ����
 * @return {int} 0: OK; -1: error
 */
ACL_API int acl_socket_read(ACL_SOCKET fd, void *buf, size_t size,
	int timeout, ACL_VSTREAM *fp, void *arg);

/**
 * ���׽���д����
 * @param fd {ACL_SOCKET} �����׽���
 * @param buf {void*} ���ݵ�ַ
 * @param size {size_t} buf ���ݴ�С
 * @param timeout {int} д��ʱʱ��(��)
 * @param fp {ACL_VSTREAM*} ������, ����Ϊ��
 * @param arg {void*} �û����ѵĲ������ڻص���ʽʱ����
 * @return {int} 0: OK; -1: error
 */
ACL_API int acl_socket_write(ACL_SOCKET fd, const void *buf,
	size_t size, int timeout, ACL_VSTREAM *fp, void *arg);

/**
 * ���׽���д����
 * @param fd {ACL_SOCKET} �����׽���
 * @param vec {const struct iovec*} ���������ַ
 * @param count {int} vec ���鳤��
 * @param timeout {int} д��ʱʱ��(��)
 * @param fp {ACL_VSTREAM*} ������, ����Ϊ��
 * @param arg {void*} �û����ѵĲ������ڻص���ʽʱ����
 * @return {int} 0: OK; -1: error
 */
ACL_API int acl_socket_writev(ACL_SOCKET fd, const struct iovec *vec,
	int count, int timeout, ACL_VSTREAM *fp, void *arg);

/**
 * ���ļ����
 * @param filepath {cosnt char*} �ļ�·��
 * @param flags {int} �򿪱�־λ, O_RDONLY | O_WRONLY | O_RDWR, 
 *  O_CREAT | O_EXCL | O_TRUNC, O_APPEND(for UNIX)
 * @param mode {int} ��Ȩ��λ, ����UNIX��Ч, �磺0700, 0755
 * @return {ACL_FILE_HANDLE} �򿪵��ļ���������� ACL_FILE_INVALID ��ʾ��ʧ��
 */
ACL_API ACL_FILE_HANDLE acl_file_open(const char *filepath, int flags, int mode);

/**
 * �رմ򿪵��ļ����
 * @param fh {ACL_FILE_HANDLE} �ļ����
 * @return {int} 0: ok; -1: error
 */
ACL_API int acl_file_close(ACL_FILE_HANDLE fh);

/**
 * ��λ�ļ�λ��
 * @param fh {ACL_FILE_HANDLE} �ļ����
 * @param offset {acl_off_t} ƫ��λ��
 * @param whence {int} λ�ñ�־λ��SEEK_CUR, SEEK_SET, SEEK_END
 * @return {acl_off_t} ��ǰ���ļ�ƫ��λ��
 */
ACL_API acl_off_t acl_lseek(ACL_FILE_HANDLE fh, acl_off_t offset, int whence);

/**
 * ���ļ��ж�����
 * @param fh {ACL_FILE_HANDLE} �ļ����
 * @param buf {void*} �洢������
 * @param size {size_t} buf ��������С
 * @param timeout {int} ����ʱʱ��(��)
 * @param fp {ACL_VSTREAM*} ��Ӧ���ļ������, ����Ϊ��
 * @param arg {void*} �û����ݵĲ���, �Իص���ʽʹ��ʱ�˲�����Ч
 * @return {int} ������ʵ������, ������� ACL_VSTREAM_EOF ��ʾ�����������
 */
ACL_API int acl_file_read(ACL_FILE_HANDLE fh, void *buf, size_t size,
	int timeout, ACL_VSTREAM *fp, void *arg);

/**
 * ���ļ���д����
 * @param fh {ACL_FILE_HANDLE} �ļ����
 * @param buf {void*} ���ݴ洢������
 * @param size {size_t} buf �����������ݳ��ȴ�С
 * @param timeout {int} д��ʱʱ��(��)
 * @param fp {ACL_VSTREAM*} ��Ӧ���ļ������, ����Ϊ��
 * @param arg {void*} �û����ݵĲ���, �Իص���ʽʹ��ʱ�˲�����Ч
 * @return {int} �ɹ�д��������, ������� ACL_VSTREAM_EOF ��ʾд����
 */
ACL_API int acl_file_write(ACL_FILE_HANDLE fh, const void *buf, size_t size,
	int timeout, ACL_VSTREAM *fp, void *arg);

/**
 * ���ļ���дһ������
 * @param fh {ACL_FILE_HANDLE} �ļ����
 * @param vec {const struct iovec*} ���ݴ洢����
 * @param count {int} vec ������Ԫ�ظ���
 * @param timeout {int} д��ʱʱ��(��)
 * @param fp {ACL_VSTREAM*} ��Ӧ���ļ������, ����Ϊ��
 * @param arg {void*} �û����ݵĲ���, �Իص���ʽʹ��ʱ�˲�����Ч
 * @return {int} �ɹ�д��������, ������� ACL_VSTREAM_EOF ��ʾд����
 */
ACL_API int acl_file_writev(ACL_FILE_HANDLE fh, const struct iovec *vec,
	int count, int timeout, ACL_VSTREAM *fp, void *arg);

/**
 * ���ļ��������е�����ȫ��д��Ӳ��
 * @param fh {ACL_FILE_HANDLE} �ļ����
 * @param fp {ACL_VSTREAM*} ��Ӧ���ļ������, ����Ϊ��
 * @param arg {void*} �û����ݵĲ���, �Իص���ʽʹ��ʱ�˲�����Ч
 * @return {int} 0: ok; -1: error
 */
ACL_API int acl_file_fflush(ACL_FILE_HANDLE fh, ACL_VSTREAM *fp, void *arg);

/**
 * �����ļ���ȡ�ø��ļ��Ĵ�С
 * @param filename {const char*} �ļ���
 * @return {acl_int64} >= 0: ok;  -1: error
 */
ACL_API acl_int64 acl_file_size(const char *filename);

/**
 * �����ļ����ȡ�ø��ļ��Ĵ�С
 * @param fh {ACL_FILE_HANDLE} �ļ����
 * @param fp {ACL_VSTREAM*} ��Ӧ���ļ������, ����Ϊ��
 * @param arg {void*} �û����ݵĲ���, �Իص���ʽʹ��ʱ�˲�����Ч
 * @return {acl_int64} >= 0: ok;  -1: error
 */
ACL_API acl_int64 acl_file_fsize(ACL_FILE_HANDLE fh, ACL_VSTREAM *fp, void *arg);

# ifdef	__cplusplus
}
# endif

#endif

