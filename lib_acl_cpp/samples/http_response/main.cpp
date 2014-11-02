// http_servlet.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <assert.h>
#include "acl_cpp/lib_acl.hpp"

static void (*format)(const char*, ...) = acl::log::msg1;

using namespace acl;

//////////////////////////////////////////////////////////////////////////

class master_service : public master_proc
{
public:
	master_service(const char* to_charset = "gb2312")
	{
		local_charset_ = to_charset;
	}

	~master_service() {}

	void set_charset(const char* to_charset)
	{
		local_charset_ = to_charset;
	}

protected:
	// �����յ��ͻ�����������ص�����ӿ�
	virtual void on_accept(socket_stream* stream)
	{
		http_response res(stream);

		// ���ͻ��˵� HTTP ����ͷ
		if (res.read_header() == false)
		{
			const char* ptr = "read request header error";
			res.response_header().set_status(400).set_keep_alive(false);
			res.response(ptr, strlen(ptr));
			return;
		}

		// �� HTTP ����ͷ��ȡ�� Content-Type �ֶ�ֵ
		// Content-Type: text/xml; charset=utf-8
		http_client* client = res.get_client();
		assert(client);
		const char* p = client->header_value("Content-Type");
		if (p == NULL || *p == 0)
		{
			const char* ptr = "no Content-Type";
			res.response_header().set_status(400).set_keep_alive(false);
			res.response(ptr, strlen(ptr));
			return;
		}

		// �� HTTP ����ͷ��ȡ�����������е�������
		// �������ͷ���У�Content-Type: text/xml; charset=utf-8
		// �� get_stype ȡ�� xml ������
		http_ctype content_type;
		content_type.parse(p);
		const char* stype = content_type.get_stype();
		const char* req_charset = content_type.get_charset();

		bool ret;
		string err;

		// ���������͵Ĳ�ͬ���ò�ͬ�Ĵ������

		if (stype == NULL)
			ret = do_plain(res, req_charset, err);

		// text/xml ��ʽ
		else if (strcasecmp(stype, "xml") == 0)
			ret = do_xml(res, req_charset, err);

		// text/json ��ʽ
		else if (strcasecmp(stype, "json") == 0)
			ret = do_json(res, req_charset, err);
		else
			ret = do_plain(res, req_charset, err);
		if (ret == false)
			res.response(err.c_str(), err.length());
	}

private:
	// ���� text/plain ������
	bool do_plain(http_response& res, const char* req_charset, string& err)
	{
		string body;
		if (res.get_body(body, local_charset_) == false)
		{
			err += "get_body error";
			return false;
		}
		printf("body:\r\n(%s)\r\n", body.c_str());

		// ������Ӧͷ�ֶ�
		http_header& header = res.response_header();
		string ctype("text/plain");
		if (req_charset)
		{
			ctype << "; charset=" << req_charset;
			header.set_content_type(ctype);
		}
		else
			header.set_content_type(ctype);

		printf(">>ctype: %s\r\n", ctype.c_str());

		// ������Ӧ������
		return response_body(res, body, req_charset);
	}

	// ���� text/xml ������
	bool do_xml(http_response& res, const char* req_charset, string& err)
	{
		xml body;
		if (res.get_body(body, local_charset_) == false)
		{
			err += "get_body error";
			return false;
		}
		xml_node* node = body.first_node();
		while (node)
		{
			const char* tag = node->tag_name();
			const char* name = node->attr_value("name");
			const char* pass = node->attr_value("pass");
			printf(">>tag: %s, name: %s, pass: %s\r\n",
				tag ? tag : "null",
				name ? name : "null",
				pass ? pass : "null");
			node = body.next_node();
		}

		// ������Ӧͷ�ֶ�
		http_header& header = res.response_header();
		string ctype("text/xml");
		if (req_charset)
		{
			ctype << "; charset=" << req_charset;
			header.set_content_type(ctype);
		}
		else
			header.set_content_type(ctype);

		printf(">>ctype: %s\r\n", ctype.c_str());

		// ������Ӧ�����ݣ���������Ӧ������
		string buf;
		body.build_xml(buf);

		// ������Ӧ������
		return response_body(res, buf, req_charset);
	}

	// ���� text/json ����
	bool do_json(http_response& res, const char* req_charset, string& err)
	{
		json body;
		if (res.get_body(body, local_charset_) == false)
		{
			err += "get_body error";
			return false;
		}

		json_node* node = body.first_node();
		while (node)
		{
			if (node->tag_name())
			{
				printf("tag: %s", node->tag_name());
				if (node->get_text())
					printf(", value: %s\r\n", node->get_text());
				else
					printf("\r\n");
			}
			node = body.next_node();
		}

		// ������Ӧͷ�ֶ�
		http_header& header = res.response_header();
		string ctype("text/json");
		if (req_charset)
		{
			ctype << "; charset=" << req_charset;
			header.set_content_type(ctype);
		}
		else
			header.set_content_type(ctype);

		printf(">>ctype: %s\r\n", ctype.c_str());

		// ������Ӧ�����ݣ���������Ӧ������
		string buf;
		body.build_json(buf);

		// ������Ӧ������
		return response_body(res, buf, req_charset);
	}

	bool response_body(http_response& res, const string& body,
		const char* req_charset)
	{
		// ��������ַ����������ַ�����ͬ����ֱ�ӷ���
		if (local_charset_ == req_charset)
			return res.response(body.c_str(), body.length());

		// ��һ��ʱ������Ҫ����ת��
		string buf;
		charset_conv conv;
		if (conv.convert(local_charset_, req_charset,
			body.c_str(), body.length(), &buf) == false)
		{
			logger_error("charset convert from %s to %s error",
				local_charset_.c_str(), req_charset);
			return false;
		}

		printf(">>response\r\n(%s)\r\n", buf.c_str());
		return res.response(buf.c_str(), buf.length());
	}

private:
	string local_charset_;
};

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
#ifdef	WIN32
	acl::acl_cpp_init();
#endif
	master_service service;  // ��������������

	// ��ʼ����

	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
	{
		if (argc >= 3)
			service.set_charset(argv[2]);

		format = (void (*)(const char*, ...)) printf;
		printf("listen: 0.0.0.0:8888 ...\r\n");
		service.run_alone("0.0.0.0:8888", NULL, 1);  // �������з�ʽ
	}
	else
		service.run_daemon(argc, argv);  // acl_master ����ģʽ����

	return 0;
}
