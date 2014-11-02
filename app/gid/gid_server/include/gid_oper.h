#ifndef	__GID_OPER_INCLUDE_H__
#define	__GID_OPER_INCLUDE_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "lib_acl.h"

#define	GID_OK			200	/* ���� */
#define	GID_ERR_SID		500	/* �Ự ID �Ų��� */
#define	GID_ERR_OVERRIDE	501	/* �ﵽ������ֵ */
#define	GID_ERR_SAVE		502	/* �洢������ʱ���� */

/**
 * ���ݴ���Ż���ַ�������
 * @param errnum {int} �����
 * @return {const char*} ��������
 */
const char *gid_serror(int errnum);

/**
 * ��ȡ��һ��GID��
 * @param path {const char*} �ļ��洢·��
 * @param tag {const char*} ID��ʶ��
 * @param step {unsigned int} ÿ�εĲ���ֵ
 * @param errnum {int*} ���ǿգ����¼����ԭ��
 * @return {acl_int64}, �������ֵ < 0�����ʾ����ʧ�ܣ�
 *  ����ԭ��ο� errnum �ķ���ֵ
 */
acl_int64 gid_next(const char *path, const char *tag,
	unsigned int step, int *errnum);

/**
 * ��ʼ��������������Ӧ���ô˺�����ʼ���ڲ���
 * @param fh_limit {int} ������ļ������������
 * @param sync_gid {int} ÿ����һ���µ� gid ���Ƿ�ͬʱͬ��������
 * @param debug_section {int} �����õı�ǩֵ
 */
void gid_init(int fh_limit, int sync_gid, int debug_section);

/**
 * �����˳�ǰ������ô˺�������ʹ�ڴ�������ˢ��������
 */
void gid_finish(void);

#ifdef	__cplusplus
}
#endif

#endif
