#ifndef ACL_AQUEUE_INCLUDE_H
#define	ACL_AQUEUE_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "stdlib/acl_define.h"
#include "thread/acl_thread.h"

#define	ACL_AQUEUE_ERR_UNKNOWN      -1
#define	ACL_AQUEUE_OK               0
#define	ACL_AQUEUE_ERR_LOCK         1
#define	ACL_AQUEUE_ERR_UNLOCK       2
#define	ACL_AQUEUE_ERR_TIMEOUT      3
#define	ACL_AQUEUE_ERR_COND_WAIT    4
#define	ACL_AQUEUE_ERR_COND_SIGNALE 5

typedef struct ACL_AQUEUE_ITEM ACL_AQUEUE_ITEM;
typedef struct ACL_AQUEUE ACL_AQUEUE;

typedef void (*ACL_AQUEUE_FREE_FN)(void *);
/**
 * ����һ���¶��ж�����
 * @return ACL_AQUEUE �ṹָ��
 */
ACL_API ACL_AQUEUE *acl_aqueue_new(void);

/**
 * �����Ƿ��ϸ�����е������ߣ�Ĭ��Ϊ����Ҫ���иü����� acl_aqueue_free
 * @param queue ACL_AQUEUE �ṹָ��
 * @param flag �����
 */
ACL_API void acl_aqueue_check_owner(ACL_AQUEUE *queue, char flag);

/**
 * ���ö��е�������, ֻ�������߲���Ȩ�ͷŶ���, ������ acl_aqueue_free()
 * @param queue ACL_AQUEUE �ṹָ��
 * @param owner ���̺߳ű�ʶ�������ߵ�ID��
 */
ACL_API void acl_aqueue_set_owner(ACL_AQUEUE *queue, unsigned int owner);

/**
 * �ͷŶ��ж�����
 * @param queue ACL_AQUEUE �ṹָ��
 * @param free_fn ���ͷŶ���ʱ, ����ú�����Ϊ��, ���ڲ�ͨ���˺����������е�
 *        �û�ע������ݶ��н����ͷ�
 */
ACL_API void acl_aqueue_free(ACL_AQUEUE *queue, ACL_AQUEUE_FREE_FN free_fn);

/**
 * �Ӷ�������ȡһ��Ԫ��, ������ʱ, һֱ�ȵ���Ԫ�ؿ��û����
 * @param queue ACL_AQUEUE �ṹָ��
 * @return �û�ͨ�� acl_aqueue_push �����Ԫ��ָ��
 */
ACL_API void *acl_aqueue_pop(ACL_AQUEUE *queue);

/**
 * �Ӷ�������ȡһ��Ԫ��, ����ʱ, һֱ�ȵ���Ԫ�ؿ��û�ʱ�����
 * @param queue ACL_AQUEUE �ṹָ��
 * @param tmo_sec �Ӷ�������ȡԪ�صĳ�ʱʱ��, ��λΪ��
 * @param tmo_usec �Ӷ�������ȡԪ�صĳ�ʱʱ��, ��λΪ΢��
 * @return �û�ͨ�� acl_aqueue_push �����Ԫ��ָ��
 */
ACL_API void *acl_aqueue_pop_timedwait(ACL_AQUEUE *queue, int tmo_sec, int tmo_usec);

/**
 * ����������һ��Ԫ��
 * @param queue ACL_AQUEUE �ṹָ��
 * @param data �û�������ָ��
 * @return {int} ��Ӷ���Ԫ���Ƿ�ɹ�, 0: ok; < 0: error
 */
ACL_API int acl_aqueue_push(ACL_AQUEUE *queue, void *data);

/**
 * �����һ�ζ��в����Ĵ����, define as: ACL_AQUEUE_XXX
 * @param queue ACL_AQUEUE �ṹָ��
 * @return �����
 */
ACL_API int acl_aqueue_last_error(const ACL_AQUEUE *queue);

/**
 * ���ö���Ϊ�˳�״̬
 * @param queue ACL_AQUEUE �ṹָ��
 */
ACL_API void acl_aqueue_set_quit(ACL_AQUEUE *queue);

#ifdef __cplusplus
}
#endif
#endif

