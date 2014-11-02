// fs_benchmark.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>

static int __nfiles = 10, __length = 102400, __parallel = 100;
static char __path[256];
static char __data[1024];
static bool __kernel_event = false;

#ifdef	SUNOS5
static int __nok = 0;  // xxx
#else
static __thread int __nok = 0;
#endif

//////////////////////////////////////////////////////////////////////////

class async_file : public acl::aio_callback
{
public:
	async_file(acl::aio_handle* handle, const char* path, int ibegin)
		: handle_(handle)
		, path_(path)
		, nfiles_(__nfiles)
		, ifile_(0)
		, ibegin_(ibegin)
		, nwrite_(0)
		, delay_(1)
		, fp_(NULL)
	{

	}
	~async_file()
	{

	}

	// ֻд��ʽ�����ļ�
	bool open_write()
	{
		fp_ = new acl::aio_fstream(handle_);
		filepath_.format("%s/%lu_%d_%d", path_.c_str(),
			(unsigned long) acl_pthread_self(), ibegin_, ifile_++);
		if (fp_->open_write(filepath_.c_str()) == false)
		{
			printf("open file %s error: %s\r\n",
				filepath_.c_str(), acl::last_serror());
			return false;
		}

		// ���д�ɹ��Ļص�����
		fp_->add_write_callback(this);

		// ��ӹرյĻص�����
		fp_->add_close_callback(this);

		// 1 �������д����
		fp_->write(__data, sizeof(__data), delay_ * 1000000);
		return true;
	}
protected:
	bool read_callback(char*, int)
	{
		return true;
	}

	bool write_callback()
	{
		nwrite_ += sizeof(__data);

		if (nwrite_ >= __length)
		{
			printf("write one file(%s) ok, nwrite: %d\r\n",
				filepath_.c_str(), nwrite_);

			return false;  // ���� false ��ʹ��ܹرո�������
		}
		fp_->write(__data, sizeof(__data));
		return true;
	}

	void close_callback()
	{
		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
		if (ifile_ >= nfiles_)
		{
			printf("write %s over!, ifile: %d, nfiles: %d\r\n",
				filepath_.c_str(), ifile_, nfiles_);
			delete this;
			__nok++;
		}
		else
		{
			//printf("ifile_: %d, nfiles_: %d\r\n", ifile_, nfiles_);
			nwrite_ = 0;
			delay_ = 0;

			// ���´�һ���µ��첽�����
			if (open_write() == false)
			{
				printf("open file error\r\n");
				exit (1);
			}
		}
	}

	bool timeout_callback()
	{
		return (true);
	}
private:
	acl::aio_handle* handle_;
	acl::string path_;
	acl::string filepath_;
	int  nfiles_;
	int  ifile_;
	int  ibegin_;
	int  nwrite_;
	int  delay_;
	acl::aio_fstream* fp_;
};

static void thread_main(void*)
{
	printf("%s: thread: %ld\r\n", __FUNCTION__,
		(unsigned long) acl_pthread_self());
	acl::aio_handle* handle;

	// ÿ���̴߳����������첽�¼����
	handle = new acl::aio_handle(__kernel_event ?
		acl::ENGINE_KERNEL : acl::ENGINE_SELECT);

	int i;
	for (i = 0; i < __parallel; i++)
	{
		async_file* fp = new async_file(handle, __path, i);
		if (fp->open_write() == false)
		{
			printf("open file error: %s\r\n", acl::last_serror());
			delete fp;
			break;
		}
	}

	if (i == 0)
	{
		printf("thread(%lu) no file opened!\r\n", (unsigned long)
			acl_pthread_self());
		delete handle;
		return;
	}

	// �����첽�¼�ѭ������
	while (true)
	{
		if (handle->check() == false)
			break;
		if (__nok == __parallel)
		{
			printf("%s: thread(%lu) over, total: %d\r\n", __FUNCTION__,
				(unsigned long) acl_pthread_self(), __nok);
			break;
		}
	}

	// ��Ϊ IO ������ӳ��ͷŵģ�������Ҫ�ٴμ��һ��
	handle->check();

	// �����첽�¼����
	delete handle;
}

static void usage(const char* proc)
{
	printf("usage: %s -h[help]\r\n"
		" -n thread_count\r\n"
		" -P parallel [#writer object]\r\n"
		" -c file_count per writer object\r\n"
		" -l file_length\r\n"
		" -k [use kernel event, default: false]\r\n"
		" -p path [default: var/]\r\n", proc);
}

int main(int argc, char* argv[])
{
#ifdef WIN32
	acl::acl_cpp_init();
#endif
	logger_open("fs_benchmark.log", "fs_benchmark");

	int   ch;
	int   nthreads = 2;

#ifdef WIN32
	snprintf(__path, sizeof(__path), "var/%d", (int) _getpid());
#else
	snprintf(__path, sizeof(__path), "var/%d", (int) getpid());
#endif
	while ((ch = getopt(argc, argv, "hn:c:l:kp:P:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 'n':
			nthreads = atoi(optarg);
			break;
		case 'c':
			__nfiles = atoi(optarg);
			break;
		case 'l':
			__length = atoi(optarg);
			break;
		case 'k':
			__kernel_event = true;
			break;
		case 'p':
#ifdef WIN32
			snprintf(__path, sizeof(__path), "%s/%d", optarg, (int) _getpid());
#else
			snprintf(__path, sizeof(__path), "%s/%d", optarg, (int) getpid());
#endif
			break;
		case 'P':
			__parallel = atoi(optarg);
			if (__parallel <= 0)
				__parallel = 1;
			break;
		default:
			break;
		}
	}

	for (size_t i = 0; i < sizeof(__data); i++)
	{
		__data[i] = 'X';
	}

	acl_make_dirs(__path, 0700);

	// �����̳߳ؾ��
	acl_pthread_pool_t* tp = acl_thread_pool_create(nthreads, 0);

	// ����̴߳�������
	for (int i = 0; i < nthreads; i++)
		acl_pthread_pool_add(tp, thread_main, NULL);

	// �����̳߳�
	acl_pthread_pool_destroy(tp);

	logger_close();

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif
	return 0;
}
