#include "lib_acl.h"
#include "lib_protocol.h"

#include "gid_oper.h"
#include "global.h"
#include "http_service.h"

static int xml_new_gid(ACL_VSTREAM *client, int keep_alive, ACL_XML_NODE *node)
{
	acl_int64 gid;
	char  buf[256], tag[64];
	const char *ptr;
	int   errnum = 0;

#define	STR	acl_vstring_str

	ACL_SAFE_STRNCPY(tag, "default:", sizeof(tag));
	ptr = acl_xml_getElementAttrVal(node, "tag");
	if (ptr && *ptr) {
		ACL_SAFE_STRNCPY(tag, ptr, sizeof(tag));
	}

	gid = gid_next(var_cfg_gid_path, tag, var_cfg_gid_step, &errnum);
	if (gid >= 0)
		snprintf(buf, sizeof(buf),
			"<respond status='ok' gid='%lld' tag='%s' />\r\n", gid, tag);
	else
		snprintf(buf, sizeof(buf),
			"<respond status='error' gid='%lld' tag='%s'"
			" err='%d' msg='%s' />\r\n",
			gid, tag, errnum, gid_serror(errnum));

	return (http_server_send_respond(client, 200, keep_alive,
			buf, (int) strlen(buf)));
}

/*--------------------------------------------------------------------------*/

typedef struct PROTO_XML {
	/* ������ */
	const char *cmd;

	/* Э�鴦������� */
	int (*handle)(ACL_VSTREAM *client, int keep_alive, ACL_XML_NODE*);
} PROTO_XML;

/* Э���������ӳ��� */
static PROTO_XML __proto_xml_tab[] = {
	{ CMD_NEW_GID, xml_new_gid },
	{ NULL, NULL },
};

/* ���� xml ���ݸ�ʽ������ */

int http_xml_service(ACL_VSTREAM *client,
	HTTP_HDR_REQ *hdr_req, ACL_XML *xml)
{
	ACL_ARRAY *a;
	ACL_ITER iter;
	char  cmd[128];
	const char *ptr;
	ACL_XML_NODE *node;
	int   ret, i, keep_alive = 0;

	/* xml ���ݸ�ʽҪ��: <request cmd='xxx' tag='xxx:sid' /> */

	/* ��� cmd ������ */

	a = acl_xml_getElementsByTagName(xml, "request");
	if (a == NULL) {
		acl_msg_error("%s(%d), %s: xml error",
			__FILE__, __LINE__, __FUNCTION__);
		return (-1);
	}

	/* �ҵ���һ����㼴�� */

	node = NULL;
	acl_foreach(iter, a) {
		node = (ACL_XML_NODE*) iter.data;
		break;
	}

	/* �� xml ������������ */

	ptr = acl_xml_getElementAttrVal(node, "cmd");
	if (ptr == NULL || *ptr == 0) {
		acl_msg_error("%s(%d), %s: no cmd attr",
			__FILE__, __LINE__, __FUNCTION__);
		acl_xml_free_array(a);
		return (-1);
	}

	ACL_SAFE_STRNCPY(cmd, ptr, sizeof(cmd));
	if (cmd[0] == 0) {
		acl_xml_free_array(a);
		acl_msg_error("%s(%d), %s: no cmd",
			__FILE__, __LINE__, __FUNCTION__);
		return (-1);
	}

	/* �ͻ����Ƿ�Ҫ�󱣳ֳ����� */
	keep_alive = hdr_req->hdr.keep_alive;

	/* ��ѯ��Ӧ����Ĵ��������� */
	ret = -1;
	for (i = 0; __proto_xml_tab[i].cmd != NULL; i++) {
		if (strcasecmp(cmd, __proto_xml_tab[i].cmd) == 0) {
			ret = __proto_xml_tab[i].handle(
					client, keep_alive, node);
			break;
		}
	}

	/* �������ڲ��� node ʱ�ſ����ͷŸ�������� */
	acl_xml_free_array(a);

	if (__proto_xml_tab[i].cmd == NULL)
		acl_msg_error("%s(%d), %s: cmd(%s) invalid",
			__FILE__, __LINE__, __FUNCTION__, cmd);

	if (ret < 0)
		return (-1);  /* ���� */
	else if (keep_alive)
		return (1);  /* ��������Ҫ���ֳ����� */
	else
		return (0);  /* �������Ƕ����� */
}
