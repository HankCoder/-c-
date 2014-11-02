#include "lib_acl.h"
#include "lib_protocol.h"
#include "gid_oper.h"
#include "global.h"
#include "http_service.h"

static int json_new_gid(ACL_VSTREAM *client, int keep_alive, ACL_JSON *json)
{
	acl_int64 gid;
	char  buf[256], tag[64];
	int   errnum;
	ACL_ARRAY *a;

#define	STR	acl_vstring_str

	ACL_SAFE_STRNCPY(tag, "default:", sizeof(tag));
	a = acl_json_getElementsByTagName(json, "tag");
	if (a != NULL) {
		ACL_ITER iter;
		acl_foreach(iter, a) {
			ACL_JSON_NODE *node = (ACL_JSON_NODE*) iter.data;
			if (ACL_VSTRING_LEN(node->text) == 0)
				continue;
			ACL_SAFE_STRNCPY(tag, STR(node->text), sizeof(tag));
			break;
		}
		acl_json_free_array(a);
	}

	errnum = 0;
	gid = gid_next(var_cfg_gid_path, tag, var_cfg_gid_step, &errnum);
	if (gid >= 0)
		snprintf(buf, sizeof(buf),
			"{ status: 'ok', gid: '%lld', tag: '%s' }\r\n",
			gid, tag);
	else
		snprintf(buf, sizeof(buf), "{ status: 'error',"
			" gid: '%lld', tag: '%s', err: '%d', msg: '%s' }\r\n",
			gid, tag, errnum, gid_serror(errnum));

	return (http_server_send_respond(client, 200, keep_alive,
			buf, (int) strlen(buf)));
}

/*--------------------------------------------------------------------------*/

typedef struct PROTO_JSON {
	/* ������ */
	const char *cmd;

	/* Э�鴦������� */
	int (*handle)(ACL_VSTREAM *client, int keep_alive, ACL_JSON*);
} PROTO_JSON;

/* Э���������ӳ��� */
static PROTO_JSON __proto_json_tab[] = {
	{ CMD_NEW_GID, json_new_gid },
	{ NULL, NULL },
};

/* ���� json ���ݸ�ʽ������ */

int http_json_service(ACL_VSTREAM *client,
	HTTP_HDR_REQ *hdr_req, ACL_JSON *json)
{
	ACL_ARRAY *a;
	ACL_ITER iter;
	char  cmd[128];
	int   ret, i, keep_alive = 0;

	/* json ���ݸ�ʽҪ��: { cmd: xxx, tag: xxx:sid } */

	/* ��� cmd ������ */

	a = acl_json_getElementsByTagName(json, "cmd");
	if (a == NULL) {
		acl_msg_error("%s(%d), %s: json error",
			__FILE__, __LINE__, __FUNCTION__);
		return (-1);
	}

	/* �� JSON ������������ */
	cmd[0] = 0;
	acl_foreach(iter, a) {
		ACL_JSON_NODE *node = (ACL_JSON_NODE*) iter.data;
		if (ACL_VSTRING_LEN(node->text) == 0)
			continue;
		ACL_SAFE_STRNCPY(cmd, acl_vstring_str(node->text), sizeof(cmd));
		break;
	}
	acl_json_free_array(a);

	if (cmd[0] == 0) {
		acl_msg_error("%s(%d), %s: no cmd",
			__FILE__, __LINE__, __FUNCTION__);
		return (-1);
	}

	/* �ͻ����Ƿ�Ҫ�󱣳ֳ����� */
	keep_alive = hdr_req->hdr.keep_alive;

	/* ��ѯ��Ӧ����Ĵ��������� */
	ret = -1;
	for (i = 0; __proto_json_tab[i].cmd != NULL; i++) {
		if (strcasecmp(cmd, __proto_json_tab[i].cmd) == 0) {
			ret = __proto_json_tab[i].handle(
					client, keep_alive, json);
			break;
		}
	}
	if (__proto_json_tab[i].cmd == NULL)
		acl_msg_error("%s(%d), %s: cmd(%s) invalid",
			__FILE__, __LINE__, __FUNCTION__, cmd);

	if (ret < 0)
		return (-1);  /* ���� */
	else if (keep_alive)
		return (1);  /* ��������Ҫ���ֳ����� */
	else
		return (0);  /* �������Ƕ����� */
}
