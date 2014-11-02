#ifndef	ACL_READLINE_INCLUDE_H
#define	ACL_READLINE_INCLUDE_H

#include "acl_vstream.h"
#include "acl_vstring.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * ���������ж�ȡһ���߼�������. ���н������ԣ����һ�еķǿո���ʼ�ַ�Ϊ "#" ��
 * ����Ҳ�����ԣ�һ���߼��е��׸��ַ������Ƿǿո񡢷� "#" �ַ���������еĺ�����
 * �Կո��TAB��ʼ����ú������ڴ��߼���
 * @param buf {ACL_VSTRING*} �洢����Ļ�����������Ϊ��
 * @param fp {ACL_VSTREAM*} ���������������Ϊ��
 * @param lineno {int} ����ǿգ����¼���߼��������е���ʵ�к�
 * @return {ACL_VSTRING*} ���δ�����߼��У��򷵻ؿգ����򷵻����� buf ����ͬ��ַ
 */
ACL_API ACL_VSTRING *acl_readlline(ACL_VSTRING *buf, ACL_VSTREAM *fp, int *lineno);

#ifdef	__cplusplus
}
#endif

#endif
