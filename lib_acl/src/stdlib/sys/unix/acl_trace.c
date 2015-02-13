#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE
#include "stdlib/acl_define.h"
#include "stdlib/acl_msg.h"
#include "stdlib/acl_mymalloc.h"
#include "thread/acl_pthread.h"
#include "init/acl_init.h"
#include "stdlib/unix/acl_trace.h"
#endif

#ifdef	ACL_LINUX
#include <unistd.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void acl_trace_save(const char *filepath)
{
	const char *myname = "acl_trace_save";
	int   fd;
	void *buffer[1000];
	size_t n;

	n = backtrace(buffer, 1000);
	if (n == 0)
		return;

	fd = open(filepath, O_WRONLY | O_CREAT | O_APPEND, 0600);
	if (fd == -1) {
		acl_msg_error("%s(%d): open %s error(%s)",
			myname, __LINE__, filepath, acl_last_serror());
		return;
	}

	backtrace_symbols_fd(buffer, n, fd);
	close(fd);
}

static acl_pthread_key_t __trace_key;
static acl_pthread_once_t __trace_once = ACL_PTHREAD_ONCE_INIT;
static unsigned int *__main_buf = NULL;

static void trace_buf_free(void *buf)
{
	if ((unsigned long) acl_pthread_self() != acl_main_thread_self())
		acl_myfree(buf);
}

static void main_buf_free(void)
{
	if (__main_buf)
		acl_myfree(__main_buf);
}

static void trace_buf_init(void)
{
	acl_assert(acl_pthread_key_create(&__trace_key, trace_buf_free) == 0);
}

void acl_trace_info(void)
{
	void *buffer[1000];
	size_t n, i;
	char **results;
	unsigned int *intbuf;

	/* ��ʼ���ֲ߳̾����� */
	if (acl_pthread_once(&__trace_once, trace_buf_init) != 0)
		return;
	intbuf = acl_pthread_getspecific(__trace_key);
	if (intbuf == NULL) {
		intbuf = acl_mymalloc(sizeof(int));
		*intbuf = 0;
		acl_assert(acl_pthread_setspecific(__trace_key, intbuf) == 0);
		if ((unsigned long) acl_pthread_self()
			== acl_main_thread_self())
		{
			__main_buf = intbuf;
			atexit(main_buf_free);
		}
	}

	/* ��������ݹ�Ƕ�ף���ֱ�ӷ��� */
	if ((*intbuf) > 0)
		return;

	n = backtrace(buffer, 1000);
	if (n == 0)
		return;

	/* ��ֹ�ݹ�Ƕ�ױ�־���� */
	(*intbuf)++;

	results = backtrace_symbols(buffer, n);

	/* ��¼�����еĶ�ջ��Ϣ */
	for (i = 0; i < n; i++)
		acl_msg_info("backtrace: %s", results[i]);

	if (results != NULL)
		free(results);

	/* ��ֹ�ݹ�Ƕ�ױ�־�Լ� */
	(*intbuf)--;
}

#else

void acl_trace_save(const char *filepath acl_unused)
{
}

void acl_trace_info(void)
{
}

#endif
