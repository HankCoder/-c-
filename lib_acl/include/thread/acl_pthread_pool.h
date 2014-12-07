#ifndef ACL_PTHREAD_POOL_INCLUDE_H
#define ACL_PTHREAD_POOL_INCLUDE_H

#include "stdlib/acl_define.h"
#include <time.h>

#ifdef	ACL_UNIX
#include <pthread.h>
#endif

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct acl_pthread_job_t acl_pthread_job_t;

/**
 * ����һ���̳߳صĹ�������
 * @param run_fn {void (*)(void*)} �����߳��б����õĻص����� 
 * @param run_arg {void*} run_fn �Ļص�����֮һ
 * @param fixed {int}
 * @return {acl_pthread_job_t*} ���ش����Ĺ�������
 */
ACL_API acl_pthread_job_t *acl_pthread_pool_alloc_job(void (*run_fn)(void*),
		void *run_arg, int fixed);

/**
 * �ͷ��� acl_pthread_pool_alloc_job �����Ĺ�������
 * @param job {acl_pthread_job_t*}
 */
ACL_API void acl_pthread_pool_free_job(acl_pthread_job_t *job);

/**
 * �̳߳ض���ṹ���Ͷ���
 */
typedef struct acl_pthread_pool_t acl_pthread_pool_t;

/**
 * �̳߳ض������ԵĽṹ���Ͷ���
 */
typedef struct acl_pthread_pool_attr_t {
	int   threads_limit;	/**< �̳߳�����߳������� */
#define ACL_PTHREAD_POOL_DEF_THREADS   100  /**< ȱʡ���ֵΪ 100 ���߳� */
	int   idle_timeout;                 /**< �����߳̿��г�ʱʱ��(��) */
#define ACL_PTHREAD_POOL_DEF_IDLE      0    /**< ȱʡ�ռ䳬ʱʱ��Ϊ 0 �� */
	size_t stack_size;                  /**< �����̵߳Ķ�ջ��С(�ֽ�) */
} acl_pthread_pool_attr_t;

/**
 * ���򵥵ش����̶߳���ķ���
 * @param threads_limit {int}  �̳߳�����󲢷��߳���
 * @param idle_timeout {int} �����߳̿��г�ʱ�˳�ʱ��(��)
 * @return {acl_pthread_pool_t*}, �����Ϊ�����ʾ�ɹ�������ʧ��
 */
ACL_API acl_pthread_pool_t *acl_thread_pool_create(
		int threads_limit, int idle_timeout);

/**
 * ����һ���̳߳ض���
 * @param attr {acl_pthread_pool_attr_t*} �̳߳ش���ʱ�����ԣ�����ò���Ϊ�գ�
 *  �����Ĭ�ϲ���: ACL_PTHREAD_POOL_DEF_XXX
 * @return {acl_pthread_pool_t*}, �����Ϊ�����ʾ�ɹ�������ʧ��
 */
ACL_API acl_pthread_pool_t *acl_pthread_pool_create(
		const acl_pthread_pool_attr_t *attr);

/**
 * �����жѻ������������ڿ����߳�����2��ʱ. ͨ���˺���������������
 * �߳�����ʱ��, ��������ô˺�����������, ������̲߳����������״̬.
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param timewait_sec {int} ���ߡ���ʱ��ֵ, ���齫��ֵ����Ϊ 1--5 ����
 * @return {int} �ɹ����� 0, ʧ�ܷ��� -1
 */
ACL_API int acl_pthread_pool_set_timewait(
		acl_pthread_pool_t *thr_pool, int timewait_sec);

/**
 * ���ע�ắ�������̴߳���������ִ�д˳�ʼ������
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param init_fn {int (*)(void*)} �����̳߳�ʼ������, ����ú������� < 0,
 *  ����߳��Զ��˳���
 * @param init_arg {void*} init_fn ����Ҫ�Ĳ���
 * @return {int} 0: OK; != 0: Error.
 */
ACL_API int acl_pthread_pool_atinit(acl_pthread_pool_t *thr_pool,
		int (*init_fn)(void *), void *init_arg);

/**
 * ���ע�ắ�������߳��˳�����ִ�д˳�����
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param free_fn {void (*)(void*)} �����߳��˳�ǰ����ִ�еĺ���
 * @param free_arg {void*} free_fn ����Ҫ�Ĳ���
 * @return {int} 0: OK; != 0: Error.
 */
ACL_API int acl_pthread_pool_atfree(acl_pthread_pool_t *thr_pool,
		void (*free_fn)(void *), void *free_arg);

/**
 * ����һ���̳߳ض���, �ɹ����ٺ�ö���������.
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @return {int} 0: �ɹ�; != 0: ʧ��
 */
ACL_API int acl_pthread_pool_destroy(acl_pthread_pool_t *thr_pool);

/**
 * ��ͣһ���̳߳ض��������, ֹͣ�󻹿���������.
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @return {int} 0: �ɹ�; != 0: ʧ��
 */
ACL_API int acl_pthread_pool_stop(acl_pthread_pool_t *thr_pool);

/**
 * ���̳߳����һ������
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param run_fn {void (*)(*)} ���п��ù����߳�ʱ�����õĻص�������
 * @param run_arg {void*} �ص����� run_fn ����Ҫ�Ļص�����
 */
ACL_API void acl_pthread_pool_add_one(acl_pthread_pool_t *thr_pool,
		void (*run_fn)(void *), void *run_arg);
#define	acl_pthread_pool_add	acl_pthread_pool_add_one

/**
 * ���̳߳����һ������
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param job {acl_pthread_job_t*} �� acl_pthread_pool_alloc_job �������߳�����
 */
ACL_API void acl_pthread_pool_add_job(acl_pthread_pool_t *thr_pool,
		acl_pthread_job_t *job);

/**
 * ��ʼ����������ʽ���������, ʵ�����ǿ�ʼ���м���
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 */
ACL_API void acl_pthread_pool_bat_add_begin(acl_pthread_pool_t *thr_pool);

/**
 * ���һ��������, ǰ�����Ѿ��ɹ�����, ������ acl_pthread_pool_bat_add_begin �ɹ�
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param run_fn {void (*)(void*)} ���п��ù����߳�ʱ�����õĻص�������
 * @param run_arg �ص����� run_fn ����Ҫ�Ļص�����
 */
ACL_API void acl_pthread_pool_bat_add_one(acl_pthread_pool_t *thr_pool,
		void (*run_fn)(void *), void *run_arg);
/**
 * ���һ��������, ǰ�����Ѿ��ɹ�����, ������ acl_pthread_pool_bat_add_begin �ɹ�
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param job {acl_pthread_job_t*} �� acl_pthread_pool_alloc_job �������߳�����
 */
ACL_API void acl_pthread_pool_bat_add_job(acl_pthread_pool_t *thr_pool,
		acl_pthread_job_t *job);

/**
 * ��������ӽ���, ʵ���ǽ���
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 */
ACL_API void acl_pthread_pool_bat_add_end(acl_pthread_pool_t *thr_pool);

/**
 * �����̳߳� POLLER ���Ⱥ�������Ҫʹ�ô˹��ܣ���Ҫ���ú���
 * acl_pthread_pool_create ��ͨ���˺������õ��Ⱥ�����Ȼ����
 * ���� acl_pthread_pool_start_poller ������̨���Ⱥ������ú�̨
 * ���Ⱥ����᲻�ϵص��� poller_fn (���û��Ļص�����)���û�����
 * �ڻص���������� acl_pthread_pool_add ����������ӽ��̳߳���
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param poller_fn {int (*)(void*)} ѭ�����������еĻص�����
 * @param poller_arg {void*} poller_fn ����Ҫ�Ĳ���
 */
ACL_API void acl_pthread_pool_set_poller(acl_pthread_pool_t *thr_pool,
		int (*poller_fn)(void *), void *poller_arg);
/**
 * ����һ���̳߳� POLLER �����߳�
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @return 0 �ɹ�; != 0 ʧ��, ���ԶԷ���ֵ���� strerror(ret) ȡ�ô���ԭ������
 */
ACL_API int acl_pthread_pool_start_poller(acl_pthread_pool_t *thr_pool);

/**
 * ��������ʽ��������ķַ�, ���ڲ���ʵ�ǵ����� acl_pthread_pool_add_one()
 * @return 0: OK; -1: err
 */
ACL_API int acl_pthread_pool_add_dispatch(void *dispatch_arg,
		void (*run_fn)(void *), void *run_arg);

/**
 * �Ե�����ӵķ�ʽ��������ķַ�
 * @return 0: OK; -1: err
 * ע��worker_fn �еĵڶ�������ΪACL_WORKER_ATTR�ṹָ�룬���̳߳ص�ĳ��
 *     �����߳�ά�����ýṹָ���еĳ�Ա���� init_data Ϊ�û��ĸ�ֵ���ͱ�����
 *     �磺���ݿ����Ӷ���ȡ�
 */
ACL_API int acl_pthread_pool_dispatch(void *dispatch_arg,
		void (*run_fn)(void *), void *run_arg);

/**
 * ��õ�ǰ�̳߳ص�����߳�������
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @return {int} ����߳�������ֵ
 */
ACL_API int acl_pthread_pool_limit(acl_pthread_pool_t *thr_pool);

/**
 * ��õ�ǰ�̳߳��е��߳���
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @return {int} �����̳߳��е����߳���������ֵ < 0 ��ʾ����
 */
ACL_API int acl_pthread_pool_size(acl_pthread_pool_t *thr_pool);

/**
 * ��õ�ǰ�̳߳��еĿ����߳���
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @return {int} �����̳߳��еĿ����߳���������ֵ < 0 ��ʾ����
 */
ACL_API int acl_pthread_pool_idle(acl_pthread_pool_t *thr_pool);

/**
 * ��õ�ǰ�̳߳��еķ�æ�߳���
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @return {int} �����̳߳��еķ�æ�߳���������ֵ < 0 ��ʾ����
 */
ACL_API int acl_pthread_pool_busy(acl_pthread_pool_t *thr_pool);

/**
 * �����߳�������ȳ�ʱ�����ʱ��(����)
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param n {acl_int64} ����ֵ > 0 ʱ������߳�����ĵ���ʱ�䳬����ֵ����¼������־(����)
 */
ACL_API void acl_pthread_pool_set_schedule_warn(
		acl_pthread_pool_t *thr_pool, acl_int64 n);

/**
 * �����̳߳������̵߳ȴ�����ĳ�ʱ��׼ʱ��(����)
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param n {acl_int64} ����ֵ > 0 ʱ�����̵߳ȴ�����ĳ�ʱ�ȴ���׼ʱ��(����)
 */
ACL_API void acl_pthread_pool_set_schedule_wait(
		acl_pthread_pool_t *thr_pool, acl_int64 n);

/**
 * ���̳߳��е��������ѻ�ʱ��ͨ���ú�������������жѻ�����ֵ
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param max {int} ������жѻ�����ֵ
 */
ACL_API void acl_pthread_pool_set_qlen_warn(
		acl_pthread_pool_t *thr_pool, int max);
/**
 * ȡ�õ�ǰ�̳߳�ȫ�ֶ�����δ������������
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @return {int} ��ǰδ�����������������ֵ < 0 ��ʾ����
 */
ACL_API int acl_pthread_pool_qlen(acl_pthread_pool_t *thr_pool);

/**
 * �����̳߳����̵߳Ķ�ջ��С
 * @param thr_pool {acl_pthread_pool_t*} �̳߳ض��󣬲���Ϊ��
 * @param size {size_t} �̴߳���ʱ�Ķ�ջ��С����λΪ�ֽ�
 */
ACL_API void acl_pthread_pool_set_stacksize(
		acl_pthread_pool_t *thr_pool, size_t size);

/**
 * ��ʼ���̳߳�����ֵ
 * @param attr {acl_pthread_pool_attr_t*}
 */
ACL_API void acl_pthread_pool_attr_init(acl_pthread_pool_attr_t *attr);

/**
 * �����̳߳������е�����ջ��С(�ֽ�)
 * @param attr {acl_pthread_pool_attr_t*}
 * @param size {size_t}
 */
ACL_API void acl_pthread_pool_attr_set_stacksize(
		acl_pthread_pool_attr_t *attr, size_t size);

/**
 * �����̳߳������е�����߳�������ֵ
 * @param attr {acl_pthread_pool_attr_t*}
 * @param threads_limit {int} �̳߳��е�����߳���
 */
ACL_API void acl_pthread_pool_attr_set_threads_limit(
		acl_pthread_pool_attr_t *attr, int threads_limit);

/**
 * �����̳߳��������߳̿��г�ʱֵ
 * @param attr {acl_pthread_pool_attr_t*}
 * @param idle_timeout {int} �߳̿��г�ʱʱ��(��)
 */
ACL_API void acl_pthread_pool_attr_set_idle_timeout(
		acl_pthread_pool_attr_t *attr, int idle_timeout);

#ifdef	__cplusplus
}
#endif

#endif	/* !__acl_pthread_pool_t_H_INCLUDED__ */
