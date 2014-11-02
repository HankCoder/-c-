#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <stdlib.h>
#ifndef WIN32
#include <pthread.h>
#endif

#ifdef WIN32
struct acl_pthread_mutex_t;
#else
# ifndef	acl_pthread_mutex_t
#  define	acl_pthread_mutex_t	pthread_mutex_t
# endif
#endif

namespace acl {

/**
 * ������������ͬʱ�����ļ������߳�����Ҳ����ֻ����һ����
 */
class ACL_CPP_API locker
{
public:
	/**
	 * ���캯��
	 * @param use_mutex {bool} �Ƿ񴴽��߳���
	 * @param nowait {bool} �Ƿ�һֱ�ȴ��������ɹ�
	 *  (�����߳������ļ���)
	 */
	locker(bool use_mutex = true, bool nowait = false);
	virtual ~locker();

	/**
	 * �����ļ�·�������ļ���
	 * @param file_path {const char*} �ļ�·�����ǿ�
	 * @return {bool} �Ƿ�ɹ�
	 * ע���˺���������� open ��������ͬʱ����һ��
	 */
	bool open(const char* file_path);

	/**
	 * �����ļ���������ļ���
	 * @param fh {int} �ļ����
	 * @return {bool} �Ƿ�ɹ�
	 */
#ifdef WIN32
	bool open(void* fh);
#else
	bool open(int fh);
#endif

	/**
	 * ����Ѿ��򿪵���(�����߳������ļ���)���м���
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool lock();

	/**
	 * ����Ѿ��򿪵���(�����߳������ļ���)���н���
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool unlock();
protected:
private:
	void init_mutex(void);

#ifndef WIN32
	pthread_mutexattr_t  mutexAttr_;
#endif
	acl_pthread_mutex_t* pMutex_;

	char* pFile_;
#ifdef WIN32
	void* fHandle_;
#else
	int   fHandle_;
#endif
	bool  myFHandle_;
	bool  nowait_;
};

}  // namespace acl
