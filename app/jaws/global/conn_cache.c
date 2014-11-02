#include "lib_acl.h"
#include "conn_cache.h"

/* ��Ӧĳ�� IP:PORT ��ֵ�����ӳؽṹ���Ͷ��� */

typedef struct CONN_POOL {
	CONN_CACHE *conn_cache;	/* ��������ĳ�����ӳػ������ */
	ACL_AIO *aio;		/* �����ӳ����������첽IO���� */
	ACL_FIFO conns;		/* �����ӳ��е����Ӷ���(CONN)���� */
	char key[256];		/* �����ӳض�������Ӧ�Ĵ洢�����ӳػ����еĴ洢�� */
} CONN_POOL;

/* ���ӳ��е�ĳ�����Ӷ������Ͷ��� */

struct CONN {
	CONN_POOL *conn_pool;	/* �������ĵ����ӳض��� */
	ACL_ASTREAM *stream;	/* �����ӵ��첽������ */
	void (*free_fn)(ACL_ASTREAM *stream, void*); /* �ͷŸ�����ʱ�Ļص����� */
	void *ctx;		/* free_fn �Բ���֮һ */
	ACL_FIFO_INFO *info;	/* �����Ӷ���洢�����ӳ�(conn_pool)�Ķ����еĶ���ָ�� */
};

/* ���ͷ����Ӷ�����ռ�õ��ڴ棬�������ر������� */

static void conn_free(CONN *conn)
{
	CONN_POOL *conns = conn->conn_pool;

	/* �ȴ����ӳض�����ɾ�� */
	if (conn->info)
		acl_fifo_delete_info(&conns->conns, conn->info);
	/* �����û��Զ���ص����� */
	if (conn->free_fn)
		conn->free_fn(conn->stream, conn->ctx);
	/* �ͷ��ڴ�ռ� */
	acl_myfree(conn);
}

/* �ͷ����Ӷ��󲢹ر������� */

static void conn_close(CONN *conn)
{
	/* �������Ƚ�ֹ�첽���Ķ����� */
	acl_aio_disable_read(conn->stream);

	/* �첽�رո�����, Ȼ�����첽����Զ����� read_close_callback */
	acl_aio_iocp_close(conn->stream);
}

/* �����ӳض����ͷ�ʱ���ô˻ص����� */

static void conn_pool_free(CONN_POOL *conns)
{
	CONN *conn;

	/* ��Ҫ�����ӳ��е��������Ӷ��ͷ� */

	while ((conn = acl_fifo_pop(&conns->conns)) != NULL) {
		if (conn->stream) {
			if (conn->free_fn)
				conn->free_fn(conn->stream, conn->ctx);
			acl_aio_clean_hooks(conn->stream);
			acl_aio_iocp_close(conn->stream);
		}
		acl_myfree(conn);
	}
	acl_myfree(conns);
}

/* �ͷ����ӳض�ʱ���ص����� */

static void conn_pool_free_timer(int event_type acl_unused, void *context)
{
	CONN_POOL *conns = (CONN_POOL*) context;

	conn_pool_free(conns);
}

/* �����ͷ����ӳصĶ�ʱ��, ֮���Բ��ö�ʱ�����ͷ����ӳض�����Ϊ��
 * ʹ�ͷŹ��̲�������ĵݹ鴦������б���ǰ�ͷ�
 */

static void set_conn_pool_free_timer(CONN_POOL *conns)
{
	acl_aio_request_timer(conns->aio, conn_pool_free_timer, conns, 1, 1);
}

static void conn_pool_stat_timer(int event_type acl_unused, void *context)
{
	const char *myname = "conn_pool_stat_timer";
	CONN_CACHE *cache = (CONN_CACHE*) context;

	/* only for test */
	if (0)
	acl_msg_info("%s(%d): nset: %d, nget: %d, nclose: %d, inter: %d",
		myname, __LINE__, cache->nset, cache->nget,
		cache->nclose, cache->nset - cache->nget - cache->nclose);
}

static void set_conn_pool_stat_timer(CONN_CACHE *cache)
{
	/* ���ö�ʱ�� */
	acl_aio_request_timer(cache->aio, conn_pool_stat_timer, cache, 2, 1);
}

CONN_CACHE *conn_cache_create(ACL_AIO *aio, int conn_limit)
{
	const char *myname = "conn_cache_create";
	CONN_CACHE *cache = (CONN_CACHE*) acl_mycalloc(1, sizeof(CONN_CACHE));

	cache->aio = aio;
	cache->conn_limit = conn_limit;
	cache->cache = acl_htable_create(1024, 0);
	acl_msg_info("%s(%d): ok, conn_limit: %d", myname, __LINE__, conn_limit);

	/* �������ӳػ���״̬��Ϣ�Ķ�ʱ�� */
	set_conn_pool_stat_timer(cache);
	return (cache);
}

/* ���ɶ�ʱ�Ļص����� */

static int read_callback(ACL_ASTREAM *stream acl_unused, void *ctx acl_unused,
	char *data acl_unused, int dlen acl_unused)
{
	const char *myname = "read_callback";

	acl_msg_info("%s(%d), %s: can read connection from server, dlen(%d), data(%s)",
		__FILE__, __LINE__, myname, dlen, data);

	/* ��Ϊ������Ϊ�������ӣ���Ӧ�����ݿɶ�����������ݿɶ�������Ϊ
	 * �޷�֪����δ�����Щ���ݶ���Ҫ�رո�����
	 */

	/* ���� -1 �Ӷ������رջص����� */
	return (-1);
}

/* ���ر�ʱ�Ļص����� */

static int read_close_callback(ACL_ASTREAM *stream acl_unused, void *ctx)
{
	CONN *conn = (CONN*) ctx;
	CONN_POOL *conns = conn->conn_pool;

	/* �ͷŸ����Ӷ�����ڴ�ռ䣬�������رո����ӣ�
	 * �رչ������첽����Զ��ر�
	 */
	conn_free(conn);

	/* ������ӳ�Ϊ�����ͷŸ����ӳ� */
	if (acl_fifo_size(&conns->conns) == 0) {
		acl_htable_delete(conns->conn_cache->cache, conns->key, NULL);
		set_conn_pool_free_timer(conns);
	}

	conns->conn_cache->nclose++;

	/* ���� acl_aio_iocp_close ���� */
	return (-1);
}

/* ������ʱʱ�Ļص����� */

static int read_timeout_callback(ACL_ASTREAM *stream acl_unused, void *ctx acl_unused)
{
	/* ���� -1 �Ӷ������رջص����� */
	return (-1);
}
	
void conn_cache_push_stream(CONN_CACHE *cache, ACL_ASTREAM *stream,
	int timeout, void (*free_fn)(ACL_ASTREAM*, void*), void *ctx)
{
	const char *key = ACL_VSTREAM_PEER(acl_aio_vstream(stream));
	CONN_POOL *conns;
	CONN *conn;

#if 0
	acl_aio_clean_hooks(stream);
#endif

	/* �鿴��KEY�����ӳؾ���Ƿ���ڣ�����������ã����򴴽��µ� */

	conns = (CONN_POOL*) acl_htable_find(cache->cache, key);
	if (conns == NULL) {
		conns = (CONN_POOL*) acl_mymalloc(sizeof(CONN_POOL));
		conns->conn_cache = cache;
		conns->aio = acl_aio_handle(stream);
		acl_fifo_init(&conns->conns);
		ACL_SAFE_STRNCPY(conns->key, key, sizeof(conns->key));
		acl_htable_enter(cache->cache, key, conns);
	}

#if 0
	/* ��������ӳ��е��������������ƣ��������ͷ���ɵ����Ӷ��� */
	if (acl_fifo_size(&conns->conns) >= cache->conn_limit) {
		conn = acl_fifo_pop(&conns->conns);
		if (conn) {
			conn->info = NULL;
			conn_close(conn);
		}
	}
#endif

	cache->nset++;

	/* �����µ��첽�����ӻ������ */
	conn = (CONN*) acl_mymalloc(sizeof(CONN));
	conn->stream = stream;
	conn->ctx = ctx;
	conn->free_fn = free_fn;

	/* ���������ӳ��� */
	conn->info = acl_fifo_push(&conns->conns, conn);
	conn->conn_pool = conns;

	/* ���ø����Ļص����� */
	acl_aio_ctl(stream,
		ACL_AIO_CTL_READ_HOOK_ADD, read_callback, conn,
		ACL_AIO_CTL_CLOSE_HOOK_ADD, read_close_callback, conn,
		ACL_AIO_CTL_TIMEO_HOOK_ADD, read_timeout_callback, conn,
		ACL_AIO_CTL_TIMEOUT, timeout,
		ACL_AIO_CTL_END);
	/* ��ʼ������������ */
	acl_aio_read(stream);
}

CONN *conn_cache_get_conn(CONN_CACHE *cache, const char *key)
{
	CONN_POOL *conns;
	CONN *conn;

	/* �Ȳ鿴��KEY�����ӳض����Ƿ���ڣ�����������򷵻�NULL */

	conns = (CONN_POOL*) acl_htable_find(cache->cache, key);
	if (conns == NULL) {
		return (NULL);
	}

	/* �Ӹ�KEY�����ӳ���ȡ��һ�����ӣ����ȡ��ΪNULL���ͷŸ����ӳض��� */

	conn = acl_fifo_pop(&conns->conns);
	if (conn == NULL) {
		/* �ȴ����ӳػ�����ɾ�� */
		acl_htable_delete(cache->cache, conns->key, NULL);
		/* �����ͷſյ����ӳض���Ķ�ʱ�� */
		set_conn_pool_free_timer(conns);
		return (NULL);
	}

	/* ��ȡ������֮ǰ���õĻص����� */
#if 1
	acl_aio_del_read_hook(conn->stream, read_callback, conn);
	acl_aio_del_close_hook(conn->stream, read_close_callback, conn);
	acl_aio_del_timeo_hook(conn->stream, read_timeout_callback, conn);
#else
	acl_aio_clean_hooks(conn->stream);
#endif

	if (conn->free_fn)
		conn->free_fn(conn->stream, conn->ctx);
	/* ȡ�������� */
	acl_aio_disable_read(conn->stream);

	cache->nget++;
	return (conn);
}

ACL_ASTREAM *conn_cache_get_stream(CONN_CACHE *cache, const char *key, void **ctx_pptr)
{
	CONN *conn;
	ACL_ASTREAM *stream;

	conn = conn_cache_get_conn(cache, key);
	if (conn == NULL) {
		if (ctx_pptr)
			*ctx_pptr = NULL;
		return (NULL);
	}

	if (ctx_pptr)
		*ctx_pptr = conn->ctx;
	stream = conn->stream;
	acl_myfree(conn);  /* ��Ϊ�Ѿ�ȡ�����������Կ����ͷ� CONN ���� */
	return (stream);
}

void conn_cache_delete_key(CONN_CACHE *cache, const char *key)
{
	CONN_POOL *conns;
	ACL_ITER iter;

	conns = (CONN_POOL*) acl_htable_find(cache->cache, key);
	if (conns == NULL)
		return;

	/* �������ӳ��е���������һһ�ر� */
	acl_foreach(iter, &conns->conns) {
		CONN *conn = (CONN*) iter.data;
		if (conn->stream) {
			acl_aio_iocp_close(conn->stream);
			conn->stream = NULL;
		}
	}

	/* �������ӳش����ӳػ�����ɾ�� */
	acl_htable_delete(cache->cache, conns->key, NULL);
	/* �����ͷųض���Ķ�ʱ�� */
	set_conn_pool_free_timer(conns);
}

void conn_cache_delete_conn(CONN *conn)
{
	/* �ͷŲ��رո����Ӷ��� */
	conn_close(conn);
}

void conn_cache_delete_stream(CONN_CACHE *cache, ACL_ASTREAM *stream)
{
	const char *key = ACL_VSTREAM_PEER(acl_aio_vstream(stream));
	CONN_POOL *conns;
	CONN *conn;
	ACL_ITER iter;

	conns = acl_htable_find(cache->cache, key);
	if (conns == NULL)
		return;
	/* �������ӳض����е��������� */
	acl_foreach(iter, &conns->conns) {
		conn = (CONN*) iter.data;
		if (conn->stream == stream) {
			/* �ͷŲ��رո����Ӷ��� */
			conn_close(conn);
			break;
		}
	}
}
