// mime.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "lib_acl.h"
#ifdef WIN32
#include <vld.h>
#else
#include <getopt.h>
#endif
#include <string>
#include <errno.h>
#include <string.h>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/ifstream.hpp"
#include "acl_cpp/stream/ofstream.hpp"
#include "acl_cpp/stdlib/charset_conv.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/mime/mime.hpp"
#include "acl_cpp/mime/mime_body.hpp"
#include "acl_cpp/mime/rfc2047.hpp"
#include "acl_cpp/mime/mime_attach.hpp"

#include "pipeline_string.h"

using namespace std;
using namespace acl;

static void header_out(const acl::mime* mime)
{
	printf("sender addr: %s\n", mime->sender().c_str());
	printf("from addr: %s\n", mime->from().c_str());
	printf("replyto addr: %s\n", mime->replyto().c_str());
	printf("returnpath addr: %s\n", mime->returnpath().c_str());

	const list<char*>& rcpts = mime->rcpt_list();
	list<char*>::const_iterator cit;

	for (cit = rcpts.begin(); cit != rcpts.end(); cit++)
	{
		const char* addr = *cit;
		printf("rcpt addr: %s\n", addr);
	}

	const list<char*>& tos = mime->to_list();
	for (cit = tos.begin(); cit != tos.end(); cit++)
	{
		const char* addr = *cit;
		printf("to addr: %s\n", addr);
	}

	const list<char*>& ccs = mime->cc_list();
	for (cit = ccs.begin(); cit != ccs.end(); cit++)
	{
		const char* addr = *cit;
		printf("cc addr: %s\n", addr);
	}

	const char* to = mime->header_value("To");
	acl::string out;
	if (to)
		rfc2047::decode(to, (int) strlen(to), &out);
	printf(">>>>To: %s, %s\n", to ? to : "null", out.c_str());
}

// �����ʼ�������ʼ�ͷ��Ϣ
static void test_mime_header(acl::mime& mime, const char* path)
{
	// ���½������ʼ�ͷ����

	printf("\r\n");
	ACL_METER_TIME("---------------parse header begin--------------------");
	acl::ifstream fp;
	if (fp.open_read(path) == false) {
		printf("open %s error %s\n", path, strerror(errno));
		return;
	}
	acl::string buf;
	const char* ptr;
	size_t n;

	// ��ʼ�ʼ���������
	mime.update_begin(path); // update_begin �ڲ����Զ����� reset()

	while (1) {
		if (fp.gets(buf, false) == false)
			break;
		if (buf == "\r\n" || buf == "\n")
			break;

		ptr = buf.c_str();

		n = buf.length();

		// ������� true ��ʾͷ���������, Ϊ��ʹ�ú������� true,
		// ���뱣֤���ô˺��������һ������Ϊ "\r\n" ������

		(void) mime.update(ptr, n);
	}

	mime.update_end();

	header_out(&mime);
	ACL_METER_TIME("---------------parse header end  --------------------");
}

// ���ʼ��������ݱ����ڴ�����
// ���ʼ��������ݱ����ڻ�������
// ���ùܵ���ģʽ���ʼ��������ݽ����ַ���ת�����ұ����ڴ�����
// ���ʼ��еĸ��������ڴ�����
// ����������ʼ���������ϲ������ڴ�����
static void mime_test1(acl::mime& mime, const char* path, bool htmlFirst)
{
	mime.reset();

	acl::string buf;
	acl::ofstream fp_out;
	acl::mime_body* pBody = NULL;

	printf("\r\n>>> %s: path: %s\n", __FUNCTION__, path);

	ACL_METER_TIME("---------------parse begin --------------------");

	if (mime.parse(path) == false)
	{
		printf("mime parse %s error\n", path);
		return;
	}

	ACL_METER_TIME("---------------parse end  --------------------");

	pBody = mime.get_body_node(htmlFirst, true, "iso-2022-jp");

	//////////////////////////////////////////////////////////////////////

	// ���ʼ��������ݱ����ڴ�����

	printf("\r\n");

	ACL_METER_TIME("---------------save_body begin--------------------");

	header_out(&mime);
	mime.mime_debug("./var");

	buf = "./var/";
	buf += path;
	mime.save_as(buf.c_str());

	if (pBody)
		pBody->save_body("./var/body_gb.txt");

	ACL_METER_TIME("---------------save_body end  --------------------");

	//////////////////////////////////////////////////////////////////////

	// ���ʼ��������ݱ����ڻ�������

	if (pBody)
	{
		acl::string buf1;
		pBody->save_body(buf1);

		if (fp_out.open_write("./var/body_string.txt"))
		{
			fp_out.write(buf1.c_str(), buf1.length());
			fp_out.close();
		}
	}

	//////////////////////////////////////////////////////////////////////

	// ���ʼ����������� utf-8 ���ڴ�����
	if (pBody)
	{
		printf("\r\n");
		ACL_METER_TIME("---------------save_body begin--------------------");
		acl::charset_conv jp2utf8;
		jp2utf8.update_begin("iso-2022-jp", "utf-8");

		acl::pipe_string pipe_out;
		pipeline_string pipeline;
		acl::pipe_manager manager;

		manager.push_front(&pipe_out);
		manager.push_front(&jp2utf8);
		manager.push_front(&pipeline);

		pBody->save(manager);

		ACL_METER_TIME("---------------save_body end1 --------------------");

		acl::string& sbuf = pipe_out.get_buf();

		fp_out.close();

		if (fp_out.open_write("./var/body_utf8.txt"))
		{
			fp_out.write(sbuf.c_str(), sbuf.length());
			fp_out.close();
		}

		ACL_METER_TIME("---------------save_body end2 --------------------");
	}

	//////////////////////////////////////////////////////////////////////

	// ���ùܵ���ģʽ���ʼ��������ݽ����ַ���ת�����ұ����ڴ�����

	if (pBody)
	{
		printf("\r\n");
		ACL_METER_TIME("---------------save_body begin--------------------");
		acl::charset_conv utf8ToGb, gbToBig5;
		utf8ToGb.update_begin("utf-8", "gbk");
		gbToBig5.update_begin("gbk", "big5");

		acl::pipe_string pipe_out;
		acl::pipe_manager manager;

		manager.push_front(&pipe_out);
		manager.push_front(&gbToBig5);
		manager.push_front(&utf8ToGb);

		pBody->save(manager);

		ACL_METER_TIME("---------------save_body end1 --------------------");

		acl::string& sbuf = pipe_out.get_buf();

		fp_out.close();

		if (fp_out.open_write("./var/body_big5.txt"))
		{
			fp_out.write(sbuf.c_str(), sbuf.length());
			fp_out.close();
		}

		ACL_METER_TIME("---------------save_body end2 --------------------");
	}

	//////////////////////////////////////////////////////////////////////

	// ���ʼ��еĸ��������ڴ�����

	printf("---------------------------------------------------------\r\n");
	printf(">>>> saving attach file now ...\r\n");
	const std::list<acl::mime_attach*>& attaches = mime.get_attachments();
	std::list<acl::mime_attach*>::const_iterator cit = attaches.begin();
	for (; cit != attaches.end(); cit++)
	{
		buf = "./var/";
		buf << (*cit)->get_filename();

		acl::string attach_name;
		acl::rfc2047 rfc2047;
		rfc2047.reset(true);
		rfc2047.decode_update(buf, (int) buf.length());
		rfc2047.decode_finish("utf-8", &attach_name);

		printf(">>> attach file: |%s|, len: %d\n",
			attach_name.c_str(), (int) attach_name.length());

		(*cit)->save(attach_name.c_str());
	}

	printf(">>>> saved attach file ok ...\r\n");
	//////////////////////////////////////////////////////////////////////

	// ����������ʼ���������ϲ������ڴ�����

	mime.save_mail("./var", "test.html");
}

// ��λ���ļ��е��ʼ����ݽ��н����������ʼ�������ת������һ���ڴ滺����
static void mime_test2(acl::mime& mime, const char* path)
{
	// ���½������ʼ�ͷ����

	printf("\r\n");
	ACL_METER_TIME("---------------parse mail begin--------------------");
	acl::ifstream fp;
	if (fp.open_read(path) == false) {
		printf("open %s error %s\n", path, strerror(errno));
		return;
	}
	acl::string buf;
	const char* ptr;
	size_t n;

	// ��ʼ�ʼ���������
	mime.update_begin(path);

	while (1) {
		if (fp.gets(buf, false) == false)
			break;
		ptr = buf.c_str();
		n = buf.length();

		// ������� true ��ʾͷ���������, Ϊ��ʹ�ú������� true,
		// ���뱣֤���ô˺��������һ������Ϊ "\r\n" ������

		//printf(">>>>%s", ptr);
		if (mime.update(ptr, n) == true)
		{
			printf(">>> parse over, last line: %s\n", ptr);
			break;
		}
		buf.clear();
	}

	// ������� update_end
	mime.update_end();

	acl::mime_body* pBody;
	pBody = mime.get_body_node(false);
	if (pBody)
	{
		acl::string buf2;
		pBody->save_body(buf2);
		printf(">>>>>>>body: %s\n", buf2.c_str());
	}

	header_out(&mime);
	ACL_METER_TIME("---------------parse mail end  --------------------");
}

// ��λ���ڴ��е��ʼ����ݽ��н����������ʼ�������ת������һ���ڴ滺����
static void mime_test3(acl::mime& mime, const char* path)
{
	// ���½������ʼ�ͷ����

	printf("\r\n");
	ACL_METER_TIME("---------------parse mail begin--------------------");
	acl::string buf;

	if (acl::ifstream::load(path, &buf) == false)
	{
		printf("load %s error %s\n", path, strerror(errno));
		return;
	}

	// ��ʼ�ʼ���������
	mime.reset();
	if (mime.update(buf.c_str(), buf.length()) != true)
	{
		printf("mime parse error\r\n");
		return;
	}

	// ������� update_end
	mime.update_end();

	acl::mime_body* pBody;
	pBody = mime.get_body_node(false);
	if (pBody)
	{
		acl::string out;
		if (pBody->save_body(out, buf.c_str(), (ssize_t) buf.length()) == false)
			printf(">>>>save_body to buffer error\r\n");
		else
			printf(">>>>>>>body: %s\n", out.c_str());
	}
	else
		printf(">>>> no body\r\n");

	header_out(&mime);
	ACL_METER_TIME("---------------parse mail end  --------------------");
}

// ��ƫ������MIME�������̣������ʼ�������ת������һ���ļ���
static void mime_test4(acl::mime& mime, const char* path)
{
	mime.reset();
	acl::ifstream in;
	if (in.open_read(path) == false)
	{
		printf("open %s error %s\n", path, strerror(errno));
		return;
	}
	acl::string buf;
	size_t off = 0;

	// �ȶ��ļ�ͷ, ���Թ��ļ�ͷ
	while (true)
	{
		if (in.gets(buf, false) == false)
			break;

		off += buf.length();

		if (buf == "\n" || buf == "\r\n")
		{
			buf.clear();
			break;
		}
		buf.clear();
	}

	// ��ʼ�����ʼ�ͷ���ʼ��岿��

	mime.update_begin(path);
	// ��ʼ���ʼ�
	while (true)
	{
		if (in.gets(buf, false) == false)
			break;
		mime.update(buf.c_str(), buf.length());
		buf.clear();
	}
	mime.update_end();

	printf("\n-----------------------------------------------------\n\n");
	acl::mime_body* pBody = mime.get_body_node(false, true, "gb2312", (off_t) off);
	if (pBody)
	{
		acl::string buf2;
		pBody->save_body(buf2);
		printf(">>>>>>>body(%d): %s\n", (int) off, buf2.c_str());
	}
}

//////////////////////////////////////////////////////////////////////////

static void usage(const char* procname)
{

	printf("usage: %s [options]\r\n"
		" -h [help]\r\n"
		" -s [html first, or text first]\r\n"
		" -t test0/test1/test2/test3/test4\r\n"
		" -f mail_file\r\n", procname);
	
	printf("test0: �����ʼ�������ʼ�ͷ��Ϣ\r\n"
		"test1: ���ʼ��������ݱ����ڴ�����\r\n"
		"\t���ʼ��������ݱ����ڻ�������\r\n"
		"\t���ùܵ���ģʽ���ʼ��������ݽ����ַ���ת�����ұ����ڴ�����\r\n"
		"\t���ʼ��еĸ��������ڴ�����\r\n"
		"\t����������ʼ���������ϲ������ڴ�����\r\n");
	printf("test2: ��λ���ļ��е��ʼ����ݽ��н����������ʼ�������ת������һ���ڴ滺����\r\n");
	printf("test3: ��λ���ڴ��е��ʼ����ݽ��н����������ʼ�������ת������һ���ڴ滺����\r\n");
	printf("test4: ��ƫ������MIME�������̣������ʼ�������ת������һ���ļ���(���ӣ�test16.eml)\r\n");
}

int main(int argc, char* argv[])
{
	char  ch;
	bool  htmlFirst = false;
	acl::string path("test.eml");
	acl::string cmd("test1");

	while ((ch = (char) getopt(argc, argv, "hst:f:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return (0);
		case 's':
			htmlFirst = true;
			break;
		case 't':
			cmd = optarg;
			break;
		case 'f':
			path = optarg;
			break;
		default:
			break;
		}
	}

	logger_open("test.log", "mime", "all:1");

	acl::mime mime;

	//////////////////////////////////////////////////////////////////////

	if (cmd == "test0")
		test_mime_header(mime, path.c_str());
	else if (cmd == "test1")
		mime_test1(mime, path.c_str(), htmlFirst);
	else if (cmd == "test2")
		mime_test2(mime, path.c_str());
	else if (cmd == "test3")
		mime_test3(mime, path.c_str());
	else if (cmd == "test4")
		mime_test4(mime, path.c_str());
	else
		printf(">>> unknown cmd: %s\n", cmd.c_str());

	//////////////////////////////////////////////////////////////////////

	printf("enter any key to exit\r\n");
	logger_close();
	getchar();
	return 0;
}
