#ifndef ACL_SET_EUGID_INCLUDE_H
#define ACL_SET_EUGID_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "../acl_define.h"
#ifdef ACL_UNIX

#include <sys/types.h>
#include <unistd.h>

/**
 * ���ó������Ч�û� ID ����Ч�� ID
 * @param euid {uid_t} ��Ч�û� ID
 * @param egid {gid_t} ��Ч�� ID
 * @return {int} 0 ��ʾ���óɹ���-1 ��ʾ����ʧ��
 */
int acl_set_eugid(uid_t euid, gid_t egid);

#endif /* ACL_UNIX */

#ifdef  __cplusplus
}
#endif


#endif

