#ifndef ACL_BITS_MAP_INCLUDE_H
#define ACL_BITS_MAP_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "acl_define.h"

/**
 * λӳ��ṹ���Ͷ���
 */
typedef struct ACL_BITS_MASK {
	char   *data;		/**< bit mask */
	size_t  data_len;	/**< data byte count */
} ACL_BITS_MASK;

/**
 * Bits per byte, byte in vector, bit offset in byte, bytes perset
 */
#define	ACL_BITS_MASK_NBBY	(8)
#define	ACL_BITS_MASK_FD_BYTE(number, mask) \
	(((unsigned char *) (mask)->data)[(number) / ACL_BITS_MASK_NBBY])
#define	ACL_BITS_MASK_FD_BIT(number)	(1 << ((number) % ACL_BITS_MASK_NBBY))
#define	ACL_BITS_MASK_BYTES_NEEDED(len) \
	(size_t) (((acl_int64) (len) + (ACL_BITS_MASK_NBBY - 1)) / ACL_BITS_MASK_NBBY)
#define	ACL_BITS_MASK_BYTE_COUNT(mask)	((mask)->data_len)

/* Memory management. */
/**
 * ����λӳ�����ռ�
 * @param mask {ACL_BITS_MASK*) ACL_BITS_MASK ָ��
 * @param nmax {size_t/unsigned int/unsigned short/unsigned char} ���ֵ���Դ�ֵ��
 *  ���� (mask)->data ռ���ڴ�ռ��С���磺�� nmax=4294967295, ���������ֵʱ����
 *  (mask)->data_len=536870912, �� (mask)->data ռ�� 536870912 Bytes; �� nmax=65535,
 *  ����� unsigned short ֵʱ���� (mask)->data_len=8192, �� (mask)->data ռ�� 8192 �ֽ�
 */
#define	ACL_BITS_MASK_ALLOC(mask, nmax) do { \
	size_t _byte_len = ACL_BITS_MASK_BYTES_NEEDED(nmax); \
	(mask)->data = (char*) acl_mymalloc(_byte_len); \
	memset((mask)->data, 0, _byte_len); \
	(mask)->data_len = _byte_len; \
} while (0)

/**
 * �ط���λӳ�����ռ�
 * @param mask {ACL_BITS_MASK*) ACL_BITS_MASK ָ��
 * @param nmax {size_t/unsigned int/unsigned short/unsigned char} ���ֵ���Դ�ֵ��
 *  ���� (mask)->data ռ���ڴ�ռ��С���磺�� nmax=4294967295, ���������ֵʱ����
 *  (mask)->data_len=536870912, �� (mask)->data ռ�� 536870912 Bytes; �� nmax=65535,
 *  ����� unsigned short ֵʱ���� (mask)->data_len=8192, �� (mask)->data ռ�� 8192 �ֽ�
 */
#define	ACL_BITS_MASK_REALLOC(mask, nmax) do { \
	size_t _byte_len = ACL_BITS_MASK_BYTES_NEEDED(nmax); \
	size_t _old_len = (mask)->data_len; \
	(mask)->data = (char*) acl_myrealloc((mask)->data, _byte_len); \
	memset((mask)->data + _old_len, 0, _byte_len - _old_len); \
	(mask)->data_len = _byte_len; \
} while (0)

/**
 * �ͷ�λӳ�������ڲ���̬�ռ�
 * @param mask {ACL_BITS_MASK*) ACL_BITS_MASK ָ��
 */
#define	ACL_BITS_MASK_FREE(mask)	acl_myfree((mask)->data)

/* Set operations, modeled after FD_ZERO/SET/ISSET/CLR. */

/**
 * ��λӳ�������ڲ���̬�ռ�����
 * @param mask {ACL_BITS_MASK*) ACL_BITS_MASK ָ��
 */
#define	ACL_BITS_MASK_ZERO(mask) \
	memset((mask)->data, 0, (mask)->data_len);

/**
 * ������ӳ��Ϊλ�洢��λӳ�����Ķ�̬�ռ���
 * @param number {unsigned int} ����ֵ
 * @param mask {ACL_BITS_MASK*) ACL_BITS_MASK ָ��
 */
#define	ACL_BITS_MASK_SET(number, mask) \
	(ACL_BITS_MASK_FD_BYTE((number), (mask)) |= ACL_BITS_MASK_FD_BIT(number))

/**
 * �ж�ĳ�������Ƿ�洢��λӳ�����Ķ�̬�ռ���
 * @param number {unsigned int} ����ֵ
 * @param mask {ACL_BITS_MASK*) ACL_BITS_MASK ָ��
 * @return {int} 0: �����ڣ�!= 0: ����
 */
#define	ACL_BITS_MASK_ISSET(number, mask) \
	(ACL_BITS_MASK_FD_BYTE((number), (mask)) & ACL_BITS_MASK_FD_BIT(number))

/**
 * ��ĳ��������λӳ�����Ķ�̬�ռ��������
 * @param number {unsigned int} ����ֵ
 * @param mask {ACL_BITS_MASK*) ACL_BITS_MASK ָ��
 */
#define	ACL_BITS_MASK_CLR(number, mask) \
	(ACL_BITS_MASK_FD_BYTE((number), (mask)) &= ~ACL_BITS_MASK_FD_BIT(number))

#ifdef __cplusplus
}
#endif

#endif
