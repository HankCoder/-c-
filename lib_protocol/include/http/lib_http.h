#ifndef	__LIB_HTTP_INCLUDE_H__
#define	__LIB_HTTP_INCLUDE_H__

#include "lib_http_status.h"
#include "lib_http_struct.h"

#ifdef	__cplusplus
extern "C" {
#endif

/*---------------------------- ͨ�� HTTP ͷ�������� --------------------------*/
/* in http_hdr.c */

/**
 * ����һ��ͨ��HTTPЭ��ͷ�Ľṹ����
 * @param size {size_t} ��������ڴ��С, ���� HTTP_HDR_REQ �� HTTP_HDR_RES �ĳߴ�
 * @return {HTTP_HDR*} !NULL: ����һ��HTTP_HDR�ṹָ��; NULL: ����.
 */
HTTP_API HTTP_HDR *http_hdr_new(size_t size);

/**
 * ��ԴHTTPͨ��ͷ�����ڲ���Ա������һ���µ�HTTPͨ��ͷ�ṹ��
 * @param src {const HTTP_HDR*} ԴHTTPͨ��ͷ���󣬲���Ϊ��
 * @param dst {HTTP_HDR*} Ŀ��HTTPͨ��ͷ���󣬲���Ϊ��
 */
HTTP_API void http_hdr_clone(const HTTP_HDR *src, HTTP_HDR *dst);

/**
 * �ͷ�һ��HTTP_HDR�ṹ�ڴ�
 * @param hh {HTTP_HDR*} ���͵�����ָ�룬����Ϊ��
 */
HTTP_API void http_hdr_free(HTTP_HDR *hh);

/**
 * ����һ��HTTPͨ��ͷ��״̬���ͷ��ڲ���Ա��������Ҫ����keep-alive�ĳ����Ӷ������
 * @param hh {HTTP_HDR*} HTTPͨ��ͷ���͵�����ָ�룬����Ϊ��
 */
HTTP_API void http_hdr_reset(HTTP_HDR *hh);

/**
 * �� HTTP_HDR ͷ������һ����Ŀ
 * @param hh {HTTP_HDR*} ͨ��ͷ���͵�����ָ�룬����Ϊ��
 * @param entry {HTTP_HDR_ENTRY*} HTTPͷ��Ŀ�ṹָ��, ����Ϊ��
 */
HTTP_API void http_hdr_append_entry(HTTP_HDR *hh, HTTP_HDR_ENTRY *entry);

/**
 * ������������, ������Э��, ��/�ΰ汾�ţ������������ͨ��HTTPͷ�ṹ��
 * @param hh {HTTP_HDR*} ���͵�����ָ�룬����Ϊ��
 * @param data {const char*} ���ݸ�ʽ��Ϊ: HTTP/1.0
 * @return {int} 0: OK; < 0: error.
 */
HTTP_API int http_hdr_parse_version(HTTP_HDR *hh, const char *data);

/**
 * �������е�ͨ��HTTPЭ��ͷ���洢�� hh �ṹ��
 * @param hh {HTTP_HDR*} ͨ��HTTPͷ���͵�����ָ�룬����Ϊ��
 * @return {int} 0: ok; < 0: error
 */
HTTP_API int http_hdr_parse(HTTP_HDR *hh);

/**
 * �ɴ���� name, value �Բ���һ�� HTTP_HDR_ENTRY ����
 * @param name {const char*} ������
 * @param value {const char*} ����ֵ
 * @return {HTTP_HDR_ENTRY*} ���Ϊ��������Ϊ�����������
 */
HTTP_API HTTP_HDR_ENTRY *http_hdr_entry_build(const char *name, const char *value);

/**
 * ���ݴ����һ�����ݽ��з���, ����һ�� HTTP_HDR_ENTRY
 * @param data {const char*} HTTP Э��ͷ�е�һ������, ��: Content-Length: 200
 * @return {HTTP_HDR_ENTRY*} !NULL: ok; NULL: err.
 */
HTTP_API HTTP_HDR_ENTRY *http_hdr_entry_new(const char *data);
HTTP_API HTTP_HDR_ENTRY *http_hdr_entry_head(char *data);
HTTP_API HTTP_HDR_ENTRY *http_hdr_entry_new2(char *data);

/**
 * ��ȡһ�� HTTP_HDR_ENTRY ��Ŀ
 * @param hh {HTTP_HDR*} ͨ��HTTPͷ���͵�����ָ�룬����Ϊ��
 * @param name {const char*} �� HTTP_HDR_ENTRY ��Ŀ�ı�ʶ��, ����Ϊ��. ��: Content-Length.
 * @return ret {HTTP_HDR_ENTRY *} ret != NULL: ok; ret == NULL: ����򲻴���.
 */
HTTP_API HTTP_HDR_ENTRY *http_hdr_entry(const HTTP_HDR *hh, const char *name);

/**
 * ��ȡHTTPЭ��ͷ��ĳ��ʵ��ͷ�ı���ֵ����ĳ��ʵ��ͷΪ��Host: www.test.com
 * Ҫ��� Host ������ֵ�����øú���������ȡ�� www.test.com
 * @param hh {HTTP_HDR*} ͨ��HTTPͷ���͵�����ָ�룬����Ϊ��
 * @param name {const char*} �� HTTP_HDR_ENTRY ��Ŀ�ı�ʶ��, ����Ϊ��. ��: Content-Length
 * @return ret {char*} ret != NULL: ok; ret == NULL: ����򲻴���.
 */
HTTP_API char *http_hdr_entry_value(const HTTP_HDR *hh, const char *name);

/**
 * �� HTTP ͷ�е�ĳ���ֶν����滻, �ù��������Ҫ��Ϊ��ʵ�� keep-alive �ֶε��滻
 * @param hh {HTTP_HDR*} ͨ��HTTPͷ���͵�����ָ�룬����Ϊ��
 * @param name {const char*} �� HTTP_HDR_ENTRY ��Ŀ�ı�ʶ��, ����Ϊ��. ��: Content-Length
 * @param value {const char*} �� name �ֶ�����Ӧ���µ�ֵ
 * @param force {int} ������滻���ֶ���ԭʼHTTP�����ﲻ����, ��ǿ�в����µ� entry �ֶβ�
 *  ����������ͷ, ����ֵΪ��0ֵʱ����ǿ�����, ��������name�������ﲻ���������.
 * @return {int} 0 ��ʾ�滻�ɹ�; < 0 ��ʾ������������� name �ֶ��ڸ�HTTP����ͷ�ﲻ����
 */
HTTP_API int http_hdr_entry_replace(HTTP_HDR *hh, const char *name, const char *value, int force);

/**
 * �� HTTP ͷ�е�ĳ���ֶ��а���ĳ���ַ�����Դ�ַ��������滻, ����֧�ֶ��ƥ���滻
 * @param hh {HTTP_HDR*} ͨ��HTTPͷ���͵�����ָ�룬����Ϊ��
 * @param name {const char*} �� HTTP_HDR_ENTRY ��Ŀ�ı�ʶ��, ����Ϊ��. ��: Cookie
 * @param from {const char*} �滻ʱ��Դ�ַ���
 * @param to {const char*} �滻ʱ��Ŀ���ַ���
 * @param ignore_case {int} �ڲ����滻ʱ�Ƿ����Դ�ַ����Ĵ�Сд
 * @return {int} 0: ��ʾδ���κ��滻, > 0: ��ʾ�滻�Ĵ���
 */
HTTP_API int http_hdr_entry_replace2(HTTP_HDR *hh, const char *name,
        const char *from, const char *to, int ignore_case);

/**
 * ��ֹHTTPЭ��ͷ�е�ĳ��
 * @param hh {HTTP_HDR* } ͨ��HTTPͷ���͵�����ָ�룬����Ϊ��
 * @param name {const char*} �� HTTP_HDR_ENTRY ��Ŀ�ı�ʶ��, ����Ϊ��. ��: Content-Length
 */
HTTP_API void http_hdr_entry_off(HTTP_HDR *hh, const char *name);

/**
 * �������HTTPЭ��ͷ�����ݣ�������ӿ�
 * @param hh {HTTP_HDR*} ͨ��HTTPͷ���͵�����ָ�룬����Ϊ��
 * @param msg {const char*} �û�ϣ����ͷ����Ϣһ��������Զ�����Ϣ, ����Ϊ��
 */
HTTP_API void http_hdr_print(const HTTP_HDR *hh, const char *msg);

/**
 * �������HTTPЭ��ͷ�����ݣ�������ӿ�
 * @param fp {ACL_VSTREAM*} ĳ����ָ�룬���������ᶨ������������(����Ϊ���������ļ���)
 * @param hh {HTTP_HDR*} ͨ��HTTPͷ���͵�����ָ�룬����Ϊ��
 * @param msg {const char*} �û�ϣ����ͷ����Ϣһ��������Զ�����Ϣ, ����Ϊ��
*/
HTTP_API void http_hdr_fprint(ACL_VSTREAM *fp, const HTTP_HDR *hh, const char *msg);

/*-------------------------------- HTTP ����ͷ�������� -----------------------*/
/* in http_hdr_req.c */

/**
 * ����һ�������HTTPЭ��ͷ����
 * @return {HTTP_HDR_REQ*} HTTP����ͷ����
 */
HTTP_API HTTP_HDR_REQ *http_hdr_req_new(void);

/**
 * ���������URL������ķ�����HTTP�汾����һ��HTTP����ͷ����
 * @param url {const char*} �����URL��������������URL���磺
 *  http://www.test.com/path/proc?name=value
 *  http://www.test.com/path/proc
 *  http://www.test.com/
 * @param method {const char*} HTTP���󷽷�������Ϊ����֮һ��
 *  GET, POST, CONNECT, HEAD, ��Ҫע����붼Ϊ��д
 * @param version {const char *} HTTP�汾������Ϊ����֮һ��
 *  HTTP/1.0, HTTP/1.1
 * @return {HTTP_HDR_REQ*} HTTP����ͷ����
 */
HTTP_API HTTP_HDR_REQ *http_hdr_req_create(const char *url,
		const char *method, const char *version);

/**
 * ��¡һ��HTTP����ͷ���󣬵����������е� chat_ctx, chat_free_ctx_fn
 * ������Ա����
 * @param hdr_req {const HTTP_HDR_REQ*} HTTP����ͷ����
 * @return {HTTP_HDR_REQ*} ��¡��HTTP����ͷ����
 */
HTTP_API HTTP_HDR_REQ *http_hdr_req_clone(const HTTP_HDR_REQ* hdr_req);

/**
 * �����ϴ�HTTP����ͷ���ݼ��ض����URL����һ���µ�HTTP����ͷ
 * @param hh {const HTTP_HDR_REQ*} �ϴε�HTTP����ͷ����
 * @param url {const char *} �ض����URL������� http[s]:// ǰ׺������Ϊ
 *  ��������URL���µ� Host �ֶν��ɸ�URL����ȡ��������̳�ԴHTTP����ͷ��
 *  �� Host �ֶ�
 * @return {HTTP_HDR_REQ*} �²������ض����HTTP����ͷ
 */
HTTP_API HTTP_HDR_REQ *http_hdr_req_rewrite(const HTTP_HDR_REQ *hh, const char *url);

/**
 * ����HTTP����ͷ���ݼ��ض����URL�������ø�HTTP����ͷ����Ϣ
 * @param hh {const HTTP_HDR_REQ*} �ϴε�HTTP����ͷ����
 * @param url {const char *} �ض����URL������� http[s]:// ǰ׺������Ϊ
 *  ��������URL���µ� Host �ֶν��ɸ�URL����ȡ��������̳�ԴHTTP����ͷ��
 *  �� Host �ֶ�
 * @return {int} 0: ok; < 0: error
 */
HTTP_API int http_hdr_req_rewrite2(HTTP_HDR_REQ *hh, const char *url);

/**
 * �ͷ�HTTP����ͷ����
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ����
 */
HTTP_API void http_hdr_req_free(HTTP_HDR_REQ *hh);

/**
 * ��HTTP����ͷ����ĳ�Ա�����ͷŲ����³�ʼ��
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ����
 */
HTTP_API void http_hdr_req_reset(HTTP_HDR_REQ *hh);

/**
 * ����HTTPЭ��ͷ��cookies
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @return {int} 0: ok;  -1: err.
 */
HTTP_API int http_hdr_req_cookies_parse(HTTP_HDR_REQ *hh);

/**
 * ����HTTP������������(��: GET /cgi-bin/test.cgi?name=value&name2=value2 HTTP/1.0)
 * ����ķ���(GET)-->hdr_request_method
 * URL���ݷ������(name=value)-->hdr_request_table
 * HTTPЭ��汾��(HTTP/1.0)-->hdr_request_proto
 * URL�����е�·������(/cgi-bin/test.cgi)-->hdr_request_url
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @return {int} 0: ok;  -1: err.
 */
HTTP_API int http_hdr_req_line_parse(HTTP_HDR_REQ *hh);

/**
 * ����HTTP����ͷЭ������, ���ڲ������ http_hdr_req_line_parse, http_hdr_req_cookies_parse
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @return {int} 0: ok;  -1: err.
 */
HTTP_API int http_hdr_req_parse(HTTP_HDR_REQ *hh);

/**
 * ����HTTP����ͷЭ������, ���ڲ������ http_hdr_req_line_parse, http_hdr_req_cookies_parse
 * ��� parse_params �� 0 �����HTTP���� url �еĲ�������; ��� parse_cookie �� 0 �����
 * HTTP�����е� cookie ����
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @param parse_params {int} �Ƿ�������� url �еĲ�������
 * @param parse_cookie {int} �Ƿ���������е� cookie ����
 * @return {int} 0: ok;  -1: err.
 */
HTTP_API int http_hdr_req_parse3(HTTP_HDR_REQ *hh, int parse_params, int parse_cookie);

/**
 * ��HTTP����ͷ�л��ĳ��cookieֵ
 * @param hh {HTTP_HDR_REQ*) HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @param name {const char*} ĳ��cookie�ı�����, ����Ϊ��
 * @return {const char*} !NULL: �÷���ֵ��Ϊ��Ҫ���cookie; NULL: �������Ҫ���cookie������
 */
HTTP_API const char *http_hdr_req_cookie_get(HTTP_HDR_REQ *hh, const char *name);

/**
 * ��HTTP����ͷ��ȡ��HTTP����ķ���, ��: POST, GET, CONNECT
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @return {const char*} ������ʾ����. NULL: error; !NULL: OK.
 */
HTTP_API const char *http_hdr_req_method(HTTP_HDR_REQ *hh);

/**
 * ��HTTP����ͷ�л�ȡ����URL��ĳ�������ֶε�����, 
 * ��ȡ: /cgi-bin/test.cgi?n1=v1&n2=v2 �е� n2��ֵv2
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @param name {const char*} ��������еı�����
 * @return {const char*} !NULL: ok, ���ر���ֵ���ڴ�ָ��;  NULL: ����������ı�����������.
 */
HTTP_API const char *http_hdr_req_param(HTTP_HDR_REQ *hh, const char *name);

/**
 * ��HTTP����ͷ�л�ȡ�������еķ���·������, ����������������������.
 * ��ԭ����������Ϊ:
 *   GET /cgi-bin/test.cgi?n1=v1&n2=v2 HTTP/1.1
 *  or
 *   GET http://www.test.com[:80]/cgi-bin/test.cgi?n1=v1&n2=v2 HTTP/1.1
 * �������Ľ������Ϊ:
 *   /cgi-bin/test.cgi?n1=v1&n2=v2
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @return {const char*} ��ʾ��URL. !NULL: OK; NULL: error.
 */
HTTP_API const char *http_hdr_req_url_part(HTTP_HDR_REQ *hh);

/**
 * ��HTTP����ͷ�л�ȡ�������еķ���·������, ������������������.
 * ��ԭ����������Ϊ:
 *   GET /cgi-bin/test.cgi?n1=v1&n2=v2 HTTP/1.1
 *  or
 *   GET http://www.test.com[:80]/cgi-bin/test.cgi?n1=v1&n2=v2 HTTP/1.1
 * �������Ľ������Ϊ:
 *   /cgi-bin/test.cgi
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @return {const char*} ��ʾ��URL. !NULL: OK; NULL: error.
 */
HTTP_API const char *http_hdr_req_url_path(HTTP_HDR_REQ *hh);

/**
 * ��HTTP����Э��ͷ�л�÷�����������IP����������ʽΪ��IP|domain[:PORT]
 * ��: 192.168.0.22:80, or www.test.com:8088
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @return {const char*} �����û���ʾ��������. !NULL: ok; NULL: error.
 */
HTTP_API const char *http_hdr_req_host(HTTP_HDR_REQ *hh);

/**
 * ��HTTP����ͷЭ���л��������URL�����ַ���
 * ��ԭHTTP����ͷΪ:
 * GET /cgi-bin/test.cgi?n1=v1&n2=v2 HTTP/1.1
 * HOST: www.test.com
 * �򾭸ú������򷵻�:
 * http://www.test.com/cgi-bin/test.cgi?n1=v1&n2=v2
 * @param hh {HTTP_HDR_REQ*} HTTP����ͷ���͵�����ָ�룬����Ϊ��
 * @return {const char*} ��ʾ��URL. !NULL: OK; NULL: error.
 * @example:
 *  void test(HTTP_HDR_REQ *hh)
 *  {
 *    const char *url = http_hdr_req_url(hh);
 *    printf(">>> url: %s\r\n", url ? url : "null");
 *  }
 *  ע��, ��Ϊ http_hdr_req_url �ڲ�ʹ�õ���һ���ֲ߳̾���̬�����ڴ���, ����
 *  ��������ʹ�ã������ʹ���ص����ݷ����ص�.
 *  void test(HTTP_HDR_REQ *hh1, HTTP_HDR_REQ *hh2)
 *  {
 *    const char *url1 = http_hdr_req_url(hh1);
 *    const char *url2 = http_hdr_req_url(hh2);
 *    printf(">>> url1: %s, url2: %s\n", url1, url2);
 *  }
 *  ��Ϊ url1, url2 ʵ���϶���ָ���ͬһ�ڴ���, �������յĽ������ url1, url2
 *  ������ͬ. ������������, Ӧ�����²���:
 *  void test(HTTP_HDR_REQ *hh1, HTTP_HDR_REQ *hh2)
 *  {
 *    const char *ptr;
 *    static char dummy[1];
 *    char *url1 = dummy, *url2 = dummy;
 *    ptr = http_hdr_req_url(hh1);
 *    if (ptr)
 *      url1 = acl_mystrdup(ptr);
 *    ptr = http_hdr_req_url(hh2);
 *    if (ptr)
 *      url2 = acl_mystrdup(ptr);
 *    printf(">>> url1: %s, url2: %s\n", url1, url2);
 *    if (url1 != dummy)
 *      acl_myfree(url1);
 *    if (url2 != dummy)
 *      acl_myfree(url2);
 *  }
 */
HTTP_API const char *http_hdr_req_url(HTTP_HDR_REQ *hh);

/**
 * ����HTTP����ͷ�е� Range �ֶ�
 * @param hdr_req {HTTP_HDR_REQ*} ����HTTPЭ��ͷ, ����Ϊ��
 * @param range_from {http_off_t*} �洢ƫ����ʼλ��
 * @param range_to {http_off_t*} �洢ƫ�ƽ���λ��
 * ע�� * {range_from}, {range_to} �±��0��ʼ
 * ����� Range ��ʽ:
 *   Range: bytes={range_from}-, bytes={range_from}-{range_to}
 */
HTTP_API int http_hdr_req_range(HTTP_HDR_REQ *hdr_req, http_off_t *range_from,
		http_off_t *range_to);

/*---------------------------- HTTP ��Ӧͷ�������� ---------------------------*/
/* in http_hdr_res.c */

/**
 * ����HTTP��Ӧͷ�е�״̬��
 *@param hh {HTTP_HDR_RES*} HTTP��Ӧͷ���͵�����ָ�룬����Ϊ��
 *@param dbuf {const char*} ״̬������, ��: HTTP/1.0 200 OK������Ϊ��
 *@return {int} 0: ok;  < 0: error����������洢�� hh �ṹ��
 */
HTTP_API int http_hdr_res_status_parse(HTTP_HDR_RES *hh, const char *dbuf);

/**
 * ����һ���µ�HTTP��Ӧͷ
 * @return {HTTP_HDR_RES*}
 */
HTTP_API HTTP_HDR_RES *http_hdr_res_new(void);

/**
 * ��¡һ��HTTP��Ӧͷ
 * @param hdr_res {const HTTP_HDR_RES*} ԴHTTP��Ӧͷ
 * @return {HTTP_HDR_RES *} �²�����HTTP��Ӧͷ
 */
HTTP_API HTTP_HDR_RES *http_hdr_res_clone(const HTTP_HDR_RES *hdr_res);

/**
 * �ͷ�һ��HTTP��Ӧͷ
 * @param hh {HTTP_HDR_RES*} HTTP��Ӧͷ
 */
HTTP_API void http_hdr_res_free(HTTP_HDR_RES *hh);

/**
 * ��HTTP��Ӧͷ���³�ʼ�����ͷ����еĳ�Ա����
 * @param hh {HTTP_HDR_RES*} HTTP��Ӧͷ
 */
HTTP_API void http_hdr_res_reset(HTTP_HDR_RES *hh);

/**
 * ����HTTP��Ӧͷ������ݣ����洢�������
 * @param hdr_res {HTTP_HDR_RES*} HTTP��Ӧͷ
 */
HTTP_API int http_hdr_res_parse(HTTP_HDR_RES *hdr_res);

/**
 * ����HTTP��Ӧͷ�е� Range �ֶ�
 * @param hdr_res {HTTP_HDR_RES*} ��ӦHTTPЭ��ͷ, ����Ϊ��
 * @param range_from {http_off_t*} �洢ƫ����ʼλ��, ����Ϊ��
 * @param range_to {http_off_t*} �洢ƫ�ƽ���λ��, ����Ϊ��
 * @param total_length {http_off_t*} ���������ļ����ܳ���, ��Ϊ��
 * ע�� * {range_from}, {range_to} �±��0��ʼ
 * ��Ӧ�� Range ��ʽ:
 *   Content-Range: bytes {range_from}-{range_to}/{total_length}
 */
HTTP_API int http_hdr_res_range(HTTP_HDR_RES *hdr_res, http_off_t *range_from,
		http_off_t *range_to, http_off_t *total_length);

/* in http_rfc1123.c */

/**
 * ��ʱ��ֵת����RFC1123��Ҫ��ĸ�ʽ
 * @param buf {char*} �洢�ռ�
 * @param size {size_t} buf �Ŀռ��С
 * @param t {time_t} ʱ��ֵ
 */
HTTP_API const char *http_mkrfc1123(char *buf, size_t size, time_t t);

/*----------------------- HTTP �첽���������� --------------------------------*/
/* in http_chat_async.c */

/**
 * �첽��ȡһ��HTTP REQUESTЭ��ͷ�����ݽ���洢��hdr��, ��ȡ��һ��������HTTPͷ��
 * ����ʱ�����û���ע�ắ�� notify
 * @param hdr {HTTP_HDR_REQ*} HTTP����ͷ���ͽṹָ�룬����Ϊ��
 * @param astream {ACL_ASTREAM*} ��ͻ������ӵ�������, ����Ϊ��
 * @param notify {HTTP_HDR_NOTIFY} ��HTTPЭ��ͷ��������ʱ���õ��û���ע�ắ��
 * @param arg {void*} notify ����ʱ��һ������
 * @param timeout {int} �������ݹ����еĶ���ʱʱ��
 */
HTTP_API void http_hdr_req_get_async(HTTP_HDR_REQ *hdr, ACL_ASTREAM *astream,
		HTTP_HDR_NOTIFY notify, void *arg, int timeout);

/**
 * �첽��ȡһ��HTTP RESPONDЭ��ͷ�����ݽ���洢��hdr��, ��ȡ��һ��������HTTPͷ��
 * ����ʱ�����û���ע�ắ�� notify
 * @param hdr {HTTP_HDR_REQ*} HTTP��Ӧͷ���ͽṹָ�룬����Ϊ��
 * @param astream {ACL_ASTREAM*} ���������ӵ�������, ����Ϊ��
 * @param notify {HTTP_HDR_NOTIFY} ��HTTPЭ��ͷ��������ʱ���õ��û���ע�ắ��
 * @param arg {void*} notify ����ʱ��һ������
 * @param timeout {int} �������ݹ����еĶ���ʱʱ��
 */
HTTP_API void http_hdr_res_get_async(HTTP_HDR_RES *hdr, ACL_ASTREAM *astream,
		HTTP_HDR_NOTIFY notify, void *arg, int timeout);

/**
 * �첽�ӿͻ��˶�ȡ�����BODYЭ����, �ڽ��չ����б߽������ص��û��� notify
 * �ص�����, ��� notify ����С�� 0 ��ֵ, ����Ϊ�����Ҳ��ټ�����������
 * @param request {HTTP_REQ*} HTTP����������ָ��, ����Ϊ��, �� request->hdr Ϊ��
 * @param astream {ACL_ASTREAM*} ��ͻ������ӵ�������, ����Ϊ��
 * @param notify {HTTP_BODY_NOTIFY} ���տͻ������ݹ����лص����û���ע�ắ��
 * @param arg {void*} notify ����ʱ��һ������
 * @param timeout {int} �������ݹ����еĶ���ʱʱ��
 */
HTTP_API void http_req_body_get_async(HTTP_REQ *request, ACL_ASTREAM *astream,
		 HTTP_BODY_NOTIFY notify, void *arg, int timeout);
/*
 * �첽�ӷ������˶�ȡ��Ӧ���ݵ�BODYЭ����, �ڽ��չ��������������ص��û���
 * notify �ص�����, ��� notify ����С�� 0 ��ֵ, ����Ϊ�����Ҳ��ټ�����������
 * @param respond {HTTP_RES*} HTTP��Ӧ������ָ��, ����Ϊ��,�� respond->hdr ��Ϊ��
 * @param astream {ACL_ASTREAM*} ���������ӵ�������, ����Ϊ��
 * @param notify {HTTP_BODY_NOTIFY} ���շ�������ݹ����лص����û���ע�ắ��
 * @param arg {void*} notify ����ʱ��һ������
 * @param timeout {int} �������ݹ����еĶ���ʱʱ��
 */
HTTP_API void http_res_body_get_async(HTTP_RES *respond, ACL_ASTREAM *astream,
		HTTP_BODY_NOTIFY notify, void *arg, int timeout);

/*----------------------- HTTP ͬ������������ --------------------------------*/
/* in http_chat_sync.c */

/**
* ͬ����ȡһ��HTTP REQUESTЭ��ͷ�����ݽ���洢��hdr��, ��ȡ��һ��������HTTPͷ��
* ����ʱ�����û���ע�ắ�� notify
* @param hdr {HTTP_HDR_REQ*} HTTP����ͷ���ͽṹָ�룬����Ϊ��
* @param stream {ACL_VSTREAM*} ��ͻ������ӵ�������, ����Ϊ��
* @param timeout {int} �������ݹ����еĶ���ʱʱ��
* @return {int} 0: �ɹ�; < 0: ʧ��
*/
HTTP_API int http_hdr_req_get_sync(HTTP_HDR_REQ *hdr,
		 ACL_VSTREAM *stream, int timeout);

/**
 * ͬ����ȡһ��HTTP RESPONDЭ��ͷ�����ݽ���洢��hdr��, ��ȡ��һ��������HTTPͷ��
 * ����ʱ�����û���ע�ắ�� notify
 * @param hdr {HTTP_HDR_REQ*} HTTP��Ӧͷ���ͽṹָ�룬����Ϊ��
 * @param stream {ACL_VSTREAM*} ���������ӵ�������, ����Ϊ��
 * @param timeout {int} �������ݹ����еĶ���ʱʱ��
 * @return {int} 0: �ɹ�; < 0: ʧ��
 */
HTTP_API int http_hdr_res_get_sync(HTTP_HDR_RES *hdr,
		ACL_VSTREAM *stream, int timeout);

/**
 * ͬ���ӿͻ��˶�ȡ�����BODYЭ����
 * @param request {HTTP_REQ*} HTTP����������ָ��, ����Ϊ��, �� request->hdr Ϊ��
 * @param stream {ACL_VSTREAM*} ��ͻ������ӵ�������, ����Ϊ��
 * @param buf {void *} �洢��������ݿռ�
 * @param size {int} buf �Ŀռ��С
 * @return ret {http_off_t} ���ζ�����HTTP�����������
 *             0: ��ʾ������HTTP���������ݣ������������������Ѿ��ر�;
 *             < 0: ��ʾ���������رջ����;
 *             > 0: ��ʾδ���꣬Ŀǰ����ret ���ֽڵ�����
 */
HTTP_API http_off_t http_req_body_get_sync(HTTP_REQ *request, ACL_VSTREAM *stream,
		void *buf, int size);
#define http_req_body_get_sync2	http_req_body_get_sync

/**
 * ͬ���ӷ���˶�ȡ��Ӧ��BODYЭ����
 * @param respond {HTTP_RES*} HTTP��Ӧ������ָ��, ����Ϊ��, �� respond->hdr Ϊ��
 * @param stream {ACL_VSTREAM*} ��ͻ������ӵ�������, ����Ϊ��
 * @param buf {void *} �洢��������ݿռ�
 * @param size {int} buf �Ŀռ��С
 * @return ret {http_off_t} ���ζ�����HTTP��Ӧ�������
 *             0: ��ʾ������HTTP���������ݣ������������������Ѿ��ر�;
 *             < 0: ��ʾ���������رջ����;
 *             > 0: ��ʾδ���꣬Ŀǰ����ret ���ֽڵ�����
 */
HTTP_API http_off_t http_res_body_get_sync(HTTP_RES *respond, ACL_VSTREAM *stream,
		void *buf, int size);
#define http_res_body_get_sync2	http_res_body_get_sync

/**
 * ��������Э��Ŀ��Ʊ�־λ
 * @param request {HTTP_REQ*} HTTP����������ָ��, ����Ϊ��, �� request->hdr Ϊ��
 * @param name {int} ��һ����־λ�������һ����־λΪ HTTP_CHAT_SYNC_CTL_END ʱ
 *  ��ʾ����
 */
HTTP_API void http_chat_sync_reqctl(HTTP_REQ *request, int name, ...);

/**
 * ������ӦЭ��Ŀ��Ʊ�־λ
 * @param respond {HTTP_RES*} HTTP��Ӧ������ָ��, ����Ϊ��, �� respond->hdr Ϊ��
 * @param name {int} ��һ����־λ�������һ����־λΪ HTTP_CHAT_SYNC_CTL_END ʱ
 *  ��ʾ����
 */
HTTP_API void http_chat_sync_resctl(HTTP_RES *respond, int name, ...);
#define	HTTP_CHAT_SYNC_CTL_END      0  /**< ������־λ */
#define	HTTP_CHAT_CTL_BUFF_ONOFF    1  /**< �Ƿ�����ݽ���ʱ��Ԥ������� */

/*------------------------ HTTP �����幹�켰�ͷź���  ------------------------*/
/* in http_req.c */

/**
 * ����HTTP����ͷ����һ�����������
 * @param hdr_req {HTTP_HDR_REQ*} ����ͷ����
 * @return {HTTP_REQ*} ���������
 */
HTTP_API HTTP_REQ *http_req_new(HTTP_HDR_REQ *hdr_req);

/**
 * �ͷ����������
 * @param request {HTTP_REQ*} ���������
 */
HTTP_API void http_req_free(HTTP_REQ *request);

/*------------------------ HTTP ��Ӧ�幹�켰�ͷź���  ------------------------*/
/* in http_res.c */

/**
* ����HTTP��Ӧͷ����һ����Ӧ�����
* @param hdr_res {HTTP_HDR_RES*} ��Ӧͷ����
* @return {HTTP_RES*} ��Ӧ�����
*/
HTTP_API HTTP_RES *http_res_new(HTTP_HDR_RES *hdr_res);

/**
 * �ͷ���Ӧ�����
 * @param respond {HTTP_RES*} ��Ӧ�����
 */
HTTP_API void http_res_free(HTTP_RES *respond);

/*------------------------------ HTTP ͷ���캯�� -----------------------------*/
/* in http_hdr_build.c */

/**
 * ��ͨ��HTTPͷ���������
 * @param hdr {HTTP_HDR*} ͨ��HTTPͷ����
 * @param name {const char*} ���������� Accept-Encoding: deflate, gzip �е� Accept-Encoding
 * @param value {const char*} ����ֵ���� Accept-Encoding: deflate, gzip �е� deflate, gzip
 */
HTTP_API void http_hdr_put_str(HTTP_HDR *hdr, const char *name, const char *value);

/**
 * ��ͨ��HTTPͷ���������
 * @param hdr {HTTP_HDR*} ͨ��HTTPͷ����
 * @param name {const char*} ���������� Content-Length: 1024 �е� Conteng-Length
 * @param value {const int} ����ֵ���� Content-Length: 1024 �е� 1024
 */
HTTP_API void http_hdr_put_int(HTTP_HDR *hdr, const char *name, int value);

/**
 * ��ͨ��HTTPͷ���������
 * @param hdr {HTTP_HDR*} ͨ��HTTPͷ����
 * @param name {const char*} ���������� Accept-Encoding: deflate, gzip �е� Accept-Encoding
 * @param fmt {const char*} ��θ�ʽ�ַ���
 */
#ifdef	WIN32
HTTP_API void http_hdr_put_fmt(HTTP_HDR *hdr, const char *name, const char *fmt, ...);
#else
HTTP_API void __attribute__((format(printf,3,4)))
	http_hdr_put_fmt(HTTP_HDR *hdr, const char *name, const char *fmt, ...);
#endif

/**
 * ��ͨ��HTTPͷ�����ʱ������
 * @param hdr {HTTP_HDR*} ͨ��HTTPͷ����
 * @param name {const char*} ������
 * @param t {time_t} ʱ��ֵ
 */
HTTP_API void http_hdr_put_time(HTTP_HDR *hdr, const char *name, time_t t);

/**
 * ����HTTP����ͷ���ֶ��������Ƿ������˱��ֳ�����, ����洢��HTTP��Ӧͷ��
 * @param req {const HTTP_HDR_REQ*} HTTP����ͷ
 * @param res {HTTP_HDR_RES*} HTTP��Ӧͷ���洢�������
 */
HTTP_API int http_hdr_set_keepalive(const HTTP_HDR_REQ *req, HTTP_HDR_RES *res);

/**
 * �÷���״̬(1xx, 2xx, 3xx, 4xx, 5xx) ��ʼ��һ��HTTP��Ӧͷ
 * @param hdr_res {HTTP_HDR_RES*} HTTP��Ӧͷ���洢�������
 * @param status {int} ״̬�ţ�nxx(1xx, 2xx, 3xx, 4xx, 5xx)
 */
HTTP_API void http_hdr_res_init(HTTP_HDR_RES *hdr_res, int status);

/**
 * �÷���״̬(nxx)����һ��HTTP��Ӧͷ
 * @param status {int} ״̬�ţ�nxx(1xx, 2xx, 3xx, 4xx, 5xx)
 * @return {HTTP_HDR_RES*} ���ɵ�HTTP��Ӧͷ
 */
HTTP_API HTTP_HDR_RES *http_hdr_res_static(int status);

/**
* �÷���״̬(nxx)����һ��HTTP��Ӧͷ
* @param status {int} ״̬�ţ�nxx(4xx, 5xx)
* @return {HTTP_HDR_RES*} ���ɵ�HTTP��Ӧͷ
*/
HTTP_API HTTP_HDR_RES *http_hdr_res_error(int status);

/**
 * ����HTTPͨ��ͷ����ͷ������������BUF��
 * @param hdr {const HTTP_HDR*} ͨ��HTTPͷ
 * @param strbuf {ACL_VSTRING*} �洢����Ļ�����
 */
HTTP_API void http_hdr_build(const HTTP_HDR *hdr, ACL_VSTRING *strbuf);

/**
 * ����HTTP����ͷ��������ͷ������BUF��
 * @param hdr_req {const HTTP_HDR_REQ*} HTTP����ͷ
 * @param strbuf {ACL_VSTRING*} �洢����Ļ�����
 */
HTTP_API void http_hdr_build_request(const HTTP_HDR_REQ *hdr_req, ACL_VSTRING *strbuf);

/*----------------------------- HTTP ��Ӧ״̬��Ϣ���� ------------------------*/
/* in http_status.c */

/**
 * ����HTTP��Ӧ��(nxx)���ظ�ֵ��������ַ���
 * @param status {int} ״̬�ţ�nxx(1xx, 2xx, 3xx, 4xx, 5xx)
 * @return {const char*} ��Ӧ������Ӧ���ַ�����ʾ
 */
HTTP_API const char *http_status_line(int status);

/*---------------------------- HTTP HTML ģ��������� ------------------------*/
/* in http_tmpl.c */

/**
 * װ��HTTP��Ӧ�����HTMLģ��
 * @param tmpl_path {const char*} HTMLģ���ļ����ڵ�·��
 */
HTTP_API void http_tmpl_load(const char *tmpl_path);

/**
 * ��ȡ��ӦHTTP��Ӧ״̬���ģ����Ϣ
 * @param status {int} HTTP ״̬��Ӧ��
 * @return {const ACL_VSTRING*} ��ӦHTTP��Ӧ״̬���ģ����Ϣ
 */
HTTP_API const ACL_VSTRING *http_tmpl_get(int status);

/**
 * ��ȡ��ӦHTTP��Ӧ״̬��ı�����ʾ��Ϣ
 * @param status {int} HTTP ״̬��Ӧ��
 * @return {const char*} ��ӦHTTP��Ӧ״̬��ı�����ʾ��Ϣ
 */
HTTP_API const char *http_tmpl_title(int status);

/**
 * �����ӦHTTP��Ӧ״̬���ģ����ʾ��Ϣ�ĳ��ȴ�С
 * @param status {int} HTTP ״̬��Ӧ��
 * @return {int} ģ����ʾ��Ϣ�ĳ��ȴ�С
 */
HTTP_API int http_tmpl_size(int status);

/*---------------------------- HTTP HTML ģ���ʼ������ ----------------------*/
/* in http_init.c */

/**
 * ��ʼ��HTTPӦ��Э��
 * @param tmpl_path {const char*} ģ����Ϣ�ļ��Ĵ��·��
 */
HTTP_API void http_init(const char *tmpl_path);

/**
 * �Ƿ��Զ����屻�ͷŵ� HTTP ͷ���󣬴Ӷ�ʹ���ڴ�����ظ�ʹ��, �ú����ڳ����ʼ��
 * ʱֻ�ܱ�����һ��
 * @param max {int} ����ֵ > 0 ʱ���Զ����� HTTP ͷ���󻺳幦��
 */
HTTP_API void http_hdr_cache(int max);

/**
 * �����ڽ��� HTTP Э�������ݴ���ʱ�Ļ�������С
 * @param size {http_off_t} ��������С
 */
HTTP_API void http_buf_size_set(http_off_t size);

/**
 * ��ý��� HTTP Э�������ݴ���ʱ�Ļ�������С
 * @return {http_off_t} ��������С
 */
HTTP_API http_off_t http_buf_size_get(void);

#ifdef	__cplusplus
}
#endif

#endif

