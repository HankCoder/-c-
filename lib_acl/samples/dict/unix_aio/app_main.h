#ifndef	__APP_MAIN_INCLUDE_H__
#define	__APP_MAIN_INCLUDE_H__

#include "lib_acl.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* �ͻ��˶�д��ʱʱ��ֵ */
extern int app_var_client_idle_limit;

/* �û��������к�������, ���ú�������ֵ != 0 ʱ, ��ܻ��Զ��رտͻ����� */
typedef int (*APP_RUN_FN)(ACL_ASTREAM *stream, void *run_ctx);

/* �û����ĳ�ʼ���������� */
typedef void (*APP_INIT_FN)(void*);
typedef void (*APP_EXIT_FN)(void*);

typedef ACL_MASTER_SERVER_THREAD_INIT_FN APP_THREAD_INIT_FN  /* void (*)(void*) */;
typedef ACL_MASTER_SERVER_THREAD_EXIT_FN APP_THREAD_EXIT_FN;  /* void (*)(void*) */

#define	APP_CTL_END			0	/* �������ƽ�����־ */
#define	APP_CTL_INIT_FN			1	/* ��ʼ������ */
#define	APP_CTL_INIT_CTX		2	/* ��ʼ���������õĲ��� */
#define	APP_CTL_CFG_BOOL		3	/* �������͵����ò����� */
#define	APP_CTL_CFG_INT			4	/* �������͵����ò����� */
#define	APP_CTL_CFG_STR			5	/* �ַ������͵����ò����� */
#define	APP_CTL_EXIT_FN			6	/* �������˳�ʱ�Ļص����� */
#define	APP_CTL_EXIT_CTX		7	/* �����˳�ʱ�ص������Ĳ��� */
#define	APP_CTL_THREAD_INIT		8	/* ÿ���߳�����ʱ�Ļص����� */
#define	APP_CTL_THREAD_INIT_CTX		9	/* �߳�����ʱ�ص������Ĳ��� */
#define	APP_CTL_THREAD_EXIT		10	/* �߳��˳�ʱ�Ļص����� */
#define	APP_CTL_THREAD_EXIT_CTX		11	/* �߳��˳�ʱ�ص������Ĳ��� */
#define	APP_CTL_DENY_INFO		12	/* ���Ƿ��ͻ��˷���ʱ��������ʾ��Ϣ */

/*----------------------------------------------------------------------------*/
/* in app_main.c */

/**
 * ���������, �û����ĳ�ʼ������ָ�뼰���к���ָ��ͨ�����Ʋ�������ע��, �������ڲ�
 * ���ڳ�ʼ��ʱ�Զ������û�����ʼ������(APP_INIT_FN ����), �����յ�������ʵĿͻ���
 * ����ʱ���Զ������û�(APP_RUN_FN ����).
 * �������к���.
 * @param argc "int main(int argc, char *argv[])" �е� argc
 * @param argv "int main(int argc, char *argv[])" �е� argv
 * @param run_fn �û�������������
 * @param run_ctx run_fn() ����ʱ�Ĳ���֮һ
 * @param name ���Ʋ����еĵ�һ����������, ��֧�ֵ��������϶���: APP_CTL_XXX
 *        ���÷�ʽ: APP_CTL_XXX, xxx; ���� APP_CTL_END Ϊ����Ŀ��Ʋ���, ��ʾ���Ʋ���
 *        ����.
 * @example:
 *   app_main(argc, argv, {run_fn}, {run_ctx},
 *		APP_CTL_INIT_FN, {run_init_fn},
 *		APP_CTL_INIT_CTX, {run_init_ctx},
 *		APP_CTL_END);
 * ע: app_main() �����в�����, argc, argv, run_fn, run_ctx(����ΪNULL), APP_CTL_END
 *     ���Ǳ����.
 */

extern void app_main(int argc, char *argv[], APP_RUN_FN run_fn, void *run_ctx, int name, ...);

/*----------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif

#endif
