#ifndef ACL_DEBUG_INCLUDE_H
#define	ACL_DEBUG_INCLUDE_H

#include "acl_define.h"
#include "acl_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define	ACL_DEBUG_INTER_BASE    0  /**< ��С���Ա�ǩֵ */
#define	ACL_DEBUG_WQ            (ACL_DEBUG_INTER_BASE + 1)  /**< ACL_WORKQ ���Ա�ǩ */
#define ACL_DEBUG_PROCTL        (ACL_DEBUG_INTER_BASE + 2)  /**< ACL_PROCTL ���Ա�ǩ */
#define ACL_DEBUG_THR_POOL      (ACL_DEBUG_INTER_BASE + 3)  /**< ACL_PTHREAD_POOL ���Ա�ǩ */
#define	ACL_DEBUG_EVENT		(ACL_DEBUG_INTER_BASE + 4)  /**< ACL_EVENT ���ȱ�ǩ */

/**
 * ��־���Ժ�ӿ�
 * @param SECTION {int} ���Ա�ǩֵ
 * @param LEVEL {int} ��Ӧ��SECTION���Ա�ǩ�ļ���
 */
#define acl_debug(SECTION, LEVEL) \
	!acl_do_debug((SECTION), (LEVEL)) ? (void) 0 : acl_msg_info

/**
 * �ͷ��ڲ�һЩ�ڴ����Դ
 */
ACL_API void acl_debug_end(void);

/**
 * ��ʼ����־���Ե��ýӿ�
 * @param ptr {const char*} ���Ա�ǩ�������ַ�����
 *  ��ʽ: 1:1, 2:10, 3:8...  or 1:1; 2:10; 3:8...
 */
ACL_API void acl_debug_init(const char *ptr);

/**
 * ��ʼ����־���Ե��ýӿ�
 * @param ptr {const char*} ���Ա�ǩ�������ַ�����
 *  ��ʽ: 1:1, 2:10, 3:8...  or 1:1; 2:10; 3:8...
 * @param max_debug_level {int} �����Ա�ǩֵ
 */
ACL_API void acl_debug_init2(const char *ptr, int max_debug_level);

/**
 * �жϸ�����ǩ�ļ����Ƿ�����־���������Χ��
 * @param section {int} ��ǩֵ
 * @param level {int} ����ֵ
 */
ACL_API int acl_do_debug(int section, int level);

#ifdef __cplusplus
}
#endif

#endif
