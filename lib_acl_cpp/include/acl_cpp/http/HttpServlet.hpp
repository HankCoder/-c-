#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/http/http_header.hpp"

namespace acl {

class socket_stream;
class session;
class HttpServletRequest;
class HttpServletResponse;

/**
 * ���� HTTP �ͻ�������Ļ��࣬������Ҫ�̳и���
 */
class ACL_CPP_API HttpServlet
{
public:
	HttpServlet(void);
	virtual ~HttpServlet(void) = 0;

	/**
	 * ���ñ����ַ�������������˱����ַ��������ڽ��� HTTP ��������ʱ�����Զ��������
	 * �ַ���תΪ�����ַ������ú��������� doRun ֮ǰ���ò���Ч
	 * @param charset {const char*} �����ַ����������ָ��Ϊ�գ�
	 *  ����������ַ���
	 * @return {HttpServlet&}
	 */
	HttpServlet& setLocalCharset(const char* charset);

	/**
	 * ���� HTTP �Ự���̵� IO ��д��ʱʱ�䣻�ú��������� doRun ֮ǰ���ò���Ч
	 * @param rw_timeout {int} ��д��ʱʱ��(��)
	 * @return {HttpServlet&}
	 */
	HttpServlet& setRwTimeout(int rw_timeout);

	/**
	 * ��� POST �������÷��������Ƿ���Ҫ�������������ݣ�Ĭ��Ϊ�������ú��������� doRun
	 * ֮ǰ���ò���Ч����������Ϊ�������� MIME ��ʽ����ʹ���ñ����������˽������ݣ�Ҳ��
	 * �����������н���
	 * @param on {bool} �Ƿ���Ҫ����
	 * @return {HttpServlet&}
	 */
	HttpServlet& setParseBody(bool on);

	/**
	 * ��� POST �������÷������ý������������󳤶ȣ���������壬�ú��������� doRun
	 * ֮ǰ���ò���Ч
	 * @param length {int} ��󳤶����ƣ��������������峤�ȹ�����ֱ�ӷ��� false
	 * @return {HttpServlet&}
	 */
	HttpServlet& setParseBodyLimit(int length);

	/**
	 * HttpServlet ����ʼ���У����� HTTP ���󣬲��ص����� doXXX �麯��
	 * @param session {session&} �洢 session ���ݵĶ���
	 * @param stream {socket_stream*} ���� acl_master ��������ܿ�����
	 *  ����ʱ���ò�������ǿգ����� apache ���� CGI ��ʽ����ʱ���ò���
	 *  ��Ϊ NULL�����⣬�ú����ڲ�����ر������ӣ�Ӧ��Ӧ���д���������
	 *  �Ĺر�������������Է����� acl_master �ܹ����
	 * @return {bool} ���ش�����
	 */
	bool doRun(session& session, socket_stream* stream = NULL);

	/**
	 * HttpServlet ����ʼ���У����� HTTP ���󣬲��ص����� doXXX �麯����
	 * ���ñ�������ζ�Ų��� memcached ���洢 session ����
	 * @param memcached_addr {const char*} memcached ��������ַ����ʽ��IP:PORT
	 * @param stream {socket_stream*} ����ͬ��
	 * @return {bool} ���ش�����
	 */
	bool doRun(const char* memcached_addr = "127.0.0.1:11211",
		socket_stream* stream = NULL);

	/**
	 * �� HTTP ����Ϊ GET ��ʽʱ���麯��
	 */
	virtual bool doGet(HttpServletRequest&, HttpServletResponse&)
	{
		logger_error("child not implement doGet yet!");
		return false;
	}

	/**
	 * �� HTTP ����Ϊ POST ��ʽʱ���麯��
	 */
	virtual bool doPost(HttpServletRequest&, HttpServletResponse&)
	{
		logger_error("child not implement doPost yet!");
		return false;
	}

	/**
	 * �� HTTP ����Ϊ PUT ��ʽʱ���麯��
	 */
	virtual bool doPut(HttpServletRequest&, HttpServletResponse&)
	{
		logger_error("child not implement doPut yet!");
		return false;
	}

	/**
	 * �� HTTP ����Ϊ CONNECT ��ʽʱ���麯��
	 */
	virtual bool doConnect(HttpServletRequest&, HttpServletResponse&)
	{
		logger_error("child not implement doConnect yet!");
		return false;
	}

	/**
	 * �� HTTP ����Ϊ PURGE ��ʽʱ���麯�����÷�������� SQUID �Ļ���
	 * ʱ���õ�
	 */
	virtual bool doPurge(HttpServletRequest&, HttpServletResponse&)
	{
		logger_error("child not implement doPurge yet!");
		return false;
	}

	/**
	 * �� HTTP ���󷽷�δ֪ʱ���麯��
	 */
	virtual bool doUnknown(HttpServletRequest&, HttpServletResponse&)
	{
		logger_error("child not implement doUnknown yet!");
		return false;
	}

	/**
	 * �� HTTP �������ʱ���麯��
	 */
	virtual bool doError(HttpServletRequest&, HttpServletResponse&)
	{
		logger_error("child not implement doError yet!");
		return false;
	}

protected:
private:
	char local_charset_[32];
	int  rw_timeout_;
	bool parse_body_enable_;
	int  parse_body_limit_;
};

} // namespace acl
