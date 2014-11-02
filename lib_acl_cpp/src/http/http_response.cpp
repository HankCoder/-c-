#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/xml.hpp"
#include "acl_cpp/stdlib/json.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/ostream.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/http/http_ctype.hpp"
#include "acl_cpp/http/http_pipe.hpp"
#include "acl_cpp/http/http_client.hpp"
#include "acl_cpp/http/http_response.hpp"

namespace acl
{

http_response::http_response(socket_stream* client)
{
	debug_ = false;
	header_ok_ = false;
	head_sent_ = false;
	client_ = NEW http_client(client);
}

http_response::~http_response(void)
{
	close();
}

void http_response::close()
{
	if (client_)
	{
		delete client_;
		client_ = NULL;
	}
	head_sent_ = false;
}

http_header& http_response::response_header()
{
	return header_;
}

http_client* http_response::get_client() const
{
	return client_;
}

bool http_response::read_header()
{
	if (client_)
	{
		client_->reset();
		header_.reset();
	}
	else
	{
		logger_error("client_ not opened");
		header_ok_ = false;
		return false;
	}

	// ��ȡ�ͻ��˵�����ͷ�����з���

	if (client_->read_head() == false)
	{
		close();
		header_ok_ = false;
		return false;
	}
	header_ok_ = true;
	return true;
}

http_pipe* http_response::get_pipe(const char* to_charset)
{
	if (to_charset == NULL || *to_charset == 0)
		return NULL;

	// ��Ҫ�����Ӧͷ�ַ�����Ϣ
	const char* ptr = client_->header_value("Content-Type");
	if (ptr == NULL || *ptr == 0)
		return NULL;

	http_ctype ctype;
	ctype.parse(ptr);

	const char* from_charset = ctype.get_charset();

	if (from_charset && strcasecmp(from_charset, to_charset) != 0)
	{
		http_pipe* hp = NEW http_pipe();
		hp->set_charset(from_charset, to_charset);
		return hp;
	}
	else
		return NULL;
}

bool http_response::get_body(xml& out, const char* to_charset /* = NULL */)
{
	if (header_ok_ == false)
	{
		logger_error("header not read yet");
		return false;
	} else if (client_->body_length() == 0)
		return true;
	else if (client_->body_length() < 0)
	{
		const char* method = client_->request_method();
		if (method && (strcmp(method, "GET") == 0
			|| strcmp(method, "CONNECT") == 0))
		{
			return true;
		}

		logger_error("client request body length(%d) invalid",
			(int) client_->body_length());
		return false;
	}

	if (debug_)
		client_->print_header("----request---");

	http_pipe* hp = get_pipe(to_charset);
	if (hp)
		hp->append(&out);

	string buf;
	int   ret;

	while (true)
	{
		// ѭ����ȡ�ͻ�������������
		ret = client_->read_body(buf);
		if (ret == 0)
			break;
		if (ret < 0)
		{
			logger_error("read client body error");
			close();
			return false;
		}

		// ��ʽ���� xml ��ʽ��������
		if (hp)
			hp->update(buf.c_str(), ret);
		else
			out.update(buf.c_str());
		if (debug_)
			printf("%s", buf.c_str());
	}

	if (hp)
	{
		hp->update_end();
		delete hp;
	}
	return true;
}

bool http_response::get_body(json& out, const char* to_charset /* = NULL */)
{
	if (header_ok_ == false)
	{
		logger_error("header not read yet");
		return false;
	} else if (client_->body_length() == 0)
		return true;
	else if (client_->body_length() < 0)
	{
		const char* method = client_->request_method();
		if (method && (strcmp(method, "GET") == 0
			|| strcmp(method, "CONNECT") == 0))
		{
			return true;
		}

		logger_error("client request body length(%d) invalid",
			(int) client_->body_length());
		return false;
	}

	if (debug_)
		client_->print_header("----request---");

	http_pipe* hp = get_pipe(to_charset);
	if (hp)
		hp->append(&out);

	string buf;
	int   ret;

	while (true)
	{
		// ѭ����ȡ�ͻ�������������
		ret = client_->read_body(buf);
		if (ret == 0)
			break;
		if (ret < 0)
		{
			logger_error("read client body error");
			close();
			return false;
		}

		// ��ʽ���� json ��ʽ��������
		if (hp)
			hp->update(buf.c_str(), ret);
		else
			out.update(buf.c_str());
		if (debug_)
			printf("%s", buf.c_str());
	}

	if (hp)
	{
		hp->update_end();
		delete hp;
	}
	return true;
}

bool http_response::get_body(string& out, const char* to_charset /* = NULL */)
{
	if (header_ok_ == false)
	{
		logger_error("header not read yet");
		return false;
	}
	else if (client_->body_length() == 0)
		return true;
	else if (client_->body_length() < 0)
	{
		const char* method = client_->request_method();
		if (method && (strcmp(method, "GET") == 0
			|| strcmp(method, "CONNECT") == 0))
		{
			return true;
		}

		logger_error("client request body length(%d) invalid",
			(int) client_->body_length());
		return false;
	}

	http_pipe* hp = get_pipe(to_charset);
	if (hp)
	{
		pipe_string ps(out);
		hp->append(&ps);
	}

	string  buf;
	int   ret;
	// �� HTTP ������
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

int http_response::read_body(char* buf, size_t size)
{
	if (header_ok_ == false)
	{
		logger_error("header not read yet");
		return -1;
	}
	return client_->read_body(buf, size);
}

bool http_response::response(const void* data, size_t len)
{
	if (client_ == NULL)
	{
		logger_error("conn not opened");
		return false;
	}

	// ��һ�ε��ñ�����ʱӦ�ȷ��� HTTP ��Ӧͷ
	if (!head_sent_)
	{
		if (client_->write_head(header_) == false)
		{
			close();
			return false;
		}
		head_sent_ = true;
	}

	// ���� HTTP ��Ӧ������
	if (client_->write_body(data, len) == false)
	{
		close();
		return false;
	}

	return true;
}

} // namespace acl
