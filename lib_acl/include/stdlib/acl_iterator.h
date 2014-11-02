#ifndef	ACL_ITERATOR_INCLUDE_H
#define	ACL_ITERATOR_INCLUDE_H

typedef struct ACL_ITER ACL_ITER;

/**
 * ACL �������ݽṹ�õ�ͨ�õ������ṹ����
 */
struct ACL_ITER {
	void *ptr;		/**< ������ָ��, ��������� */
	void *data;		/**< �û�����ָ�� */
	int   dlen;		/**< �û����ݳ���, ʵ���߿����ô�ֵҲ�ɲ����� */
	const char *key;	/**< ��Ϊ��ϣ��ĵ�����, ��Ϊ��ϣ��ֵ��ַ */
	int   klen;		/**< ��ΪACL_BINHASH������, ��Ϊ������ */
	int   i;		/**< ��ǰ�������������е�λ������ */
	int   size;		/**< ��ǰ������Ԫ���ܸ��� */
};

/**
 * �������������Ԫ��
 * @param iter {ACL_ITER}
 * @param container {void*} ������ַ
 * @examples: samples/iterator/
 */
#define	ACL_FOREACH(iter, container)  \
        for ((container)->iter_head(&(iter), (container));  \
             (iter).ptr;  \
             (container)->iter_next(&(iter), (container)))

/**
 * �������������Ԫ��
 * @param iter {ACL_ITER}
 * @param container {void*} ������ַ
 * @examples: samples/iterator/
 */
#define	ACL_FOREACH_REVERSE(iter, container)  \
        for ((container)->iter_tail(&(iter), (container));  \
             (iter).ptr;  \
             (container)->iter_prev(&(iter), (container)))

/**
 * ��õ�ǰ����ָ����ĳ���������ĳ�Ա�ṹ���Ͷ���
 * @param iter {ACL_ITER}
 * @param container {void*} ������ַ
 */
#define	ACL_ITER_INFO(iter, container)  \
	(container)->iter_info(&(iter), (container))

#define	acl_foreach_reverse	ACL_FOREACH_REVERSE
#define	acl_foreach		ACL_FOREACH
#define	acl_iter_info		ACL_ITER_INFO

#endif
