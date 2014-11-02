#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stdlib/xml.hpp"
#include "acl_cpp/stdlib/json.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/stdlib/charset_conv.hpp"
#include "acl_cpp/stdlib/pipe_stream.hpp"
#include "acl_cpp/http/http_client.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/http/HttpCookie.hpp"
#include "acl_cpp/http/http_ctype.hpp"
#include "acl_cpp/http/http_pipe.hpp"
#include "acl_cpp/http/http_request.hpp"

namespace acl
{

#define RESET_RANGE() do \
{ \
	range_from_ = -1; \
	range_to_ = -1; \
	range_max_ = -1; \
} while(0)

http_request::http_request(socket_stream* client,
	int conn_timeout /* = 60 */, bool unzip /* = true */)
{
	// ���ý�ѹ����
	client_ = NEW http_client(client, client->get_vstream()->rw_timeout,
		true, unzip);
	unzip_ = unzip;
	local_charset_[0] = 0;
	conv_ = NULL;

	const char* ptr = client->get_peer(true);
	acl_assert(ptr);
	ACL_SAFE_STRNCPY(addr_, ptr, sizeof(addr_));
	rw_timeout_ = client->get_vstream()->rw_timeout;
	conn_timeout_ = conn_timeout;
	header_.set_url("/");
	header_.set_keep_alive(true);
	header_.set_host(addr_);
	cookie_inited_ = false;
	cookies_ = NULL;
	need_retry_ = true;
	RESET_RANGE();
}

http_request::http_request(const char* addr, int conn_timeout /* = 60 */,
	int rw_timeout /* = 60 */, bool unzip /* = true */)
{
	acl_assert(addr && *addr);
	ACL_SAFE_STRNCPY(addr_, addr, sizeof(addr_));
	conn_timeout_ = conn_timeout;
	rw_timeout_ = rw_timeout;
	unzip_ = unzip;
	local_charset_[0] = 0;
	conv_ = NULL;

	header_.set_url("/");
	header_.set_keep_alive(true);
	header_.set_host(addr_);
	cookie_inited_ = false;
	cookies_ = NULL;
	client_ = NULL;
	need_retry_ = true;
	RESET_RANGE();
}

http_request::~http_request(void)
{
	close();
	if (cookies_)
	{
		reset();
		delete cookies_;
	}
	else
		delete conv_;
}

void http_request::close(void)
{
	delete client_;
	client_ = NULL;
}

void http_request::reset(void)
{
	if (cookies_)
	{
		std::vector<HttpCookie*>::iterator it = cookies_->begin();
		for (; it != cookies_->end(); ++it)
			(*it)->destroy();
		cookies_->clear();
		cookie_inited_ = false;
	}

	delete conv_;
	conv_ = NULL;
	need_retry_ = true;
	RESET_RANGE();
}

void http_request::set_timeout(int conn_timeout, int rw_timeout)
{
	conn_timeout_ = conn_timeout;
	rw_timeout_ = rw_timeout;
}

http_request& http_request::set_unzip(bool on)
{
	unzip_ = on;
	return *this;
}

bool http_request::open()
{
	bool reuse;
	return try_open(&reuse);
}

bool http_request::try_open(bool* reuse_conn)
{
	if (client_)
	{
		client_->reset();
		*reuse_conn = true;
		return true;
	}

	*reuse_conn = false;

	client_ = new http_client();

	if (client_->open(addr_, conn_timeout_,
		rw_timeout_, unzip_) == false)
	{
		logger_error("connect server(%s) error(%s)",
			addr_, last_serror());
		close();
		return false;
	}

	return true;
}

http_header& http_request::request_header()
{
	return header_;
}

http_client* http_request::get_client(void) const
{
	return client_;
}

bool http_request::write_head()
{
	acl_assert(client_);  // ���뱣֤�������Ѿ���
	bool  reuse_conn;
	http_method_t method = header_.get_method();

	while (true)
	{
		// ���Դ�Զ������
		if (try_open(&reuse_conn) == false)
		{
			logger_error("connect server error");
			need_retry_ = false;
			return false;

		}

		// ������´��������ӣ���������
		if (!reuse_conn)
			need_retry_ = false;

		// ������󷽷��� GET������Ҫ����̽��һ�������Ƿ�����
		if (method != HTTP_METHOD_GET && method != HTTP_METHOD_PURGE)
		{
			socket_stream& ss = client_->get_stream();

			/* ��Ϊϵͳ write API �ɹ������ܱ�֤��������������ֻ��
			 * �ǵ���ϵͳ read API ��̽�������Ƿ��������ú����ڲ�
			 * �Ὣ�׽ӿ���ת�������׽ӿڽ��ж����������Բ���������
			 * ͬʱ��ʹ�����ݶ���Ҳ���ȷŵ� ACL_VSTREAM �Ķ������У�
			 * ����Ҳ���ᶪʧ����
			 */
			if (ss.alive() == false)
			{
				close();

				// �����´��������ӣ���ֱ�ӱ���
				if (!reuse_conn)
				{
					logger_error("new connection error");
					return false;
				}
				need_retry_ = false;
				continue;
			}
		}

		client_->reset();  // ����״̬

		// ���� HTTP ����ͷ
		if (client_->write_head(header_) > 0)
			return true;

		close();

		if (!need_retry_)
			return false;

		// ��ȡ�����Ա�־λ��Ȼ��������һ��
		need_retry_ = false;
	}
}

bool http_request::write_body(const void* data, size_t len)
{
	while (true)
	{
		if (client_->write_body(data, len) == false)
		{
			if (!need_retry_)
				return false;

			// ȡ�����Ա�־λ
			need_retry_ = false;

			// ������һ��
			if (write_head() == false)
				return false;

			// �ٴ�д������
			continue;
		}

		// ˵�������Ѿ�����д�����ˣ�����Ӧ��ȡ�����Ա�־λ
		need_retry_ = false;

		// ������ݷǿգ���˵���������ݿ�д
		if (data != NULL && len > 0)
			return true;

		// data == NULL || len == 0 ʱ����ʾ��������
		// �Ѿ�������ϣ���ʼ�ӷ���˶�ȡ HTTP ��Ӧ����
		// �� HTTP ��Ӧͷ
		if (client_->read_head() == true)
			break;

		return false;
	}

	// ˵�����������Ѿ�������ϣ����ҳɹ���ȡ�� HTTP ��Ӧͷ��
	// ������Զ�ȡ HTTP ��Ӧ��������

	// �����ַ���ת����
	set_charset_conv();

	// ��鷵��ͷ���Ƿ��� Content-Range �ֶ�
	check_range();

	return true;
}

bool http_request::send_request(const void* data, size_t len)
{
	acl_assert(client_);  // ���뱣֤�������Ѿ���
	client_->reset();  // ����״̬

	// д HTTP ����ͷ
	if (client_->write_head(header_) < 0)
	{
		close();
		return false;
	}

	// д HTTP ������
	if (client_->write_body(data, len) == false)
	{
		close();
		return false;
	}

	return true;
}

bool http_request::request(const void* data, size_t len)
{
	bool  have_retried = false;
	bool  reuse_conn;

	// ���� HTTP ����ͷ
	if (data && len > 0)
	{
		http_method_t method = header_.get_method();
		// ����������������£�HTTP ���󷽷�����Ϊ��������֮һ��
		// HTTP_METHOD_POST �� HTTP_METHOD_PUT
		if (method != HTTP_METHOD_POST && method != HTTP_METHOD_PUT)
			header_.set_method(HTTP_METHOD_POST);
		header_.set_content_length(len);
	}

	while (true)
	{
		// ���Դ�Զ������
		if (try_open(&reuse_conn) == false)
		{
			logger_error("connect server error");
			return false;
		}

		// ���� HTTP ������������
		if (send_request(data, len) == false)
		{
			if (have_retried || !reuse_conn)
			{
				logger_error("send request error");
				return false;
			}

			// ���ڳ����ӣ�����ǵ�һ��IOʧ�ܣ������������һ��
			have_retried = true;
			continue;
		}

		client_->reset();  // ����״̬

		// �� HTTP ��Ӧͷ
		if (client_->read_head() == true)
			break;

		if (have_retried || !reuse_conn)
		{
			logger_error("read response header error");
			close();
			return false;
		}

		// �ȹر�֮ǰ��������
		close();

		// ���ڳ����ӣ�����ǵ�һ��IOʧ�ܣ������������һ��
		have_retried = true;
	}

	// �����ַ���ת����
	set_charset_conv();

	// ��鷵��ͷ���Ƿ��� Content-Range �ֶ�
	check_range();

	return true;
}

int http_request::http_status() const
{
	acl_assert(client_);
	return client_->response_status();
}

acl_int64 http_request::body_length() const
{
	acl_assert(client_);
	return client_->body_length();
}

bool http_request::keep_alive() const
{
	acl_assert(client_);
	return client_->keep_alive();
}

const char* http_request::header_value(const char* name) const
{
	acl_assert(client_);
	return client_->header_value(name);
}

bool http_request::body_finish() const
{
	acl_assert(client_);
	return client_->body_finish();
}

void http_request::check_range()
{
	http_off_t range_from, range_to;
	acl_int64 length;

	// ��ȡ���û�������ʱ���õ� range �ֶΣ����û������ֱ�ӷ���
	header_.get_range(&range_from, &range_to);
	if (range_from < 0)
		return;

	const HTTP_HDR_RES* hdr_res = client_->get_respond_head(NULL);

	// �� HTTP ��������Ӧ�л�� range ��Ӧ�ֶΣ����û����˵��
	// ��������֧�� range ����
	if (http_hdr_res_range((HTTP_HDR_RES*) hdr_res, &range_from_,
		&range_to_, &range_max_) < 0)
	{
		RESET_RANGE();
	}

	// ������������ص� range ����������Ĳ�һ�£���˵���д�
	else if (range_from_ != range_from)
	{
		logger_error("range_from(%lld) != %lld",
			range_from_, range_from);
		RESET_RANGE();
	}
	else if (range_to >= range_from && range_to_ != range_to)
	{
		logger_error("range_to(%lld) != %lld", range_to_, range_to);
		RESET_RANGE();
	}

	// ��Ȼ�û������� range ���󣬵����͵��������������������壬
	// ����Ҫ��������峤�ȵ�һ����
	else if (range_from == 0 && range_to < 0
		&& (length = client_->body_length()) > 0
		&& range_max_ != length)
	{
		logger_error("range_total_length: %lld != content_length: %lld",
			range_max_, length);
		RESET_RANGE();
	}
}

bool http_request::support_range() const
{
	return range_from_ >= 0 ? true : false;
}

acl_int64 http_request::get_range_from() const
{
	return range_from_;
}

acl_int64 http_request::get_range_to() const
{
	return range_to_;
}

acl_int64 http_request::get_range_max() const
{
	return range_max_;
}

http_request& http_request::set_local_charset(const char* local_charset)
{
	ACL_SAFE_STRNCPY(local_charset_, local_charset,
		sizeof(local_charset_));
	return *this;
}

void http_request::set_charset_conv()
{
	if (client_ == NULL || local_charset_[0] == 0)
		return;

	// ��Ҫ�����Ӧͷ�ַ�����Ϣ
	const char* ptr = client_->header_value("Content-Type");
	if (ptr == NULL || *ptr == 0)
		return;

	http_ctype ctype;
	ctype.parse(ptr);

	const char* from_charset = ctype.get_charset();

	if (from_charset == NULL || *from_charset == 0
		|| strcasecmp(from_charset, local_charset_) == 0)
	{
		return;
	}

	// ��ʼ���ַ���ת����

	if (conv_ == NULL)
		conv_ = charset_conv::create(from_charset, local_charset_);

	// ����֮ǰ�������ַ���ת����
	else if (conv_->update_begin(from_charset, local_charset_) == false)
	{
		logger_error("invalid charset conv, from %s, to %s",
			from_charset, local_charset_);
		delete conv_;
		conv_ = NULL;
	}
}

http_pipe* http_request::get_pipe(const char* to_charset)
{
	if (to_charset != NULL)
	{
		// ���������ַ���ת����
		set_local_charset(to_charset);

		// ��ȡ�ַ���ת����
		set_charset_conv();
	}

	if (conv_ == NULL)
		return NULL;

	http_pipe* hp = NEW http_pipe();

	// ���ַ���ת�������� http_pipe ����
	hp->set_charset(conv_);
	conv_ = NULL;

	return hp;
}

bool http_request::get_body(xml& out, const char* to_charset /* = NULL */)
{
	if (client_ == NULL)
		return false;

	http_pipe* hp = get_pipe(to_charset);
	if (hp)
		hp->append(&out);

	string  buf(4096);
	int   ret;

	// �� HTTP ��Ӧ�壬���� xml ��ʽ���з���
	while (true)
	{
		// ���ÿ����Զ���ѹ���Ķ�����
		ret = client_->read_body(buf);
		if (ret < 0)
		{
			close();
			break;
		}
		else if (ret == 0)
			break;
		if (hp)
			hp->update(buf.c_str(), ret);
		else
			out.update(buf.c_str());
	}

	if (hp)
	{
		hp->update_end();
		delete hp;
	}
	return true;
}

bool http_request::get_body(json& out, const char* to_charset /* = NULL */)
{
	if (client_ == NULL)
		return false;

	http_pipe* hp = get_pipe(to_charset);
	if (hp)
		hp->append(&out);

	string  buf(4096);
	int   ret;
	// �� HTTP ��Ӧ�壬���� json ��ʽ���з���Ա
	while (true)
	{
		ret = client_->read_body(buf);
		if (ret < 0)
		{
			close();
			break;
		}
		else if (ret == 0)
			break;
		if (hp)
			hp->update(buf.c_str(), ret);
		else
			out.update(buf.c_str());
	}

	if (hp)
	{
		hp->update_end();
		delete hp;
	}
	return true;
}

bool http_request::get_body(string& out, const char* to_charset /* = NULL */)
{
	if (client_ == NULL)
		return false;

	http_pipe* hp = get_pipe(to_charset);
	if (hp)
	{
		pipe_string ps(out);
		hp->append(&ps);
	}

	string  buf(4096);
	int   ret;
	// �� HTTP ��Ӧ��
	while (true)
	{
		ret = client_->read_body(buf);
		if (ret < 0)
		{
			close();
			break;
		}
		else if (ret == 0)
			break;
		if (hp)
			hp->update(buf.c_str(), ret);
		else
			out.append(buf);
	}

	if (hp)
	{
		hp->update_end();
		delete hp;
	}
	return true;
}

int http_request::read_body(string& out, bool clean /* = NULL */,
	int* real_size /* = NULL */)
{
	if (clean)
		out.clear();
	if (client_ == NULL)
		return -1;

	int   ret;

	if (conv_ == NULL)
	{
		ret = client_->read_body(out, clean, real_size);
		if (ret < 0)
			close();
		return ret;
	}

	size_t saved_size = out.length();
	string  buf(4096);
	ret = client_->read_body(buf, true, real_size);
	if (ret < 0)
	{
		conv_->update_finish(&out);
		close();
		return ret;
	}

	if (ret == 0)
		conv_->update_finish(&out);
	else if (ret > 0)
		conv_->update(buf.c_str(), ret, &out);

	size_t curr_size = out.length();

	// �ڽ����ַ���ת��ʱ�����ݳߴ���ܱ仯�����Ը���ǰ��ʵ��
	// ���ݳߴ�֮�������㱾�ζ��������ݳ���
	return (int) (curr_size - saved_size);
}

bool http_request::body_gets(string& out, bool nonl /* = true */,
	size_t* size /* = NULL */)
{
	if (size)
		*size = 0;
	if (client_ == NULL)
		return false;

	if (conv_ == NULL)
	{
		if (client_->body_gets(out, nonl, size) == true)
			return true;
		else
		{
			if (client_->disconnected())
				close();
			return false;
		}
	}

	size_t n, size_saved = out.length();
	string line(1024);
	if (client_->body_gets(line, nonl, &n) == false)
	{
		if (!line.empty())
			conv_->update(line.c_str(), line.length(), &out);
		conv_->update_finish(&out);
		if (size)
			*size = out.length() - size_saved;
		if (client_->disconnected())
			close();
		return false;
	}

	if (!line.empty())
		conv_->update(line.c_str(), line.length(), &out);
	conv_->update_finish(&out);
	if (size)
		*size = out.length() - size_saved;

	return true;
}


int http_request::read_body(char* buf, size_t size)
{
	if (client_ == NULL)
		return -1;
	return client_->read_body(buf, size);
}

const std::vector<HttpCookie*>* http_request::get_cookies() const
{
	if (cookies_ && cookie_inited_)
		return cookies_;
	const_cast<http_request*>(this)->create_cookies();
	if (cookie_inited_ == false)
		return NULL;
	return cookies_;
}

const HttpCookie* http_request::get_cookie(const char* name,
	bool case_insensitive /* = true */) const
{
	if (!cookie_inited_)
		const_cast<http_request*>(this)->create_cookies();
	if (cookies_ == NULL)
		return NULL;
	std::vector<HttpCookie*>::const_iterator cit = cookies_->begin();
	for (; cit != cookies_->end(); ++cit)
	{
		if (case_insensitive)
		{
			if (strcasecmp((*cit)->getName(), name) == 0)
			{
				return *cit;
			}
		}
		else if (strcmp((*cit)->getName(), name) == 0)
		{
			return *cit;
		}
	}
	return NULL;
}

void http_request::create_cookies()
{
	acl_assert(cookie_inited_ == false);
	if (client_ == NULL)
		return;
	const HTTP_HDR_RES* res = client_->get_respond_head(NULL);
	if (res == NULL)
		return;

	if (cookies_ == NULL)
		cookies_ = NEW std::vector<HttpCookie*>;

	int n = acl_array_size(res->hdr.entry_lnk);
	for (int i = 0; i < n; i++)
	{
		const HTTP_HDR_ENTRY* hdr = (const HTTP_HDR_ENTRY*)
			acl_array_index(res->hdr.entry_lnk, i);
		if (strcasecmp(hdr->name, "Set-Cookie") != 0)
			continue;
		if (hdr->value == NULL || *(hdr->value) == 0)
			continue;
		HttpCookie* cookie = NEW HttpCookie();
		if (cookie->setCookie(hdr->value) == false)
		{
			cookie->destroy();
			continue;
		}
		cookies_->push_back(cookie);
	}
	cookie_inited_ = true;
}

} // namespace acl
