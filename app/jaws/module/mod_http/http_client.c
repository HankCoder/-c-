#include "lib_acl.h"
#include "lib_protocol.h"
#include "service.h"
#include "http_service.h"

void http_client_free(CLIENT_ENTRY *entry)
{
	HTTP_CLIENT *client = (HTTP_CLIENT*) entry;

	while (1) {
		HTTP_CLIENT_REQ *req =(HTTP_CLIENT_REQ*)
			acl_fifo_pop(&client->req_list);
		if (req == NULL)
			break;
		http_client_req_free(req);
	}

	if (client->req_curr) {
		http_client_req_free(client->req_curr);
		client->req_curr = NULL;
	}

	if (client->res) {
		http_res_free(client->res);
		client->res = NULL;
		client->hdr_res = NULL;
	} else if (client->hdr_res) {
		http_hdr_res_free(client->hdr_res);
		client->hdr_res = NULL;
	}
	if (client->fp) {
		acl_vstream_close(client->fp);
		client->fp = NULL;
	}
	if (client->buf) {
		acl_vstring_free(client->buf);
		client->buf = NULL;
	}

	client_entry_free(entry);
}

HTTP_CLIENT *http_client_new(HTTP_SERVICE *service, ACL_ASTREAM *stream)
{
	HTTP_CLIENT *client;

	client = (HTTP_CLIENT*) client_entry_new((SERVICE*) service,
			sizeof(HTTP_CLIENT), stream);
	client->req_curr = NULL;
	acl_fifo_init(&client->req_list);
	client->entry.free_fn = http_client_free;
	client->flag = 0;

	return (client);
}

void http_client_reset(HTTP_CLIENT *client)
{
	while (1) {
		HTTP_CLIENT_REQ *req =(HTTP_CLIENT_REQ*)
			acl_fifo_pop(&client->req_list);
		if (req == NULL)
			break;
		http_client_req_free(req);
	}
	client->req_curr = NULL;

	if (client->hdr_res) {
		http_hdr_res_free(client->hdr_res);
		client->hdr_res = NULL;
	}
	if (client->res) {
		client->res->hdr_res = NULL;
		http_res_free(client->res);
		client->res = NULL;
	}
	if (client->fp) {
		acl_vstream_close(client->fp);
		client->fp = NULL;
	}
	if (client->buf)
		ACL_VSTRING_RESET(client->buf);
	if (client->cache)
		client->cache = NULL;
	client->flag = 0;
}

HTTP_CLIENT_REQ *http_client_req_new(HTTP_CLIENT *http_client)
{
	HTTP_CLIENT_REQ *req = (HTTP_CLIENT_REQ*)
		acl_mycalloc(1, sizeof(HTTP_CLIENT_REQ));
	req->http_client = http_client;
	return (req);
}

void http_client_req_free(HTTP_CLIENT_REQ *req)
{
	if (req->req)
		http_req_free(req->req);
	else if (req->hdr_req)
		http_hdr_req_free(req->hdr_req);
	acl_myfree(req);
}

int http_client_req_filter(HTTP_CLIENT *http_client)
{
	HTTP_SERVICE *service = (HTTP_SERVICE*) http_client->entry.service;
	HTTP_HDR_REQ *hdr_req;
	HTTP_PLUGIN *plugin = NULL;
	void *plugin_req_ctx;
	ACL_ASTREAM *astream;
	ACL_VSTREAM *stream;
	ACL_ITER iter;
	int   ret = 0;

	/* xxx: plugin_req_ctx �ò�����ÿ�������п��ܲ�һ��, ���ģ��Ӧ�����й��� */
	http_client->plugin_req_ctx = NULL;

	/* ����û��Զ�������� */

	acl_foreach(iter, &service->request_plugins) {
		HTTP_PLUGIN *tmp = (HTTP_PLUGIN*) iter.data;
		astream  = http_client->entry.client;
		stream = acl_aio_vstream(astream);

		if ((ret = tmp->filter.request(stream,
			http_client->req_curr->hdr_req,
			&http_client->plugin_req_ctx)) != 0)
		{
			plugin = tmp;
			break;
		}
	}

	/* ���� 0 ��ʾ�û��Զ����������׼���ӹܸ����� */
	if (plugin == NULL || ret == 0)
		return (0);

	astream  = http_client->entry.client;
	stream = acl_aio_vstream(astream);

	if (ret < 0) {
		/* ���ظ�ֵ��ʾ��ֹ�����󣬿���Ϊ��-4xx, -5xx */

		acl_aio_refer(astream);

		switch (-ret) {
		case 403:
			acl_aio_writen(astream, HTTP_REQUEST_DENY,
				(int) strlen(HTTP_REQUEST_DENY));
			break;
		case 404:
			acl_aio_writen(astream, HTTP_REQUEST_NOFOUND,
				(int) strlen(HTTP_REQUEST_NOFOUND));
			break;
		case 500:
		default:
			acl_aio_writen(astream, HTTP_INTERNAL_ERROR,
				(int) strlen(HTTP_INTERNAL_ERROR));
			break;
		}

		acl_aio_unrefer(astream);

		http_client_req_free(http_client->req_curr);
		http_client->req_curr = NULL;
		client_entry_detach(&http_client->entry, stream);
		acl_aio_disable_readwrite(astream);
		acl_aio_clean_hooks(astream);
		acl_aio_iocp_close(astream);
		return (-1);
	}

	hdr_req = http_client->req_curr->hdr_req;
	http_client->req_curr->hdr_req = NULL;

	plugin_req_ctx = http_client->plugin_req_ctx;

	/* �ͷŵ��첽����صĶ���, ���ͻ�����������ô���������,
	 * ��Ϊ��ʱΪ����ʱ�Ĺ�����������ֻ�������û����Ӧ�ˣ���
	 * ���� client_entry_detach �����Ϊ���ü���Ϊ0���Զ���
	 * http_client �ͷ�
	 */
	client_entry_detach(&http_client->entry, stream);

	/* �رն������¼� */
	acl_aio_disable_read(astream);
	/* ����ص����� */
	acl_aio_clean_hooks(astream);

	/* ���ͻ����첽�����������ÿ� */
	acl_aio_ctl(astream, ACL_AIO_CTL_STREAM, NULL, ACL_AIO_CTL_END);

	/* xxx: �첽�ر� astream �첽�� */
	acl_aio_iocp_close(astream);

	/* �������ɷ�����ģʽת��Ϊ����ģʽ */
	acl_non_blocking(ACL_VSTREAM_SOCK(stream), ACL_BLOCKING);

	/* ת���û��Զ��崦����� */
	plugin->forward.request(stream, hdr_req, plugin_req_ctx);
	return (1);
}

