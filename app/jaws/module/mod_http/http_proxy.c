#include "lib_acl.h"
#include "lib_protocol.h"
#include "assert.h"
#include "dns_lookup.h"
#include "conn_cache.h"
#include "service.h"
#include "http_module.h"
#include "http_service.h"

#ifdef ACL_MS_WINDOWS
#include <process.h>
#define getpid _getpid
#endif

static int http_proxy_next(HTTP_CLIENT *http_client);
static void http_proxy_req_get(HTTP_CLIENT *http_client);

/*---------------------------------------------------------------------------*/

/* ��ǰ���������, �Ƿ������һ���ͻ�������? */

static void http_proxy_server_complete(HTTP_CLIENT *http_client, int keep_alive)
{
	HTTP_SERVICE *service = (HTTP_SERVICE*) http_client->entry.service;
	ACL_ASTREAM *server;
	ACL_VSTREAM *sstream;

	if (http_client->res) {
		http_res_free(http_client->res);
		http_client->res = NULL;
		http_client->hdr_res = NULL;
	} else if (http_client->hdr_res) {
		http_hdr_res_free(http_client->hdr_res);
		http_client->hdr_res = NULL;
	}

	server = http_client->entry.server;
	/* �Ƿ�Ӧ�����˱��ֳ�����? */
	if (server == NULL) {
		return;
	}

	/* �Ƚ�ֹ�Ը��첽���������� */
	acl_aio_disable_readwrite(server);

	/* ����첽�������й��ӻص���������ֹ����������ص������� */
	acl_aio_clean_hooks(server);

	sstream = acl_aio_vstream(server);

	if (keep_alive) {
		int   timeout = 60;
		/* ������������ */
		client_entry_detach(&http_client->entry, sstream);
		ACL_VSTRING_RESET(&server->strbuf);

		/* �������˵��������������ӳ��� */
		conn_cache_push_stream(service->service.conn_cache,
				server, timeout, NULL, NULL);
	} else {
		client_entry_detach(&http_client->entry, sstream);
		/* �ر��첽�� */
		acl_aio_iocp_close(server);
	}
}

static void http_proxy_client_complete(HTTP_CLIENT *http_client, int keep_alive)
{
	ACL_ASTREAM *client;

	/* �������Դ��� */
	http_client->entry.nretry_on_error = 0;
	http_client->entry.ip_ntry = 0;

	/* ������λỰ��ɱ�־λ */
	http_client->flag &= ~HTTP_FLAG_FINISH;

	client = http_client->entry.client;
	if (client == NULL) {
		return;
	}

	/* ����첽�������й��ӻص���������ֹ����������ص������� */
	acl_aio_clean_hooks(client);

	/* �Ƿ�Ӧ��ͻ��˱��ֳ�����? */
	if (keep_alive) {
		if (http_client->req_curr) {
			http_client_req_free(http_client->req_curr);
			http_client->req_curr = NULL;
		}
		http_proxy_next(http_client);
	} else {
		ACL_VSTREAM *cstream = acl_aio_vstream(client);

		if (http_client->req_curr) {
			http_client_req_free(http_client->req_curr);
			http_client->req_curr = NULL;
		}
		acl_aio_disable_readwrite(client);
		/* ��ͻ��������� */
		client_entry_detach(&http_client->entry, cstream);
		/* �ر��ϵ��첽�� */
		acl_aio_iocp_close(client);
	}
}

static void http_proxy_complete(HTTP_CLIENT *http_client, int error_happen)
{
	/* ��Ҫ��ǰ֪����������Ϳͻ������Ƿ��Ѿ������룬��Ϊ���澭��
	 * http_proxy_server_complete �� http_proxy_client_complte ��
	 * http_client ��ռ�ڴ�����Ѿ����ͷţ�������ǰ֪�������/�ͻ�
	 * ������״̬���Ա����ڴ�Ƿ�����
	 */
	int  server_null = http_client->entry.server == NULL;
	int  client_null = http_client->entry.client == NULL;

	/* �ж���������Ƿ�Ӧ���ֳ����� */

	if (var_cfg_http_server_keepalive && !error_happen
		&& http_client->hdr_res
		&& http_client->hdr_res->hdr.keep_alive)
	{
		http_client->flag |= HTTP_FLAG_SERVER_KEEP_ALIVE;
	}

	/* �ж��ͻ������Ƿ�Ӧ���ֳ����� */

	if (var_cfg_http_client_keepalive && !error_happen
		&& (http_client->flag & HTTP_FLAG_SERVER_KEEP_ALIVE)
		&& http_client->req_curr
		&& http_client->req_curr->hdr_req
		&& http_client->req_curr->hdr_req->hdr.keep_alive)
	{
		http_client->flag |= HTTP_FLAG_CLIENT_KEEP_ALIVE;
	} else {
		http_client->flag &= ~HTTP_FLAG_CLIENT_KEEP_ALIVE;
	}

	/* ������������������״̬�������رշ������ */

	if (!server_null && !(http_client->flag & HTTP_FLAG_SERVER_LOCKED)) {
		http_proxy_server_complete(http_client,
			(http_client->flag & HTTP_FLAG_SERVER_KEEP_ALIVE));
	}

	/* ����ͻ�������������״̬�������رտͻ����� */

	if (!client_null && !(http_client->flag & HTTP_FLAG_CLIENT_LOCKED)) {
		http_proxy_client_complete(http_client,
			(http_client->flag & HTTP_FLAG_CLIENT_KEEP_ALIVE));
	}
}

/*---------------------------------------------------------------------------*/

/* ������Ӧ�������ͻ��ˣ�Ϊ�˼���IO�������ϲ���Ӧͷͬ��Ӧ������һ���� */

static void send_to_client(HTTP_CLIENT *http_client, char *data, int dlen)
{
	int   hdr_len = (int) LEN(http_client->buf);

	/* �Ƿ���ͬHTTP��Ӧͷһ����? */
	if (hdr_len > 0) {
		/* ��HTTP��Ӧͷ��һ����������һ����,
		 * �������Լ��� IO д����
		 */

		struct iovec iov[2];

		iov[0].iov_base = STR(http_client->buf);
		iov[0].iov_len  = hdr_len;
		iov[1].iov_base = (char*) data;
		iov[1].iov_len  = dlen;

		/* �����ύ����������ǰ��λ������Ӱ���ڲ����� */
		ACL_VSTRING_RESET(http_client->buf);
		/* ���ͻ�������������ֹ����ǰ�ر� */
		http_client->flag |= HTTP_FLAG_CLIENT_LOCKED;
		acl_aio_writev(http_client->entry.client, iov, 2);
	} else {
		/* ��Ӧͷ�Ѿ����ͣ��˴���������Ӧ�岿������ */

		/* ���ͻ�������������ֹ����ǰ�ر� */
		http_client->flag |= HTTP_FLAG_CLIENT_LOCKED;
		acl_aio_writen(http_client->entry.client, data, dlen);
	}
}

/* ��÷�����HTTP�����岢����������� */

static void forward_respond_body_data(HTTP_CLIENT *http_client,
	char *data, int dlen)
{
	const char *data_saved = data;
	char *data_ptr = data;
	char *ptr;
	ACL_ITER iter;
	plugin_dat_free_fn last_plugin_free = NULL;
	char *last_plugin_buf = NULL;
	void *plugin_res_ctx = http_client->plugin_res_ctx;
	HTTP_SERVICE *service = (HTTP_SERVICE*) http_client->entry.service;

	/* �������е������������ */
	acl_foreach(iter, &service->respond_dat_plugins) {
		HTTP_PLUGIN *tmp = (HTTP_PLUGIN*) iter.data;
		int  stop = 0, ret;
		ptr = tmp->data_filter(data_ptr, dlen, &ret, &stop, plugin_res_ctx);

		/* �ͷ�ǰһ������������Ķ�̬�ڴ� */
		if (last_plugin_buf && last_plugin_buf != data_saved && last_plugin_free)
			last_plugin_free(last_plugin_buf, plugin_res_ctx);

		dlen = ret;
		data = data_ptr = ptr;
		last_plugin_buf = ptr;
		last_plugin_free = tmp->data_free;

		if (ret < 0 || ptr == NULL) {
			ret = -1;
			data = NULL;
			break;
		} else if (stop)
			break;
	}

	/* ��ͻ���д���� */
	if (dlen > 0 && data)
		send_to_client(http_client, data, dlen);

	/* �ͷ�ǰһ������������Ķ�̬�ڴ� */
	if (last_plugin_buf && last_plugin_buf != data_saved && last_plugin_free)
		last_plugin_free(last_plugin_buf, plugin_res_ctx);
}

/* �ɹ��ӷ�����������Ӧ�����ݵĻص����� */

static int read_respond_body_ready(int status, char *data, int dlen, void *arg)
{
	HTTP_CLIENT *http_client = (HTTP_CLIENT *) arg;

acl_msg_info("%s(%d)", __FUNCTION__, __LINE__); /* only for test */
	if (data == NULL || dlen <= 0) {
		/* ȡ���������������״̬ */
		http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;
		/* ���ûỰ��ɱ�־λ */
		http_client->flag |= HTTP_FLAG_FINISH;
		http_proxy_complete(http_client, -1);
		return (-1);
	}

	http_client->total_size += dlen;

	/* client ���п��ܱ���ǰ�ر��� */
	if (http_client->entry.client == NULL) {
		/* ȡ���������������״̬ */
		http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;
		/* ���ûỰ��ɱ�־λ */
		http_client->flag |= HTTP_FLAG_FINISH;
		http_proxy_complete(http_client, -1);
		return (-1);
	}

	if (status >= HTTP_CHAT_ERR_MIN) {
		/* ȡ���������������״̬ */
		http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;
		/* ���ûỰ��ɱ�־λ */
		http_client->flag |= HTTP_FLAG_FINISH;
		http_proxy_complete(http_client, -1);
		return (-1);
	} else if (status == HTTP_CHAT_OK) {
		/* ���ûỰ��ɱ�־λ */
		http_client->flag |= HTTP_FLAG_FINISH;
#if 1
		/* ȡ���������������״̬ */
		http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;
#endif
	}

	/* ��� HTTP_FLAG_FINISH ��־���ã������ forward_respond_body_data ֮��
	 * �ص����� send_respond_body_complete ���� http_proxy_complete
	 */
	forward_respond_body_data(http_client, data, dlen);
#if 0
	if (status == HTTP_CHAT_OK) {
		/* ȡ���������������״̬ */
		http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;
		http_proxy_complete(http_client, 0);
	}
#endif

	return (0);
}

/* ������Ӧ���������ͻ���, ���ȷ���Ѿ����������һ�������򴥷��������� */

static int send_respond_body_complete(ACL_ASTREAM *client acl_unused, void *ctx)
{
	HTTP_CLIENT *http_client = (HTTP_CLIENT *) ctx;

	/* ȡ���ͻ�����������״̬, �Ӷ������������쳣�ر�ʱ������
	 * on_close_server �Ⱥ�������� http_proxy_complete ʱ��رտͻ���!
	 */
	http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;

	/* �����������������ɱ��λỰ���� */
	if ((http_client->flag & HTTP_FLAG_FINISH)) {
		if ((http_client->flag & HTTP_FLAG_SERVER_CLOSED))
			http_proxy_complete(http_client, -1);
		else
			http_proxy_complete(http_client, 0);
	}
	return (0);
}

/* �����������ӦHTTP�������������
 * ���øù��̵ĺ�����Ҫע�����رձ�����ʩ
 */

static void forward_respond_hdr_body(HTTP_CLIENT *http_client)
{
	/* ����HTTP��Ӧ����� */
	http_client->res = http_res_new(http_client->hdr_res);

	/* �����ͻ���������Ӧ��ʧ�ܻ��Զ������� send_request_hdr_complete
	 * ����Կͻ������õĻص����� on_close_clinet
	 */

	/* ������ͻ����������ݳɹ��Ļص����� */
	acl_aio_add_write_hook(http_client->entry.client,
		send_respond_body_complete, http_client);

	/* �����������������״̬, �Ӷ���ֹ����ǰ�ر� */
	http_client->flag |= HTTP_FLAG_SERVER_LOCKED;

acl_msg_info("%s(%d)", __FUNCTION__, __LINE__); /* only for test */
	/* ��ʼ�ӷ�������ȡHTTP���������� */
	http_res_body_get_async(http_client->res,
		http_client->entry.server,
		read_respond_body_ready,
		http_client,
		http_client->entry.service->rw_timeout);
}

/* ���ͷ�������Ӧͷ������� */

static int send_respond_hdr_complete(ACL_ASTREAM *client acl_unused, void *ctx)
{
	const char *myname = "send_respond_hdr_complete";
	HTTP_CLIENT *http_client = (HTTP_CLIENT *) ctx;

	/* ȡ���ͻ�����������״̬ */
	http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;

	if (http_client->hdr_res == NULL) {
		acl_msg_error("%s(%d): http_client->hdr_res null", myname, __LINE__);
		http_proxy_complete(http_client, -1);
		return (0);
	}

	/* ��Ϊ����˺����� client ������ֵ�Ѿ��� acl_aio_xxx �Զ���1�ˣ�
	 * ����Ҳ���ص����ظ��ر�����������
	 */
	http_proxy_complete(http_client, 0);
	return (0);
}

/* �� forward ��Ӧͷ����Ϊû����Ӧ�� */

static void forward_respond_hdr(HTTP_CLIENT *http_client)
{
	acl_aio_add_write_hook(http_client->entry.client,
		send_respond_hdr_complete, http_client);

	/* �趨�ͻ�����Ϊ����״̬ */
	http_client->flag |= HTTP_FLAG_CLIENT_LOCKED;

	acl_aio_writen(http_client->entry.client,
		acl_vstring_str(http_client->buf),
		(int) ACL_VSTRING_LEN(http_client->buf));
}

static void start_forward_respond(HTTP_CLIENT *http_client)
{
	/* �Ƿ������˱��ֳ�����? */
	if (!var_cfg_http_client_keepalive) {
		http_hdr_entry_replace(&http_client->hdr_res->hdr,
			"Connection", "close", 1);
		http_hdr_entry_replace(&http_client->hdr_res->hdr,
			"Proxy-Connection", "close", 0);
	} else if (http_client->req_curr->hdr_req->hdr.keep_alive
		&& http_client->hdr_res->hdr.keep_alive)
	{
		http_hdr_entry_replace(&http_client->hdr_res->hdr,
			"Connection", "keep-alive", 1);
		http_hdr_entry_replace(&http_client->hdr_res->hdr,
			"Proxy-Connection", "keep-alive", 0);
	}

	/* �������HTTP��Ӧͷ */
	http_hdr_build(&http_client->hdr_res->hdr, http_client->buf);

	/* ���� 3xx, 4xx �ķ�������Ӧ����Ӧ�������岿�� */

	if (http_client->hdr_res->hdr.content_length == 0
	    || (http_client->hdr_res->hdr.content_length == -1
		&& !http_client->hdr_res->hdr.chunked
		&& http_client->hdr_res->reply_status > 300
		&& http_client->hdr_res->reply_status < 400))
	{
		/* ���û�������壬�����������ͷ */
		forward_respond_hdr(http_client);
		return;
	}

	/* ������ͷ��ͬһ����������һ���͸��ͻ��ˣ��Ӷ�����io���� */

	/* xxx: ����û�� content-length �� content-length > 0
	 * ����������Ӧ״̬�벻Ϊ 3xx, 4xx �����
	 */
	forward_respond_hdr_body(http_client);
}

/**
 * ���HTTP��Ӧͷ�Ĺ�����������̣�������ݸ���Ӧͷ������������ȫ�ӹ�
 * ����Ӧ���������ٴ���÷���������ͻ�����
 * ���� 0 ��ʾ���й����������ӹܸ���Ӧ, �����ʾ�ӹ�
 */
static int reply_plugin_takeover(HTTP_CLIENT *http_client)
{
	const char *myname = "reply_plugin_takeover";
	HTTP_SERVICE *service = (HTTP_SERVICE*) http_client->entry.service;
	HTTP_PLUGIN *plugin = NULL;
	ACL_ITER iter;

	/* xxx: plugin_res_ctx �ò�����ÿ�������п��ܲ�һ��, ���ģ��Ӧ�����й��� */
	http_client->plugin_res_ctx = NULL;

	/* �������еĲ���ص������� */

	acl_foreach(iter, &service->respond_plugins) {
		ACL_ASTREAM *client = http_client->entry.client;
		ACL_ASTREAM *server = http_client->entry.server;
		ACL_VSTREAM *client_stream = acl_aio_vstream(client);
		ACL_VSTREAM *server_stream = acl_aio_vstream(server);
		HTTP_PLUGIN *tmp = (HTTP_PLUGIN*) iter.data;

		if (tmp->filter.respond(client_stream, server_stream,
			http_client->req_curr->hdr_req,
			http_client->hdr_res,
			&http_client->plugin_res_ctx))
		{
			plugin = tmp;
			break;
		}
	}

	if (plugin && plugin->forward.respond) {
		ACL_ASTREAM *client = http_client->entry.client;
		ACL_ASTREAM *server = http_client->entry.server;
		ACL_VSTREAM *client_stream = acl_aio_vstream(client);
		ACL_VSTREAM *server_stream = acl_aio_vstream(server);
		HTTP_HDR_REQ *hdr_req;
		HTTP_HDR_RES *hdr_res;
		void *plugin_res_ctx = http_client->plugin_res_ctx;

		/* �� http_client �е� hdr_req/hdr_res �ÿ� */
		if (http_client->req_curr) {
			hdr_req = http_client->req_curr->hdr_req;
			http_client->req_curr->hdr_req = NULL;
			if (http_client->req_curr->req)
				http_client->req_curr->req->hdr_req = NULL;
		} else {
			acl_msg_fatal("%s(%d): req_curr null", myname, __LINE__);
			/* XXX: can't reach here just avoid compiling warning */
			hdr_req = NULL;
		}
		hdr_res = http_client->hdr_res;
		http_client->hdr_res = NULL;

		/* ���ͻ�����������ô��������� */
		client_entry_detach(&http_client->entry, client_stream);

		/* �������������ô��������� */
		/* ��Ϊ entry �����������ü���Ϊ0����������ڸ÷��뺯��
		 * ���Զ����ͷ�
		 */
		client_entry_detach(&http_client->entry, server_stream);

		/* ��ֹ�첽���Ķ�/д��� */
		acl_aio_disable_readwrite(client);
		acl_aio_disable_readwrite(server);

		/* ����ص����� */
		acl_aio_clean_hooks(client);
		acl_aio_clean_hooks(server);

		/* ���ͻ����첽�����������ÿ� */
		acl_aio_ctl(client, ACL_AIO_CTL_STREAM, NULL, ACL_AIO_CTL_END);
		/* ��������첽�����������ÿ� */
		acl_aio_ctl(server, ACL_AIO_CTL_STREAM, NULL, ACL_AIO_CTL_END);

		/* xxx: �첽�ر� client/server �첽�� */
		acl_aio_iocp_close(client);
		acl_aio_iocp_close(server);

		/* �������ɷ�����ģʽת��Ϊ����ģʽ */
		acl_non_blocking(ACL_VSTREAM_SOCK(client_stream), ACL_BLOCKING);
		acl_non_blocking(ACL_VSTREAM_SOCK(server_stream), ACL_BLOCKING);

		/* �����ڷ�����ͨ��ʱ���õĹرջص����������֮ */
		acl_vstream_call_close_handles(client_stream);
		acl_vstream_call_close_handles(server_stream);

		/* ���ˣ��Ѿ����ͻ����������ɷ�����ģʽת��Ϊ����ģʽ��ͬʱ�ر�
		 * �������˵���������������������ת����ش���ģ�鴦���첽
		 * �����ٴ���ÿͻ��˵����󼰷���˵���Ӧ
		 * ע�⣺client_stream, hdr_res, hdr_res �˴���δ�ͷţ�
		 * ��Ҫ���ش���ģ��������Ϻ��Լ������ͷ�
		 */
		plugin->forward.respond(client_stream, server_stream,
				hdr_req, hdr_res, plugin_res_ctx);
		return (1);
	}

	return (0);
}

static int http_request_reforward(HTTP_CLIENT *http_client);
static int read_respond_hdr_timeout(ACL_ASTREAM *server, void *ctx);
static int read_respond_hdr_error(ACL_ASTREAM *server, void *ctx);

/* ��÷�������Ӧͷ */
static void begin_read_respond(HTTP_CLIENT *http_client);

static int get_respond_hdr_ready(int status, void *arg)
{
	const char *myname = "get_respond_hdr_ready";
	HTTP_CLIENT *http_client = (HTTP_CLIENT *) arg;
	ACL_ASTREAM *client = http_client->entry.client;
	ACL_ASTREAM *server = http_client->entry.server;
 
	/* ȡ��������������״̬ */
	http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;

	/* xxx: sanity check */

	if (client == NULL) {
		acl_msg_warn("%s: client null(%s)", myname,
			http_client->flag & HTTP_FLAG_FINISH
			? "finished" : "not finished");
		http_client->flag |= HTTP_FLAG_FINISH;
		http_proxy_complete(http_client, -1);
		return (0);
	}

	/* ��Ҫ�ر������ص���������ֹ���� read_respond_hdr_error
	 * �� read_respond_hdr_timeout ����(read_respond_hdr_timeout �ᴥ��
	 * read_respond_hdr_error), ���� read_respond_hdr_error ������
	 * http_request_reforward
	 */
	acl_aio_ctl(server,
		ACL_AIO_CTL_CLOSE_HOOK_DEL, read_respond_hdr_error, http_client,
		ACL_AIO_CTL_TIMEO_HOOK_DEL, read_respond_hdr_timeout, http_client,
		ACL_AIO_CTL_END);

acl_msg_info("%s(%d)", __FUNCTION__, __LINE__); /* only for test */
	if (status != HTTP_CHAT_OK) {
		/* �������Ӧͷ���ִ�������Ҫ���� */

		/* �������� */
		if (http_request_reforward(http_client) == 0) {
			/* ����Ѿ���ʼ���Թ��̣���ֱ�ӷ��� */
			return (0);
		}

		http_proxy_complete(http_client, -1);
		/* xxx: Ӧ�÷��� 5xx ��Ϣ���ͻ��� */
		return (0);
	}

	/* ���� HTTP ��Ӧͷ */

	if (http_hdr_res_parse(http_client->hdr_res) < 0) {
		/* ���������Ӧͷʧ������Ҫ���� */

		acl_msg_error("%s: parse hdr_res error", myname);
		/* �������� */
		if (http_request_reforward(http_client) == 0) {
			/* ����Ѿ���ʼ���Թ��̣���ֱ�ӷ��� */
			return (0);
		}

		http_proxy_complete(http_client, -1);
		/* xxx: Ӧ�÷��� 5xx ��Ϣ���ͻ��� */
		return (0);
	}

	/* ���� 100 continue �Ļ�Ӧ */
	if (http_client->hdr_res->reply_status == 100) {
		begin_read_respond(http_client);
		return (0);
	}

	/* �ж��Ƿ���Ҫ����������ģ��ӹ� */
	if (reply_plugin_takeover(http_client)) {
		return (0);
	}

	/* ��ʼת�����������ص����ݸ��ͻ��� */
	start_forward_respond(http_client);
	return (0);
}

static int read_respond_hdr_timeout(ACL_ASTREAM *server, void *ctx)
{
	const char *myname = "read_respond_hdr_timeout";
	HTTP_CLIENT *http_client = (HTTP_CLIENT *) ctx;

	/* ȡ��������������״̬ */
	http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;

	/* ȡ�� HTTP ��Ӧͷ�Ķ��رջص����� */
	acl_aio_clean_close_hooks(server);

	if (http_client->entry.client == NULL) {
		acl_msg_warn("%s(%d): client null", myname, __LINE__);
		http_proxy_complete(http_client, -1);
		/* ���뷵�� -1, ��Ϊ��ϣ���������������ĳ�ʱ�ص����� */
		return (-1);
	}

	/* �������� */
	if (http_request_reforward(http_client) == 0) {
		/* ����Ѿ���ʼ���Թ��̣���ֱ�ӷ��� */
		/* ���뷵�� -1, ��Ϊ��ϣ���������������ĳ�ʱ�ص����� */
		return (-1);
	}

	/* �����ͻ����� */
	http_client->flag |= HTTP_FLAG_CLIENT_LOCKED;

	/* ���ظ��ͻ��˶��������Ӧ��ʱ��Ϣ */
	acl_aio_writen(http_client->entry.client,
		HTTP_REPLY_TIMEOUT, (int) strlen(HTTP_REPLY_TIMEOUT));

	/* �����ͻ����� */
	http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;

	http_proxy_complete(http_client, -1);
	/* ���뷵�� -1, ��Ϊ��ϣ���������������ĳ�ʱ�ص����� */
	return (-1);
}

static int read_respond_hdr_error(ACL_ASTREAM *server acl_unused, void *ctx)
{
	const char *myname = "read_respond_hdr_error";
	HTTP_CLIENT *http_client = (HTTP_CLIENT *) ctx;

	/* ȡ��������������״̬ */
	http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;

	if (http_client->entry.client == NULL) {
		acl_msg_warn("%s(%d): client null", myname, __LINE__);
		/* ���뷵�� -1, ��Ϊ��ϣ���������������Ĺرջص����� */
		http_proxy_complete(http_client, -1);
		return (-1);
	}

	/* �������� */
	if (http_request_reforward(http_client) == 0) {
		/* ����Ѿ���ʼ���Թ��̣���ֱ�ӷ��� */
		/* ���뷵�� -1, ��Ϊ��ϣ���������������Ĺرջص����� */
		return (-1);
	}

	/* �����ͻ����� */
	http_client->flag |= HTTP_FLAG_CLIENT_LOCKED;

	/* ���ظ��ͻ��˶��������Ӧ������Ϣ */
	acl_aio_writen(http_client->entry.client,
		HTTP_REPLY_ERROR, (int) strlen(HTTP_REPLY_ERROR));

	/* �����ͻ����� */
	http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;

	http_proxy_complete(http_client, -1);
	/* ���뷵�� -1, ��Ϊ��ϣ���������������Ĺرջص����� */
	return (-1);
}

/* XXX: �ú�����Ҫ���ڹرձ���״̬�������ô˺����ĺ�����Ҫ�Է�������ӱ�����ʩ */

static void begin_read_respond(HTTP_CLIENT *http_client)
{
	/* ����һ�� HTTP ��Ӧͷ */
	http_client->hdr_res = http_hdr_res_new();

	/* �趨������������״̬ */
	http_client->flag |= HTTP_FLAG_SERVER_LOCKED;

	/* ���ôӷ������Ķ����󼰶���ʱ�Ļص����� */
	acl_aio_ctl(http_client->entry.server,
		ACL_AIO_CTL_CLOSE_HOOK_ADD, read_respond_hdr_error, http_client,
		ACL_AIO_CTL_TIMEO_HOOK_ADD, read_respond_hdr_timeout, http_client,
		ACL_AIO_CTL_END);

	/* ��ʼ������˵� HTTP ��Ӧͷ */
	http_hdr_res_get_async(http_client->hdr_res,
		http_client->entry.server,
		get_respond_hdr_ready,
		http_client,
		http_client->entry.service->rw_timeout);
}

/*----------------------------------------------------------------------------*/

/* ������������������������Ļص�����, ���˺�����ͨ�ŷ�����
 * �ı䣬��ԭ���Ĵӿͻ��������ݡ��������д���ݱ�Ϊ�ӷ�������
 * ���ݡ���ͻ���д����
 */

static int send_request_body_complete(ACL_ASTREAM *server, void *context)
{
	HTTP_CLIENT *http_client = (HTTP_CLIENT*) context;

	/* ȡ��������������״̬ */
	http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;

	/* xxx: sanity check */
	if (http_client->entry.client == NULL) {
		http_proxy_complete(http_client, -1);
		return (0);
	}

	/* ������������ݷ��������ʼ��ȡ��������Ӧ */
	if ((http_client->flag & HTTP_FLAG_REQEND)) {
		/* ȡ��֮ǰ���õķ���������ɹ��Ļص����� */
		acl_aio_del_write_hook(server, send_request_body_complete,
				http_client);
		http_client->flag &= ~HTTP_FLAG_REQEND;
		/* ��ʼ��ȡ����˵���Ӧ���� */
		begin_read_respond(http_client);
	}
	return (0);
}

/* ����һЩHTTP���������� */

static int read_request_body_ready(int status, char *data, int dlen, void *arg)
{
	const char *myname = "read_request_body_ready";
	HTTP_CLIENT *http_client = (HTTP_CLIENT *) arg;

	if (data == NULL || dlen <= 0) {
		acl_msg_error("%s(%d): data: %s, dlen: %d",
			myname, __LINE__, data ? "not null" : "null", dlen);
		/* ȡ���ͻ���������״̬ */
		http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;
		/* �������������ϱ�־λ */
		http_client->flag |= HTTP_FLAG_REQEND;
		http_proxy_complete(http_client, -1);
		return (0);
	}

	if (http_client->entry.server == NULL) {
		/* �п�����������д����ʱ����������� on_close_server ����,
		 * �Ӷ����� http_proxy_complete ���̱�����
		 */

		/* ȡ���ͻ���������״̬ */
		http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;
		/* �������������ϱ�־λ */
		http_client->flag |= HTTP_FLAG_REQEND;
		http_proxy_complete(http_client, -1);
		return (0);
	}

	if (status >= HTTP_CHAT_ERR_MIN) {
		/* ȡ���ͻ�����������״̬ */
		http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;
		/* �������������ϱ�־λ */
		http_client->flag |= HTTP_FLAG_REQEND;
		http_proxy_complete(http_client, -1);
		return (0);
	} else if (status == HTTP_CHAT_OK) {
		/* �Ѿ���������������λỰ���������� */

		/* ��Ϊ�Ѿ��ӿͻ��˶����˱��λỰ���������ݣ����Դ˴�����
		 * ȡ���ͻ���������״̬, ������������д���ݳ���ʱ������
		 * on_close_server �йرտͻ�����
		 */
		http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;
		/* �������������ϱ�־λ */
		http_client->flag |= HTTP_FLAG_REQEND;
	}

	/* �趨�������������״̬ */
	http_client->flag |= HTTP_FLAG_SERVER_LOCKED;
	/* ��������������������岿�ַ����������� */
	acl_aio_writen(http_client->entry.server, data, dlen);
	return (0);
}

/* �������������ת�������������������� */

static void forward_request_body(HTTP_CLIENT *http_client)
{
	/* ��������ͷ����������������� */
 	http_client->req_curr->req = http_req_new(http_client->req_curr->hdr_req);

	/* ���÷���������ɹ��Ļص����� */
	acl_aio_add_write_hook(http_client->entry.server,
		send_request_body_complete, http_client);

	/* ���ͻ�������������״̬, �Ӷ���ֹ����ǰ�ر� */
	http_client->flag |= HTTP_FLAG_CLIENT_LOCKED;

	 /* ��ʼ���ͻ������������� */
	http_req_body_get_async(http_client->req_curr->req,
		http_client->entry.client,
		read_request_body_ready,
		http_client,
		http_client->entry.service->rw_timeout);
}

/* ��������ͷ��������ʱ����Ļص����� */
static int send_request_hdr_complete(ACL_ASTREAM *server, void *ctx);

static int send_request_hdr_error(ACL_ASTREAM *server acl_unused, void *ctx)
{
	HTTP_CLIENT *http_client = (HTTP_CLIENT *) ctx;

	/* ȡ��������������״̬ */
	http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;

	/* ������Ǵ�������ͷʱ��������Խ������� */
	if (http_request_reforward(http_client) == 0) {
		/* ����Ѿ���ʼ���Թ��̣���ֱ�ӷ��� */
		/* ���뷵�� -1, ��Ϊ��ϣ���������������Ĺرջص����� */
		return (-1);
	}

	/* ��ֹ��ͻ���д���ݳ���ʱ��ǰ�رտͻ��� */
	acl_aio_refer(http_client->entry.client);

	/* ���ظ��ͻ��˶��������Ӧ������Ϣ */
	acl_aio_writen(http_client->entry.client,
		HTTP_SEND_ERROR, (int) strlen(HTTP_SEND_ERROR));

	/* �ָ��ͻ���Ϊ�ɹر�״̬ */
	acl_aio_unrefer(http_client->entry.client);

	/* �ûỰ��� */
	http_proxy_complete(http_client, -1);
	/* ���뷵�� -1, ��Ϊ��ϣ���������������Ĺرջص����� */
	return (-1);
}

/* ��������ͷ���������ɹ�ʱ�Ļص����� */

static int send_request_hdr_complete(ACL_ASTREAM *server acl_unused, void *ctx)
{
	HTTP_CLIENT *http_client = (HTTP_CLIENT *) ctx;

	/* ȡ��������������״̬ */
	http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;

	/* �ر��ϴ�ע���д��ɼ�����Ļص����� */
	acl_aio_ctl(http_client->entry.server,
		ACL_AIO_CTL_WRITE_HOOK_DEL, send_request_hdr_complete, http_client,
		ACL_AIO_CTL_CLOSE_HOOK_DEL, send_request_hdr_error, http_client,
		ACL_AIO_CTL_END);

	if (http_client->req_curr->hdr_req->hdr.content_length > 0) {
		/* ����������壬���ȡ�ͻ������������� */
		forward_request_body(http_client);
	} else {
		/* û�������壬��ʼ������˵ķ������� */
		begin_read_respond(http_client);
	}

	return (0);
}

/* ���¹���HTTP����ͷ */

static void rebuild_request(HTTP_HDR_REQ *hdr_req, ACL_VSTRING *buf)
{
	ACL_ITER iter;
	HTTP_HDR_ENTRY *entry;
	int   i = 0;

	/* XXX: nginx ��ʱ�Ժ��� Proxy-Connection ��������ʱ�����ӳ�? */
#if 0
	http_hdr_entry_off(&hdr_req->hdr, "Proxy-Connection");
#endif

#if 0
	acl_vstring_sprintf(buf, "%s http://%s%s HTTP/%d.%d\r\n",
		hdr_req->method, hdr_req->host,
		acl_vstring_str(hdr_req->url_part),
		hdr_req->hdr.version.major,
		hdr_req->hdr.version.minor);
#else
	acl_vstring_sprintf(buf, "%s %s HTTP/%d.%d\r\n",
		hdr_req->method,
		acl_vstring_str(hdr_req->url_part),
		hdr_req->hdr.version.major,
		hdr_req->hdr.version.minor);
#endif

	acl_foreach(iter, hdr_req->hdr.entry_lnk) {
		if (i++ == 0)
			continue;
		entry = (HTTP_HDR_ENTRY*) iter.data;
		if (entry->off)
			continue;
		acl_vstring_strcat(buf, entry->name);
		acl_vstring_strcat(buf, ": ");
		acl_vstring_strcat(buf, entry->value);
		acl_vstring_strcat(buf, "\r\n");
	}
	acl_vstring_strcat(buf, "\r\n");
}

/* ���ӷ������ɹ�����ʼ�����������HTTP����ͷ */
static void start_forward_request(HTTP_CLIENT *http_client)
{
	/* ���䶯̬�ڴ� */
	if (http_client->buf == NULL) {
		http_client->buf = acl_vstring_alloc(HTTP_HDRLEN_DEF);
	} else {
		ACL_VSTRING_RESET(http_client->buf);
	}

	if (var_cfg_http_proxy_connection_off) {
		/* ��Ҫ��һЩ�Ƚ���������GFW�Ķ����ƺ�������ֶ������⣬�����ӳ� */
		http_hdr_entry_off(&http_client->req_curr->hdr_req->hdr, "Proxy-Connection");
	}

	/* ���´��� HTTP ����ͷ�� http_client->buf �� */
	rebuild_request(http_client->req_curr->hdr_req, http_client->buf);

	/* ���ûص����� */
	acl_aio_ctl(http_client->entry.server,
		ACL_AIO_CTL_WRITE_HOOK_ADD, send_request_hdr_complete, http_client,
		ACL_AIO_CTL_CLOSE_HOOK_ADD, send_request_hdr_error, http_client,
		ACL_AIO_CTL_END);

	/* ��������� */
	http_client->flag |= HTTP_FLAG_SERVER_LOCKED;

	/* �������ת���ͻ��˵�HTTP����ͷ���� */
	acl_aio_writen(http_client->entry.server,
		acl_vstring_str(http_client->buf),
		(int) ACL_VSTRING_LEN(http_client->buf));
}

/*----------------------------------------------------------------------------*/

/* ���ͻ��������������������������������� */

static int http_request_reforward(HTTP_CLIENT *http_client)
{
	const char *myname = "http_request_reforward";
	ACL_ASTREAM *server = http_client->entry.server;

	/* ������Ǵ����ӳ���ȡ�õ����������Դ����� 1 */
	if (!http_client->entry.flag_conn_reuse)
		http_client->entry.nretry_on_error++;

	if (http_client->hdr_res) {
		http_hdr_res_free(http_client->hdr_res);
		http_client->hdr_res = NULL;
	}

	/* �Ͽ������˵����ӣ���������������˵����� */
	if (server) {
		/* ʹ��������ûỰ���� */
		client_entry_detach(&http_client->entry, acl_aio_vstream(server));
		/* ȡ�� HTTP ��Ӧͷ�Ļص����� */
		acl_aio_clean_hooks(server);
		/* only for test */
		if (acl_aio_iswset(server)) {
			acl_msg_info("%s(%d): defer free(%d)\n", myname, __LINE__, ACL_VSTREAM_SOCK(server->stream));
                } else
			acl_msg_info("%s(%d): not defer free(%d)\n", myname, __LINE__, ACL_VSTREAM_SOCK(server->stream));
		/* ���첽�رշ������ */
		acl_aio_iocp_close(server);
	}

	/* ������Դ�����������������Ӧ��IP���������򷵻ش��󣬲������� */
	if (http_client->entry.nretry_on_error > http_client->entry.dns_ctx.ip_cnt) {
		acl_msg_error("%s(%d): has retried before(%d,%d), reuse connecion %s",
			myname, __LINE__, http_client->entry.nretry_on_error,
			http_client->entry.dns_ctx.ip_cnt,
			http_client->entry.flag_conn_reuse ? "yes" : "no");
		return (-1);
	}

	/* ������Դ���������ֵ����������ֱ�ӷ��ش��� */
	if (http_client->entry.nretry_on_error >= MAX_RETRIED) {
		acl_msg_error("%s(%d): has retried before(%d,%d)",
			myname, __LINE__, http_client->entry.nretry_on_error,
			MAX_RETRIED);
		return (-1);
	}

	/* ��ʼ����������һ��IP */
	forward_start((CLIENT_ENTRY*) http_client);
	return (0);
}

/*----------------------------------------------------------------------------*/

static char HTTP_CONNECT_FIRST[] = "HTTP/1.0 200 Connection established\r\n\r\n";

/* ����������ر�ʱ�Ļص����� */

static int on_close_server(ACL_ASTREAM *server acl_unused, void *ctx)
{
	HTTP_CLIENT *http_client = (HTTP_CLIENT*) ctx;

	/* ȡ���������������״̬ */
	http_client->flag &= ~HTTP_FLAG_SERVER_LOCKED;
	http_client->flag |= HTTP_FLAG_SERVER_CLOSED | HTTP_FLAG_FINISH;
	http_proxy_complete(http_client, -1);
	/* ���뷵�� -1, ��Ϊ��ϣ���������������Ĺرջص����� */
	return (-1);
}

static int http_proxy_connect_complete(CLIENT_ENTRY *entry)
{
	HTTP_CLIENT *http_client = (HTTP_CLIENT*) entry;
	const char *method = http_client->req_curr->hdr_req->method;

	/* ��ӷ������ر�ʱ�Ļص����� */
	acl_aio_add_close_hook(http_client->entry.server,
		on_close_server, http_client);

	/* CONNECT ����, ת�� TCP ����ģʽ, �Ӷ�����֧�� SSL */
	if (var_cfg_http_method_connect_enable && strcasecmp(method, "CONNECT") == 0) {
		if (entry->client == NULL) {
			acl_msg_warn("%s(%d): client null", __FILE__, __LINE__);
			http_proxy_complete(http_client, -1);
			return (0);
		}

		acl_aio_writen(entry->client, HTTP_CONNECT_FIRST,
			(int) strlen(HTTP_CONNECT_FIRST));
		if (entry->client && entry->server) {
			tcp_start(entry);
			return (0);
		} else {
			http_proxy_complete(http_client, -1);
			return (0);
		}
	}

	if (strcasecmp(method, "GET") != 0 && strcasecmp(method, "POST") != 0) {
		if (entry->client == NULL) {
			acl_msg_error("%s(%d): client null", __FILE__, __LINE__);
		} else
			acl_aio_writen(entry->client, HTTP_REQUEST_INVALID,
				(int) strlen(HTTP_REQUEST_INVALID));
		acl_msg_error("%s(%d): method(%s) invalid",
			__FILE__, __LINE__, method);
		http_proxy_complete(http_client, -1);
		return (0); /* ���أ�1��ʹ�첽��ܹرո��첽�� */
	}

	/* ���� GET��POST ���� */
	start_forward_request(http_client);
	return (0);
}

static void http_proxy_connect_timeout(CLIENT_ENTRY *entry)
{
	const char *myname = "http_proxy_connect_timeout";

	if (entry->client == NULL) {
		acl_msg_error("%s(%d): client null", myname, __LINE__);
		http_proxy_complete((HTTP_CLIENT*) entry, -1);
	} else {
		acl_msg_error("%s(%d): connect(%s, %s) timeout",
			myname, __LINE__, entry->domain_key,
			entry->domain_addr);
		acl_aio_refer(entry->client);
		acl_aio_writen(entry->client, HTTP_CONNECT_TIMEOUT,
			(int) strlen(HTTP_CONNECT_TIMEOUT));
		acl_aio_unrefer(entry->client);
		http_proxy_complete((HTTP_CLIENT*) entry, -1);
	}
}

static void http_proxy_connect_error(CLIENT_ENTRY *entry)
{
	const char *myname = "http_proxy_connect_error";

	if (entry->client == NULL) {
		acl_msg_error("%s(%d): client null", myname, __LINE__);
		http_proxy_complete((HTTP_CLIENT*) entry, -1);
	} else {
		acl_msg_error("%s(%d): connect(%s, %s) error",
			myname, __LINE__, entry->domain_key,
			entry->domain_addr);
		acl_aio_refer(entry->client);
		acl_aio_writen(entry->client, HTTP_CONNECT_ERROR,
			(int) strlen(HTTP_CONNECT_ERROR));
		acl_aio_unrefer(entry->client);
		http_proxy_complete((HTTP_CLIENT*) entry, -1);
	}
}

static void nslookup_complete_fn(CLIENT_ENTRY *entry, int status)
{
	if (status == NSLOOKUP_OK) {
		/* �������ӳɹ���Ļص����� */
		entry->connect_notify_fn = http_proxy_connect_complete;
		entry->connect_timeout_fn = http_proxy_connect_timeout;
		entry->connect_error_fn = http_proxy_connect_error;
		forward_start(entry);
	} else {
		acl_aio_refer(entry->client);
		acl_aio_writen(entry->client, HTTP_REPLY_DNS_ERR,
			(int) strlen(HTTP_REPLY_DNS_ERR));
		acl_aio_unrefer(entry->client);
		http_proxy_complete((HTTP_CLIENT*) entry, -1);
	}
}

static void handle_one(HTTP_CLIENT *http_client, HTTP_CLIENT_REQ *req)
{
	int   ret;
	http_client->req_curr = req;  /* ���õ�ǰ���Դ�������� */

	/* �ȼ���û��Զ�������� */
	if ((ret = http_client_req_filter(http_client))) {
		/* ������ط�0ֵ���ʾ����������ӹ��˸����� */
		return;
	}

	/* ����DNS��ѯ�ص����� */
	http_client->entry.nslookup_notify_fn = nslookup_complete_fn;
	http_client->entry.dns_errmsg = NULL;

	/* �������������ͷ�л�ȡ����˵Ķ˿ں� */
	dns_lookup(&http_client->entry, req->hdr_req->host, req->hdr_req->port);
}

/**
 * �ɹ�����HTTP����ͷ��Ļص�����
 */
static int request_header_ready(int status, void *arg)
{
	const char *myname = "request_header_ready";
	HTTP_CLIENT_REQ *req = (HTTP_CLIENT_REQ *) arg;
	HTTP_CLIENT *http_client = req->http_client;
	const char *via;
	static char *via_static = NULL;
	static int   via_max = 256;

	/* ȡ���ͻ�����������״̬ */
	http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;

	if (status != HTTP_CHAT_OK) {
		http_proxy_complete(http_client, -1);
		return (0);
	}

	if (http_hdr_req_parse3(req->hdr_req, 0 , 0) < 0) {
		acl_msg_error("%s: parse hdr_req error", myname);
		http_proxy_complete(http_client, -1);
		return (0);
	}

#ifdef WIN32
#define snprintf _snprintf
#endif

	if (via_static == NULL) {
		via_static = (char*) acl_mycalloc(1, via_max);
		snprintf(via_static, via_max, "jaws-%d", getpid());
	}

	/* ����Ƿ������·���� */

	via = http_hdr_entry_value(&req->hdr_req->hdr, "x-via-jaws");
	if (via == NULL) {
		http_hdr_put_str(&req->hdr_req->hdr, "x-via-jaws", via_static);
	} else if (strcasecmp(via, via_static) == 0) {
		acl_msg_warn("%s(%d): loop tested, via(%s), url(http://%s%s)",
			myname, __LINE__, via, req->hdr_req->host,
			acl_vstring_str(req->hdr_req->url_part));

		/* �����ͻ����� */
		http_client->flag |= HTTP_FLAG_CLIENT_LOCKED;

		acl_aio_writen(http_client->entry.client,
			HTTP_REQUEST_LOOP,
			(int) strlen(HTTP_REQUEST_LOOP));

		/* ȡ���ͻ�����������״̬ */
		http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;
		http_proxy_complete(http_client, -1);
		return (0);
	}

	/* �������Ѿ���ɣ�ȡ����ȴ�״̬ */
	req->flag &= ~CLIENT_READ_WAIT;

	if (http_client->req_curr != NULL) {
		/* ���ǰһ������δ������ϣ��򷵻� */
		return (0);
	}

	/* ��ǰû�����ڴ����������̣����Կ�ʼ��������� */

	/* �Ӷ����е������������ⱻ�ظ�ʹ�� */
	if (acl_fifo_pop(&http_client->req_list) != req)
		acl_msg_fatal("%s(%d): request invalid", myname, __LINE__);

	handle_one(http_client, req);
	return (0);
}

static int http_proxy_next(HTTP_CLIENT *http_client)
{
	HTTP_CLIENT_REQ *req = acl_fifo_head(&http_client->req_list);

	if (req) {
		if (!(req->flag & CLIENT_READ_WAIT))
			handle_one(http_client, req);
	} else {
		http_proxy_req_get(http_client);
	}

	return (0);
}

/* ������Ӧ�����ͻ���ʧ��ʱ�Ļص����� */

static int on_close_client(ACL_ASTREAM *client acl_unused, void *ctx)
{
	HTTP_CLIENT *http_client = (HTTP_CLIENT*) ctx;

#if 0
	acl_msg_info("%s(%d): close client(%lx, fd=%d) now, server %s",
		__FUNCTION__, __LINE__, (long) client,
		ACL_VSTREAM_SOCK(client->stream),
		http_client->entry.server ? "not null" : "null");
#endif
	/* ȡ���ͻ�����������״̬ */
	http_client->flag &= ~HTTP_FLAG_CLIENT_LOCKED;
	/* ���ÿͻ����Ĺر�״̬ */
	http_client->flag |= HTTP_FLAG_CLIENT_CLOSED | HTTP_FLAG_FINISH;
	http_proxy_complete(http_client, -1);
	/* ���뷵�� -1, ��Ϊ��ϣ���������������Ĺرջص����� */
	return (-1);
}

static void http_proxy_req_get(HTTP_CLIENT *http_client)
{
	HTTP_CLIENT_REQ *req = http_client_req_new(http_client);

	req->flag |= CLIENT_READ_WAIT;
	req->hdr_req = http_hdr_req_new();
	acl_fifo_push(&http_client->req_list, req);

	/* ���ôӿͻ���������ʧ�ܻ������������ʱ�Ļص����� */
	acl_aio_add_close_hook(http_client->entry.client,
		on_close_client, http_client);

	/* �����ͻ����� */
	http_client->flag |= HTTP_FLAG_CLIENT_LOCKED;

	http_client->total_size = 0;
	http_client->flag &= ~HTTP_FLAG_CLIENT_KEEP_ALIVE;
	http_client->flag &= ~HTTP_FLAG_SERVER_KEEP_ALIVE;

	/* ��ʼ��ȡHTTP����ͷ */
	http_hdr_req_get_async(req->hdr_req,
		http_client->entry.client,
		request_header_ready,
		req,
		http_client->entry.service->rw_timeout);
}

int http_proxy_start(HTTP_CLIENT *http_client)
{
acl_msg_info("%s(%d)", __FUNCTION__, __LINE__); /* only for test */
	http_proxy_req_get(http_client);
	return (0);
}
