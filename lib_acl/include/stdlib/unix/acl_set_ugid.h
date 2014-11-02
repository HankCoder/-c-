#ifndef ACL_SET_UGID_INCLUDE_H
#define ACL_SET_UGID_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "../acl_define.h"
#ifdef ACL_UNIX

#include <sys/types.h>
#include <unistd.h>

/**
 * ���ó�����û� ID ���� ID
 * @param uid {uid_t} �û� ID
 * @param gid {gid_t} �� ID
 * @return {int} �����Ƿ�ɹ���0 ��ʾ�ɹ���-1 ��ʾʧ��
 */
int acl_set_ugid(uid_t uid, gid_t gid);

/**
 * �޸ĳ�����û� ID Ϊָ���û��� ID
 * @param user {char* } ϵͳ�û��˺���
 * @return {int} �����Ƿ�ɹ���0 ��ʾ�ɹ���-1 ��ʾʧ��
 */
int acl_change_uid(const char *user);

#endif /* ACL_UNIX*/

#ifdef  __cplusplus
}
#endif


#endif

