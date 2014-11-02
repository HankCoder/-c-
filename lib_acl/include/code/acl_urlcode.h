#ifndef ACL_URLCODE_INCLUDE_H
#define ACL_URLCODE_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"

/**
 * URL ���뺯��
 * @param str {const char*} Դ�ַ���
 * @return {char*} �������ַ���������ֵ������Ϊ�գ���Ҫ�� acl_myfree �ͷ�
 */
ACL_API char *acl_url_encode(const char *str);

/**
 * URL ���뺯��
 * @param str {const char*} ��URL�������ַ���
 * @return {char*} �������ַ���������ֵ������Ϊ�գ���Ҫ�� acl_myfree �ͷ�
 */
ACL_API char *acl_url_decode(const char *str);

#ifdef __cplusplus
}
#endif
#endif
