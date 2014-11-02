#ifndef ACL_FLOCK_INCLUDE_H
#define ACL_FLOCK_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"

/**
 * �Դ򿪵��ļ�������м���
 * @param fd {ACL_FILE_HANDLE} �ļ����
 * @param lock_style {int} ϵͳ�ṩ��API�������ͣ�����UNIX��Ч��
 *  ACL_FLOCK_STYLE_FLOCK or ACL_FLOCK_STYLE_FCNTL
 * @param operation {int} ����������ʽ, ACL_FLOCK_OP_XXX
 * @return {int} 0: �����ɹ�; -1: ����ʧ��
 */
ACL_API int acl_myflock(ACL_FILE_HANDLE fd, int lock_style, int operation);

/*
 * Lock styles.
 */
#define ACL_FLOCK_STYLE_FLOCK     1  /**< ���� flock ��������(unix) */
#define ACL_FLOCK_STYLE_FCNTL     2  /**< ���� fcntl ��������(unix) */

/*
 * Lock request types.
 */
#define ACL_FLOCK_OP_NONE         0  /**< ���� */
#define ACL_FLOCK_OP_SHARED       1  /**< ������ */
#define ACL_FLOCK_OP_EXCLUSIVE    2  /**< ���������� */

/**
 * �޵ȴ�����, ������ ACL_FLOCK_OP_SHARED, �� ACL_FLOCK_OP_EXCLUSIVE ����,
 * ������ ACL_FLOCK_OP_SHARED ����
 */
#define ACL_FLOCK_OP_NOWAIT       4

/**
 * ������ʽ��λ����
 */
#define ACL_FLOCK_OP_BITS \
	(ACL_FLOCK_OP_SHARED | ACL_FLOCK_OP_EXCLUSIVE | ACL_FLOCK_OP_NOWAIT)

#ifdef  __cplusplus
}
#endif

#endif
