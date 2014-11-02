#include "lib_acl.h"
#include "lib_protocol.h"

#include "global.h"
#include "http_service.h"

/* �����ݷ��ظ� HTTP �ͻ��� */

int http_server_send_respond(ACL_VSTREAM* client, int status,
	int keep_alive, char* body, int len)
{
	int   ret;
	struct iovec vector[2];  /* �������� */
	ACL_VSTRING* buf = acl_vstring_alloc(256);
	HTTP_HDR_RES* hdr_res = http_hdr_res_static(status);

	/* ������ͷ����������������ݳ��� */
	http_hdr_put_int(&hdr_res->hdr, "Content-Length", len);
	/* ���ó�����ѡ�� */
	http_hdr_put_str(&hdr_res->hdr, "Connection",
		keep_alive ? "keep-alive" : "close");

	/* ���� HTTP ��Ӧͷ���� */
	http_hdr_build(&hdr_res->hdr, buf);

	/* ���� HTTP ͷ */
	vector[0].iov_base = acl_vstring_str(buf);
	vector[0].iov_len = ACL_VSTRING_LEN(buf);
	/* ���� HTTP �� */
	vector[1].iov_base = body;
	vector[1].iov_len = len;

	/* ������Ӧͷ����Ӧ�� */
	ret = acl_vstream_writevn(client, vector, 2);
	/* �ͷ� HTTP ��Ӧͷ���� */
	http_hdr_res_free(hdr_res);
	/* �ͷŻ����� */
	acl_vstring_free(buf);

	if (ret == ACL_VSTREAM_EOF)
		return -1;

	/* ����HTTP��Ӧ�ɹ� */
	return 0;
}

/* HTTP Э�鴦�������� */

int http_service(ACL_VSTREAM *client)
{
	HTTP_HDR_REQ *hdr_req = http_hdr_req_new();
	HTTP_REQ *req;
	char  buf[4096];
	int   ret, json_fmt;
	const char *ptr;

	/* ��ȡHTTP����ͷ */
	ret = http_hdr_req_get_sync(hdr_req, client, var_cfg_io_timeout);
	if (ret < 0) {
		http_hdr_req_free(hdr_req);
		return (-1);
	}

	/* ����HTTP����ͷ */
	if (http_hdr_req_parse(hdr_req) < 0) {
		http_hdr_req_free(hdr_req);
		acl_msg_error("%s(%d), %s: http request header invalid",
			__FILE__, __LINE__, __FUNCTION__);
		return (-1);
	}

	/* ���뱣֤�����峤�� > 0  */
	if (hdr_req->hdr.content_length <= 0) {
		http_hdr_req_free(hdr_req);
		acl_msg_error("%s(%d), %s: http request header invalid",
			__FILE__, __LINE__, __FUNCTION__);
		return (-1);
	}

	/* �� HTTP ����ͷ�л�ȡ�������е����ݸ�ʽ��XML �� JSON ��ʽ */
	ptr = http_hdr_entry_value(&hdr_req->hdr, "x-gid-format");
	if (ptr != NULL && strcasecmp(ptr, "xml") == 0)
		json_fmt = 0;
	else
		json_fmt = 1;

	req = http_req_new(hdr_req);  /* ����HTTP������� */

	if (json_fmt) {
		ACL_JSON *json = acl_json_alloc();  /* ����JSON���������� */

		/* ���ϴӿͻ��������ж�ȡ���ݣ�������JSON�������н��н��� */
		while (1) {
			ret = http_req_body_get_sync(req, client,
					buf, sizeof(buf) - 1);
			if (ret < 0) {
				/* ˵��û�ж�����Ҫ������ݳ��ȣ��������� */
				http_req_free(req);
				acl_json_free(json);
				return (-1);
			} else if (ret == 0)  /* �����Ѿ����������е����� */
				break;
			buf[ret] = 0;
			acl_json_update(json, buf);
		}

		ret = http_json_service(client, hdr_req, json);
		acl_json_free(json);
	} else {
		ACL_XML *xml = acl_xml_alloc();  /* ����XML���������� */

		while (1) {
			ret = http_req_body_get_sync(req, client,
					buf, sizeof(buf) - 1);
			if (ret < 0) {
				http_req_free(req);
				acl_xml_free(xml);
				return (-1);
			} else if (ret == 0)
				break;
			buf[ret] = 0;
			acl_xml_update(xml, buf);
		}

		ret = http_xml_service(client, hdr_req, xml);
		acl_xml_free(xml);
	}

	http_req_free(req);
	return (ret);
}
