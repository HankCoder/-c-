#ifndef	__HTTP_CLIENT_INCLUDE_H__
#define	__HTTP_CLIENT_INCLUDE_H__

/**
 * ��������� POST ��ʽ��������
 * @param client {ACL_VSTREAM*} ������
 * @param url {const char*} URL �ַ���
 * @param keepalive {int} �Ƿ������˱��ֳ�����
 * @param gid_fmt {const char*} ���ݸ�ʽ��xml �� json
 * @param body {char*} �������ַ
 * @param len {int} �����峤��
 * @param errnum {int*} ���ǿ����¼����ʱ��ԭ��
 * @return {int} 0 ��ʾ�ɹ��������ʾʧ��
 */
int http_client_post_request(ACL_VSTREAM *client, const char *url, int keepalive,
	const char *gid_fmt, char* body, int len, int *errnum);

/**
 * �ӷ�������ȡ��Ӧ����
 * @param client {ACL_VSTREAM*} ������
 * @param json {ACL_JSON*} ���ǿգ������ json ��ʽ���н���
 * @param xml {ACL_XML*} ���ǿգ������ xml ��ʽ���н���
 * @param errnum {int*} ���ǿ����¼����ʱ��ԭ��
 * @param dump {ACL_VSTRING*} �ǿ���洢��Ӧ����
 * @return {int} 0 ��ʾ�ɹ��������ʾʧ��
 * ע��ACL_JSON* �� ACL_XML* ��������ֻ��һ���ǿ�
 */
int http_client_get_respond(ACL_VSTREAM* client, ACL_JSON *json,
	ACL_XML *xml, int *errnum, ACL_VSTRING *dump);

#endif
