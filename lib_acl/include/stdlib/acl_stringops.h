#ifndef ACL_STRINGOPS_INCLUDE_H
#define ACL_STRINGOPS_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"

/**
 * �жϸ����ַ����Ƿ�ȫΪ����
 * @param str {const char*} �ַ���
 * @return {int} 0: ��; 1: ��
 */
ACL_API int acl_alldig(const char *str);

/**
 * ������ַ���ƴ�ӳ�һ���ַ���
 * @param arg0 {const char*} ��һ���ǿ��ַ���
 * @param ... �������ַ������ϣ��������� NULL
 * @return {char*} ����ַ�������Ϊ��, ���ַ�����Ҫ���� acl_myfree �ͷ�
 */
ACL_API char *acl_concatenate(const char *arg0,...);

/**
 * ��һ��ȫ·�����ļ�����ȡ���ļ������֣��磺
 * �� "/tmp/test.txt" �� "\\tmp\\test.txt" ��ȡ�� test.txt
 * @param path {const char*} ����·�����ļ������磺"/tmp/test.txt"
 *  �� "\\tmp\\test.txt"
 * @return {const char*} �ļ������÷���ֵ����Ҫ�ͷţ�������صĵ�ַ
 *  Ϊ�մ�(����һ���ֽ�Ϊ '\0') ��˵������·�������ļ���
 */
ACL_API const char *acl_safe_basename(const char *path);

/**
 * ���������ַ������зָ����ֱ�ȡ�� name, value ��ַ, �����ַ�������Ϊ
 * {sp}{name}{sp}={sp}{value}{sp}, ��������ɹ����򽫽���ֱ���д洢,
 * ���� {sp} �ַ�����Ϊ: �ո�, "\t", "\r", "\n"
 * @param buf {char*} ������ַ���, ����Ϊ��
 * @param name {char**} �洢����ĵ�ַָ�룬����Ϊ��
 * @param value {char**} �洢����ĵ�ַָ�룬����Ϊ��
 * @return {const char*} ����ԭ�����Ϊ�����ʾ�����ɹ��������ʾ����ʧ�ܲ�����
 *  ʧ��ԭ��
 */
ACL_API const char *acl_split_nameval(char *buf, char **name, char **value);

#ifdef  __cplusplus
}
#endif

#endif

