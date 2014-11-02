#include "StdAfx.h"
#include "global/global.h"
#include "global/util.h"
#include "rpc/rpc_manager.h"
#include "mime_builder.hpp"
#include "smtp_store.h"
#include "smtp_client.h"

smtp_client::smtp_client()
{
	memset(&meter_, 0, sizeof(meter_));
}

smtp_client::~smtp_client()
{

}

smtp_client& smtp_client::set_callback(smtp_callback* c)
{
	callback_ = c;
	return *this;
}

smtp_client& smtp_client::set_conn_timeout(int n)
{
	connect_timeout_ = n;
	return *this;
}

smtp_client& smtp_client::set_rw_timeout(int n)
{
	rw_timeout_ = n;
	return *this;
}

smtp_client& smtp_client::set_account(const char* s)
{
	auth_account_ = s;
	return *this;
}

smtp_client& smtp_client::set_passwd(const char* s)
{
	auth_passwd_ = s;
	return *this;
}

smtp_client& smtp_client::set_smtp(const char* addr, int port)
{
	smtp_addr_ = addr;
	smtp_port_ = port;
	smtp_ip_ = addr;
	return *this;
}

smtp_client& smtp_client::set_from(const char* s)
{
	mail_from_ = s;
	return *this;
}

smtp_client& smtp_client::add_to(const char* s)
{
	ACL_ARGV* tokens = acl_argv_split(s, ";,���� \t\r\n");
	ACL_ITER iter;

	acl_foreach(iter, tokens)
	{
		const char* to = (const char*) iter.data;
		recipients_.push_back(to);
	}

	acl_argv_free(tokens);

	return *this;
}

smtp_client& smtp_client::set_subject(const char* s)
{
	acl::string buf;
	//acl::charset_conv conv;
	//if (conv.convert("gbk", "utf-8", s, strlen(s), &buf) == false)
	//	logger_error("convert from gbk to utf-8 failed");
	//else
	//{
	//	acl::string buf2;
	//	conv.convert("utf-8", "gbk", buf.c_str(),
	//		(int) buf.length(), &buf2);
	//	printf(">>>buf: %s, buf2: %s\r\n", buf.c_str(), buf2.c_str());
	//}
	buf = s;
	acl::rfc2047::encode(s, (int) buf.length(), &subject_, "gbk", 'B', false);
	return *this;
}

smtp_client& smtp_client::add_file(const char* p)
{
	files_.push_back(p);
	return *this;
}

//////////////////////////////////////////////////////////////////////////

struct UP_CTX
{
	acl::string msg;
	size_t total;
	size_t curr;
};

//////////////////////////////////////////////////////////////////////////
// ���߳�������

void smtp_client::rpc_onover()
{
	smtp_store* smtp = new smtp_store(auth_account_.c_str(), smtp_ip_.c_str(),
		meter_, *callback_);
	rpc_manager::get_instance().fork(smtp);
	delete this;
}

void smtp_client::rpc_wakeup(void* ctx)
{
	UP_CTX* up = (UP_CTX*) ctx;

	callback_->smtp_report(up->msg.c_str(),
		up->total, up->curr, meter_);
	delete up;
}

//////////////////////////////////////////////////////////////////////////
// ���߳�������

void smtp_client::rpc_run()
{
	UP_CTX* up;

	// �����ʼ�����
	acl::ifstream in;
	if (create_mail(in) == false)
		return;


	struct timeval begin, last, now;
	gettimeofday(&begin, NULL);

	//////////////////////////////////////////////////////////////////////////
	// ������������

	gettimeofday(&last, NULL);
	if (get_ip() == false)
	{
		up = new UP_CTX;
		up->curr = 0;
		up->total = (size_t) in.fsize();
		up->msg.format("���� smtp ������%s ʧ�ܣ�", smtp_addr_.c_str());
		rpc_signal(up);

		return;
	}
	gettimeofday(&now, NULL);
	meter_.smtp_nslookup_elapsed = util::stamp_sub(&now, &last);
	acl::string smtp_addr;
	smtp_addr.format("%s:%d", smtp_ip_.c_str(), smtp_port_);

	//////////////////////////////////////////////////////////////////////////
	// Զ������ SMTP ������

	up = new UP_CTX;
	up->curr = 0;
	up->total = (size_t) in.fsize();
	up->msg.format("���� SMTP ������ ...");
	rpc_signal(up);

	SMTP_CLIENT* conn = smtp_open(smtp_addr.c_str(), connect_timeout_,
		rw_timeout_, 1024);
	if (conn == NULL)
	{
		logger_error("connect smtp server(%s) error", smtp_addr);
		up = new UP_CTX;
		up->curr = 0;
		up->total = (size_t) in.fsize();
		up->msg.format("���� smtp ��������%s ʧ�ܣ�", smtp_addr.c_str());
		rpc_signal(up);
		return;
	}

	gettimeofday(&now, NULL);
	meter_.smtp_connect_elapsed = util::stamp_sub(&now, &last);

	//////////////////////////////////////////////////////////////////////////
	struct timeval envelop_begin;
	gettimeofday(&envelop_begin, NULL);

	up = new UP_CTX;
	up->curr = 0;
	up->total = (size_t) in.fsize();
	up->msg.format("���� SMTP ��������ӭ��Ϣ(���Ӻ�ʱ %.2f ����) ...",
		meter_.smtp_connect_elapsed);
	rpc_signal(up);

	gettimeofday(&last, NULL);
	if (smtp_get_banner(conn) != 0)
	{
		logger_error("get smtpd banner error");
		smtp_close(conn);
		return;
	}
	else if (smtp_greet(conn, "localhost", 1) != 0)
	{
		logger_error("send EHLO error(%d:%s) to server %s",
			conn->smtp_code, conn->buf, smtp_addr.c_str());
		smtp_close(conn);
		return;
	}
	gettimeofday(&now, NULL);
	meter_.smtp_banner_elapsed = util::stamp_sub(&now, &last);
	//////////////////////////////////////////////////////////////////////////
	// ��֤�û����

	up = new UP_CTX;
	up->curr = 0;
	up->total = (size_t) in.fsize();
	up->msg.format("��֤�û���� ...");
	rpc_signal(up);

	gettimeofday(&last, NULL);

	if (smtp_auth(conn, auth_account_.c_str(),
		auth_passwd_.c_str()) != 0)
	{
		logger_error("smtp auth error(%d:%s) from server(%s), "
			"account: %s, passwd: %s", conn->smtp_code, conn->buf,
			smtp_addr.c_str(), auth_account_.c_str(),
			auth_passwd_.c_str());
		smtp_close(conn);
		return;
	}

	gettimeofday(&now, NULL);
	meter_.smtp_auth_elapsed = util::stamp_sub(&now, &last);

	up = new UP_CTX;
	up->curr = 0;
	up->total = (size_t) in.fsize();
	up->msg.format("�����ʼ��ŷ�(��֤��ʱ %.2f ����) ...",
		meter_.smtp_auth_elapsed);
	rpc_signal(up);

	//////////////////////////////////////////////////////////////////////////

	gettimeofday(&last, NULL);
	if (smtp_mail(conn, mail_from_.c_str()) != 0)
	{
		logger_error("smtp MAIL FROM error(%d:%s), from: %s, server: %s",
			mail_from_.c_str(), conn->smtp_code,
			conn->buf, smtp_addr.c_str());
		smtp_close(conn);
		return;
	}
	gettimeofday(&now, NULL);
	meter_.smtp_mail_elapsed = util::stamp_sub(&now, &last);

	//////////////////////////////////////////////////////////////////////////

	gettimeofday(&last, NULL);
	std::list<acl::string>::const_iterator cit2 = recipients_.begin();
	for (; cit2 != recipients_.end(); ++cit2)
	{
		if (smtp_rcpt(conn, (*cit2).c_str()) != 0)
		{
			logger_error("smtp RCPT TO error(%d:%s), to: %s, server: %s",
				conn->smtp_code, conn->buf, (*cit2).c_str(),
				smtp_addr.c_str());
			smtp_close(conn);
			return;
		}
		else
			logger("smtp RCPT TO ok, to: %s, server: %s",
				(*cit2).c_str(), smtp_addr.c_str());
	}
	gettimeofday(&now, NULL);
	meter_.smtp_rcpt_elapsed = util::stamp_sub(&now, &last);

	//////////////////////////////////////////////////////////////////////////

	gettimeofday(&last, NULL);

	if (smtp_data(conn) != 0)
	{
		logger_error("smtp DATA to server %s error(%d:%s)",
			smtp_addr.c_str(), conn->smtp_code, conn->buf);
		smtp_close(conn);
		return;
	}
	gettimeofday(&now, NULL);
	meter_.smtp_data_elapsed = util::stamp_sub(&now, &last);

	meter_.smtp_envelope_eplased = util::stamp_sub(&now, &envelop_begin);

	// �����ʼ�����

	char buf[8192];
	int  ret;
	int  n = 0;

	while (!in.eof())
	{
		ret = in.read(buf, sizeof(buf), false);
		if (ret == -1)
			break;
		if (acl_vstream_writen(conn->conn, buf, ret) == ACL_VSTREAM_EOF)
		{
			logger_error("smtp send data to server %s error(%d:%s)",
				smtp_addr.c_str(), conn->smtp_code, conn->buf);
			smtp_close(conn);
			return;
		}
		n += ret;
		up = new UP_CTX;
		up->curr = n;
		up->total = (size_t) in.fsize();
		up->msg.format("�����ʼ���(%d/%d �ֽ�) ...", up->curr, up->total);
		rpc_signal(up);
	}

	/* ���� \r\n.\r\n ��ʾ�ʼ����ݷ������ */
	if (smtp_data_end(conn) != 0)
	{
		logger_error("send . error: %s, code: %d\r\n",
			conn->buf, conn->smtp_code);
		smtp_close(conn);
	}

	gettimeofday(&now, NULL);
	meter_.smtp_body_elapsed = util::stamp_sub(&now, &last);
	meter_.smtp_total_elapsed = util::stamp_sub(&now, &begin);
	meter_.smtp_speed = (int) up->total / (int) meter_.smtp_body_elapsed;

	up = new UP_CTX;
	up->curr = (size_t) in.fsize();
	up->total = (size_t) in.fsize();
	up->msg.format("�����ʼ��ɹ���(%d/%d �ֽ�, ��ʱ %.2f ����)",
		up->curr, up->total, meter_.smtp_total_elapsed);
	rpc_signal(up);
}

bool smtp_client::create_mail(acl::ifstream& in)
{
	mime_builder builer;
	builer.primary_header()
		.set_from(mail_from_.c_str())
		.set_sender(mail_from_.c_str());

	std::list<acl::string>::const_iterator cit1 = recipients_.begin();
	for (; cit1 != recipients_.end(); ++cit1)
		builer.primary_header().add_to((*cit1).c_str());
	builer.primary_header().set_subject(subject_.c_str());

	acl::string body_text("test");
	builer.set_body_text(body_text.c_str(), body_text.length());
	std::vector<acl::string>::const_iterator cit = files_.begin();
	for (; cit != files_.end(); ++cit)
		builer.add_file((*cit).c_str());

	mailpath_.format("%s/%ld.eml", global::get_instance().get_path(),
		time(NULL));
	if (builer.save_as(mailpath_.c_str()) == false)
	{
		logger_error("build email(%s) error(%s)",
			mailpath_.c_str(), acl::last_serror());
		mailpath_.clear();
		return false;
	}

	if (in.open_read(mailpath_.c_str()) == false)
	{
		logger_error("open %s error %s", mailpath_.c_str(),
			acl::last_serror());
		mailpath_.clear();
		return false;
	}
	return true;
}

bool smtp_client::get_ip()
{
	ACL_DNS_DB* dns_db = acl_gethostbyname(smtp_addr_.c_str(), NULL);
	if (dns_db == NULL)
	{
		logger_error("gethostbyname(%s) failed", smtp_addr_.c_str());
		return false;
	}
	const char* first_ip = acl_netdb_index_ip(dns_db, 0);
	if (first_ip == NULL || *first_ip == 0)
	{
		logger_error("no ip for domain: %s", smtp_addr_.c_str());
		acl_netdb_free(dns_db);
		return false;
	}
	smtp_ip_ = first_ip;
	acl_netdb_free(dns_db);
	return true;
}
