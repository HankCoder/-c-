#include "lib_acl.h"
#include <assert.h>

/**
 * �û��Զ������ݽṹ
 */
typedef struct THREAD_CTX {
	acl_pthread_pool_t *thr_pool;
	int   i;
} THREAD_CTX;

/* ȫ���Ծ�̬���� */
static acl_pthread_pool_t *__thr_pool = NULL;

/* �ֲ߳̾��洢����(C99֧�ִ��ַ�ʽ�������������) */
static __thread unsigned int __local = 0;

static void free_buf_fn(void *arg)
{
	ACL_VSTRING *buf = (ACL_VSTRING*) arg;

	printf(">> current thread id=%u, buf = %s\r\n",
		(unsigned int) acl_pthread_self(), acl_vstring_str(buf));
	acl_vstring_free(buf);
}

static void worker_thread(void *arg)
{
	THREAD_CTX *ctx = (THREAD_CTX*) arg; /* ����û��Զ������ */
	unsigned int   i = 0;
	static __thread ACL_VSTRING *buf1 = NULL;
	static __thread ACL_VSTRING *buf2 = NULL;

	/* ������֤�������ݹ��� */
	assert(ctx->thr_pool == __thr_pool);

	if (buf1 == NULL)
		buf1 = acl_vstring_alloc(256);
	if (buf2 == NULL)
		buf2 = acl_vstring_alloc(256);

	acl_vstring_sprintf(buf1, "buf1: tid=%u",
		(unsigned int) acl_pthread_self());
	acl_vstring_sprintf(buf2, "buf2: tid=%u",
		(unsigned int) acl_pthread_self());
	/* ע�ắ���������߳��˳�ʱ�Զ��ͷ� buf �ڴ�ռ� */
	acl_pthread_atexit_add(buf1, free_buf_fn);
	acl_pthread_atexit_add(buf2, free_buf_fn);

	while (i < 5) {
		if (__local != i)
			acl_msg_fatal("__local=%d invalid", __local);
		printf("thread id=%u, i=%d, __local=%d\r\n",
			(unsigned int) acl_pthread_self(), ctx->i, __local);
		i++;
		/* �ڱ��߳��н��ֲ߳̾�������1 */
		__local++;
		sleep(1);
	}

	acl_myfree(ctx);

	/* ���ˣ��ù����߳̽������״̬��ֱ�����г�ʱ�˳� */
}

static int on_thread_init(void *arg)
{
	const char *myname = "on_thread_init";
	acl_pthread_pool_t *thr_pool = (acl_pthread_pool_t*) arg;

	/* �ж�һ�£�����Ϊ����֤�������ݹ��� */
	assert(thr_pool == __thr_pool);
	printf("%s: thread(%u) init now\r\n", myname, (unsigned int) acl_pthread_self());

	/* ����0��ʾ����ִ�и��̻߳�õ������񣬷���-1��ʾִֹͣ�и����� */
	return (0);
}

static void on_thread_exit(void *arg)
{
	const char *myname = "on_thread_exit";
	acl_pthread_pool_t *thr_pool = (acl_pthread_pool_t*) arg;

	/* �ж�һ�£�����Ϊ����֤�������ݹ��� */
	assert(thr_pool == __thr_pool);
	printf("%s: thread(%u) exit now\r\n", myname, (unsigned int) acl_pthread_self());
}

static void run_thread_pool(acl_pthread_pool_t *thr_pool)
{
	THREAD_CTX *ctx;  /* �û��Զ������ */

	/* ����ȫ�־�̬���� */
	__thr_pool = thr_pool;

	/* �����߳̿�ʼʱ�Ļص����� */
	(void) acl_pthread_pool_atinit(thr_pool, on_thread_init, thr_pool);

	/* �����߳��˳�ʱ�Ļص����� */
	(void) acl_pthread_pool_atfree(thr_pool, on_thread_exit, thr_pool);

	ctx = (THREAD_CTX*) acl_mycalloc(1, sizeof(THREAD_CTX));
	assert(ctx);
	ctx->thr_pool = thr_pool;
	ctx->i = 0;

	/**
	* ���̳߳�����ӵ�һ�����񣬼�������һ�������߳�
	* @param wq �̳߳ؾ��
	* @param worker_thread �����̵߳Ļص�����
	* @param event_type �˴�д0����
	* @param ctx �û��������
	*/
	acl_pthread_pool_add(thr_pool, worker_thread, ctx);
	sleep(1);

	ctx = (THREAD_CTX*) acl_mycalloc(1, sizeof(THREAD_CTX));
	assert(ctx);
	ctx->thr_pool = thr_pool;
	ctx->i = 1;
	/* ���̳߳�����ӵڶ������񣬼������ڶ��������߳� */
	acl_pthread_pool_add(thr_pool, worker_thread, ctx);
}

static void main_thread_atexit(void *arg)
{
	ACL_VSTRING *buf = (ACL_VSTRING*) arg;

	printf("main thread exit now, tid=%u, buf=%s\r\n",
		(unsigned int) acl_pthread_self(), acl_vstring_str(buf));
	printf("in the main thread_atexit, input any key to exit\r\n");
	getchar();
}

static acl_pthread_pool_t *thr_pool_create(int threads, int timeout)
{
	acl_pthread_pool_attr_t attr;
	acl_pthread_pool_t *thr_pool;

	acl_pthread_pool_attr_init(&attr);
	acl_pthread_pool_attr_set_threads_limit(&attr, threads);
	acl_pthread_pool_attr_set_idle_timeout(&attr, timeout);

	/* ������פ���߳̾�� */
	thr_pool = acl_pthread_pool_create(&attr);
	assert(thr_pool);
	return (thr_pool);
}

typedef struct {
	ACL_VSTREAM *fp;
	int  i;
} RUN_CTX;
static acl_pthread_mutex_t __mutex;
static int  __i = 0;
static void run_thread(void *arg)
{
	RUN_CTX *ctx = (RUN_CTX*) arg;

	acl_pthread_mutex_lock(&__mutex);
	if (0)
		acl_vstream_fprintf(ctx->fp, "hello world, id: %d, i: %d\n", ctx->i, __i++);
	else
		__i++;
	acl_pthread_mutex_unlock(&__mutex);
	acl_myfree(ctx);
}

static void test_thread_pool(void)
{
	acl_pthread_pool_t *thr_pool;
	ACL_VSTREAM *fp = acl_vstream_fopen("test.log", O_WRONLY | O_CREAT, 0600, 4096);
	int   i;

	acl_pthread_mutex_init(&__mutex, NULL);
	thr_pool = acl_thread_pool_create(100, 10);

	for (i = 0; i < 1000000; i++) {
		RUN_CTX *ctx = (RUN_CTX*) acl_mymalloc(sizeof(RUN_CTX));
		ctx->fp = fp;
		ctx->i = i;
		acl_pthread_pool_add(thr_pool, run_thread, ctx);
	}

	acl_pthread_pool_destroy(thr_pool);
	acl_pthread_mutex_destroy(&__mutex);
	acl_vstream_close(fp);
	printf("last i: %d\r\n", __i);
}

int main(int argc acl_unused, char *argv[] acl_unused)
{
	acl_pthread_pool_t *thr_pool;
	int  max_threads = 20;  /* ��ಢ��20���߳� */
	int  idle_timeout = 10; /* ÿ�������߳̿���10����Զ��˳� */
	static __thread ACL_VSTRING *buf = NULL;

	if (1) {
		test_thread_pool();
		exit (0);
	}

	buf = acl_vstring_alloc(256);
	acl_vstring_sprintf(buf, "in main thread, id=%u",
		(unsigned int) acl_pthread_self());
	acl_pthread_atexit_add(buf, main_thread_atexit);


	thr_pool = thr_pool_create(max_threads, idle_timeout);
	run_thread_pool(thr_pool);

	if (0) {
		/* ����������� acl_pthread_pool_destroy�������ڵ������̳߳����ٺ�����
		 * ���̱߳�����֪ͨ�����߳��˳������п����̲߳��صȴ����г�ʱʱ�����˳�,
		 */
		printf("> wait all threads to be idle and free thread pool\r\n");
		/* ���������̳߳� */
		acl_pthread_pool_destroy(thr_pool);
	} else {
		/* ��Ϊ���������� acl_pthread_pool_destroy���������п����̶߳��ǵ�����
		 * ��ʱʱ�䵽�����˳�
		 */
		while (1) {
			int   ret;

			ret = acl_pthread_pool_size(thr_pool);
			if (ret == 0)
				break;
			printf("> current threads in thread pool is: %d\r\n", ret);
			sleep(1);
		}
		/* �̳߳��еĹ����߳���Ϊ0ʱ�����̳߳� */
		printf("> all worker thread exit now\r\n");
		acl_pthread_pool_destroy(thr_pool);
	}

	/* ���̵߳ȴ��û����ն����������ַ����˳� */
	printf("> enter any key to exit\r\n");
	getchar();

	return (0);
}
