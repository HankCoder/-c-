// http_servlet.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include <assert.h>
#include <getopt.h>
#include "acl_cpp/lib_acl.hpp"

//////////////////////////////////////////////////////////////////////////

class http_request_test
{
public:
	http_request_test(const char* server_addr, const char* file,
		const char* stype, const char* charset)
	{
		server_addr_= server_addr;
		file_ = file;
		stype_ = stype;
		charset_ = charset;
		to_charset_ = "gb2312";
	}

	~http_request_test() {}

	bool run(void)
	{
		acl::string body;

		// �ӱ����ļ��м�����������
		if (acl::ifstream::load(file_, &body) == false)
		{
			logger_error("load %s error", file_.c_str());
			return false;
		}

		// ����  HTTP ����ͻ���
		acl::http_request req(server_addr_);

		// ��� HTTP ����ͷ�ֶ�

		acl::string ctype("text/");
		ctype << stype_ << "; charset=" << charset_;

		acl::http_header& hdr = req.request_header();  // ����ͷ���������
		hdr.set_url("/");
		hdr.set_content_type(ctype);

		// ���� HTTP ��������
		if (req.request(body.c_str(), body.length()) == false)
		{
			logger_error("send http request to %s error",
				server_addr_.c_str());
			return false;
		}

		// ȡ�� HTTP ��Ӧͷ�������ֶ�
		const char* p = req.header_value("Content-Type");
		if (p == NULL || *p == 0)
		{
			logger_error("no Content-Type");
			return false;
		}

		// ���� HTTP ��Ӧͷ����������
		acl::http_ctype content_type;
		content_type.parse(p);

		// ��Ӧͷ�������͵�������
		const char* stype = content_type.get_stype();

		bool ret;
		if (stype == NULL)
			ret = do_plain(req);
		else if (strcasecmp(stype, "xml") == 0)
			ret = do_xml(req);
		else if (strcasecmp(stype, "json") == 0)
			ret = do_json(req);
		else
			ret = do_plain(req);
		if (ret == true)
			printf("read ok!\r\n");
		return ret;
	}

private:
	// ���� text/plain ��������
	bool do_plain(acl::http_request& req)
	{
		acl::string body;
		if (req.get_body(body, to_charset_) == false)
		{
			logger_error("get http body error");
			return false;
		}
		printf("body:\r\n(%s)\r\n", body.c_str());
		return true;
	}

	// ���� text/xml ��������
	bool do_xml(acl::http_request& req)
	{
		acl::xml body;
		if (req.get_body(body, to_charset_) == false)
		{
			logger_error("get http body error");
			return false;
		}
		acl::xml_node* node = body.first_node();
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
		return true;
	}

	// ���� text/json ��������
	bool do_json(acl::http_request& req)
	{
		acl::json body;
		if (req.get_body(body, to_charset_) == false)
		{
			logger_error("get http body error");
			return false;
		}

		acl::json_node* node = body.first_node();
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
		return true;
	}

private:
	acl::string server_addr_;	// web ��������ַ
	acl::string file_;		// ��������������ļ�
	acl::string stype_;		// �������ݵ�����������
	acl::string charset_;		// �������������ļ����ַ���
	acl::string to_charset_;	// ����������Ӧ����תΪ�����ַ���
};

//////////////////////////////////////////////////////////////////////////

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n", procname);
	printf("options:\r\n");
	printf("\t-f request file\r\n");
	printf("\t-t request stype[xml/json/plain]\r\n");
	printf("\t-c request file's charset[gb2312/utf-8]\r\n");
}

int main(int argc, char* argv[])
{
	int   ch;
	acl::string server_addr("127.0.0.1:8888"), file("./xml.txt");
	acl::string stype("xml"), charset("gb2312");

	while ((ch = getopt(argc, argv, "hs:f:t:c:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 'f':
			file = optarg;
			break;
		case 't':
			stype = optarg;
			break;
		case 'c':
			charset = optarg;
			break;
		default:
			usage(argv[0]);
			return 0;
		}
	}

	// ����־�������Ļ
	acl::log::stdout_open(true);

	// ��ʼ����
	http_request_test request(server_addr, file, stype, charset);
	request.run();

	return 0;
}

