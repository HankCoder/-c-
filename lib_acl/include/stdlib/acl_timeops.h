/*
 * Name: misc.h
 * Author: zsx
 * Date: 2003-12-16
 * Version: 1.0
 */

#ifndef	ACL_MISC_INCLUDE_H
#define	ACL_MISC_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif
#include "acl_define.h"
#include <time.h>

/* acl_str2time.c */
/**
 * ��ʱ���ַ���ת��Ϊ time_t ����
 * @param str ʱ���ַ�����ʽΪ: year-month-mday(��: 2004-1-1)
 * @return time_t ���͵�ֵ
 */
ACL_API time_t acl_str2time_t(const char *str);

#ifdef  __cplusplus
}
#endif

#endif

