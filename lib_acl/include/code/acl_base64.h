#ifndef	ACL_BASE64_CODE_INCLUDE_H
#define	ACL_BASE64_CODE_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"

/**
 * BASE64 ���뺯��
 * @param plain_in {const char*} �����Դ��������
 * @param len {int} plain_in �����ݳ���
 * @return {unsigned char*} BASE64���������ݣ����� acl_myfree �ͷ�
 */
ACL_API unsigned char *acl_base64_encode(const char *plain_in, int len);

/**
 * BASE64 ���뺯��
 * @param code_in {const char*} ��BASE64����������
 * @param ppresult {char**} �������ɹ�����洢���������Ҳ���ʱ����
 *  acl_myfree ���ͷ����ڴ�ռ�
 * @return {int} -1: ��ʾ����ʧ���� *ppresult ָ��NULL; >0: ��ʾ��������������
 *  ���ȣ��� *ppresult ָ��һ�¶�̬������ڴ������ڲ��洢������������ acl_myfree
 *  �ͷ� *ppresult �Ķ�̬�ڴ�
 */
ACL_API int acl_base64_decode(const char *code_in, char **ppresult);


#ifdef  __cplusplus
}
#endif

#endif

