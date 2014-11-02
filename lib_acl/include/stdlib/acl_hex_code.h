#ifndef ACL_HEX_CODE_INCLUDE_H
#define ACL_HEX_CODE_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_vstring.h"

/**
 * �����������ݽ��б��룬һ���ֽ�ת���������ֽں󣬴Ӷ�תΪ�ı��ַ���
 * @param buf {ACL_VSTRING*} �洢ת�����
 * @param ptr {const char*} ����������
 * @param len {int} ptr ���ݵĳ���
 * @return {ACL_VSTRING*} ���ת���ɹ������� buf ��ͬ
 */
ACL_API ACL_VSTRING *acl_hex_encode(ACL_VSTRING *buf, const char *ptr, int len);

/**
 * �����������ݽ��н���
 * @param buf {ACL_VSTRING*} �洢ת�����
 * @param ptr {const char*} ��������
 * @param len {int} ptr ���ݳ���
 * @return {ACL_VSTRING*} �������ɹ������� buf ��ͬ, ���򷵻� NULL
 */
ACL_API ACL_VSTRING *acl_hex_decode(ACL_VSTRING *buf, const char *ptr, int len);

#ifdef  __cplusplus
}
#endif

#endif

