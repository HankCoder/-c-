#ifndef	__HTTP_MODULE_INCLUDE_H__
#define	__HTTP_MODULE_INCLUDE_H__

#include "lib_acl.h"
#include "service_struct.h"

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _USRDLL
# ifdef MOD_HTTP_EXPORTS
#  define MOD_HTTP_API __declspec(dllexport)
# else
#  define MOD_HTTP_API __declspec(dllimport)
# endif
#else
#  define MOD_HTTP_API
#endif

extern ACL_DLL_ENV *var_http_dll_env;
extern char *var_cfg_http_domain_allow;
extern int   var_cfg_http_client_keepalive;
extern int   var_cfg_http_server_keepalive;
extern int   var_cfg_http_domain_allow_all;
extern int   var_cfg_http_method_connect_enable;
extern int   var_cfg_http_proxy_connection_off;

/* ��̬���صĺ����ӿ� */

/**
 * ����̬���ص�ģ��ĳ�ʼ����������������һ��
 * @param dll_env {ACL_DLL_ENV*} �ɼ��س��򴫵ݹ����Ļ�������
 * @param cfg_dir {const char*} �ö�̬ģ��������ļ�����·��
 */
MOD_HTTP_API void module_service_init(ACL_DLL_ENV *dll_env, const char *cfg_dir);

/**
 * �ɶ�̬ģ�鴴��һ������ʵ�������Ա����س�����ö���Բ����������ʵ��
 * @return {SERVICE*} �ɶ�̬ģ�鴴���ķ������ʵ��
 */
MOD_HTTP_API SERVICE *module_service_create(void);

/**
 * �����س�����յ�һ���ͻ������Ӻ���ô˺���
 * @param service {SERVICE*} �� module_service_create �����ķ������
 * @param stream {ACL_ASTREAM*} �ɼ��س�����յĿͻ����첽������
 */
MOD_HTTP_API void module_service_main(SERVICE *service, ACL_ASTREAM *stream);

#ifdef	__cplusplus
}
#endif

#endif
