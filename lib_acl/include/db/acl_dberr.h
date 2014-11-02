#ifndef ACL_DBERR_INCLUDE_H
#define ACL_DBERR_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#define ACL_DB_OK		0       /* �ɹ� */
#define ACL_DB_ERR_CALLBACK	1       /* �û��Ļص���������ʧ�� */
#define ACL_DB_ERR_SELECT	100     /* �û���ѯ���ʧ�� */
#define ACL_DB_ERR_UPDATE	101     /* �û��������ʧ�� */
#define ACL_DB_ERR_EMPTY	102     /* ��ѯ���Ϊ�� */
#define ACL_DB_ERR_STORE	103     /* ��ѯ����õĽ����洢�ڱ���ʱʧ�� */
#define ACL_DB_ERR_AFFECTED	104     /* �����������ݿ���ʵ�ʸ��²��� */
#define	ACL_DB_ERR_ALLOC	105     /* �ڲ������ڴ�ʧ�� */

#ifdef __cplusplus
}
#endif

#endif

