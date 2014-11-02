#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/http/http_ctype.hpp"
#include "acl_cpp/http/http_type.hpp"

namespace acl {

class istream;
class ostream;
class socket_stream;
class http_client;
class http_mime;
class session;
class HttpSession;
class HttpCookie;
class HttpServletResponse;

/**
 * �� HTTP �ͻ���������ص��࣬���಻Ӧ���̳У��û�Ҳ����Ҫ
 * ����򴴽��������
 */
class ACL_CPP_API HttpServletRequest
{
public:
	/**
	 * ���캯��
	 * @param res {HttpServletResponse&}
	 * @param store {session&} �洢�Ự���ݵĶ���
	 * @param stream {socket_stream&} ���������ڲ����������ر���
	 * @param local_charset {const char*} �����ַ�������ֵ�ǿ�ʱ��
	 *  �ڲ����Զ��� HTTP ���������ת��Ϊ�����ַ���������ת��
	 * @param body_parse {bool} ��� POST �������ò���ָ���Ƿ���Ҫ
	 *  ��ȡ HTTP ���������岢�� n/v ��ʽ���з�������Ϊ true ����
	 *  �����ȡ HTTP ���������ݣ������з��������û����� getParameter
	 *  ʱ���������Ի�� URL �еĲ�����ͬʱ���Ի�� POST ��������
	 *  �Ĳ��������ò���Ϊ false ʱ�򲻶�ȡ�����壬�Ѷ������������
	 *  �������ദ��
	 * @param body_limit {int} ��� POST ��������������Ϊ�ı�����
	 *  ����ʱ���˲�������������ĳ��ȣ���������Ϊ�������� MIME
	 *  ��ʽ�� body_read Ϊ false���˲�����Ч
	 */
	HttpServletRequest(HttpServletResponse& res, session& store,
		socket_stream& stream, const char* local_charset = NULL,
		bool body_parse = true, int body_limit = 102400);
	~HttpServletRequest(void);

	/**
	 * ��� HTTP �ͻ������󷽷���GET, POST, PUT, CONNECT, PURGE
	 * @return {http_method_t}
	 */
	http_method_t getMethod(void) const;

	/**
	 * ��� HTTP �ͻ������������ cookie ���󼯺�
	 * @return {const std::vector<HttpCookie*>&}
	 */
	const std::vector<HttpCookie*>& getCookies(void) const;

	/**
	 * ��� HTTP �ͻ��������ĳ�� cookie ֵ
	 * @param name {const char*} cookie ���ƣ�����ǿ�
	 * @return {const char*} cookie ֵ�������� NULL ʱ��ʾ
	 *  cookie ֵ������
	 */
	const char* getCookieValue(const char* name) const;

	/**
	 * �� HTTP ���������� cookie ����
	 * @param name {const char*} cookie �����ǿ��ַ���
	 * @param value {const char*} cookie ֵ���ǿ��ַ���
	 */
	void setCookie(const char* name, const char* value);

	/**
	 * ��� HTTP ����ͷ�е�ĳ���ֶ�ֵ
	 * @param name {const char*} HTTP ����ͷ�е��ֶ������ǿ�
	 * @return {const char*} HTTP ����ͷ�е��ֶ�ֵ������ NULL
	 *  ʱ��ʾ������
	 */
	const char* getHeader(const char* name) const;

	/**
	 * ��� HTTP GET ����ʽ URL �еĲ������֣��� ? ����Ĳ���
	 * @return {const char*} û�н���URL ���������������֣�
	 *  ���� NULL ���ʾ URL ��û�в���
	 */
	const char* getQueryString(void) const;

	/**
	 * ���  http://test.com.cn/cgi-bin/test?name=value �е�
	 * /cgi-bin/test ·������
	 * @return {const char*} ���ؿձ�ʾ�����ڣ�
	 */
	const char* getPathInfo(void) const;

	/**
	 * ���  http://test.com.cn/cgi-bin/test?name=value �е�
	 * /cgi-bin/test?name=value ·������
	 * @return {const char*} ���ؿձ�ʾ�����ڣ�
	 */
	const char* getRequestUri(void) const;

	/**
	 * ������ HTTP �Ự��ص� HttpSession ��������
	 * @param create {bool} �� session ������ʱ�Ƿ��ڻ���������Զ�������
	 *  ��ĳ�ͻ��˵� session �������Ҹò���Ϊ false ʱ����ú�����
	 *  �ص� session �������û�б������������޷����ж�д����
	 * @param sid {const char*} �� session �����ڣ��� create �����ǿ�ʱ��
	 *  ��� sid �ǿգ���ʹ�ô�ֵ�����û���Ψһ�Ự��ͬʱ��ӽ��ͻ��˵�
	 *  cookie ��
	 * @return {HttpSession&}
	 *  ע�����ȼ�������� COOKIE > create = true > sid != NULL
	 */
	HttpSession& getSession(bool create = true, const char* sid = NULL);

	/**
	 * ����� HTTP �ͻ������ӹ�������������������
	 * @return {istream&}
	 */
	istream& getInputStream(void) const;

	/**
	 * ��� HTTP �������ݵ����ݳ���
	 * @return {acl_int64} ���� -1 ��ʾ����Ϊ GET ������
	 *  �� HTTP ����ͷ��û�� Content-Length �ֶ�
	 */
#ifdef WIN32
	__int64 getContentLength(void) const;
#else
	long long int getContentLength(void) const;
#endif
	/**
	 * ��� HTTP ����ͷ�� Content-Type: text/html; charset=gb2312
	 * Content-Type ���ֶ�ֵ
	 * @param part {bool} ���Ϊ true �򷵻� text�����򷵻�������
	 * ֵ���磺text/html; charset=gb2312
	 * @return {const char*} ���� NULL ��ʾ Content-Type �ֶβ�����
	 */
	const char* getContentType(bool part = true) const;

	/**
	 * ��� HTTP ����ͷ�е� Content-Type: text/html; charset=gb2312
	 * �е� charset �ֶ�ֵ gb2312
	 * @return {const char*} ���� NULL ��ʾ Content-Type �ֶ� ��
	 *  charset=xxx ������
	 */
	const char* getCharacterEncoding(void) const;

	/**
	 * ���ر��ص��ֶ��ַ���
	 * @ return {const char*} ���� NULL ��ʾû�����ñ����ַ���
	 */
	const char* getLocalCharset(void) const;

	/**
	 * ���� HTTP ���ӵı��� IP ��ַ
	 * @return {const char*} ���ؿգ���ʾ�޷����
	 */
	const char* getLocalAddr(void) const;

	/**
	 * ���� HTTP ���ӵı��� PORT ��
	 * @return {unsigned short} ���� 0 ��ʾ�޷����
	 */
	unsigned short getLocalPort(void) const;

	/**
	 * ���� HTTP ���ӵ�Զ�̿ͻ��� IP ��ַ
	 * @return {const char*} ���ؿգ���ʾ�޷����
	 */
	const char* getRemoteAddr(void) const;

	/**
	 * ���� HTTP ���ӵ�Զ�̿ͻ��� PORT ��
	 * @return {unsigned short} ���� 0 ��ʾ�޷����
	 */
	unsigned short getRemotePort(void) const;

	/**
	 * ��� HTTP ����ͷ�����õ� Host �ֶ�
	 * @return {const char*} ���Ϊ�գ����ʾ������
	 */
	const char* getRemoteHost(void) const;

	/**
	 * ��� HTTP ����ͷ�����õ� User-Agent �ֶ�
	 * @return {const char*} ���Ϊ�գ����ʾ������
	 */
	const char* getUserAgent(void) const;

	/**
	 * ��� HTTP �����еĲ���ֵ����ֵ�Ѿ��� URL ������
	 * ת���ɱ���Ҫ����ַ�������� GET ���������ǻ��
	 * URL �� ? ����Ĳ���ֵ����� POST ����������Ի��
	 * URL �� ? ����Ĳ���ֵ���������еĲ���ֵ
	 */
	const char* getParameter(const char* name) const;

	/**
	 * �� HTTP ����ͷ�е� Content-Type Ϊ
	 * multipart/form-data; boundary=xxx ��ʽʱ��˵��Ϊ�ļ��ϴ�
	 * �������ͣ������ͨ���˺������ http_mime ����
	 * @return {const http_mime*} ���� NULL ��˵��û�� MIME ����
	 *  ���ص�ֵ�û������ֹ��ͷţ���Ϊ�� HttpServletRequest ����
	 *  ���л��Զ��ͷ�
	 */
	http_mime* getHttpMime(void) const;

	/**
	 * ��� HTTP �������ݵ�����
	 * @return {http_request_t}��һ��� POST �����е��ϴ�
	 *  �ļ�Ӧ�ö��ԣ���Ҫ���øú�������Ƿ����ϴ��������ͣ�
	 *  ���ú������� HTTP_REQUEST_OTHER ʱ���û�����ͨ������
	 *  getContentType ��þ���������ַ���
	 */
	http_request_t getRequestType(void) const;

	/**
	 * ��� HTTP ����ҳ��� referer URL
	 * @return {const char*} Ϊ NULL ��˵���û�ֱ�ӷ��ʱ� URL
	 */
	const char* getRequestReferer(void) const;

	/**
	 * ��ø��� HTTP ����ͷ��õ� http_ctype ����
	 * @return {const http_ctype&}
	 */
	const http_ctype& getHttpCtype(void) const;

	/**
	 * �ж� HTTP �ͻ����Ƿ�Ҫ�󱣳ֳ�����
	 * @return {bool}
	 */
	bool isKeepAlive(void) const;

	/**
	 * ���ͻ���Ҫ�󱣳ֳ�����ʱ���� HTTP ����ͷ�л�ñ��ֵ�ʱ��
	 * @return {int} ����ֵ < 0 ��ʾ������ Keep-Alive �ֶ�
	 */
	int getKeepAlive(void) const;

	/*
	 * �� HTTP ����Ϊ POST ������ͨ�����������ö� HTTP �������
	 * IO ��ʱʱ��ֵ(��)
	 * @param rw_timeout {int} ��������ʱ�ĳ�ʱʱ��(��)
	 */
	void setRwTimeout(int rw_timeout);

	/**
	 * ����ϴγ���Ĵ����
	 * @return {http_request_error_t}
	 */
	http_request_error_t getLastError(void) const;

	/**
	 * �� HttpServlet ���Է���ģʽ(���� CGI ��ʽ)����ʱ�����Ե��ô�
	 * ������ÿͻ������ӵ� HTTP ����󣬴Ӷ���ø���Ĳ���
	 * @return {http_client*} ���Է���ģʽ����ʱ���˺������� HTTP �ͻ���
	 *  ���ӷǿն��󣻵��� CGI ��ʽ����ʱ���򷵻ؿ�ָ��
	 */
	http_client* getClient(void) const;

	/**
	 * �� HTTP ����ͷ��������У��ļ�������������
	 * @param out {ostream&}
	 * @param prompt {const char*} ��ʾ����
	 */	 
	void fprint_header(ostream& out, const char* prompt);
private:
	http_request_error_t req_error_;
	char cookie_name_[64];
	HttpServletResponse& res_;
	session& store_;
	HttpSession* http_session_;
	socket_stream& stream_;
	bool body_parse_;
	int  body_limit_;

	std::vector<HttpCookie*> cookies_;
	bool cookies_inited_;
	http_client* client_;
	http_method_t method_;
	bool cgi_mode_;
	http_ctype content_type_;
	char localAddr_[32];
	char remoteAddr_[32];
	char localCharset_[32];
	int  rw_timeout_;
	std::vector<HTTP_PARAM*> params_;
	http_request_t request_type_;
	http_mime* mime_;

	bool readHeaderCalled_;
	bool readHeader(void);

	void parseParameters(const char* str);
};

} // namespace acl
