#ifndef ACL_MASK_ADDR_H_INCLUDED
#define ACL_MASK_ADDR_H_INCLUDED

#ifdef  __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"

/**
 * �����������볤�ȼ�IP��ַ������������ַ
 * @param addr_bytes {unsigned char*} �����������ֽ��� IP ��ַ,
 *  (����ΪIPv4/IPv6), �ò���Ϊֵ���ͣ�������ڸõ�ַ��
 * @param addr_type_count {unsigned} addr_bytes ��ַ����
 * @param network_bits {unsigned} ��������ĳ���
 */
ACL_API void acl_mask_addr(unsigned char *addr_bytes,
		unsigned addr_byte_count, unsigned network_bits);

#ifdef  __cplusplus
}
#endif

#endif

