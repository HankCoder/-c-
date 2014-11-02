#ifndef	ACL_ACCESS_INCLUDE_H
#define	ACL_ACCESS_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_stdlib.h"

/**
 * ������б������һ������� ip ��ַ��
 * @param data ��� IP ��ַ�������ַ���. ��: 10.0.0.1:10.0.250.1, 192.168.0.1:192.168.0.255
 * @param sep1 ÿ�� IP ��ַ��֮��ķָ���, �������е� "," �ָ���
 * @param sep2 ÿ�� IP ��ַ�θߵ�ַ��͵�ַ֮��ķָ���, �������е� ":" �ָ���
 * @return ��ӡ����. 0: �ɹ�; < 0: ʧ��
 * ע: �ú������̲߳���ȫ��
 */
ACL_API int acl_access_add(const char *data, const char *sep1, const char *sep2);

/**
 * �������ļ��ж�ȡ IP ��ַ�ַ���, ���Զ����� IP ��ַ�����б�
 * @param xcp �Ѿ��ɹ������������ļ��Ľ�����
 * @param name xcp ���������� IP ��ַ������صı�����
 * @return �Ƿ���ӳɹ�. 0: �ɹ�; < 0: ʧ��.
 * ע: �ú������̲߳���ȫ��
 */
ACL_API int acl_access_cfg(ACL_XINETD_CFG_PARSER *xcp, const char *name);

/**
 * �û����������Լ�����־��¼����, ��������ô˺���, �򱾿��Զ�ʹ�� aclMsg.c�еĿ�.
 * @param log_fn �û��Լ�����־��¼����.
 * ע: �ú������̲߳���ȫ��
 */
ACL_API void acl_access_setup_logfn(void (*log_fn)(const char *fmt, ...));

/**
 * �ж����� IP ��ַ�Ƿ�������ķ��� IP ��ַ�б���.
 * @param ip ��ʽ: 192.168.0.1
 * @return �Ƿ�������ķ����б���, != 0: ��; == 0: ����.
 */
ACL_API int acl_access_permit(const char *ip);

/**
 * �����ʵ�ַ����ӡ����.
 */
ACL_API void acl_access_debug(void);

#ifdef	__cplusplus
}
#endif

#endif
