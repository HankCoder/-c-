#ifndef	__LIB_HTTP_UTIL_INCLUDE_H__
#define	__LIB_HTTP_UTIL_INCLUDE_H__

#include "lib_http_struct.h"

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct HTTP_UTIL {
	HTTP_HDR_REQ *hdr_req;		/**< HTTP ����ͷ */
	HTTP_HDR_RES *hdr_res;		/**< HTTP ��Ӧͷ */
	HTTP_RES *http_res;		/**< HTTP ��Ӧ�� */
	char  server_addr[256];		/**< Զ�� HTTP ��������ַ */
	ACL_VSTREAM *stream;		/**< �� HTTP ������������������ */
	int   conn_timeout;		/**< ���� HTTP �������ĳ�ʱʱ�� */
	int   rw_timeout;		/**< �� HTTP ������ͨ��ʱÿ�� IO �ĳ�ʱʱ�� */
	ACL_VSTRING *req_buf;		/**< ������ */
	int   res_body_dlen;		/**< HTTP ��Ӧ������ĳ��� */
	ACL_VSTREAM *dump_stream;	/**< ת���������ݵ��� */
	unsigned int   flag;		/**< ��־λ */
#define	HTTP_UTIL_FLAG_SET_DUMP_FILE	(1 << 0)	/**< ����ת����Ӧ�����ļ� */
#define	HTTP_UTIL_FLAG_SET_DUMP_STREAM	(1 << 1)	/**< ����ת����Ӧ������ */
#define	HTTP_UTIL_FLAG_HAS_RES_BODY	(1 << 2)	/**< �� HTTP ��Ӧ�� */
#define	HTTP_UTIL_FLAG_NO_RES_BODY	(1 << 3)	/**< �� HTTP ��Ӧ�� */
} HTTP_UTIL;

/**
 * ����һ�� HTTP_UTIL �������
 * @param url {const char*} ���������� url
 * @param method {const char*} ���󷽷�����Ч�����󷽷��У�GET, POST, HEAD, CONNECT
 * @return {HTTP_UTIL*}
 */
HTTP_API HTTP_UTIL *http_util_req_new(const char *url, const char *method);

/**
 * ����һ�� HTTP_UTIL ��Ӧ����
 * @param status {int} ״̬�룬��Ч��״̬��Ϊ: 1xx, 2xx, 3xx, 4xx, 5xx
 * @return {HTTP_UTIL*}
 */
HTTP_API HTTP_UTIL *http_util_res_new(int status);

/**
 * �ͷ�һ�� HTTP_UTIL ����
 * @param http_util {HTTP_UTIL*}
 */
HTTP_API void http_util_free(HTTP_UTIL *http_util);

/**
 * ���� HTTP ����ͷ��Ϣ, ��: Accept-Encoding: gzip,deflate
 * @param http_util {HTTP_UTIL*}
 * @param name {const char*} ����ͷ���ֶ�����, �� Accept-Encoding
 * @param value {const char*} ����ͷ���ֶε�ֵ, �� gzip,deflate
 */
HTTP_API void http_util_set_req_entry(HTTP_UTIL *http_util, const char *name, const char *value);

/**
 * �ر� HTTP ����ͷ�е�ĳ�������ֶΣ��������ֶβ��ᷢ��������
 * @param http_util {HTTP_UTIL*}
 * @param name {const char*} ����ͷ���ֶ�����, �� Accept-Encoding
 */
HTTP_API void http_util_off_req_entry(HTTP_UTIL *http_util, const char *name);

/**
 * �������ͷ��ĳ���ֶε�ֵ
 * @param http_util {HTTP_UTIL*}
 * @param name {const char*} ����ͷ���ֶ�����, �� Accept-Encoding
 * @return {char*} ����ǿ���Ϊ�����ֶ�ֵ������������ֶβ�����
 */
HTTP_API char *http_util_get_req_value(HTTP_UTIL *http_util, const char *name);

/**
 * �������ͷ��ĳ���ֶε� HTTP_HDR_ENTRY ����
 * @param http_util {HTTP_UTIL*}
 * @param name {const char*} ����ͷ���ֶ�����, �� Accept-Encoding
 * @return {HTTP_HDR_ENTRY*} ��Ϊ�����ʾ���ֶβ�����
 */
HTTP_API HTTP_HDR_ENTRY *http_util_get_req_entry(HTTP_UTIL *http_util, const char *name);

/**
 * ��������ͷ�� HTTP ����������ݳ���
 * @param http_util {HTTP_UTIL*}
 * @param len {int} HTTP �����峤��(���� >= 0)
 */
HTTP_API void http_util_set_req_content_length(HTTP_UTIL *http_util, int len);

/**
 * ��������ͷ�� HTTP �Ự���ֳ����ӵĴ��ʱ��(��λΪ��)
 * @param http_util {HTTP_UTIL*}
 * @param timeout {int} HTTP �����ӵĴ��ʱ��(��λΪ��)
 */
HTTP_API void http_util_set_req_keep_alive(HTTP_UTIL *http_util, int timeout);

/**
 * ��������ͷ�� Connection �ֶ�
 * @param http_util {HTTP_UTIL*}
 * @param value {const char*} �ֶ�ֵ����Ч���ֶ�Ϊ: keep-alive, close
 */
HTTP_API void http_util_set_req_connection(HTTP_UTIL *http_util, const char *value);

/**
 * ��������ͷ�е� Referer �ֶ�
 * @param http_util {HTTP_UTIL*}
 * @param refer {const char*} ������ url, ��: http://www.test.com
 */
HTTP_API void http_util_set_req_refer(HTTP_UTIL *http_util, const char *refer);

/**
 * ��������ͷ�е� Cookie �ֶΣ����õ���׷�ӷ�ʽ
 * @param http_util {HTTP_UTIL*}
 * @param name {const char*} Cookie ����
 * @param value {const char*} Cookie ֵ
 */
HTTP_API void http_util_set_req_cookie(HTTP_UTIL *http_util, const char *name, const char *value);

/**
 * ���� HTTP �����������ַ
 * @param http_util {HTTP_UTIL*}
 * @param proxy {const char*} �����������ַ����Ч��ʽΪ: IP:PORT, DOMAIN:PORT,
 *  ��: 192.168.0.1:80, 192.168.0.2:8088, www.g.cn:80
 */
HTTP_API void http_util_set_req_proxy(HTTP_UTIL *http_util, const char *proxy);

/**
 * ���� HTTP ��Ӧ���ת���������ú� HTTP ��Ӧ�����ݱ��ͬʱ�����ת��
 * @param http_util {HTTP_UTIL*}
 * @param stream {ACL_VSTREAM *} ת����
 */
HTTP_API void http_util_set_dump_stream(HTTP_UTIL *http_util, ACL_VSTREAM *stream);

/**
 * ���� HTTP ��Ӧ���ת���ļ������ú� HTTP ��Ӧ�����ݱ��ת���ڸ��ļ�
 * @param http_util {HTTP_UTIL*}
 * @param filename {const char*} ת���ļ���
 * @return {int} �������ֵ < 0 ���ʾ�޷��򿪸��ļ�, �����ʾ���ļ��ɹ�
 */
HTTP_API int http_util_set_dump_file(HTTP_UTIL *http_util, const char *filename);

/**
 * ��Զ�� HTTP �������������������ӣ�ͬʱ���� HTTP ����ͷ���ݲ��ҽ�������
 * �����½�������������
 * @param http_util {HTTP_UTIL*}
 * @return {int} 0: �ɹ�; -1: �޷������ӻ�������ͷ����ʧ��
 */
HTTP_API int http_util_req_open(HTTP_UTIL *http_util);

/**
 * ������ POST ����ʱ������ͨ���˺����� HTTP ������������������������������,
 * ��һ����������У����Զ�ε��ñ�����ֱ������������������
 * @param http_util {HTTP_UTIL*}
 * @param data {const char*} ���η��͵����ݵ�ַ������ǿ�
 * @param dlen {size_t} data ���ݳ���, ������� 0
 * @param {int} > 0 ��ʾ���γɹ����͵�����; -1: ��ʾ��������ʧ��, Ӧ����
 *  http_util_free �ر����������ͷ��ڴ���Դ
 */
HTTP_API int http_util_put_req_data(HTTP_UTIL *http_util, const char *data, size_t dlen);

/**
 * �������������ݺ���ô˺����� HTTP ��������ȡ������ HTTP ��Ӧͷ
 * @param http_util {HTTP_UTIL*}
 * @return {int} 0: �ɹ�; -1: ʧ��
 */
HTTP_API int http_util_get_res_hdr(HTTP_UTIL *http_util);

/**
 * �� HTTP ��Ӧͷ�л��ĳ���ֶ�ֵ
 * @param http_util {HTTP_UTIL*}
 * @param name {const char*} �ֶ�����, �� Content-Length
 * @return {char*} ��Ӧ name ���ֶ�ֵ, ���Ϊ�����ʾ���ֶβ�����
 */
HTTP_API char *http_util_get_res_value(HTTP_UTIL *http_util, const char *name);

/**
 * �� HTTP ��Ӧͷ�л��ĳ���ֶζ���
 * @param http_util {HTTP_UTIL*}
 * @param name {const char*} �ֶ�����, �� Content-Length
 * @return {HTTP_HDR_ENTRY*} ��Ӧ name ���ֶζ���, ���Ϊ�����ʾ���ֶβ�����
 */
HTTP_API HTTP_HDR_ENTRY *http_util_get_res_entry(HTTP_UTIL *http_util, const char *name);

/**
 * ���� HTTP ��Ӧͷ�е�ĳ���ֶ�ֵ
 * @param http_util {HTTP_UTIL*}
 * @param name {const char*} �ֶ�����, �� Content-Type
 * @param value {const char*} �ֶ�ֵ, �� text/html
 */
HTTP_API void http_util_set_res_entry(HTTP_UTIL *http_util, const char *name, const char *value);

/**
 * �ر� HTTP ��Ӧͷ�е�ĳ���ֶ�
 * @param http_util {HTTP_UTIL*}
 * @param name {const char*} �ֶ�����, �� Content-Type
 */
HTTP_API void http_util_off_res_entry(HTTP_UTIL *http_util, const char *name);

/**
 * ���� HTTP ��Ӧͷ����ô˺����ж��Ƿ��� HTTP ��Ӧ��
 * @param http_util {HTTP_UTIL*}
 * @return {int}  0: ��ʾ����Ӧ��; !0: ��ʾ����Ӧ��
 */
HTTP_API int http_util_has_res_body(HTTP_UTIL *http_util);

/**
 * ���� HTTP ��Ӧͷ����ô˺����� HTTP ��������ȡ HTTP ���������ݣ���Ҫ��������
 * �˺�����ֱ������ֵ <= 0, ���֮ǰ������ת���ļ���ת�����ڶ�ȡ���ݹ�����ͬʱ��
 * ����һ�����ݸ�ת���ļ���ת����
 * @param http_util {HTTP_UTIL*}
 * @param buf {char *} �洢 HTTP ��Ӧ��Ļ�����
 * @param size {size_t} buf �Ŀռ��С
 * @return {int} <= 0: ��ʾ������; > 0: ��ʾ���ζ��������ݳ���
 */
HTTP_API int http_util_get_res_body(HTTP_UTIL *http_util, char *buf, size_t size);

/**
 * ��ĳ�� url ����Ӧ������ת����ĳ���ļ���
 * @param url {const char*} �������� url, ��: http://www.g.cn
 * @param dump {const char*} ת���ļ���
 * @param {int} ��������Ӧ�����ݳ���, >=0: ��ʾ�ɹ�, -1: ��ʾʧ��
 */
HTTP_API int http_util_dump_url(const char *url, const char *dump);

/**
 * ��ĳ�� url ����Ӧ������ת����ĳ������
 * @param url {const char*} �������� url, ��: http://www.g.cn
 * @param stream {ACL_VSTREAM *} ת����
 * @param {int} ��������Ӧ�����ݳ���, >=0: ��ʾ�ɹ�, -1: ��ʾʧ��
 */
HTTP_API int http_util_dump_url_to_stream(const char *url, ACL_VSTREAM *stream);

#ifdef	__cplusplus
}
#endif

#endif
