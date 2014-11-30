#ifndef	__SPOOL_MAIN_INCLUDE_H__
#define	__SPOOL_MAIN_INCLUDE_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "lib_util.h"
#include "global.h"

typedef struct SPOOL {
	ACL_SPOOL *h_spool;
} SPOOL;
/*----------------------------------------------------------------------------*/
/* in spool_main.c */
/**
 * ����һ�����������
 * @param max_threads ����߳���
 * @param idle_timeout ÿ���߳̿��г�ʱʱ��
 * @return SPOOL* ���������ӳؾ��
 */
extern SPOOL *spool_create(int max_threads, int idle_timeout);

/**
 * ������������
 * @param spool ����������ؾ��
 */
extern int spool_start(const SPOOL *spool);

/**
 * ������������һ����������
 * @param spool ����������ؾ��
 * @param cstream �ͻ���������ָ��
 * ע: cstream ���������ڸú����ڲ��Ļص������н��йر�, ���Ըú����ĵ����߲�Ҫ
 *     �رո���.
 */
extern void spool_add_worker(SPOOL *spool, ACL_VSTREAM *cstream);
/*----------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif

#endif
