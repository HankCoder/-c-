#ifndef	__HTTP_SERVICE_INCLUDE_H__
#define	__HTTP_SERVICE_INCLUDE_H__

#include "lib_acl.h"
#include "lib_protocol.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* in http_service.c */

/**
 * HTTP Э�鷽ʽ������
 * @param client {ACL_VSTREAM*} �ͻ�����
 * @return {int} 0����ʾ������1����ʾ�����ұ��ֳ����ӣ�-1����ʾ����
 */
int http_service(ACL_VSTREAM *client);

/**
 * ����˷��� HTTP ��Ӧ���ͻ���
 * @param client {ACL_VSTREAM*} �ͻ�����
 * @param status {int} HTTP ��Ӧ״̬�룬1xx, 2xx, 3xx, 4xx, 5xx
 * @param keep_alive {int} �Ƿ���ͻ��˱��ֳ�����
 * @param body {const char*} ����������
 * @param len {int} �����峤��
 */
int http_server_send_respond(ACL_VSTREAM* client, int status,
	int keep_alive, char* body, int len);

/* in http_json.c */

/**
 * �������ݵĸ�ʽΪ JSON ��ʽ�Ĵ���
 * @param client {ACL_VSTREAM*}
 * @param hdr_req {HTTP_HDR_REQ*} HTTP ����Э��ͷ����
 * @param json {ACL_JSON*} json ����������
 * @return {int} 0����ʾ������1����ʾ�����ұ��ֳ����ӣ�-1����ʾ����
 */
int http_json_service(ACL_VSTREAM *client,
	HTTP_HDR_REQ *hdr_req, ACL_JSON *json);

/* in http_xml.c */

/**
 * �������ݵĸ�ʽΪ XML ��ʽ�Ĵ���
 * @param client {ACL_VSTREAM*}
 * @param hdr_req {HTTP_HDR_REQ*} HTTP ����Э��ͷ����
 * @param xml {ACL_XML*} xml ����������
 * @return {int} 0����ʾ������1����ʾ�����ұ��ֳ����ӣ�-1����ʾ����
 */
int http_xml_service(ACL_VSTREAM *client,
        HTTP_HDR_REQ *hdr_req, ACL_XML *xml);

#ifdef	__cplusplus
}
#endif

#endif
