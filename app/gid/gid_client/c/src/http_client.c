#include "lib_acl.h"
#include "lib_protocol.h"

#include "lib_gid.h"
#include "global.h"
#include "http_client.h"

int http_client_post_request(ACL_VSTREAM *client, const char *url, int keepalive,
	const char *gid_fmt, char* body, int len, int *errnum)
{
	HTTP_HDR_REQ* hdr_req = http_hdr_req_create(url, "POST", "HTTP/1.1");
	ACL_VSTRING* buf = acl_vstring_alloc(256);
	struct iovec vector[2];  /* �������� */
	int   ret;

	/* ������ͷ����������������ݳ��� */
	http_hdr_put_int(&hdr_req->hdr, "Content-Length", len);
	if (keepalive)
		http_hdr_put_str(&hdr_req->hdr, "Connection", "keep-alive");
	if (gid_fmt && *gid_fmt)
		http_hdr_put_str(&hdr_req->hdr, "x-gid-format", gid_fmt);
	http_hdr_build_request(hdr_req, buf);  /* ���� HTTP ����ͷ���� */

	/* ���� HTTP ͷ */
	vector[0].iov_base = acl_vstring_str(buf);
	vector[0].iov_len = ACL_VSTRING_LEN(buf);

	/* ���� HTTP �� */
	vector[1].iov_base = (char*) body;
	vector[1].iov_len = len;

	ret = acl_vstream_writevn(client, vector, 2);  /* ��������ͷ�������� */
	http_hdr_req_free(hdr_req);  /* �ͷ� HTTP ����ͷ���� */
	acl_vstring_free(buf);  /* �ͷŻ����� */

	if (ret == ACL_VSTREAM_EOF) {
		if (errnum)
			*errnum = GID_ERR_IO;
		return -1;
	}

	/* ����HTTP����ɹ� */
	return (0);
}

int http_client_get_respond(ACL_VSTREAM* client, ACL_JSON *json,
	ACL_XML *xml, int *errnum, ACL_VSTRING *dump)
{
	HTTP_HDR_RES* hdr_res;
	HTTP_RES* res;
	char  buf[1024];
	int   ret;

	acl_assert(json != NULL || xml != NULL);

	hdr_res = http_hdr_res_new();  /* ����HTTP ��Ӧͷ���� */
	/* ��ȡ HTTP ��������Ӧͷ */
	ret = http_hdr_res_get_sync(hdr_res, client, var_gid_rw_timeout);
	if (ret < 0) {
		http_hdr_res_free(hdr_res); /* �ͷ� HTTP ��Ӧͷ���� */
		return -1;
	}
	/* ���� HTTP ��Ӧͷ */
	if (http_hdr_res_parse(hdr_res) < 0) {  /* �������� */
		http_hdr_res_free(hdr_res);
		return -1;
	}

	/*
	http_hdr_print(&hdr_res->hdr, "---respond---");
	*/

	/* ��Ҫ�ȸ��� HTTP ��Ӧͷ�ж��Ƿ��������� */
	if (hdr_res->hdr.content_length == 0  ||
		(hdr_res->hdr.content_length == -1 && !hdr_res->hdr.chunked  
		&& hdr_res->reply_status > 300  && hdr_res->reply_status < 400)) 
	{
		http_hdr_res_free(hdr_res);
		return 0;  
	}
	res = http_res_new(hdr_res);   /* ���� HTTP ��Ӧ����� */
	while (1) {
		/* ������������ */
		ret = http_res_body_get_sync(res, client, buf, sizeof(buf) - 1);
		if (ret <= 0)
			break;
		buf[ret] = 0;
		if (json)
			acl_json_update(json, buf);
		else
			acl_xml_update(xml, buf);
		if (dump)
			acl_vstring_strcat(dump, buf);
	}

	/* ��Ϊ res �к��� hdr_res ���Ի�һͬ�� hdr_res �ͷ� */
	http_res_free(res);
	return (0);
}
