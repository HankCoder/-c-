#ifndef	ACL_VSTRING_VSTREAM_INCLUDE_H
#define	ACL_VSTRING_VSTREAM_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_vstring.h"
#include "acl_vstream.h"

/**
 * ���������ж�һ�����ݣ�ֱ������һ�С����������Ϊֹ
 * @param vp {ACL_VSTRING*} �洢����Ļ�����
 * @param fp {ACL_VSTREAM*} ������
 * @return {int} ���һ���ַ���ASCII���� ACL_VSTREAM_EOF
 */
ACL_API int acl_vstring_gets(ACL_VSTRING *vp, ACL_VSTREAM *fp);

/**
 * ���������ж�һ�����ݣ�ֱ������һ�С����������Ϊֹ, �������ݲ�����
 * "\r\n" �� "\n"
 * @param vp {ACL_VSTRING*} �洢����Ļ�����
 * @param fp {ACL_VSTREAM*} ������
 * @return {int} ���һ���ַ���ASCII���� ACL_VSTREAM_EOF
 */
ACL_API int acl_vstring_gets_nonl(ACL_VSTRING *vp, ACL_VSTREAM *fp);

/**
 * ���������ж��� "\0" ��β�����ݻ����Ϊֹ, �������� "\0"
 * @param vp {ACL_VSTRING*} �洢����Ļ�����
 * @param fp {ACL_VSTREAM*} ������
 * @return {int} ���һ���ַ���ASCII���� ACL_VSTREAM_EOF
 */
ACL_API int acl_vstring_gets_null(ACL_VSTRING *vp, ACL_VSTREAM *fp);

/**
 * ���������ж�һ�����ݣ����������ݳ��Ȳ��ó����޶�ֵ
 * @param vp {ACL_VSTRING*} �洢����Ļ�����
 * @param fp {ACL_VSTREAM*} ������
 * @param bound {ssize_t} ����������󳤶�����
 * @return {int} ���һ���ַ���ASCII���� ACL_VSTREAM_EOF
 */
ACL_API int acl_vstring_gets_bound(ACL_VSTRING *vp, ACL_VSTREAM *fp, ssize_t bound);

/**
 * ���������ж�һ�����ݣ����������ݳ��Ȳ��ó����޶�ֵ, �ҽ���в����� "\n" �� "\r\n"
 * @param vp {ACL_VSTRING*} �洢����Ļ�����
 * @param fp {ACL_VSTREAM*} ������
 * @param bound {ssize_t} ����������󳤶�����
 * @return {int} ���һ���ַ���ASCII���� ACL_VSTREAM_EOF
 */
ACL_API int acl_vstring_gets_nonl_bound(ACL_VSTRING *vp, ACL_VSTREAM *fp, ssize_t bound);

/**
 * ���������ж��� "\0" ��β�����ݻ����򳬹���󳤶�����Ϊֹ������в����� "\0"
 * @param vp {ACL_VSTRING*} �洢����Ļ�����
 * @param fp {ACL_VSTREAM*} ������
 * @param bound {ssize_t} ����������󳤶�����
 * @return {int} ���һ���ַ���ASCII���� ACL_VSTREAM_EOF
 */
ACL_API int acl_vstring_gets_null_bound(ACL_VSTRING *vp, ACL_VSTREAM *fp, ssize_t bound);

/**
 * Backwards compatibility for code that still uses the vstring_fgets()
 * interface. Unfortunately we can't change the macro name to upper case.
 */

#define acl_vstring_fgets(s, p) \
	(acl_vstring_gets((s), (p)) == ACL_VSTREAM_EOF ? 0 : (s))
#define acl_vstring_fgets_nonl(s, p) \
	(acl_vstring_gets_nonl((s), (p)) == ACL_VSTREAM_EOF ? 0 : (s))
#define acl_vstring_fgets_null(s, p) \
	(acl_vstring_gets_null((s), (p)) == ACL_VSTREAM_EOF ? 0 : (s))
#define acl_vstring_fgets_bound(s, p, l) \
	(acl_vstring_gets_bound((s), (p), (l)) == ACL_VSTREAM_EOF ? 0 : (s))
#define acl_vstring_fgets_nonl_bound(s, p, l) \
	(acl_vstring_gets_nonl_bound((s), (p), (l)) == ACL_VSTREAM_EOF ? 0 : (s))

#ifdef	__cplusplus
}
#endif

#endif
