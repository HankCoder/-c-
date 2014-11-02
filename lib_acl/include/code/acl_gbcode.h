#ifndef	ACL_GBCODE_INCLUDE_H
#define	ACL_GBCODE_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "stdlib/acl_define.h"

/**
 * ��GBK�ַ����еļ���ת��ΪGBK�ַ����еķ���
 * @param data {const char*} ��������
 * @param dlen {size_t} data ����
 * @param buf {char*} �洢ת������������ buf ��ַ�� data ������ͬһ��ַ
 * @param size {size_t} buf �ռ��С
 */
ACL_API void acl_gbjt2ft(const char *data, size_t dlen, char *buf, size_t size);


/**
 * ��GBK�ַ����еķ���ת��ΪGBK�ַ����еļ���
 * @param data {const char*} ��������
 * @param dlen {size_t} data ����
 * @param buf {char*} �洢ת������������ buf ��ַ�� data ������ͬһ��ַ
 * @param size {size_t} buf �ռ��С
 */
ACL_API void acl_gbft2jt(const char *data, size_t dlen, char *buf, size_t size);

#ifdef	__cplusplus
}
#endif

#endif
