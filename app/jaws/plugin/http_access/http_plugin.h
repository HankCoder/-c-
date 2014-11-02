#ifndef	__HTTP_PLUGIN_INCLUDE_H__
#define	__HTTP_PLUGIN_INCLUDE_H__

#include "lib_acl.h"
#include "lib_protocol.h"

#ifdef	__cplusplus
extern "C" {
#endif

/*------------------------- ��̬ģ����Ҫ�ĺ����ӿ� ---------------------------*/

/**
 * HTTP ��ʼ��ע�ắ��, �ú�������ǿգ������������ڸտ�ʼ����ʱ����
 * @param dll_env {ACL_DLL_ENV*} DLL ��������, dll_env->logfp ��־�ļ����,
 *  �����ϣ������־������������־�ļ��У�����Խ���ֵ�ÿ�
 * @param dmptr {ACL_DEBUG_MEM*} �����ڴ���Եľ��
 * @param cfg_dir {const char*} �����ļ�����·������·���´�����ж�̬��������ļ�
 */
void http_plugin_init(ACL_DLL_ENV *dll_env, const char *cfg_dir);

/**
 * HTTP �������ע�ắ�����ú�������ǿգ��������������յ�HTTP����ͷʱ����
 * @param client {ACL_VSTREAM*} �ͻ�����
 * @param hdr_req {HTTP_HDR_REQ*} HTTP����ͷ
 * @param ctx_ptr {void**} ��ָ����������洢�û��Ķ�̬����
 * @return {int} < 0: ��ʾ��ֹ��HTTP����, ��ֵ��ʾ����ţ�����Ϊ��-5xx/-4xx;
 *  0: ��ʾ��HTTP�����������������; > 0: ��ʾ��HTTP������������ģ��ĵ���
 *  �̴߳���
 */
int http_request_filter(ACL_VSTREAM *client, HTTP_HDR_REQ *hdr_req, void **ctx_ptr);

/**
 * HTTP ����ӹܴ���ע�ắ�����ú�������ǿգ��� http_request_filter() > 0������
 * ��������ô˺�����ȫ�ӹܸ�HTTP������Ӧ���������ٴ����HTTP����HTTP��Ӧ
 * @param client {ACL_VSTREAM*} HTTP�ͻ�������������
 * @param hdr_req {HTTP_HDR_REQ*} �ͻ���HTTP����ͷ
 * @param ctx {void*} �������Ĳ������ö�̬�������� http_request_filter �е� ctx_ptr
 *  �������ص�
 */
void http_request_forward(ACL_VSTREAM *client, HTTP_HDR_REQ *hdr_req, void *ctx);

/*------------------------------ ���������ӿ� --------------------------------*/

#define	DEBUG_BASE	500
#define	DBG_REQ		(DEBUG_BASE + 1)
#define	DBG_RES		(DEBUG_BASE + 2)

#define	STR	acl_vstring_str
#define	LEN	ACL_VSTRING_LEN

extern char *var_cfg_log_name;
extern char *var_cfg_http_domain_allow;
extern char *var_cfg_http_domain_redirect;
extern int   var_cfg_http_domain_allow_all;

#ifdef	__cplusplus
}
#endif

#endif
