#ifndef ACL_SCAN_DIR_INCLUDE_H
#define ACL_SCAN_DIR_INCLUDE_H

#include <sys/stat.h>

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"
/**
 * Ŀ¼ɨ�������Ͷ���
 */
typedef struct ACL_SCAN_DIR ACL_SCAN_DIR;

/**
 * Ŀ¼�����������û��������õĻص��������Ͷ���
 * @param scan {ACL_SCAN_DIR*} Ŀ¼ɨ��ָ��
 * @param ctx {void*} �û�����ָ��
 */
typedef int (*ACL_SCAN_DIR_FN)(ACL_SCAN_DIR *scan, void *ctx);

/**
 * ��ɨ��·��, Ϊ���� acl_scan_dir ������ĳ�ʼ������
 * @param path {const char*} Ҫ�򿪵�·������
 * @param recursive {int} �Ƿ���Ҫ�ݹ�ɨ����Ŀ¼
 * @return {ACL_SCAN_DIR*} NULL: Err; != NULL, OK
 */
ACL_API ACL_SCAN_DIR *acl_scan_dir_open(const char *path, int recursive);

/**
 * �ر�ɨ����
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 */
ACL_API void acl_scan_dir_close(ACL_SCAN_DIR *scan);

/**
 * ��Ŀ¼ɨ��������ͳ����Ϣ��صı�����0
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 */
ACL_API void acl_scan_dir_reset(ACL_SCAN_DIR *scan);

/**
 * ͨ���˽ӿ�����ɨ�����Ļص������������ȣ�������һ�����Ʊ�־
 * Ϊ ACL_SCAN_CTL_END ʱ��ʾ���Ʋ����б����
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 * @param name {int} ��һ������� ACL_SCAN_CTL_XXX
 */
ACL_API void acl_scan_dir_ctl(ACL_SCAN_DIR *scan, int name, ...);
#define ACL_SCAN_CTL_END	0  /**< ���ƽ�����־ */
#define ACL_SCAN_CTL_FN		1  /**< ���� ACL_SCAN_DIR_FN ��־ */
#define ACL_SCAN_CTL_CTX	2  /**< �����û����� */

/**
 * ��õ�ǰ״̬�µ����·��(����ڳ������ acl_scan_dir_open
 * ����ʱ�ĳ�������·��)
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 * @return {const char*} ���·��, == NULL: err; != NULL, OK
 */
ACL_API const char *acl_scan_dir_path(ACL_SCAN_DIR *scan);

/**
 * ��ǰ��ɨ����ļ��������ɨ��Ķ������ļ����򷵻� "\0"
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 * @return {const char*} �ļ���
 */
ACL_API const char *acl_scan_dir_file(ACL_SCAN_DIR *scan);

/**
 * ��ǰ�Ѿ�ɨ���Ŀ¼�ܸ���
 * @param scan {ACL_SCAN_DIR*}
 * @return {int} Ŀ¼�ܸ���, < 0 ��ʾ����
 */
ACL_API int acl_scan_dir_ndirs(ACL_SCAN_DIR *scan);

/**
 * ��ǰ�Ѿ�ɨ����ļ��ܸ���
 * @param scan {ACL_SCAN_DIR*}
 * @return {int} �ļ��ܸ���, < 0 ��ʾ����
 */
ACL_API int acl_scan_dir_nfiles(ACL_SCAN_DIR *scan);

/**
 * ��ǰ�Ѿ�ɨ����ļ���С�ܺ�
 * @param scan {ACL_SCAN_DIR*}
 * @return {acl_int64} -1: Error; >= 0: Ok
 */
ACL_API acl_int64 acl_scan_dir_nsize(ACL_SCAN_DIR *scan);

/**
 * ȡ�õ�ǰ�ļ���Ŀ¼��������Ϣ�������ڱ�׼�� stat() ����
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 * @param sbuf: {struct acl_stat*} ����ָ��
 * @return {int} 0: Ok, -1: Error
 */
ACL_API int acl_scan_stat(ACL_SCAN_DIR *scan, struct acl_stat *sbuf);

/**
 * Ŀ¼�Ƿ�ɨ�����
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 * @return {int} 0: ��ʾδɨ����ϣ� !=0: ��ʾɨ�����
 */
ACL_API int acl_scan_dir_end(ACL_SCAN_DIR *scan);

/**
 * ����Ҫ����ɨ������·��ѹջ
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 * @param path {const char*} ��Ҫѹջ�����·��
 * @return {int} 0: OK; -1: Err
 */
ACL_API int acl_scan_dir_push(ACL_SCAN_DIR *scan, const char *path);

/**
 * ������һ��·��
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 * @return {ACL_SCAN_DIR*} ���ض�ջ�е���һ������, == NULL: ����; != NULL, OK
 */
ACL_API ACL_SCAN_DIR *acl_scan_dir_pop(ACL_SCAN_DIR *scan);

/**
 * ��� scan ��ǰ����·������һ��·�������ļ���, ע�⣬�ú����ڲ�����ݹ�ɨ��,
 * �� acl_scan_dir_open �еĲ��� recursive �Ըú�����Ч
 *  1�� ".." �� "." ����������
 *  2�� ����������, ������·��, ·�������� acl_scan_dir_path ���
 * @param scan {ACL_SCAN_DIR*} ָ���ַ
 * @return {const char*} ΪĿ¼���ƻ��ļ�����, != NULL: OK; == NULL, ɨ�����
 */
ACL_API const char *acl_scan_dir_next(ACL_SCAN_DIR *scan);

/**
 * �����һ���ļ���(������·����, ���·��������ͨ�� acl_scan_dir_path ���),
 * �ú����ڲ�֧�ֵݹ�ɨ��Ŀ¼����, acl_scan_dir_open �еĲ��� recursive �Ըú�����Ч
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 * @return {const char*} ������һ��ɨ����ļ���: !NULL, OK; NULL ɨ����ϣ�Ӧ����ɨ��
 */
ACL_API const char *acl_scan_dir_next_file(ACL_SCAN_DIR *scan);

/**
 * �����һ��Ŀ¼��(������·����, ���·��������ͨ�� acl_scan_dir_path ���),
 * �ú����ڲ�֧�ֵݹ�ɨ��Ŀ¼����, acl_scan_dir_open �еĲ��� recursive �Ըú�����Ч
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 * @return {const char*} ������һ��ɨ���Ŀ¼��: !NULL, OK; NULL ɨ�����, Ӧ����ɨ��
 */
ACL_API const char *acl_scan_dir_next_dir(ACL_SCAN_DIR *scan);

/**
 * �����һ��Ŀ¼�����ļ���(������·����, ���·��������ͨ�� acl_scan_dir_path ���),
 * �ú����ڲ�֧�ֵݹ�ɨ��Ŀ¼����, acl_scan_dir_open �еĲ��� recursive �Ըú�����Ч
 * @param scan {ACL_SCAN_DIR*} ����ָ��
 * @param is_file {int*} �����ؽ���ǿ�ʱ���õ�ַ�洢��ֵ��ʾ��ɨ�赽���Ƿ���
 *  �ļ������Ϊ true ��Ϊ�ļ�������ΪĿ¼
 * @return {const char*} ������һ��ɨ���Ŀ¼�����ļ���: !NULL, OK; NULL ɨ�����,
 *  Ӧ����ɨ��
 */
ACL_API const char *acl_scan_dir_next_name(ACL_SCAN_DIR *scan, int *is_file);

/**
 * ȡ�õ�ǰĿ¼����ռ���̿ռ��С(���ֽڼ���)
 * �ú����ڲ�֧�ֵݹ�ɨ��Ŀ¼����, acl_scan_dir_open �еĲ��� recursive �Ըú�����Ч
 * @param scan {ACL_SCAN_DIR*} ��Ŀ¼ʱ��ɨ����
 * @param nfile {int*} ɨ������¼��ɨ����ļ�����
 * @param ndir {int*} ɨ������¼��ɨ���Ŀ¼����
 * @return {acl_int64} -1: Error; >= 0: Ok
 */
ACL_API acl_int64 acl_scan_dir_size2(ACL_SCAN_DIR *scan, int *nfile, int *ndir);

/**
 * ȡ�õ�ǰĿ¼����ռ���̿ռ��С(���ֽڼ���)
 * @param pathname {const char*} Ŀ¼·����
 * @param recursive {int} �Ƿ�Ҫ�ݹ�ɨ���Ŀ¼�µ�������Ŀ¼
 * @param nfile {int*} ɨ������¼��ɨ����ļ�����
 * @param ndir {int*} ɨ������¼��ɨ���Ŀ¼����
 * @return {acl_int64} -1: Error, >= 0: Ok
 */
ACL_API acl_int64 acl_scan_dir_size(const char *pathname, int recursive,
		int *nfile, int *ndir);

/**
 * ɾ������·�������е��ļ���Ŀ¼
 * @param nfile {int*} ɨ������¼��ɨ����ļ�����
 * @param ndir {int*} ɨ������¼��ɨ���Ŀ¼����
 * �ú����ڲ�֧�ֵݹ�ɨ��Ŀ¼����, acl_scan_dir_open �еĲ��� recursive �Ըú�����Ч
 * @param scan {ACL_SCAN_DIR*} ��Ŀ¼ʱ��ɨ����
 * @return {acl_int64} >= 0: ʵ��ɾ�����ļ�����Ŀ¼���ĳߴ��С֮��(�ֽ�); < 0: ����.
 */
ACL_API acl_int64 acl_scan_dir_rm2(ACL_SCAN_DIR *scan, int *nfile, int *ndir);

/**
 * ɾ������·�������е��ļ���Ŀ¼
 * @param pathname {const char*} ·����
 * @param recursive {int} �Ƿ�ݹ�ɾ��������Ŀ¼����Ŀ¼�µ��ļ�
 * @param ndir {int*} ���ò����ǿգ����̽����� *ndir �����ܹ�ɾ����Ŀ¼��Ŀ
 * @param nfile {int*} ���ò����ǿգ����̽����� *nfile �����ܹ�ɾ�����ļ���Ŀ
 * @return {acl_int64} >= 0: ʵ��ɾ�����ļ�����Ŀ¼���ĳߴ��С֮��(�ֽ�); < 0: ����.
 */
ACL_API acl_int64 acl_scan_dir_rm(const char *pathname, int recursive,
		int *ndir, int *nfile);

#ifdef  __cplusplus
}
#endif

#endif


