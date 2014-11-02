#ifndef ACL_CHUNK_CHAIN_INCLUDE_H
#define ACL_CHUNK_CHAIN_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "acl_define.h"

/**
 * ���������Ͷ���
 */
typedef struct ACL_CHAIN ACL_CHAIN;

/**
 * ����һ������������
 * @param init_size {size_t} �������ݶ�̬�ڴ�ĳ�ʼ�ߴ��С
 * @param off_begin {acl_int64} �������ݿ����ʼλ��
 * @return {ACL_CHAIN*} ����������
 */
ACL_API ACL_CHAIN *acl_chain_new(size_t init_size, acl_int64 off_begin);

/**
 * �ͷ�����������
 * @param chain {ACL_CHAIN*} ����������
 */
ACL_API void acl_chain_free(ACL_CHAIN *chain);

/**
 * �����������ݿ����һ��ƫ��λ��
 * @param chain {ACL_CHAIN*} ����������
 * @param from_next {acl_int64} �������ݿ����һ��ƫ��λ��
 */
ACL_API void acl_chain_set_from_next(ACL_CHAIN *chain, acl_int64 from_next);

/**
 * �������������󣬲�����ʼλ������Ϊ����ֵ
 * @param chain {ACL_CHAIN*} ����������
 * @param off_begin {acl_int64} �����������ݿ����ʼλ��
 */
ACL_API void acl_chain_reset(ACL_CHAIN *chain, acl_int64 off_begin);

/**
 * ��õ�ǰ�������������������ݿ��е���һ��λ��
 * @param chain {ACL_CHAIN*} ����������
 * @return {acl_int64} �������ݿ����һ��λ��
 */
ACL_API acl_int64 acl_chain_from_next(ACL_CHAIN *chain);

/**
 * ��õ�ǰ�������������ʼλ��
 * @param chain {ACL_CHAIN*} ����������
 * @return {acl_int64} ����������ʼλ��
 */
ACL_API acl_int64 acl_chain_off_begin(ACL_CHAIN *chain);

/**
 * ��õ�ǰ���������������ݿ����ʼ�洢ָ���ַ
 * @param chain {ACL_CHAIN*} ����������
 * @return {const char*} �������ݿ����ʼ�洢ָ���ַ
 */
ACL_API const char *acl_chain_data(ACL_CHAIN *chain);

/**
 * ��õ�ǰ���������������ݿ�����ݳ���
 * @param chain {ACL_CHAIN*} ����������
 * @return {int} �������ݿ�����ݳ���
 */
ACL_API int acl_chain_data_len(ACL_CHAIN *chain);

/**
 * ��ǰ�������з��������ݿ�ĸ���
 * @param chain {ACL_CHAIN*} ����������
 * @return {int} ���������ݿ�ĸ���
 */
ACL_API int acl_chain_size(ACL_CHAIN *chain);

/**
 * ��õ�ǰ�������з��������ݿ�����������ܳ���
 * @param chain {ACL_CHAIN*} ����������
 * @return {int} ���������ݿ���ܳ���
 */
ACL_API int acl_chain_chunk_data_len(ACL_CHAIN *chain);

/**
 * �������������һ�����ݿ飬�ڲ��Զ�ȥ���ص�����
 * @param chain {ACL_CHAIN*} ����������
 * @param data {const void*} ���ݿ�ָ��
 * @param from {acl_int64} �������ݿ����ʼλ��
 * @param dlen {int} ���ݿ�ĳ���
 */
ACL_API void acl_chain_add(ACL_CHAIN *chain, const void *data,
	acl_int64 from, int dlen);

/**
 * ��ӡ�����ǰ���������������ݿ鼰���������ݿ����ʼλ����Ϣ
 * @param chain {ACL_CHAIN*} ����������
 */
ACL_API void acl_chain_list(ACL_CHAIN *chain);

#ifdef __cplusplus
}
#endif

#endif
