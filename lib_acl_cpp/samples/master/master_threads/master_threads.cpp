// master_threads.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/master/master_threads.hpp"
#include "acl_cpp/event/event_timer.hpp"
#include "acl_cpp/stream/socket_stream.hpp"

static char *var_cfg_debug_msg;

static acl::master_str_tbl var_conf_str_tab[] = {
	{ "debug_msg", "test_msg", &var_cfg_debug_msg },

	{ 0, 0, 0 }
};

static int  var_cfg_debug_enable;
static int  var_cfg_keep_alive;
static int  var_cfg_loop;

static acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "debug_enable", 1, &var_cfg_debug_enable },
	{ "keep_alive", 1, &var_cfg_keep_alive },
	{ "loop_read", 1, &var_cfg_loop },

	{ 0, 0, 0 }
};

static int  var_cfg_io_timeout;

static acl::master_int_tbl var_conf_int_tab[] = {
	{ "io_timeout", 120, &var_cfg_io_timeout, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

static void (*format)(const char*, ...) = acl::log::msg1;

//////////////////////////////////////////////////////////////////////////

class master_timer_test : public acl::event_timer
{
public:
	master_timer_test(acl::socket_stream* stream)
	: max_(0)
	, count_(0)
	, stream_(stream)
	{
		(void) stream_;
	}

	void set_max(int max)
	{
		max_ = max;
	}

protected:
	// �����麯��
	virtual void timer_callback(unsigned int id)
	{
		format("timer callback, id: %u\r\n", id);
		if (count_++ >= max_)
		{
			printf("clear all timer task now\r\n");
			clear();
		}
		//else
		//	set_task(1000, 1000000);
	}

	virtual void destroy()
	{
		format("destroy called\r\n");
		delete this;
	}

private:
	int  max_;
	int  count_;
	acl::socket_stream* stream_;

	~master_timer_test()
	{
		format("timer destroy now!\r\n");
	}
};

//////////////////////////////////////////////////////////////////////////

class master_threads_test : public acl::master_threads
{
public:
	master_threads_test()
	{

	}

	~master_threads_test()
	{

	}

protected:
	// ���ി�麯�������ͻ������������ݿɶ���ر�ʱ�ص��˺��������� true ��ʾ
	// ������ͻ��˱��ֳ����ӣ������ʾ��Ҫ�رտͻ�������
	virtual bool thread_on_read(acl::socket_stream* stream)
	{
		while (true)
		{
			if (on_read(stream) == false)
				return false;
			if (var_cfg_loop == 0)
				break;
		}
		return true;
	}

	bool on_read(acl::socket_stream* stream)
	{
		format("%s(%d)", __FILE__, __LINE__);
		acl::string buf;
		if (stream->gets(buf) == false)
		{
			format("gets error: %s", acl::last_serror());
			format("%s(%d)", __FILE__, __LINE__);
			return false;
		}
		if (buf == "quit")
		{
			stream->puts("bye!");
			return false;
		}

		if (buf == "timer")
		{
			int  max = 0;
			master_timer_test* timer = new master_timer_test(stream);
			timer->keep_timer(true);

			timer->set_task(1000, 1000000);
			max += 2;
			timer->set_task(1001, 1000000);
			max += 2;
			timer->set_task(1002, 1000000);
			max += 2;
			timer->set_task(1003, 1000000);
			max += 2;
			timer->set_max(max);

			// ���û��෽�����ö�ʱ������
			proc_set_timer(timer);
			stream->format("set timer ok\r\n");
			return true;
		}

		if (buf.empty())
		{
			if (stream->write("\r\n") == -1)
			{
				format("write 1 error: %s", acl::last_serror());
				return false;
			}
		}
		else if (stream->write(buf) == -1)
		{
			format("write 2 error: %s, buf(%s), len: %d",
				acl::last_serror(), buf.c_str(), (int) buf.length());
			return false;
		}
		else if (stream->write("\r\n") == -1)
		{
			format("write 3 client error: %s", acl::last_serror());
			return false;
		}
		return true;
	}

	// �����麯���������յ�һ���ͻ�������ʱ�����ô˺���������
	// �������ȶԿͻ������ӽ��д������� true ��ʾ����������
	// Ҫ��رոÿͻ�������
	virtual bool thread_on_accept(acl::socket_stream* stream)
	{
		format("accept one client, peer: %s, local: %s, var_cfg_io_timeout: %d\r\n",
			stream->get_peer(), stream->get_local(), var_cfg_io_timeout);
		if (stream->format("hello, you're welcome!\r\n") == -1)
			return false;
		return true;
	}

	// �����麯�������ͻ������ӹر�ʱ���ô˺���
	virtual void thread_on_close(acl::socket_stream*)
	{
		format("client closed now\r\n");
	}

	// �����麯�������̳߳ش���һ�����߳�ʱ���ô˺���
	virtual void thread_on_init()
	{
#ifdef WIN32
		format("thread init: tid: %lu\r\n", GetCurrentThreadId());
#else
		format("thread init: tid: %lu\r\n", pthread_self());
#endif
	}

	// �����麯�������̳߳��е�һ���߳��˳�ʱ���ô˺���
	virtual void thread_on_exit()
	{
#ifdef WIN32
		format("thread exit: tid: %lu\r\n", GetCurrentThreadId());
#else
		format("thread exit: tid: %lu\r\n", pthread_self());
#endif
	}

	// �����麯������������л��û����ǰ���ô˺���
	virtual void proc_pre_jail()
	{
		format("proc_pre_jail\r\n");
	}

	// �����麯������������л��û���ݺ���ô˺���
	virtual void proc_on_init()
	{
		format("proc init\r\n");
	}

	// �����麯������������˳�ǰ���ô˺���
	virtual void proc_on_exit()
	{
		format("proc exit\r\n");
	}
private:
};

int main(int argc, char* argv[])
{
#if 0
	int base = 8, nslice = 1024, nalloc_gc = 1000000;
	unsigned int slice_flag = ACL_SLICE_FLAG_GC2 | ACL_SLICE_FLAG_RTGC_OFF;

	acl_mem_slice_init(base, nslice, nalloc_gc, slice_flag);
#endif

	master_threads_test mt;

	// �������ò�����
	mt.set_cfg_int(var_conf_int_tab);
	mt.set_cfg_int64(NULL);
	mt.set_cfg_str(var_conf_str_tab);
	mt.set_cfg_bool(var_conf_bool_tab);

	// ��ʼ����

	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
	{
		int   task_count = 2, threads_count = 2;
		format = (void (*)(const char*, ...)) printf;
		format("listen: 127.0.0.1:8888\r\n");

		// �������з�ʽ
		if (argc >= 3)
			mt.run_alone("127.0.0.1:8888", argv[2],
				task_count, threads_count);
		else
			mt.run_alone("127.0.0.1:8888", NULL,
				task_count, threads_count);
	}

	// acl_master ����ģʽ����
	else
	{
#ifdef	WIN32
		int   task_count = 2, threads_count = 2;
		format = (void (*)(const char*, ...)) printf;
		format("listen: 127.0.0.1:8888\r\n");

		// �������з�ʽ
		mt.run_alone("127.0.0.1:8888", NULL,
			task_count, threads_count);
#else
		mt.run_daemon(argc, argv);
#endif
	}

	return 0;
}
