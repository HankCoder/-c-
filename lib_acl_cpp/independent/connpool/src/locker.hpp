#pragma once
#include <stdlib.h>
#ifndef WIN32
#include <pthread.h>
#endif

#ifdef WIN32
# error "win32 not support!"
#endif

namespace acl_min
{

/**
 * ������������ͬʱ�����ļ������߳�����Ҳ����ֻ����һ����
 */
class locker
{
public:
	locker(bool nowait = false);
	virtual ~locker();

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

private:
	pthread_mutexattr_t  mutexAttr_;
	pthread_mutex_t* pMutex_;
	bool  nowait_;

	void init_mutex(void);
};

}  // namespace acl_min
