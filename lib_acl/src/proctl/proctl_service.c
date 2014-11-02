#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#endif  /* ACL_PREPARE_COMPILE */

#ifdef WIN32

#include "stdlib/acl_stdlib.h"
#include "thread/acl_thread.h"
#include <windows.h>
#include "proctl_internal.h"

static ACL_ARRAY *__services = NULL;  /* ��ǰ�������еķ��������� */
static acl_pthread_mutex_t __mutex_running_service;
static HANDLE *__handles = NULL;
static HANDLE __sem_handle = INVALID_HANDLE_VALUE;
static int   __max_handle = 64;
static int   __cur_handle = 0;
static ACL_FIFO *__services_wait = NULL;  /* ��ǰ���ڴ������ķ��������� */
static acl_pthread_mutex_t __mutex_waiting_service;

#define LOCK_RUNNING_SERVICE do \
{ \
	acl_pthread_mutex_lock(&__mutex_running_service); \
} while(0);

#define UNLOCK_RUNNING_SERVICE do \
{ \
	acl_pthread_mutex_unlock(&__mutex_running_service); \
} while(0);

#define LOCK_WAITING_SERVICE do \
{ \
	acl_pthread_mutex_lock(&__mutex_waiting_service); \
} while(0);

#define UNLOCK_WAITING_SERVICE do \
{ \
	acl_pthread_mutex_unlock(&__mutex_waiting_service); \
} while(0);

/* ��ʼ�����̾������ */

static void handles_init(void)
{
	const char *myname = "handles_init";
	int   i;

	__handles = (HANDLE *) acl_mycalloc(__max_handle, sizeof(HANDLE));
	acl_assert(__handles);
	__cur_handle = 0;

	for (i = 0; i < __max_handle; i++) {
		__handles[i] = INVALID_HANDLE_VALUE;
	}
}

/* ����̾������������µĽ��̾�� */

static void handles_add(HANDLE handle)
{
	const char *myname = "handles_add";
	int   i;

	if (__cur_handle >= __max_handle)
		acl_msg_fatal("%s(%d): too many handle", myname, __LINE__);

	for (i = 0; i < __max_handle; i++) {
		if (__handles[i] == INVALID_HANDLE_VALUE) {
			__handles[i] = handle;
			__cur_handle++;
			return;
		}
	}

	acl_msg_fatal("%s(%d): no position for new handle", myname, __LINE__);
}

/* �ӽ��̾��������ɾ����� */

static void handles_del(HANDLE handle)
{
	const char *myname = "handles_del";
	int   i;

	for (i = 0; i < __cur_handle; i++) {
		if (__handles[i] == handle) {
			__cur_handle--;
			if (i < __cur_handle) {
				__handles[i] = __handles[__cur_handle];
			}
			__handles[__cur_handle] = INVALID_HANDLE_VALUE;
			return;
		}
	}

	acl_msg_fatal("%s(%d): not found the handle", myname, __LINE__);
}

/* ���̹��������ʼ�� */

void proctl_service_init()
{
	const char *myname = "proctl_service_init";
	char  ebuf[256];

	__services = acl_array_create(10);
	__services_wait = acl_fifo_new();
	acl_pthread_mutex_init(&__mutex_running_service, NULL);
	acl_pthread_mutex_init(&__mutex_waiting_service, NULL);
	handles_init();

	__sem_handle = CreateSemaphore(NULL, 0, 1024, NULL);
	if (__sem_handle == NULL)
		acl_msg_fatal("%s(%d): CreateSemaphore error(%s)",
			myname, __LINE__, acl_last_strerror(ebuf, sizeof(ebuf)));
	handles_add(__sem_handle);
}

/* ����һ���µķ������ */

PROCTL_SERVICE *proctl_service_alloc(const char *filepath, ACL_VSTRING *cmdline)
{
	PROCTL_SERVICE *service;

	service = (PROCTL_SERVICE*) acl_mycalloc(1, sizeof(PROCTL_SERVICE));
	acl_assert(service);
	service->filepath = acl_mystrdup(filepath);
	service->cmdline = cmdline;
	service->hProcess = INVALID_HANDLE_VALUE;

	return (service);
}

static void proctl_service_add(PROCTL_SERVICE *service)
{
	const char *myname = "proctl_service_add";

	/* ��������������������µķ������ */

	LOCK_RUNNING_SERVICE;
	if (acl_array_append(__services, service) < 0)
		acl_msg_fatal("%s(%d): add service to array error", myname, __LINE__);
	UNLOCK_RUNNING_SERVICE;
}

PROCTL_SERVICE *proctl_service_new(const char *filepath, int argc, char *argv[])
{
	const char *myname = "proctl_service_new";
	PROCTL_SERVICE *service;
	ACL_VSTRING *cmdline = acl_vstring_alloc(256);
	int   i;

	acl_assert(cmdline);

	/* �齨�������������в����� */

	/* Ϊ�˱����������ʱ���������м京�пո�����ָ���
	 * ���������������Ҫ�ڲ������߼�������
	 */

	acl_vstring_strcat(cmdline, "\"");
	acl_vstring_strcat(cmdline, filepath);
	acl_vstring_strcat(cmdline, "\" ");

	for (i = 0; i < argc; i++) {
		acl_vstring_strcat(cmdline, "\"");
		acl_vstring_strcat(cmdline, argv[i]);
		acl_vstring_strcat(cmdline, "\" ");
	}

	acl_msg_info("%s(%d): filepath=%s, cmdline=%s",
		myname, __LINE__, filepath, acl_vstring_str(cmdline));

	service = proctl_service_alloc(filepath, cmdline);
	proctl_service_add(service);
	return (service);
}

/* �ͷ�һ��������� */

void proctl_service_free(PROCTL_SERVICE *service)
{
	if (service->hProcess != INVALID_HANDLE_VALUE)
		CloseHandle(service->hProcess);
	acl_myfree(service->filepath);
	acl_vstring_free(service->cmdline);

	LOCK_RUNNING_SERVICE;
	acl_array_delete_obj(__services, service, NULL);
	UNLOCK_RUNNING_SERVICE;

	acl_myfree(service);
}

/* ���ݽ��̾����ѯ������� */

static PROCTL_SERVICE *proctl_service_find(HANDLE handle)
{
	const char *myname = "proctl_service_find";
	PROCTL_SERVICE *service;
	int   i, n;

	LOCK_RUNNING_SERVICE;

	n = acl_array_size(__services);
	for (i = 0; i < n; i++) {
		service = (PROCTL_SERVICE*) acl_array_index(__services, i);
		if (service->hProcess == handle) {
			UNLOCK_RUNNING_SERVICE;
			return (service);
		}
	}

	UNLOCK_RUNNING_SERVICE;
	return (NULL);
}

/* ������еķ������ĳ������ƣ���������洢��һ�������� */

ACL_ARGV *proctl_serivce_get_all()
{
	ACL_ARGV *argv = acl_argv_alloc(10);
	PROCTL_SERVICE *service;
	int   i, n;

	LOCK_RUNNING_SERVICE;
	n = acl_array_size(__services);
	for (i = 0; i < n; i++) {
		service = (PROCTL_SERVICE*) acl_array_index(__services, i);
		acl_argv_add(argv, service->filepath, NULL);
	}
	UNLOCK_RUNNING_SERVICE;

	if (argv->argc == 0) {
		acl_argv_free(argv);
		return (NULL);
	}
	return (argv);
}

/* �ͷ��� proctl_service_get_all �����Ķ������� */

void proctl_service_free_all(ACL_ARGV *argv)
{
	if (argv)
		acl_argv_free(argv);
}

/* ���ݳ���ȫ·�����鿴ĳ�������Ƿ��������� */

int proctl_service_exist(const char *filepath)
{
	PROCTL_SERVICE *service;
	int   i, n;

	LOCK_RUNNING_SERVICE;
	n = acl_array_size(__services);
	for (i = 0; i < n; i++) {
		service = (PROCTL_SERVICE*) acl_array_index(__services, i);
		if (strcasecmp(service->filepath, filepath) == 0)
			break;
	}
	UNLOCK_RUNNING_SERVICE;

	if (n <= 0 || i == n)
		return (0);
	return (1);
}

/* �ͷ�ĳ��������������󶨵Ľ��̾�� */

static void proctl_service_stopped(PROCTL_SERVICE *service)
{
	handles_del(service->hProcess);
	proctl_service_free(service);
}

/* ��ʼ����ĳ��������� */

int proctl_service_start(PROCTL_SERVICE *service)
{
	const char *myname = "proctl_service_start";
	HANDLE	hThread;
	char  ebuf[256];

	if(!CreateProcess(service->filepath, acl_vstring_str(service->cmdline),
		NULL, NULL, 0, DETACHED_PROCESS, NULL, NULL,
		&service->start_info, &service->process_info))
	{

		acl_msg_error("%s(%d): CreateProcess error(%s), file(%s)",
			myname, __LINE__, acl_last_strerror(ebuf, sizeof(ebuf)),
			service->filepath);
		return (-1);
	}

	service->hProcess = service->process_info.hProcess;
	hThread = service->process_info.hThread;
	CloseHandle(hThread);
	handles_add(service->hProcess);

	return (0);
}

/* ��������ĳ��������� */

static int proctl_service_restart(PROCTL_SERVICE *service)
{
	handles_del(service->hProcess);
	CloseHandle(service->hProcess);
	service->hProcess = INVALID_HANDLE_VALUE;

	return (proctl_service_start(service));
}

PROCTL_MSG *proctl_msg_new(int msg_type)
{
	PROCTL_MSG *msg = (PROCTL_MSG *) acl_mycalloc(1, sizeof(PROCTL_MSG));

	msg->msg_type = msg_type;
	return (msg);
}

void proctl_msg_free(PROCTL_MSG *msg)
{
	if (msg->free_fn && msg->arg)
		msg->free_fn(msg->arg);
	acl_myfree(msg);
}

/* ����Ϣ���������һ����Ϣ���������̷߳���֪ͨ */
void proctl_msg_push(PROCTL_MSG *msg)
{
	LOCK_WAITING_SERVICE;
	acl_fifo_push(__services_wait, msg);
	UNLOCK_WAITING_SERVICE;

	/* �����̷߳�����Ϣ */
	ReleaseSemaphore(__sem_handle, 1, NULL);
}

/* ������Ϣ������һ�������ӽ��� */
static void proctl_msg_start(PROCTL_MSG *msg)
{
	PROCTL_SERVICE *service = msg->service;

	proctl_service_add(service);
	(void) proctl_service_start(service);
}

/* ���������ڼ����̵߳���Ϣ���������� */
static void proctl_msg_main(void)
{
	const char *myname = "proctl_msg_main";
	PROCTL_MSG *msg;

	LOCK_WAITING_SERVICE;
	while (1) {
		msg = acl_fifo_pop(__services_wait);
		if (msg == NULL)
			break;
		switch (msg->msg_type) {
		case PROCTL_MSG_START:
			proctl_msg_start(msg);
			break;
		default:
			acl_msg_error("%s(%d): unknown msg type(%d)",
				myname, __LINE__, msg->msg_type);
			break;
		}

		proctl_msg_free(msg);
	}
	
	UNLOCK_WAITING_SERVICE;
}

/* �ȴ����еķ�����̵�״̬ */

int proctl_service_wait(void)
{
	const char *myname = "proctl_service_wait";
	DWORD timeout = 1000 * 2, ret;
	char  ebuf[256];
	HANDLE handle_sem;

	if (__cur_handle == 0)
		return (0);

	/* Create the semaphore, with max value 32K */
	handle_sem = CreateSemaphore(NULL, 0, 32 * 1024, NULL);
	while (1) {
		ret = WaitForMultipleObjects(__cur_handle, __handles, FALSE, timeout);
		if (ret == WAIT_OBJECT_0) {
			proctl_msg_main();
		} else if (ret == WAIT_FAILED) {
			acl_msg_error("%s(%d): wait child object error(%s)",
				myname, __LINE__, acl_last_strerror(ebuf, sizeof(ebuf)));
			return (-1);
		} else if (ret != WAIT_TIMEOUT)
			break;
	}

	acl_debug(ACL_DEBUG_PROCTL, 2) ("%s(%d): __cur_handle=%d",
		myname, __LINE__, __cur_handle);
	return (0);
}

/* ����һЩ�˳��Ľ��̣����������������쳣�˳��Ľ��� */

int proctl_service_join(void)
{
	const char *myname = "proctl_service_join";
	PROCTL_SERVICE *service;
	HANDLE hProcess;
	DWORD status;
	int   i;
	char  ebuf[256];

	for (i = 0; i < __cur_handle; i++) {
		hProcess = __handles[i];
		if (hProcess == INVALID_HANDLE_VALUE)
			acl_msg_fatal("%s(%d): invalid handle in array, i(%d)",
				myname, __LINE__, i);

		service = proctl_service_find(hProcess);
		if (service == NULL) {
			if (hProcess == __sem_handle)
				continue;
			acl_msg_fatal("%s(%d): not found hProcess", myname, __LINE__);
		}

		status = 0;
		if (!GetExitCodeProcess(hProcess, &status)) {
			acl_msg_error("%s(%d): get child exit error(%s)",
				myname, __LINE__, acl_last_strerror(ebuf, sizeof(ebuf)));
			if (proctl_service_restart(service) < 0)
				proctl_service_stopped(service);
		} else if (status == 0) {
			acl_msg_error("%s(%d): child exit status 0, error(%s)",
				myname, __LINE__, acl_last_strerror(ebuf, sizeof(ebuf)));

			/* ��Ϊ�����������˳������Բ���Ҫ������ */
			proctl_service_stopped(service);
		} else if (status != STILL_ACTIVE) {
			/* child has exited abnormaly */
			acl_msg_error("%s(%d): child exit status %d, child exit(%s)",
				myname, __LINE__, (int) status,
				acl_last_strerror(ebuf, sizeof(ebuf)));

			/* ��Ϊ�������쳣�˳���������Ҫ������ */
			if (proctl_service_restart(service) < 0)
				proctl_service_stopped(service);
		}
		/* else: STILL_ACTIVE */
	}

	return (0);
}

#endif /*WIN32 */
