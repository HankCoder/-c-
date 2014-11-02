#ifndef ACL_PROCTL_INCLUDE_H
#define ACL_PROCTL_INCLUDE_H

#include "stdlib/acl_define.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ��ȡ���ƽ��̵�ִ�г������ڵ�·��λ��
 * @param buf {char*} �洢������ڴ�λ��, ���صĽ���Ľ�β
 *  ������ "\" �� "/",���磺"C:\\test_path\\test1_path", ������
 *  "C:\\test_path\\test1_path\\"
 * @param size {size_t} buf �Ŀռ��С
 */
ACL_API void acl_proctl_daemon_path(char *buf, size_t size);

/**
 * ��ʼ�����̿��ƿ�ܣ��� acl_proctl_start ��Ҫ��
 * @param progname {const char*} ���ƽ��̽�����
 */
ACL_API void acl_proctl_deamon_init(const char *progname);

/**
 * ���ƽ�����Ϊ��̨����������У����������ӽ��̵�����״̬��
 * ����ӽ����쳣�˳�����������ӽ���
 */
ACL_API void acl_proctl_daemon_loop(void);

/**
 * �ڿ��ƽ�������������һ���ӽ���
 * @param progchild {const char*} �ӽ��̵ĳ�����
 * @param argc {int} argv ����ĳ���
 * @param argv {char* []}
 * @return 0: ok; -1: error
 */
ACL_API int acl_proctl_deamon_start_one(const char *progchild, int argc, char *argv[]);

/**
 * �����ʽ����ĳ���ӽ���
 * @param progname {const char*} ���ƽ��̽�����
 * @param progchild {const char*} �ӽ��̽�����
 * @param argc {int} argv ����ĳ���
 * @param argv {char* []} ���ݸ��ӽ��̵Ĳ���
 */
ACL_API void acl_proctl_start_one(const char *progname,
	const char *progchild, int argc, char *argv[]);

/**
 * �����ʽֹͣĳ���ӽ���
 * @param progname {const char*} ���ƽ��̽�����
 * @param progchild {const char*} �ӽ��̽�����
 * @param argc {int} argv ����ĳ���
 * @param argv {char* []} ���ݸ��ӽ��̵Ĳ���
 */
ACL_API void acl_proctl_stop_one(const char *progname,
	const char *progchild, int argc, char *argv[]);

/**
 * �����ʽֹͣ���е��ӽ���
 * @param progname {const char*} ���ƽ��̽�����
 */
ACL_API void acl_proctl_stop_all(const char *progname);

/**
 * �����ʽ֪ͨ���ƽ���ֹͣ���е��ӽ��̣������ӽ����˳�����ƽ���Ҳ�Զ��˳�
 * @param progname {const char*} ���ƽ��̽�����
 */
ACL_API void acl_proctl_quit(const char *progname);

/**
 * �г���ǰ�����������еķ������
 * @param progname {const char*} ���ƽ��̽�����
 */
ACL_API void acl_proctl_list(const char *progname);

/**
 * ̽��ĳ����������Ƿ�������
 * @param progname {const char*} ���ƽ��̽�����
 * @param progchild {const char*} �ӽ��̽�����
 */
ACL_API void acl_proctl_probe(const char *progname, const char *progchild);

/**
 * �ӽ��̵��ýӿڣ�ͨ���˽ӿ��븸����֮�佨������/�����ƹ�ϵ
 * @param progname {const char*} �ӽ��̽�����
 * @param onexit_fn {void (*)(void*)} ����ǿ����ӽ����˳�ʱ���õĻص�����
 * @param arg {void*} onexit_fn ����֮һ
 */
ACL_API void acl_proctl_child(const char *progname, void (*onexit_fn)(void *), void *arg);

#ifdef __cplusplus
}
#endif

#endif
