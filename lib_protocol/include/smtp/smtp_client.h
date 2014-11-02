#ifndef	__SMTP_CLIENT_INCLUDE_H__
#define	__SMTP_CLIENT_INCLUDE_H__

#include "lib_acl.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SMTP_DLL
# ifdef SMTP_EXPORTS
#  define SMTP_API __declspec(dllexport)
# else
#  define SMTP_API __declspec(dllimport)
# endif
#else
#  define SMTP_API
#endif

typedef struct SMTP_CLIENT {
	ACL_VSTREAM *conn;
	int   smtp_code;
	char* buf;
	int   size;
	unsigned int flag;
#define SMTP_FLAG_PIPELINING	(1 << 0)
#define SMTP_FLAG_AUTH          (1 << 1)
#define SMTP_FLAG_8BITMIME      (1 << 2)
#define SMTP_FLAG_DSN           (1 << 3)
#define SMTP_FLAG_VRFY          (1 << 4)
#define SMTP_FLAG_ETRN          (1 << 5)
#define SMTP_FLAG_SIZE          (1 << 6)
	int   message_size_limit;
} SMTP_CLIENT;

/**
 * Զ������ SMTP ������
 * @param addr {const char*} SMTP ��������ַ����ʽ��domain:port
 * @param conn_timeout {int} ���ӳ�ʱʱ��
 * @param rw_timeout {int} IO��д��ʱʱ��
 * @param line_limit {int} SMTP �Ự������ÿ�е���󳤶�����
 * @return {SMTP_CLIENT*} ���ӳɹ����طǿ�ֵ�����򷵻� NULL
 */
SMTP_API SMTP_CLIENT *smtp_open(const char *addr, int conn_timeout,
	int rw_timeout, int line_limit);

/**
 * �ر��� smtp_open �򿪵� SMTP ���Ӳ��ͷŶ���
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 */
SMTP_API void smtp_close(SMTP_CLIENT *client);

/**
 * ��� SMTP �������Ļ�ӭ��Ϣ
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_get_banner(SMTP_CLIENT *client);

/**
 * �� SMTP ���������� HELO/EHLO ����
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @param name {const char*} ������Ϣ��һ��������
 * @param ehlo {int} �� 0 ʱʹ�� EHLO������ʹ�� HELO
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */

SMTP_API int smtp_greet(SMTP_CLIENT *client, const char* name, int ehlo);

/**
 * �� SMTP ���������� HELO ����
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @param helo {const char*} ������Ϣ��һ��������
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_helo(SMTP_CLIENT *client, const char *helo);

/**
 * �� SMTP ���������� EHLO ����
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @param ehlo {const char*} ������Ϣ��һ��������
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_ehlo(SMTP_CLIENT *client, const char *ehlo);

/**
 * �� SMTP ������������֤��Ϣ
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @param user {const char*} SMTP �ʼ��˺�
 * @param pass {const char*} SMTP �ʼ��˺�����
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_auth(SMTP_CLIENT *client, const char *user, const char *pass);

/**
 * �� SMTP ���������� MAIL FROM ����
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @param from {const char*} ����������
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_mail(SMTP_CLIENT *client, const char *from);

/**
 * �� SMTP ���������� RCPT TO ����
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @param to {const char*} ����������
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_rcpt(SMTP_CLIENT *client, const char *to);

/**
 * �� SMTP ���������� DATA ����
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_data(SMTP_CLIENT *client);

/**
 * �� SMTP �����������ʼ������ݣ�����ѭ�����ñ�����ֱ�����ݷ������
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @param src {const char*} �����ʼ� MIME �����ʽ���ʼ�������
 * @param len {size_t} src ���ݳ���
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_send(SMTP_CLIENT *client, const char* src, size_t len);

/**
 * �� SMTP �����������ʼ������ݣ�����ѭ�����ñ�����ֱ�����ݷ������
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @param fmt {const char*} ��ʽ�ַ���
 * @param ... ���
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_printf(SMTP_CLIENT *client, const char* fmt, ...);

/**
 * �������ʼ����ݺ���ñ��������� SMTP �������ʼ��������
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_data_end(SMTP_CLIENT *client);

/**
 * �� SMTP ����������ָ����·�����ʼ��ļ�
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @param filepath {const char*} �ʼ��ļ�·��
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_send_file(SMTP_CLIENT *client, const char *filepath);

/**
 * �� SMTP ���������͸����ļ������ʼ�����
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @param int {ACL_VSTREAM*} �ʼ��ļ�������
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_send_stream(SMTP_CLIENT *client, ACL_VSTREAM *in);

/**
 * �� SMTP �����������˳�(QUIT)����
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_quit(SMTP_CLIENT *client);

/**
 * �� SMTP ���������� NOOP ����
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_noop(SMTP_CLIENT *client);

/**
 * �� SMTP ���������� RSET ����
 * @param client {SMTP_CLIENT*} SMTP ���Ӷ���
 * @return {int} 0 ��ʾ�ɹ�(SMTP_CLIENT::smtp_code ��ʾ�����룬
 *  SMTP_CLIENT::buf �洢��Ӧ����)�������ʾ����Ӧ�ùر����Ӷ���
 */
SMTP_API int smtp_rset(SMTP_CLIENT *client);

#ifdef __cplusplus
}
#endif

#endif
