#ifndef	ACL_PROCESS_INCLUDE_H
#define	ACL_PROCESS_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * �������й����л�ÿ�ִ�г���洢���ļ�ϵͳ�е�ȫ·��
 * @return {const char*} NULL: �޷����; != NULL: ����ֵ���ǳ�����
 *    �ļ�ϵͳ�ϵĴ洢ȫ·��
 */
ACL_API const char *acl_process_path(void);

/**
 * �������й����л��������·��
 * @return {const char*} NULL: �޷����; != NULL: ����ֵ��Ϊ���������·��
 */
ACL_API const char *acl_getcwd(void);

#ifdef	__cplusplus
}
#endif

#endif
