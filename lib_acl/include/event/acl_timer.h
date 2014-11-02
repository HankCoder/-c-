#ifndef	ACL_TIMER_INCLUDE_H
#define	ACL_TIMER_INCLUDE_H

#include "stdlib/acl_define.h"
#include <time.h>
#include "stdlib/acl_iterator.h"
#include "stdlib/acl_ring.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * ��ʱ�����Ͷ���
 */
typedef struct ACL_TIMER_INFO {               
	/* public */
	void *obj;              /**< �û������ݶ���ָ�� */
	acl_int64 when;         /**< ��������ʱ���(΢�) */

	/* private */
	ACL_RING entry;         /**< �ڲ��õĶ�ʱ�� */
} ACL_TIMER_INFO;

/* ��ʱ������ṹ */
typedef struct ACL_TIMER ACL_TIMER;

struct ACL_TIMER {
        acl_int64 (*request)(ACL_TIMER *timer, void *obj, acl_int64 delay);
        acl_int64 (*cancel)(ACL_TIMER *timer, void *obj);
        void* (*popup)(ACL_TIMER* timer);

        ACL_RING timer_header;
        acl_int64 present;
        acl_int64 time_left;

	/* for acl_iterator */

	/* ȡ������ͷ���� */
	const void *(*iter_head)(ACL_ITER*, struct ACL_TIMER*);
	/* ȡ��������һ������ */
	const void *(*iter_next)(ACL_ITER*, struct ACL_TIMER*);
	/* ȡ������β���� */
	const void *(*iter_tail)(ACL_ITER*, struct ACL_TIMER*);
	/* ȡ��������һ������ */
	const void *(*iter_prev)(ACL_ITER*, struct ACL_TIMER*);

	/* ����뵱ǰ����ָ��������� ACL_TIMER_INFO ���� */
	const ACL_TIMER_INFO *(*iter_info)(ACL_ITER*, struct ACL_TIMER*);
};

/**
 * ��Ӷ�ʱ����
 * @param timer {ACL_TIMER*}����ʱ�����
 * @param obj {void*}���û�����̬����
 * @param delay {acl_int64}����������ʱ����(΢�뼶)
 * @return {acl_int64} �µĶ�ʱ����Ľ��ʱ���(΢�뼶)
 */
ACL_API acl_int64 acl_timer_request(ACL_TIMER* timer, void *obj, acl_int64 delay);

/**
 * ȡ����ʱ����
 * @param timer {ACL_TIMER*}����ʱ�����
 * @param obj {void*}���û�����̬����
 * @return {acl_int64}��������һ����ʱ���񱻴�����ʱ����(΢�뼶)
 */
ACL_API acl_int64 acl_timer_cancel(ACL_TIMER* timer, void *obj);

/**
 * �Ӷ�ʱ���л�ȡ��ʱ�Ķ�ʱ����
 * @param timer {ACL_TIMER*}����ʱ�����
 * @return {void*}���û�����̬����
 */
ACL_API void *acl_timer_popup(ACL_TIMER* timer);

/**
 * ������һ����ʱ���񱻴�����ʱ����
 * @param timer {ACL_TIMER*}����ʱ�����
 * @return {acl_int64} ����ֵ��λΪ΢��
 */
ACL_API acl_int64 acl_timer_left(ACL_TIMER* timer);

/**
 * ������ʱ��������ж�ʱ������
 * @param timer {ACL_TIMER*}����ʱ�����
 * @param action {void (*)(ACL_TIMER_INFO*, void*)} �û��ı����ص�����
 * @param arg {void*} action �еĵڶ�������
 */
ACL_API void acl_timer_walk(ACL_TIMER *timer, void (*action)(ACL_TIMER_INFO *, void *), void *arg);

/**
 * ������ʱ�����
 * @return {ACL_TIMER*}
 */
ACL_API ACL_TIMER *acl_timer_new(void);

/**
 * �ͷŶ�ʱ�����
 * @param timer {ACL_TIMER*}
 * @param free_fn {void (*)(void*)} �ͷŶ�ʱ������û�����Ļص��ͷź���
 */
ACL_API void acl_timer_free(ACL_TIMER* timer, void (*free_fn)(void*));

/**
 * ��ö�ʱ���ﶨʱ���������
 * @param timer {ACL_TIMER*}
 * @return {int} >= 0
 */
ACL_API int acl_timer_size(ACL_TIMER *timer);

#ifdef	__cplusplus
}
#endif

#endif
