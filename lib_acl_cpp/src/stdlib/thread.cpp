#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/thread.hpp"
#ifdef	ACL_FREEBSD
#include <pthread_np.h>
#endif

namespace acl
{

thread::thread()
: detachable_(true)
, stack_size_(0)
, thread_id_(0)
{
#ifdef WIN32
	thread_ = (acl_pthread_t*) acl_mycalloc(1, sizeof(acl_pthread_t));
#endif
	return_arg_ = NULL;
}

thread::~thread()
{
#ifdef WIN32
	acl_myfree(thread_);
#endif
}

thread& thread::set_detachable(bool yes /* = true */)
{
	detachable_ = yes;
	return *this;
}

thread& thread::set_stacksize(size_t size)
{
	stack_size_ = size;
	return *this;
}

void* thread::thread_run(void* arg)
{
	thread* thr = (thread*) arg;
#ifdef	WIN32
	thr->thread_id_ = GetCurrentThreadId();
#elif	defined(ACL_FREEBSD)
	thr->thread_id_ = pthread_getthreadid_np();
#else
	thr->thread_id_ = (unsigned long) pthread_self();
#endif

	// ����̴߳���ʱΪ����ģʽ���� run ����ʱ�û��п���
	// ���̶߳��������ˣ����Բ����ٽ� thr->return_arg_ ����
	// ��ֵ��������п��ܳ����ڴ�Ƿ�����
	if (thr->detachable_)
		return thr->run();

	thr->return_arg_ = thr->run();
	return thr->return_arg_;
}

bool thread::start()
{
	acl_pthread_attr_t attr;
	acl_pthread_attr_init(&attr);

	if (detachable_)
		acl_pthread_attr_setdetachstate(&attr, 1);
	if (stack_size_ > 0)
		acl_pthread_attr_setstacksize(&attr, stack_size_);

#ifdef	WIN32
	int   ret = acl_pthread_create((acl_pthread_t*) thread_,
			&attr, thread_run, this);
#else
	int   ret = acl_pthread_create((pthread_t*) &thread_, &attr,
			thread_run, this);
#endif
	if (ret != 0)
	{
		acl_set_error(ret);
		logger_error("create thread error %s", last_serror());
		return false;
	}

#ifdef	WIN32
	thread_id_ = ((acl_pthread_t*) thread_)->id;
#elif	defined(LINUX2)
	thread_id_ = thread_;
#endif

	return true;
}

bool thread::wait(void** out /* = NULL */)
{
	if (detachable_)
	{
		logger_error("detachable thread can't be wait!");
		return false;
	}

	for (int i = 0; i < 10; i++)
	{
		if (thread_id_ != 0)
			break;
		sleep(1);
	}

	if (thread_id_ == 0)
	{
		logger_error("thread not running!");
		return false;
	}

	void* ptr;

#ifdef WIN32
	int   ret = acl_pthread_join(*((acl_pthread_t*) thread_), &ptr);
#else
	int   ret = acl_pthread_join(thread_, &ptr);
#endif

	if (ret != 0)
	{
		acl_set_error(ret);
		logger_error("pthread_join error: %s", last_serror());
		return false;
	}

	// �Ƚ�ͨ���� thread_run �нػ�Ĳ����� pthread_join ��õĲ����Ƿ���ͬ
	if (ptr != return_arg_)
		logger_warn("pthread_josin's arg invalid?");

	if (out)
		*out = ptr;
	return true;
}

unsigned long thread::thread_id() const
{
	return thread_id_;
}

unsigned long thread::thread_self()
{
#ifdef	ACL_FREEBSD
	return (unsigned long) pthread_getthreadid_np();
#else
	return (unsigned long) acl_pthread_self();
#endif
}

} // namespace acl
