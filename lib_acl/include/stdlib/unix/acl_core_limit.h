#ifndef	ACL_CORE_LIMIT_INCLUDE_H
#define	ACL_CORE_LIMIT_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "../acl_define.h"
#ifdef ACL_UNIX

/**
 * ���ô˺������ó������ʱ������ core �ļ������ֵ
 * @param max {unsigned long long int} ����ֵ == 0 ʱ��
 *  ��ʹ��ϵͳ�涨�����ֵ������ʹ�ø�ֵ
 */
void acl_set_core_limit(unsigned long long int max);

#endif  /* ACL_UNIX */

#ifdef  __cplusplus
}
#endif

#endif
