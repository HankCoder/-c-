#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

class mythread : public acl::thread_job
{
public:
	mythread(bool auto_destroy = false)
	{
		auto_destroy_ = auto_destroy;
	}
	~mythread() {}

protected:
	virtual void* run()
	{
		const char* myname = "run";
		printf("%s: thread id: %lu\r\n",
			myname, acl::thread::thread_self());
		sleep(1);
		if (auto_destroy_)
			delete this;
		return NULL;
	}

private:
	bool auto_destroy_;
};

//////////////////////////////////////////////////////////////////////////

class mythread_pool : public acl::thread_pool
{
public:
	mythread_pool() {}
	~mythread_pool()
	{
		printf("thread pool destroy now, tid: %lu\r\n",
			(unsigned long) acl_pthread_self());
	}

protected:
	virtual bool thread_on_init()
	{
		const char* myname = "thread_on_init";
		printf("%s: curr tid: %lu\r\n", myname,
			acl::thread::thread_self());
		return true;
	}

	virtual void thread_on_exit()
	{
		const char* myname = "thread_on_exit";
		printf("%s: curr tid: %lu\r\n", myname,
			acl::thread::thread_self());
	}
private:

};

//////////////////////////////////////////////////////////////////////////

static void test_thread_pool(int n)
{
	if (n == 0)
	{
		mythread_pool* threads = new mythread_pool;
		threads->start();

		mythread thread1, thread2;

		threads->execute(&thread1);
		threads->execute(&thread2);

		// ��Ϊ thread1, thread2 �Ƕ�ջ���������Ա������
		// threads->stop �ȴ����߳����н���
		threads->stop();
		delete threads;
	}
	else if (n == 1)
	{
		mythread_pool threads;
		threads.start();

		mythread *thread1 = new mythread, *thread2 = new mythread;
		threads.execute(thread1);
		threads.execute(thread2);

		// Ϊ�˱�֤ threads1, thread2 ��̬�ڴ汻��ȷ�ͷţ�
		// ������� threads.stop �ȴ����߳����н�������
		// ���߳��н����ͷ�
		threads.stop();

		// �����߳����ͷŶ�̬����Ķ���
		delete thread1;
		delete thread2;
	}
	else if (n == 2)
	{
		mythread_pool threads;
		threads.start();

		mythread thread1, thread2;

		threads.execute(&thread1);
		threads.execute(&thread2);

		// ��Ϊ thread1, thread2 �Ƕ�ջ���������Ա������
		// threads->stop �ȴ����߳����н���
		threads.stop();
	}
	else if (n == 3)
	{
		mythread_pool threads;
		threads.start();

		mythread *thread1 = new mythread(true);
		mythread *thread2 = new mythread(true);

		threads.execute(thread1);
		threads.execute(thread2);

		threads.stop();
	}
	else if (n == 4)
	{
		static mythread_pool threads;
		threads.set_idle(1);
		threads.start();

		mythread *thread1 = new mythread(true);
		mythread *thread2 = new mythread(true);

		threads.execute(thread1);
		threads.execute(thread2);

		if (threads.threads_count() > 0)
		{
			printf("current task count: %d\r\n", threads.task_qlen());
			threads.stop();
		}
	}
	else if (n == 5)
	{
		// ����ֱ��ʹ�û���
		static acl::thread_pool threads;
		threads.set_idle(1);
		threads.start();

		mythread *thread1 = new mythread(true);
		mythread *thread2 = new mythread(true);

		threads.execute(thread1);
		threads.execute(thread2);

		if (threads.threads_count() > 0)
		{
			printf("current task count: %d\r\n", threads.task_qlen());
			threads.stop();
		}
	}
	else
	{
		mythread_pool threads;
		threads.start();

		static mythread thread1, thread2;

		threads.execute(&thread1);
		threads.execute(&thread2);

		threads.stop();
	}
}

//////////////////////////////////////////////////////////////////////////

static void usage(const char* proc)
{
	printf("usage: %s -h [help]\r\n"
		"	-c which_case [0, 1, 2, 3, 4, 5]\r\n",
		proc);
}

int main(int argc, char* argv[])
{
	int   ch, n = 0;

	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	while ((ch = getopt(argc, argv, "hc:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 'c':
			n = atoi(optarg);
			break;
		default:
			break;
		}
	}

	test_thread_pool(n);

#ifdef WIN32
	printf("enter any key to exit ...\r\n");
	getchar();
#endif
	return 0;
}
