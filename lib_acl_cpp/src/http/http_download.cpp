#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/http/http_utils.hpp"
#include "acl_cpp/http/http_request.hpp"
#include "acl_cpp/http/http_client.hpp"
#include "acl_cpp/http/http_download.hpp"

namespace acl
{

http_download::http_download(const char* url, const char* addr /* = NULL */)
{
	if (addr)
		ACL_SAFE_STRNCPY(addr_, addr, sizeof(addr_));
	else if (http_utils::get_addr(url, addr_, sizeof(addr_)) == false)
	{
		logger_error("url(%s) invalid", url);
		addr_[0] = 0;
		url_ = NULL;
		req_ = NULL;
		return;
	}

	url_ = acl_mystrdup(url);
	req_ = NEW http_request(addr_);  // HTTP �������
	req_->request_header().set_url(url_)
		.set_content_type("text/html")
		.set_host(addr_);
}

http_download::~http_download()
{
	if (url_)
		acl_myfree(url_);
	delete req_;
}

const char* http_download::get_url() const
{
	return url_;
}

const char* http_download::get_addr() const
{
	return addr_[0] == 0 ? NULL : addr_;
}

bool http_download::reset(const char* url /* = NULL */,
	const char* addr /* = NULL */)
{
	if (url)
	{
		if (url_)
		{
			acl_myfree(url_);
			url_ = NULL;
		}
		if (addr)
			ACL_SAFE_STRNCPY(addr_, addr, sizeof(addr_));
		else if (http_utils::get_addr(url, addr_,
			sizeof(addr_)) == false)
		{
			logger_error("url(%s) invalid", url);
			addr_[0] = 0;
			if (req_)
			{
				delete req_;
				req_ = NULL;
			}
			return false;
		}
		url_ = acl_mystrdup(url);
	}
	else if (url_ == NULL)
		return false;
	else if (addr)
		ACL_SAFE_STRNCPY(addr_, addr, sizeof(addr_));

	if (req_)
		delete req_;
	req_ = NEW http_request(addr_);  // HTTP �������
	req_->request_header().set_url(url_)
		.set_content_type("text/html")
		.set_host(addr_);
	return true;
}

http_header* http_download::request_header() const
{
	return req_ ? &req_->request_header() : NULL;
}

http_request* http_download::request() const
{
	return req_ ? req_ : NULL;
}

bool http_download::on_response(http_client*)
{
	return true;
}

bool http_download::on_length(long long int)
{
	return true;
}

bool http_download::get(acl_int64 from /* = -1 */, acl_int64 to /* = -1 */,
	const char* body /* = NULL */, size_t len /* = 0 */)
{
	if (req_ == NULL)
	{
		logger_error("no valid url");
		return false;
	}
	else if (from >= 0)
		return save_range(body, len, from, to);
	else
		return save_total(body, len);
}

bool http_download::save_total(const char* body, size_t len)
{
	// ���Ͳ��� range �ֶε���������

	http_method_t method = body && len > 0
		? HTTP_METHOD_POST : HTTP_METHOD_GET;

	// ���� HTTP ����ͷ��Ϣ
	req_->request_header().set_method(method);

	// ���� HTTP ��������
	if (req_->request(body, len) == false)
	{
		logger_error("send request error, url: %s", url_);
		return false;
	}

	http_client* conn = req_->get_client();
	if (conn == NULL)
		logger_fatal("no connect to server");

	// �ص�������ӿ�ʵ��
	if (on_response(conn) == false)
	{
		logger_error("deny url(%s)'s download", url_);
		return false;
	}

	// ����ļ�����
	acl_int64 length = conn->body_length();

	// �ص�������ӿ�ʵ��
	if (on_length(length) == false)
	{
		logger_error("deny url(%s)'s download", url_);
		return false;
	}

	// ��ʼ�������������
	return save(req_);
}

bool http_download::save_range(const char* body, size_t len,
	acl_int64 range_from, acl_int64 range_to)
{
	if (range_from < 0)
	{
		logger_error("invalid range_from: %lld", range_from);
		return false;
	}
	else if (range_to >= 0 && range_to < range_from)
	{
		logger_error("invalid, 0 <= range_to: %lld < range_from: %lld",
			range_to, range_from);
		return false;
	}

	http_method_t method = body && len > 0
		? HTTP_METHOD_POST : HTTP_METHOD_GET;

	// ���ʹ� range �ֶε���������

	// ���� HTTP ����ͷ��Ϣ
	req_->request_header().set_method(method)
		.set_range(range_from, range_to);

	// ���� HTTP ��������
	if (req_->request(NULL, 0) == false)
	{
		logger_error("send request error, url: %s", url_);
		return false;
	}

	// ����ļ�����
	acl_int64 length = req_->get_range_max();
	if (length <= 0)
	{
		// �����Ƿ�������֧�� range ���ܣ����Ի��ô�ͷ����
		return false;
	}

	http_client* conn = req_->get_client();
	if (conn == NULL)
		logger_fatal("no connect to server");

	// �ص�������ӿ�ʵ��
	if (on_response(conn) == false)
	{
		logger_error("deny url(%s)'s download", url_);
		return false;
	}

	// �ص�������ӿ�ʵ��
	if (on_length(length) == false)
	{
		logger_error("deny url(%s)'s download", url_);
		return false;
	}

	return save(req_);
}

bool http_download::save(http_request* req)
{
	// ��ʼ���շ���˵� HTTP ������

	char  buf[8192];
	int   ret;
	while (true)
	{
		ret = req->read_body(buf, sizeof(buf));
		if (ret <= 0)
			break;
		if (on_save(buf, ret) == false)
		{
			logger_error("on_save error, url: %s", url_);
			return false;
		}
	}
	return true;
}

} // namespace acl
