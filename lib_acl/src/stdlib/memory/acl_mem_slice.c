#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#include <string.h>
#include <stddef.h>     /* for offsetof */
#include "init/acl_init.h"
#include "stdlib/acl_mem_hook.h"
#include "stdlib/acl_malloc.h"
#include "stdlib/acl_slice.h"
#include "stdlib/acl_mem_slice.h"
#include "stdlib/acl_meter_time.h"

#endif

#include "../../private/private_array.h"
#include "malloc_vars.h"

/* xxx: �����Ҫʹ�� pthread_spinlock_t �򲻿ɽ� stdlib.h ����ǰ��,
 * ������뱨��
 */

#ifdef ACL_UNIX
# ifndef  _GNU_SOURCE
#  define _GNU_SOURCE
# endif
# ifndef __USE_XOPEN2K
#  define __USE_XOPEN2K
# endif
# include <pthread.h>
#endif

#ifdef  ACL_HAS_SPINLOCK
typedef pthread_spinlock_t mylock_t;

#define MUTEX_INIT(x)		pthread_spin_init(&(x)->lock, PTHREAD_PROCESS_PRIVATE)
#define MUTEX_DESTROY(x)	pthread_spin_destroy(&(x)->lock)
#define MUTEX_LOCK(x)		pthread_spin_lock(&(x)->lock)
#define MUTEX_UNLOCK(x)		pthread_spin_unlock(&(x)->lock)

#include "../../private/thread.h"

#else

#include "../../private/thread.h"

typedef acl_pthread_mutex_t mylock_t;

#define MUTEX_INIT(x)		thread_mutex_init(&(x)->lock, NULL)
#define MUTEX_DESTROY(x)	thread_mutex_destroy(&(x)->lock)
#define MUTEX_LOCK(x)		thread_mutex_lock(&(x)->lock)
#define MUTEX_UNLOCK(x)		thread_mutex_unlock(&(x)->lock)

#endif

#include "thread/acl_pthread.h"

struct ACL_MEM_SLICE {
	ACL_SLICE_POOL *slice_pool;	/* �ڴ���Ƭ�� */
	mylock_t  lock;			/* ������ */
	ACL_ARRAY *list;		/* �ӹ������̵߳��ͷ��ڴ�Ķ��� */
	acl_pthread_key_t  tls_key;	/* �ֲ߳̾��洢��Ӧ�ļ� */
	unsigned long tid;		/* ӵ�д��̳߳ض�����߳�ID�� */
	unsigned int  nalloc;		/* �����ڴ����Ĵ��� */
	unsigned int  nalloc_gc;	/* ������ٴ��ڴ���Զ������ڴ��������� */
	unsigned int  slice_flag;	/* �ڴ���Ƭ����ʱ�ı�־λ, �� ACL_SLICE_FLAG_GC2 | ACL_SLICE_FLAG_RTGC_OFF */
	ACL_ARRAY *slice_list;		/* �����̵߳��ڴ�صļ��� */
	acl_pthread_mutex_t *slice_list_lock; /* ����ȫ�� __mem_slice_list �Ļ����� */
	int  delay_free;		/* �����߳��˳�ʱ����Ϊ�����ڴ�Ƭ�������߳�ռ���ţ�
					 * ���Բ��������ͷţ���Ҫ�����̻߳������߳�Э���ͷ�
					 */
};

#include "stdlib/acl_msg.h"
#include <stdio.h>

/*----------------------------------------------------------------------------*/

typedef struct {
	size_t length;			/* ������ϣ��������ڴ��С */
	int    signature;		/* ǩ�� */
	ACL_MEM_SLICE *mem_slice;	/* �������ڴ���Ƭ���� */
	union {
		ALIGN_TYPE align;
		char  payload[1];
	} u;
} MBLOCK;

#define SIGNATURE       0xdead
#define FILLER          0x0

#define CHECK_PTR(_ptr, _real_ptr, _len, _fname, _line) { \
	if (_ptr == 0) \
		acl_msg_panic("%s(%d): null pointer input", _fname, _line); \
	_real_ptr = (MBLOCK *) ((char *) _ptr - offsetof(MBLOCK, u.payload[0])); \
	if (_real_ptr->signature != SIGNATURE) \
		acl_msg_panic("%s(%d)(CHECK_PTR): corrupt or unallocated memory block(%d, 0x%x, 0x%x)", \
			_fname, _line, (int) _real_ptr->length, _real_ptr->signature, SIGNATURE); \
	if ((_len = _real_ptr->length) < 1) \
		acl_msg_panic("%s(%d): corrupt memory block length", _fname, _line); \
}

#define CHECK_IN_PTR(_ptr, _real_ptr, _len, _fname, _line) { \
	if (_ptr == 0) \
		acl_msg_panic("%s(%d): null pointer input", _fname, _line); \
	_real_ptr = (MBLOCK *) ((char *) _ptr - offsetof(MBLOCK, u.payload[0])); \
	if (_real_ptr->signature != SIGNATURE) \
		acl_msg_panic("%s(%d)(CHECK_IN_PTR): corrupt or unallocated memory block(%d, 0x%x, 0x%x)", \
			_fname, _line, (int) _real_ptr->length, _real_ptr->signature, SIGNATURE); \
	_real_ptr->signature = 0; \
	if ((_len = _real_ptr->length) < 1) \
		acl_msg_panic("%s(%d): corrupt memory block length", _fname, _line); \
}

#define CHECK_IN_PTR2(_ptr, _real_ptr, _len, _fname, _line) { \
	if (_ptr == 0) \
		acl_msg_panic("%s(%d): null pointer input", _fname, _line); \
	_real_ptr = (MBLOCK *) ((char *) _ptr - offsetof(MBLOCK, u.payload[0])); \
	if (_real_ptr->signature != SIGNATURE) \
		acl_msg_panic("%s(%d)(CHECK_IN_PTR2): corrupt or unallocated memory block(%d, 0x%x, 0x%x)", \
			_fname, _line, (int) _real_ptr->length, _real_ptr->signature, SIGNATURE); \
	if ((_len = _real_ptr->length) < 1) \
		acl_msg_panic("%s(%d): corrupt memory block length", _fname, _line); \
}

#define CHECK_OUT_PTR(_ptr, _real_ptr, _mem_slice, _len) { \
	_real_ptr->signature = SIGNATURE; \
	_real_ptr->mem_slice = _mem_slice; \
	_real_ptr->length = _len; \
	_ptr = _real_ptr->u.payload; \
}

#define SPACE_FOR(_len)  (offsetof(MBLOCK, u.payload[0]) + _len)

static acl_pthread_key_t __mem_slice_key = (acl_pthread_key_t) -1;
static int __mem_base = 8;
static int __mem_nslice = 1024;
static int __mem_nalloc_gc = 100;
static int __mem_list_init_size = 1000;
static unsigned int __mem_slice_flag = ACL_SLICE_FLAG_GC2 | ACL_SLICE_FLAG_RTGC_OFF;

static ACL_ARRAY *__mem_slice_list = NULL;
static acl_pthread_mutex_t *__mem_slice_list_lock = NULL;

static int mem_slice_gc(ACL_MEM_SLICE *mem_slice);

/* �߳��˳�ǰ��Ҫ���ô˺����ͷ��Լ����ֲ߳̾��ڴ�洢�� */

static void mem_slice_free(ACL_MEM_SLICE *mem_slice)
{
	const char *myname = "mem_slice_free";
	int   n;

	if (mem_slice == NULL) {
		acl_msg_info("%s(%d): mem_slice null", myname, __LINE__);
		return;
	}

	/* �Ȼ��ձ����̵߳������ڴ�Ƭ */
	mem_slice_gc(mem_slice);

	if ((n = acl_slice_pool_used(mem_slice->slice_pool)) > 0) {
		acl_msg_info("%s(%d): thread(%ld) mem slice busy slices: %d, delay free it",
			myname, __LINE__, mem_slice->tid, n);

		if (__mem_slice_list_lock)
			thread_mutex_lock(__mem_slice_list_lock);
		mem_slice->delay_free = 1;
		if (__mem_slice_list_lock)
			thread_mutex_unlock(__mem_slice_list_lock);

		/* ��������һЩ�Ѿ���ȫ�ͷŵ��ڴ� */
		acl_slice_pool_gc(mem_slice->slice_pool);
	} else {
		acl_msg_info("%s(%d): thread(%ld) free mem slice now",
			myname, __LINE__, mem_slice->tid);
		acl_slice_pool_destroy(mem_slice->slice_pool);
		private_array_destroy(mem_slice->list, NULL);
		mem_slice->list = NULL;

		/* �����̵߳��ֲ߳̾��洢�ڴ�ش�ȫ���ڴ�ؾ��������ɾ�� */
		if (__mem_slice_list_lock)
			thread_mutex_lock(__mem_slice_list_lock);
		private_array_delete_obj(__mem_slice_list, mem_slice, NULL);
		if (__mem_slice_list_lock)
			thread_mutex_unlock(__mem_slice_list_lock);

		acl_default_free(__FILE__, __LINE__, mem_slice);
	}
}

static ACL_MEM_SLICE *__main_mem_slice = NULL;

static void main_thread_slice_free(void)
{
	if (__main_mem_slice) {
		mem_slice_free(__main_mem_slice);
		__main_mem_slice = NULL;
	}
}

static void free_global_ctx(void)
{
	if (__mem_slice_list) {
		private_array_destroy(__mem_slice_list, NULL);
		__mem_slice_list = NULL;
	}
	if (__mem_slice_list_lock) {
		thread_mutex_destroy(__mem_slice_list_lock);
		__mem_slice_list_lock = NULL;
	}
}

static void slice_key_init(void)
{
	unsigned long curr_tid = (unsigned long) acl_pthread_self();
	unsigned long main_tid = (unsigned long) acl_main_thread_self();

	if (curr_tid == main_tid) {
		acl_pthread_key_create(&__mem_slice_key, NULL);
		atexit(main_thread_slice_free);
	} else
		acl_pthread_key_create(&__mem_slice_key, (void (*)(void*)) mem_slice_free);
}

static acl_pthread_once_t once_control = ACL_PTHREAD_ONCE_INIT;

static ACL_MEM_SLICE *mem_slice_create(void)
{
	const char *myname = "mem_slice_create";
	ACL_MEM_SLICE *mem_slice;

	acl_pthread_once(&once_control, slice_key_init);

	if (__mem_slice_key == (acl_pthread_key_t) -1)
		acl_msg_fatal("%s(%d): __mem_slice_key(%d) invalid,"
			" call acl_mem_slice_init or acl_mem_slice_set first",
			myname, __LINE__, (int) __mem_slice_key);

	mem_slice = acl_pthread_getspecific(__mem_slice_key);
	if (mem_slice != NULL)
		return (mem_slice);

	mem_slice = (ACL_MEM_SLICE*)
		acl_default_calloc(__FILE__, __LINE__, 1, sizeof(ACL_MEM_SLICE));
	if (mem_slice == NULL)
		acl_msg_fatal("%s(%d): can't alloc for mem_slice(%s)",
			myname, __LINE__, acl_last_serror());

	mem_slice->slice_pool = acl_slice_pool_create(__mem_base,
			__mem_nslice, __mem_slice_flag);
	mem_slice->tid = (unsigned long) acl_pthread_self();
	mem_slice->list = private_array_create(__mem_list_init_size);
	MUTEX_INIT(mem_slice);
	mem_slice->tls_key = __mem_slice_key;
	mem_slice->nalloc_gc = __mem_nalloc_gc;
	mem_slice->slice_flag = __mem_slice_flag;

	acl_pthread_setspecific(__mem_slice_key, mem_slice);

	if ((unsigned long) acl_pthread_self() == acl_main_thread_self())
		__main_mem_slice = mem_slice;

	acl_msg_info("%s(%d): thread(%ld) set myown mem_slice(%lx)",
		myname, __LINE__, (long) mem_slice->tid, (long) mem_slice);

	return (mem_slice);
}

static void tls_mem_free(const char *filename, int line, void *ptr)
{
	MBLOCK *real_ptr;
	size_t len;

	CHECK_IN_PTR2(ptr, real_ptr, len, filename, line);

#if 1
	if (real_ptr->mem_slice->tid != (unsigned long) acl_pthread_self()) {
#else
	if (real_ptr->mem_slice->tid != mem_slice->tid) {
#endif
		MUTEX_LOCK(real_ptr->mem_slice);
		PRIVATE_ARRAY_PUSH(real_ptr->mem_slice->list, real_ptr);
		MUTEX_UNLOCK(real_ptr->mem_slice);
	} else
		acl_slice_pool_free(filename, line, real_ptr);
}

static void *tls_mem_alloc(const char *filename, int line, size_t len)
{
	const char *myname = "tls_mem_alloc";
	ACL_MEM_SLICE *mem_slice = acl_pthread_getspecific(__mem_slice_key);
	char *ptr;
	MBLOCK *real_ptr;

	if (mem_slice == NULL) {
		/* ÿ�����̻߳���Լ����ֲ߳̾��洢�ڴ�� */
		mem_slice = mem_slice_create();
		mem_slice->slice_list = __mem_slice_list;

		/* �����̵߳��ֲ߳̾��洢�ڴ������ȫ���ڴ�ؾ�������� */
		if (__mem_slice_list_lock)
			thread_mutex_lock(__mem_slice_list_lock);
		private_array_push(__mem_slice_list, mem_slice);
		if (__mem_slice_list_lock)
			thread_mutex_unlock(__mem_slice_list_lock);
	}

	real_ptr = (MBLOCK *) acl_slice_pool_alloc(filename, line,
			mem_slice->slice_pool, SPACE_FOR(len));
	if (real_ptr == 0) {
		acl_msg_error("%s(%d): malloc: insufficient memory",
			myname, __LINE__);
		return (0);
	}

	mem_slice->nalloc++;
	if (mem_slice->nalloc == mem_slice->nalloc_gc) {
		mem_slice->nalloc = 0;
		mem_slice_gc(mem_slice);
	}
	CHECK_OUT_PTR(ptr, real_ptr, mem_slice, len);
	return (ptr);
}

static void *tls_mem_calloc(const char *filename, int line, size_t nmemb, size_t size)
{
	void *ptr = tls_mem_alloc(filename, line, nmemb * size);

	memset(ptr, 0, nmemb * size);
	return (ptr);
}

static void *tls_mem_realloc(const char *filename, int line, void *ptr, size_t size)
{
	void *buf = tls_mem_alloc(filename, line, size);
	MBLOCK *old_real_ptr;
	size_t old_len;

	CHECK_IN_PTR2(ptr, old_real_ptr, old_len, filename, line);
	memcpy(buf, ptr, old_len > size ? size : old_len);
	if (old_real_ptr->mem_slice->tid != (unsigned long) acl_pthread_self()) {
		MUTEX_LOCK(old_real_ptr->mem_slice);
		PRIVATE_ARRAY_PUSH(old_real_ptr->mem_slice->list, old_real_ptr);
		MUTEX_UNLOCK(old_real_ptr->mem_slice);
	} else
		acl_slice_pool_free(filename, line, old_real_ptr);

	return (buf);
}

static void *tls_mem_memdup(const char *filename, int line, const void *ptr, size_t len)
{
	void *buf = tls_mem_alloc(filename, line, len);

	memcpy(buf, ptr, len);
	return (buf);
}

static char *tls_mem_strdup(const char *filename, int line, const char *str)
{
	size_t size = strlen(str) + 1;
	void *buf = tls_mem_alloc(filename, line, size);

	memcpy(buf, str, size);
	return ((char*) buf);
}

static char *tls_mem_strndup(const char *filename, int line, const char *str, size_t len)
{
	size_t size = strlen(str);
	char *buf;

	size = size > len ? len : size;
	buf = (char*) tls_mem_alloc(filename, line, size + 1);
	memcpy(buf, str, size);
	return (buf);
}

static int mem_slice_gc(ACL_MEM_SLICE *mem_slice)
{
	int   n = 0;

	/* �ͷ��������߳̽������ڴ�Ƭ */

	MUTEX_LOCK(mem_slice);
	while (1) {
		void *ptr;
		PRIVATE_ARRAY_POP(mem_slice->list, ptr);
		if (ptr == NULL)
			break;
		acl_slice_pool_free(__FILE__, __LINE__, ptr);
		n++;
	}
	MUTEX_UNLOCK(mem_slice);
	/* ʵʱ������������? */
	if ((mem_slice->slice_flag & ACL_SLICE_FLAG_RTGC_OFF) == 0)
		acl_slice_pool_gc(mem_slice->slice_pool);
	return (n);
}

int acl_mem_slice_gc(void)
{
	ACL_MEM_SLICE *mem_slice = acl_pthread_getspecific(__mem_slice_key);

	if (!mem_slice)
		return (-1);
	return (mem_slice_gc(mem_slice));
}

void acl_mem_slice_destroy(void)
{
	ACL_MEM_SLICE *mem_slice = acl_pthread_getspecific(__mem_slice_key);

	if (mem_slice == NULL)
		return;
	/* �ͷŸ��߳���ӵ�е��ڴ���Ƭ�ض��� */
	mem_slice_free(mem_slice);
	acl_pthread_setspecific(__mem_slice_key, NULL);
	if ((unsigned long) acl_pthread_self() == acl_main_thread_self())
		__main_mem_slice = NULL;
}

void acl_mem_slice_delay_destroy(void)
{
	const char *myname = "acl_mem_slice_delay_destroy";
	int   i, n;

	if (__mem_slice_list_lock == NULL)
		return;

	thread_mutex_lock(__mem_slice_list_lock);
	n = private_array_size(__mem_slice_list);
	for (i = 0; i < n; i++) {
		ACL_MEM_SLICE *mem_slice = (ACL_MEM_SLICE*)
			private_array_index(__mem_slice_list, i);
		if (mem_slice == NULL)
			break;
		if (mem_slice->delay_free == 0)
			continue;
		if (acl_slice_pool_used(mem_slice->slice_pool) <= 0) {
			acl_msg_info("%s(%d): thread(%ld) free mem slice now",
				myname, __LINE__, mem_slice->tid);

			acl_slice_pool_destroy(mem_slice->slice_pool);
			private_array_destroy(mem_slice->list, NULL);
			mem_slice->list = NULL;

			/* �����̵߳��ֲ߳̾��洢�ڴ�ش�ȫ���ڴ�ؾ��������ɾ�� */
			private_array_delete_obj(__mem_slice_list, mem_slice, NULL);
			acl_default_free(__FILE__, __LINE__, mem_slice);
		} else
			mem_slice_gc(mem_slice);
	}

	thread_mutex_unlock(__mem_slice_list_lock);
}

ACL_MEM_SLICE *acl_mem_slice_init(int base, int nslice,
	int nalloc_gc, unsigned int slice_flag)
{
	const char *myname = "acl_mem_slice_init";
	ACL_MEM_SLICE *mem_slice;

	if (__mem_slice_key != (acl_pthread_key_t) -1) {
		acl_msg_error("%s(%d): has been init", myname, __LINE__);
		return (NULL);
	}

	__mem_base = base;
	__mem_nslice = nslice;
	__mem_nalloc_gc = nalloc_gc < 10 ? 10 : nalloc_gc;
	__mem_slice_flag = slice_flag;
	__mem_list_init_size = nalloc_gc / 10;
	if (__mem_list_init_size < 1000)
		__mem_list_init_size = 1000;
	else if (__mem_list_init_size > 1000000)
		__mem_list_init_size = 1000000;

	/* ���̻߳���Լ����ֲ߳̾��洢�ڴ�� */
	mem_slice = mem_slice_create();
	if (mem_slice == NULL)
		acl_msg_fatal("%s(%d): mem_slice null", myname, __LINE__);

	/* �������̿ռ���ȫ�ֵ��ڴ�ؼ��϶���, ��洢�����̵߳Ĵ洢�ڴ�ؾ�� */
	__mem_slice_list = private_array_create(10);
	__mem_slice_list_lock = thread_mutex_create();
	private_array_push(__mem_slice_list, mem_slice);
	mem_slice->slice_list = __mem_slice_list;
	mem_slice->slice_list_lock = __mem_slice_list_lock;

	if (__mem_slice_list == NULL)
		acl_msg_fatal("%s(%d): __mem_slice_list null", myname, __LINE__);
	if (__mem_slice_list_lock == NULL)
		acl_msg_fatal("%s(%d): __mem_slice_list_lock null", myname, __LINE__);

	atexit(free_global_ctx);

	mem_slice->tls_key  = __mem_slice_key;

	acl_mem_hook(tls_mem_alloc,
		tls_mem_calloc,
		tls_mem_realloc,
		tls_mem_strdup,
		tls_mem_strndup,
		tls_mem_memdup,
		tls_mem_free);
	acl_msg_info("%s(%d): use ACL_MEM_SLICE, with tls", myname, __LINE__);
	return (mem_slice);
}

void acl_mem_slice_set(ACL_MEM_SLICE *mem_slice)
{
	const char *myname = "acl_mem_slice_set";

	if (__mem_slice_key != (acl_pthread_key_t) -1) {
		return;
	}

	__mem_slice_key = mem_slice->tls_key;
	__mem_base = mem_slice->slice_pool->base;
	__mem_nslice = mem_slice->slice_pool->nslice;
	__mem_nalloc_gc = mem_slice->nalloc_gc;
	__mem_slice_flag = mem_slice->slice_flag;
	__mem_slice_list = mem_slice->slice_list;
	__mem_slice_list_lock = mem_slice->slice_list_lock;

	__mem_list_init_size = __mem_nalloc_gc / 10;
	if (__mem_list_init_size < 1000)
		__mem_list_init_size = 1000;
	else if (__mem_list_init_size > 1000000)
		__mem_list_init_size = 1000000;

	acl_mem_hook(tls_mem_alloc,
		tls_mem_calloc,
		tls_mem_realloc,
		tls_mem_strdup,
		tls_mem_strndup,
		tls_mem_memdup,
		tls_mem_free);
	acl_msg_info("%s(%d): set ACL_MEM_SLICE, with tls", myname, __LINE__);
}
