#ifndef	ACL_SANE_BASENAME_INCLUDE_H
#define	ACL_SANE_BASENAME_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "acl_vstring.h"

/**
 * ���ļ�ȫ·������ȡ�ļ���
 * @parm bp {ACL_VSTRING*} �洢����Ļ���������Ϊ NULL �������ڲ����ֲ߳̾��洢
 * @return {char*} ����Ϊ��
 */
ACL_API char *acl_sane_basename(ACL_VSTRING *bp, const char *path);

/**
 * ���ļ�ȫ·������ȡ�ļ�����Ŀ¼
 * @parm bp {ACL_VSTRING*} �洢����Ļ���������Ϊ NULL �������ڲ����ֲ߳̾��洢
 * @return {char*} ����Ϊ��
 */
ACL_API char *acl_sane_dirname(ACL_VSTRING *bp, const char *path);

#ifdef	__cplusplus
}
#endif

#endif
