#ifndef	__PROTO_CMD_INCLUDE_H__
#define	__PROTO_CMD_INCLUDE_H__

#include "lib_acl.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * �����з�ʽ��Э�鴦��ʽ
 * @param client {ACL_VSTREAM*} �ͻ�����
 * @return {int} 0����ʾ������1����ʾ�����ұ��ֳ����ӣ�-1����ʾ����
 */
int cmdline_service(ACL_VSTREAM *client);

#ifdef	__cplusplus
}
#endif

#endif
