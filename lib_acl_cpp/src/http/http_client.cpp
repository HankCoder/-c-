#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/zlib_stream.hpp"
#include "acl_cpp/stream/ostream.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/http/http_client.hpp"

namespace acl
{

http_client::http_client(void)
: stream_(NULL)
, stream_fixed_(false)
, hdr_res_(NULL)
, res_(NULL)
, hdr_req_(NULL)
, req_(NULL)
, rw_timeout_(120)
, unzip_(true)
, zstream_(NULL)
, is_request_(true)
, body_finish_(false)
, disconnected_(true)
, chunked_transfer_(false)
, buf_(NULL)
{
}

http_client::http_client(socket_stream* client, int rw_timeout /* = 120 */,
	bool is_request /* = false */, bool unzip /* = true */)
: stream_(client)
, stream_fixed_(true)
, hdr_res_(NULL)
, res_(NULL)
, hdr_req_(NULL)
, req_(NULL)
, rw_timeout_(rw_timeout)
, unzip_(unzip)
, zstream_(NULL)
, is_request_(is_request)
, body_finish_(false)
, disconnected_(false)
, chunked_transfer_(false)
, buf_(NULL)
{
}

http_client::~http_client(void)
{
	reset();
	if (!stream_fixed_)
		delete stream_;
	delete buf_;
}

void http_client::reset(void)
{
	if (buf_)
		buf_->clear();

	if (res_)
	{
		// ˵���ǳ����ӵĵڶ�������������Ҫ���ϴ������
		// ��Ӧͷ������Ӧ������ͷ�

		acl_assert(hdr_res_);
		http_res_free(res_);
		hdr_res_ = NULL;
		res_ = NULL;
	}
	else if (hdr_res_)
	{
		// ˵���ǳ����ӵĵڶ���������Ϊ�п��ܵ�һ������
		// ֻ����Ӧͷ�����Խ���Ҫ�ͷ��ϴε���Ӧͷ����

		http_hdr_res_free(hdr_res_);
		hdr_res_ = NULL;
	}

	if (req_)
	{
		acl_assert(hdr_req_);
		http_req_free(req_);
		hdr_req_ = NULL;
		req_  = NULL;
	}
	else if (hdr_req_)
	{
		http_hdr_req_free(hdr_req_);
		hdr_req_  = NULL;
	}

	if (zstream_)
	{
		delete zstream_;
		zstream_ = NULL;
	}

	last_ret_ = -1;
	body_finish_ = false;
	chunked_transfer_ = false;
}

bool http_client::open(const char* addr, int conn_timeout /* = 60 */,
	int rw_timeout /* = 60 */, bool unzip /* = true */)
{
	is_request_ = true;

	if (stream_ && !stream_fixed_)
	{
		delete stream_;
		stream_ = NULL;
		disconnected_ = true;
	}

	socket_stream* stream = NEW socket_stream();
	rw_timeout_ = rw_timeout;
	unzip_ = unzip;

	if (stream->open(addr, conn_timeout, rw_timeout) == false)
	{
		delete stream;
		disconnected_ = true;
		return false;
	}

	disconnected_ = false;
	stream_ = stream;
	return true;
}

int http_client::write_head(const http_header& header)
{
	chunked_transfer_ = header.chunked_transfer();
	string buf;
	if (header.is_request())
		header.build_request(buf);
	else
		header.build_response(buf);
	int ret = get_ostream().write(buf.c_str(), buf.length());
	if (ret < 0)
		disconnected_ = true;
	return ret;
}

bool http_client::write_body(const void* data, size_t len)
{
	ostream& fp = get_ostream();

	if (chunked_transfer_ == false)
	{
		if (data == NULL || len == 0)
			return true;
		if (fp.write(data, len) == -1)
		{
			disconnected_ = true;
			return false;
		}
		else
			return true;
	}

	// ��������� chunked ���䷽ʽ���򰴿鴫�䷽ʽд����

	if (data == NULL || len == 0)
	{
		if (fp.format("0\r\n\r\n") == -1)
		{
			disconnected_ = true;
			return false;
		}
		else
			return true;
	}

	if (fp.format("%x\r\n", (int) len) == -1
		|| fp.write(data, len) == -1
		|| fp.write("\r\n", 2) == -1)
	{
		disconnected_ = true;
		return false;
	}
	return true;
}

ostream& http_client::get_ostream(void) const
{
	acl_assert(stream_);
	return *stream_;
}

istream& http_client::get_istream(void) const
{
	acl_assert(stream_);
	return *stream_;
}

socket_stream& http_client::get_stream(void) const
{
	acl_assert(stream_);
	return *stream_;
}

bool http_client::read_head(void)
{
	if (is_request_)
		return read_response_head();
	else
		return read_request_head();
}

bool http_client::read_response_head(void)
{
	// �Է���һ����������ܵ��ϴ�����Ĳ������м����ݶ���
	reset();

	if (stream_ == NULL)
	{
		logger_error("connect stream not open yet");
		disconnected_ = true;
		return false;
	}
	ACL_VSTREAM* vstream = stream_->get_vstream();
	if (vstream == NULL)
	{
		logger_error("connect stream null");
		disconnected_ = true;
		return false;
	}

	hdr_res_ = http_hdr_res_new();
	int   ret = http_hdr_res_get_sync(hdr_res_, vstream, rw_timeout_);
	if (ret == -1)
	{
		http_hdr_res_free(hdr_res_);
		hdr_res_ = NULL;
		disconnected_ = true;
		return false;
	}

	if (http_hdr_res_parse(hdr_res_) < 0)
	{
		logger_error("parse response header error");
		http_hdr_res_free(hdr_res_);
		hdr_res_ = NULL;
		disconnected_ = true;
		return false;
	}

	// �鴫������ȼ����
	if (!hdr_res_->hdr.chunked)
	{
		// �����������Ӧʱ��ȷָ���˳���Ϊ 0 ���ʾ��û��������
		if (hdr_res_->hdr.content_length == 0)
		{
			body_finish_ = true;
			return true;
		}
	}

	const char* ptr = http_hdr_entry_value(&hdr_res_->hdr,
			"Content-Encoding");
	if (ptr && unzip_)
	{
		// Ŀǰ��֧�� gzip ���ݵĽ�ѹ
		if (strcasecmp(ptr, "gzip") == 0)
		{
			zstream_ = NEW zlib_stream();
			if (zstream_->unzip_begin(false) == false)
			{
				logger_error("unzip_begin error");
				delete zstream_;
				zstream_ = NULL;
			}
			gzip_header_left_ = 10;
		}
		else
			logger_warn("unknown compress format: %s", ptr);
	}

	return true;
}

bool http_client::read_request_head(void)
{
	// �Է���һ����������ܵ��ϴ�����Ĳ������м����ݶ���
	reset();

	if (stream_ == NULL)
	{
		logger_error("client stream not open yet");
		disconnected_ = true;
		return false;
	}
	ACL_VSTREAM* vstream = stream_->get_vstream();
	if (vstream == NULL)
	{
		logger_error("client stream null");
		disconnected_ = true;
		return false;
	}

	hdr_req_ = http_hdr_req_new();
	int   ret = http_hdr_req_get_sync(hdr_req_, vstream, rw_timeout_);
	if (ret == -1)
	{
		http_hdr_req_free(hdr_req_);
		hdr_req_ = NULL;
		disconnected_ = true;
		return false;
	}

	if (http_hdr_req_parse(hdr_req_) < 0)
	{
		logger_error("parse request header error");
		http_hdr_req_free(hdr_req_);
		hdr_req_ = NULL;
		disconnected_ = true;
		return false;
	}

	if (hdr_req_->hdr.content_length <= 0)
		body_finish_ = true;
	return true;
}

acl_int64 http_client::body_length() const
{
	if (is_request_)
	{
		if (hdr_res_)
			return hdr_res_->hdr.content_length;
	}
	else if (hdr_req_)
		return hdr_req_->hdr.content_length;
	return -1;
}

bool http_client::keep_alive() const
{
	if (is_request_)
	{
		if (hdr_res_)
			return hdr_res_->hdr.keep_alive ? true : false;
	}
	else if (hdr_req_)
		return hdr_req_->hdr.keep_alive ? true : false;
	return false;
}

const char* http_client::header_value(const char* name) const
{
	if (is_request_)
	{
		if (hdr_res_)
			return http_hdr_entry_value(&hdr_res_->hdr, name);
	}
	else if (hdr_req_)
		return http_hdr_entry_value(&hdr_req_->hdr, name);
	return NULL;
}

int http_client::response_status(void) const
{
	if (is_request_ && hdr_res_)
		return hdr_res_->reply_status;
	return -1;
}

const char* http_client::request_host(void) const
{
	if (!is_request_ && hdr_req_)
		return hdr_req_->host;
	return NULL;
}

int http_client::request_port(void) const
{
	if (!is_request_ && hdr_req_)
		return hdr_req_->port;
	return -1;
}

const char* http_client::request_method(void) const
{
	if (!is_request_ && hdr_req_)
		return hdr_req_->method;
	return NULL;
}

const char* http_client::request_url(void) const
{
	if (!is_request_ && hdr_req_ && hdr_req_->url_part)
		return acl_vstring_str(hdr_req_->url_part);
	return NULL;
}

const char* http_client::request_path(void) const
{
	if (!is_request_ && hdr_req_ && hdr_req_->url_path)
		return acl_vstring_str(hdr_req_->url_path);
	return NULL;
}

const char* http_client::request_params(void) const
{
	if (!is_request_ && hdr_req_ && hdr_req_->url_params)
		return acl_vstring_str(hdr_req_->url_params);
	return NULL;
}

const char* http_client::request_param(const char* name) const
{
	if (!is_request_ && hdr_req_)
		return http_hdr_req_param(hdr_req_, name);
	return NULL;
}

const char* http_client::request_cookie(const char* name) const
{
	if (!is_request_ && hdr_req_)
		return http_hdr_req_cookie_get(hdr_req_, name);
	return NULL;
}

int http_client::read_body(char* buf, size_t size)
{
	if (is_request_)
		return read_response_body(buf, size);
	else
		return read_request_body(buf, size);
}

int http_client::read_response_body(char* buf, size_t size)
{
	if (hdr_res_ == NULL)
	{
		logger_error("response header not get yet");
		disconnected_ = true;
		return -1;
	}

	if (stream_ == NULL)
	{
		logger_error("not connected yet");
		disconnected_ = true;
		return -1;
	}
	ACL_VSTREAM* vstream = stream_->get_vstream();
	if (vstream == NULL)
	{
		logger_error("connect stream null");
		disconnected_ = true;
		return -1;
	}

	if (res_ == NULL)
		res_ = http_res_new(hdr_res_);

	// ������̫����û���κ�����
	if (size >= 1024000)
		size = 1024000;
	http_off_t ret = http_res_body_get_sync(res_, vstream, buf, size);

	if (ret <= 0)
	{
		// ����ڶ���Ӧͷʱ������ unzip_begin������뱣֤��������
		// ���ٵ��� unzip_finish���������Ϊ zlib �������ƶ�����
		// �ڴ�й¶
		if (zstream_ != NULL)
		{
			string dummy(64);
			zstream_->unzip_finish(&dummy);
		}
		body_finish_ = true;
		if (ret < 0)
			disconnected_ = true;
	}

	return (int) ret;
}

int http_client::read_request_body(char* buf, size_t size)
{
	if (hdr_req_ == NULL)
	{
		logger_error("request header not get yet");
		disconnected_ = true;
		return -1;
	}

	if (stream_ == NULL)
	{
		logger_error("not connected yet");
		disconnected_ = true;
		return -1;
	}
	ACL_VSTREAM* vstream = stream_->get_vstream();
	if (vstream == NULL)
	{
		logger_error("client stream null");
		disconnected_ = true;
		return -1;
	}

	if (req_ == NULL)
		req_ = http_req_new(hdr_req_);

	// ������̫����û���κ�����
	if (size >= 1024000)
		size = 1024000;
	http_off_t ret = http_req_body_get_sync(req_, vstream, buf, (int) size);

	if (ret <= 0)
	{
		body_finish_ = true;
		if (ret < 0)
			disconnected_ = true;
	}

	return ((int) ret);
}

bool http_client::body_finish(void) const
{
	return body_finish_;
}

bool http_client::disconnected() const
{
	return disconnected_;
}

int http_client::read_body(string& out, bool clean /* = true */,
	int* real_size /* = NULL */)
{
	if (is_request_)
		return read_response_body(out, clean, real_size);
	else
		return read_request_body(out, clean, real_size);
}

int http_client::read_response_body(string& out, bool clean,
	int* real_size)
{
	if (real_size)
		*real_size = 0;
	if (body_finish_)
		return (last_ret_);

	if (stream_ == NULL)
	{
		logger_error("connect null");
		disconnected_ = true;
		return -1;
	}
	ACL_VSTREAM* vstream = stream_->get_vstream();
	if (vstream == NULL)
	{
		logger_error("connect stream null");
		disconnected_ = true;
		return -1;
	}

	if (hdr_res_ == NULL)
	{
		logger_error("response header not get yet");
		disconnected_ = true;
		return -1;
	}
	if (res_ == NULL)
		res_ = http_res_new(hdr_res_);

	if (clean)
		out.clear();

	int   saved_count = (int) out.length();
	char  buf[8192];

SKIP_GZIP_HEAD_AGAIN:  // ������ GZIP ͷ���ݣ�������Ҫ�ظ���

	int ret = (int) http_res_body_get_sync(res_, vstream, buf, sizeof(buf));

	if (zstream_ == NULL)
	{
		if (ret > 0)
		{
			out.append(buf, ret);
			if (real_size)
				*real_size = ret;
		}
		else
		{
			body_finish_ = true; // ��ʾ�����Ѿ�����
			if (ret < 0)
				disconnected_ = true;
			last_ret_ = ret;
		}
		return ret;
	}

	if (ret <= 0)
	{
		if (zstream_->unzip_finish(&out) == false)
		{
			logger_error("unzip_finish error");
			return -1;
		}

		last_ret_ = ret; // ��¼����ֵ
		body_finish_ = true; // ��ʾ�����Ѿ������ҽ�ѹ�����
		if (ret < 0)
			disconnected_ = true;
		return (int) out.length() - saved_count;
	}

	if (real_size)
		(*real_size) += ret;

	// ��Ҫ������ gzip ͷ

	if (gzip_header_left_ >= ret)
	{
		gzip_header_left_ -= ret;
		goto SKIP_GZIP_HEAD_AGAIN;
	}

	int  n;
	if (gzip_header_left_ > 0)
	{
		n = gzip_header_left_;
		gzip_header_left_ = 0;
	}
	else
		n = 0;
	if (zstream_->unzip_update(buf + n, ret - n, &out) == false)
	{
		logger_error("unzip_update error");
		return -1;
	}
	return (int) out.length() - saved_count;
}

int http_client::read_request_body(string& out, bool clean,
	int* real_size)
{
	if (real_size)
		*real_size = 0;
	if (body_finish_)
		return last_ret_;

	if (stream_ == NULL)
	{
		logger_error("client null");
		disconnected_ = true;
		return -1;
	}
	ACL_VSTREAM* vstream = stream_->get_vstream();
	if (vstream == NULL)
	{
		logger_error("client stream null");
		disconnected_ = true;
		return -1;
	}

	if (hdr_req_ == NULL)
	{
		logger_error("request header not get yet");
		disconnected_ = true;
		return -1;
	}
	if (req_ == NULL)
		req_ = http_req_new(hdr_req_);

	if (clean)
		out.clear();

	char  buf[8192];

	int ret = (int) http_req_body_get_sync(req_, vstream, buf, sizeof(buf));

	if (ret > 0)
	{
		out.append(buf, ret);
		if (real_size)
			*real_size = ret;
	}
	else
	{
		body_finish_ = true; // ��ʾ�����Ѿ�����
		if (ret < 0)
			disconnected_ = true;
		last_ret_ = ret;
	}
	return ret;
}

bool http_client::body_gets(string& out, bool nonl /* = true */,
	size_t* size /* = NULL */)
{
	if (buf_ == NULL)
		buf_ = new string(4096);

	size_t len, size_saved = out.length();

	// �����ж��Ƿ��Ѿ����� HTTP ������
	if (body_finish_)
	{
		if (buf_->empty())
		{
			if (size)
				*size = 0;
			return false;
		}

		// �������������ݷǿ�ʱ���ȳ��Դ��л�ȡһ������
		if (buf_->scan_line(out, nonl, &len) == true)
		{
			if (size)
				*size = out.length() - size_saved;
			return true;
		}

		// ������ܶ��������������� HTTP ����������������򽫶�
		// �������ڵ����ݶ�������Ŀ�껺����
		out.append(buf_);
		buf_->clear();

		if (size)
			*size = out.length() - size_saved;

		return true;
	}

	// ������ HTTP �����壬�����Դ��ж�ȡһ������

	len = 0;

	while (true)
	{
		if (!buf_->empty())
		{
			if (buf_->scan_line(out, nonl, &len) == true)
			{
				if (size)
					*size = out.length() - size_saved;
				return true;
			}

			// Ϊ�˼����´�ѭ��ʱ���� scan_line ���ַ������Ҵ�����
			// �����������е������ȿ�����Ŀ�껺������

			len += buf_->length();
			out.append(buf_);
			buf_->clear();
		}

		if (body_finish_)
		{
			if (size)
				*size = len;
			return len > 0 ? true : false;
		}

		if (read_body(*buf_, false) <= 0)
			body_finish_ = true;
		else
			body_finish_ = false;
	}
}

HTTP_HDR_RES* http_client::get_respond_head(string* buf)
{
	if (hdr_res_ == NULL)
		return (NULL);
	if (buf)
	{
		ACL_VSTRING* vbf = buf->vstring();
		http_hdr_build(&hdr_res_->hdr, vbf);
	}
	return hdr_res_;
}

HTTP_HDR_REQ* http_client::get_request_head(string* buf)
{
	if (hdr_req_ == NULL)
		return NULL;
	if (buf)
	{
		ACL_VSTRING* vbf = buf->vstring();
		http_hdr_build(&hdr_req_->hdr, vbf);
	}
	return hdr_req_;
}

void http_client::print_header(const char* prompt)
{
	if (hdr_res_)
		http_hdr_print(&hdr_res_->hdr, prompt ? prompt : "dummy");
	else if (hdr_req_)
		http_hdr_print(&hdr_req_->hdr, prompt ? prompt : "dummy");
}

void http_client::fprint_header(ostream& out, const char* prompt)
{
	ACL_VSTREAM* fp = out.get_vstream();
	if (fp == NULL)
	{
		logger_error("fp stream null");
		return;
	}
	if (hdr_res_)
		http_hdr_fprint(fp, &hdr_res_->hdr, prompt ? prompt : "dummy");
	else if (hdr_req_)
		http_hdr_fprint(fp, &hdr_req_->hdr, prompt ? prompt : "dummy");
}

void http_client::sprint_header(string& out, const char* prompt)
{
	ACL_VSTRING* bf = out.vstring();
	if (bf == NULL)
	{
		logger_error("vstring null");
		return;
	}
	if (hdr_res_)
		http_hdr_sprint(bf, &hdr_res_->hdr, prompt ? prompt : "dummy");
	else if (hdr_req_)
		http_hdr_sprint(bf, &hdr_req_->hdr, prompt ? prompt : "dummy");
}

}  // namespace acl
