#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/session/memcache_session.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/http/HttpSession.hpp"
#include "acl_cpp/http/HttpServletRequest.hpp"
#include "acl_cpp/http/HttpServletResponse.hpp"
#include "acl_cpp/http/HttpServlet.hpp"

namespace acl
{

HttpServlet::HttpServlet(void)
{
	local_charset_[0] = 0;
	rw_timeout_ = 60;
}

HttpServlet::~HttpServlet(void)
{
}

void HttpServlet::setLocalCharset(const char* charset)
{
	if (charset && *charset)
		snprintf(local_charset_, sizeof(local_charset_), "%s", charset);
	else
		local_charset_[0] =0;
}

void HttpServlet::setRwTimeout(int rw_timeout)
{
	rw_timeout_ = rw_timeout;
}

bool HttpServlet::doRun(session& session, socket_stream* stream /* = NULL */,
	bool body_parse /* = true */, int body_limit /* = 102400 */)
{
	socket_stream* in;
	socket_stream* out;
	bool cgi_mode;

	if (stream == NULL)
	{
		// ������Ϊ�գ��� CGI ģʽ��������׼�������
		// ��Ϊ������
		stream = NEW socket_stream();
		(void) stream->open(ACL_VSTREAM_IN);
		in = stream;

		stream = NEW socket_stream();
		(void) stream->open(ACL_VSTREAM_OUT);
		out = stream;
		cgi_mode = true;
	}
	else
	{
		in = out = stream;
		cgi_mode = false;
	}

	// req/res ����ջ�����������ڴ�������

	HttpServletResponse res(*out);
	HttpServletRequest req(res, session, *in, local_charset_,
		body_parse, body_limit);

	if (rw_timeout_ >= 0)
		req.setRwTimeout(rw_timeout_);

	res.setCgiMode(cgi_mode);

	bool  ret;

	http_method_t method = req.getMethod();
	if (method == HTTP_METHOD_GET)
		ret = doGet(req, res);
	else if (method == HTTP_METHOD_POST)
		ret = doPost(req, res);
	else if (method == HTTP_METHOD_PUT)
		ret = doPut(req, res);
	else if (method == HTTP_METHOD_CONNECT)
		ret = doConnect(req, res);
	else if (method == HTTP_METHOD_PURGE)
		ret = doPurge(req, res);
	else
	{
		ret = false; // �п�����IOʧ�ܻ�δ֪����
		http_request_error_t n = req.getLastError();
		if (n == HTTP_REQ_ERR_METHOD)
			doUnknown(req, res);
		else
			doError(req, res);
	}

	if (in != out)
	{
		// ����Ǳ�׼���������������Ҫ�Ƚ����������׼����������
		// Ȼ������ͷ������������������ڲ����Զ��ж�������Ϸ���
		// �������Ա�֤��ͻ��˱��ֳ�����
		in->unbind();
		out->unbind();
		delete in;
		delete out;
	}

	return ret;
}

bool HttpServlet::doRun(const char* memcached_addr /* = "127.0.0.1:11211" */,
	socket_stream* stream /* = NULL */,
	bool body_parse /* = true */, int body_limit /* = 102400 */)
{
	memcache_session session(memcached_addr);
	return doRun(session, stream, body_parse, body_limit);
}

} // namespace acl
