#ifndef ACL_VSTRING_BASE64_CODE_INCLUDE_H
#define ACL_VSTRING_BASE64_CODE_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#include "stdlib/acl_vstring.h"

/**
 * BASE64 ���뺯��
 * @param vp {ACL_VSTRING*} �洢�������
 * @param in {const char*} Դ����
 * @param len {int} in Դ���ݵĳ���
 * @return {ACL_VSTRING*} �� vp ��ͬ
 */
ACL_API ACL_VSTRING *acl_vstring_base64_encode(ACL_VSTRING *vp, const char *in, int len);

/**
 * BASE64 ���뺯��
 * @param vp {ACL_VSTRING*} �洢�������
 * @param in {const char*} ����������
 * @param len {int} in ���ݳ���
 * @return {ACL_VSTRING*} NULL: ����ʧ��; !=NULL: ����ɹ����� vp ��ͬ��ַ
 */
ACL_API ACL_VSTRING *acl_vstring_base64_decode(ACL_VSTRING *vp, const char *in, int len);

#ifdef  __cplusplus
}
#endif

#endif
