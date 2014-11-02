#include "StdAfx.h"
#include "rpc/rpc_manager.h"
#include "global/util.h"
#include "pop3_store.h"
#include "pop3_client.h"

pop3_client::pop3_client()
{
	memset(&meter_, 0, sizeof(meter_));
	meter_.pop3_nslookup_elapsed = 0.00;
	meter_.pop3_connect_elapsed = 0.00;
	meter_.pop3_banner_elapsed = 0.00;
	meter_.pop3_auth_elapsed = 0.00;
	meter_.pop3_uidl_elapsed = 0.00;
	meter_.pop3_list_elapsed = 0.00;
	meter_.pop3_total_elapsed = 0.00;
	meter_.pop3_recv_elapsed = 0.00;
	meter_.pop3_quit_elapsed = 0.00;
	recv_limit_ = 0;
	resv_save_ = false;
}

pop3_client::~pop3_client()
{

}

pop3_client& pop3_client::set_callback(pop3_callback* c)
{
	callback_ = c;
	return *this;
}

pop3_client& pop3_client::set_conn_timeout(int n)
{
	connect_timeout_ = n;
	return *this;
}

pop3_client& pop3_client::set_rw_timeout(int n)
{
	rw_timeout_ = n;
	return *this;
}

pop3_client& pop3_client::set_account(const char* s)
{
	auth_account_ = s;
	return *this;
}

pop3_client& pop3_client::set_passwd(const char* s)
{
	auth_passwd_ = s;
	return *this;
}

pop3_client& pop3_client::set_pop3(const char* addr, int port)
{
	pop3_addr_ = addr;
	pop3_port_ = port;
	pop3_ip_ = addr;
	return *this;
}

pop3_client& pop3_client::set_recv_count(int n)
{
	recv_limit_ = n;
	return *this;
}

pop3_client& pop3_client::set_recv_save(bool on)
{
	resv_save_ = on;
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

void pop3_client::rpc_onover()
{
	pop3_store* pop3 = new pop3_store(auth_account_.c_str(),
		pop3_ip_.c_str(), meter_, *callback_);
	rpc_manager::get_instance().fork(pop3);
	delete this;
}

void pop3_client::rpc_wakeup(void* ctx)
{
	UP_CTX* up = (UP_CTX*) ctx;

	callback_->pop3_report(up->msg.c_str(),
		up->total, up->curr, meter_);
	delete up;
}

//////////////////////////////////////////////////////////////////////////
// ���߳�������

void pop3_client::rpc_run()
{
	UP_CTX* up;
	struct timeval begin, last, now;
	gettimeofday(&begin, NULL);

	//////////////////////////////////////////////////////////////////
	// ������������

	gettimeofday(&last, NULL);
	if (get_ip() == false)
	{
		up = new UP_CTX;
		up->curr = 0;
		up->total = 0;
		up->msg.format("���� pop3 ������%s ʧ�ܣ�",
			pop3_addr_.c_str());
		rpc_signal(up);
		return;
	}
	gettimeofday(&now, NULL);
	meter_.pop3_nslookup_elapsed = util::stamp_sub(&now, &last);
	acl::string pop3_addr;
	pop3_addr.format("%s:%d", pop3_ip_.c_str(), pop3_port_);

	//////////////////////////////////////////////////////////////////
	// Զ������ SMTP ������

	up = new UP_CTX;
	up->curr = 0;
	up->total = 0;
	up->msg.format("���� POP3 ������ ...");
	rpc_signal(up);

	acl::socket_stream conn;
	if (conn.open(pop3_addr.c_str(), connect_timeout_,
		rw_timeout_) == false)
	{
		logger_error("connect pop3 server(%s) error", pop3_addr);
		up = new UP_CTX;
		up->curr = 0;
		up->total = 0;
		up->msg.format("���� pop3 ��������%s ʧ�ܣ�",
			pop3_addr.c_str());
		rpc_signal(up);
		return;
	}

	gettimeofday(&now, NULL);
	meter_.pop3_connect_elapsed = util::stamp_sub(&now, &last);

	//////////////////////////////////////////////////////////////////
	// ��� POP3 �������Ļ�ӭ��Ϣ

	up = new UP_CTX;
	up->curr = 0;
	up->total = 0;
	up->msg.format("���� POP3 ��������ӭ��Ϣ(���Ӻ�ʱ %.2f ����) ...",
		meter_.pop3_connect_elapsed);
	rpc_signal(up);

	gettimeofday(&last, NULL);
	if (pop3_get_banner(conn) == false)
		return;
	gettimeofday(&now, NULL);
	meter_.pop3_banner_elapsed = util::stamp_sub(&now, &last);

	up = new UP_CTX;
	up->curr = 0;
	up->total = 0;
	up->msg.format("��� banner ��ʱ %.2f ���룬��ʼ��֤�˺���Ϣ ...",
		meter_.pop3_banner_elapsed);
	rpc_signal(up);

	//////////////////////////////////////////////////////////////////
	// ��֤�û������

	gettimeofday(&last, NULL);
	if (pop3_auth(conn, auth_account_.c_str(),
		auth_passwd_.c_str()) == false)
	{
		return;
	}
	gettimeofday(&now, NULL);
	meter_.pop3_auth_elapsed = util::stamp_sub(&now, &last);
	up = new UP_CTX;
	up->curr = 0;
	up->total = 0;
	up->msg.format("�û���֤�ɹ�(��ʱ %.2f ����)",
		meter_.pop3_auth_elapsed);
	rpc_signal(up);

	//////////////////////////////////////////////////////////////////
	// uidl �û��ռ�����ʼ��б�

	up = new UP_CTX;
	up->curr = 0;
	up->total = 0;
	up->msg.format("���û��ռ����ʼ�(UIDL) ...");
	rpc_signal(up);

	std::vector<acl::string> uidl_list;
	gettimeofday(&last, NULL);
	if (pop3_uidl(conn, uidl_list) == false)
		return;
	gettimeofday(&now, NULL);
	meter_.pop3_uidl_elapsed = util::stamp_sub(&now, &last);

	up = new UP_CTX;
	up->curr = 0;
	up->total = 0;
	up->msg.format("�û��ռ����ʼ��б����(��ʱ %.2f ����)",
		meter_.pop3_uidl_elapsed);
	rpc_signal(up);

	//////////////////////////////////////////////////////////////////
	// LIST �û��ռ����ʼ��б�

	up = new UP_CTX;
	up->curr = 0;
	up->total = 0;
	up->msg.format("���û��ռ����ʼ��ߴ�(LIST) ...");
	rpc_signal(up);

	std::vector<size_t> size_list_;
	gettimeofday(&last, NULL);
	if (pop3_list(conn, size_list_) == false)
	{
		logger_error("pop3_list failed for %s", auth_account_.c_str());
		return;
	}
	gettimeofday(&now, NULL);
	meter_.pop3_list_elapsed = util::stamp_sub(&now, &last);

	up = new UP_CTX;
	up->curr = 0;
	up->total = 0;
	up->msg.format("���û��ռ����ʼ��ߴ�(LIST) ��ʱ %.2f",
		meter_.pop3_list_elapsed);
	rpc_signal(up);

	//////////////////////////////////////////////////////////////////
	// ��ȡ�û��ռ�����ʼ�

	up = new UP_CTX;
	up->curr = 0;
	up->total = size_list_.size();
	up->msg.format("��ʼ�����ռ����ʼ� ...");
	rpc_signal(up);

	gettimeofday(&last, NULL);
	if (pop3_retr(conn, size_list_) == false)
	{
		logger_error("pop3_retr failed for %s", auth_account_.c_str());
		return;
	}
	gettimeofday(&now, NULL);
	meter_.pop3_recv_elapsed = util::stamp_sub(&now, &last);

	up = new UP_CTX;
	up->curr = 0;
	up->total = size_list_.size();
	up->msg.format("�����ռ����ʼ���ɣ���ʱ %0.2f",
		meter_.pop3_recv_elapsed);
	rpc_signal(up);

	//////////////////////////////////////////////////////////////////
	// �˳�����

	gettimeofday(&last, NULL);
	pop3_quit(conn);
	gettimeofday(&now, NULL);
	meter_.pop3_quit_elapsed = util::stamp_sub(&now, &last);

	//////////////////////////////////////////////////////////////////
	// ͳ���ܹ��ķѵ�ʱ��

	gettimeofday(&now, NULL);
	meter_.pop3_total_elapsed = util::stamp_sub(&now, &begin);

	up = new UP_CTX;
	up->curr = 0;
	up->total = size_list_.size();
	up->msg.format("�ռ����̹���ʱ %0.2f",
		meter_.pop3_recv_elapsed);
	rpc_signal(up);
}

bool pop3_client::get_ip()
{
	ACL_DNS_DB* dns_db = acl_gethostbyname(pop3_addr_.c_str(), NULL);
	if (dns_db == NULL)
	{
		logger_error("gethostbyname(%s) failed", pop3_addr_.c_str());
		return false;
	}
	const char* first_ip = acl_netdb_index_ip(dns_db, 0);
	if (first_ip == NULL || *first_ip == 0)
	{
		logger_error("no ip for domain: %s", pop3_addr_.c_str());
		acl_netdb_free(dns_db);
		return false;
	}
	pop3_ip_ = first_ip;
	acl_netdb_free(dns_db);
	return true;
}

bool pop3_client::pop3_get_banner(acl::socket_stream& conn)
{
	acl::string line;
	if (conn.gets(line) == false)
	{
		logger_error("get pop3 banner from %s error",
			pop3_ip_.c_str());
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("pop3 server(%s) banner error: %s for user: %s",
			pop3_ip_.c_str(), line.c_str(), auth_account_.c_str());
		return false;
	}
	logger("banner: %s", line.c_str());
	return true;
}

bool pop3_client::pop3_auth(acl::socket_stream& conn,
	const char* user, const char* pass)
{
	if (conn.format("USER %s\r\n", auth_account_.c_str()) == -1)
	{
		logger_error("send USER %s to pop3 server(%s) error(%s)",
			auth_account_.c_str(), pop3_ip_.c_str(),
			acl::last_serror());
		return false;
	}

	acl::string line;

	if (conn.gets(line) == false)
	{
		logger_error("get USER(%s)'s reply error(%s) from server %s",
			auth_account_.c_str(), acl::last_serror(),
			pop3_ip_.c_str());
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("pop3 server(%s) reply error: %s for cmd USER %s",
			pop3_ip_.c_str(), line.c_str(), auth_account_.c_str());
		return false;
	}

	logger("USER's reply: %s", line.c_str());

	if (conn.format("PASS %s\r\n", auth_passwd_.c_str()) == -1)
	{
		logger_error("send PASS %s to pop3 server(%s) error(%s)",
			auth_passwd_.c_str(), pop3_ip_.c_str(),
			acl::last_serror());
		return false;
	}
	if (conn.gets(line) == false)
	{
		logger_error("get PASS(%s) reply error(%s) from server %s",
			auth_passwd_.c_str(), acl::last_serror(),
			pop3_ip_.c_str());
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("pop3 server(%s) reply error: %s for cmd PASS %s",
			pop3_ip_.c_str(), line.c_str(), auth_passwd_.c_str());
		return false;
	}
	logger("PASS's reply: %s", line.c_str());
	return true;
}

bool pop3_client::pop3_uidl(acl::socket_stream& conn,
	std::vector<acl::string>& out)
{
	if (conn.puts("UIDL") == -1)
	{
		logger_error("send UIDL to pop3 server(%s) error %s, "
			"user: %s", pop3_ip_.c_str(), acl::last_serror(),
			auth_account_.c_str());
		return false;
	}

	acl::string line;
	if (conn.gets(line) == false)
	{
		logger_error("gets UIDL's reply from server(%s) error(%s), "
			"user: %s", pop3_ip_.c_str(), acl::last_serror(),
			auth_account_.c_str());
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("UIDL's reply(%s) error from server %s, user: %s",
			line.c_str(), pop3_ip_.c_str(), auth_account_.c_str());
		return false;
	}

	logger("UIDL's first reply: %s", line.c_str());

	while (true)
	{
		if (conn.gets(line) == false)
		{
			logger_error("UIDL's reply error %s, from server %s, "
				"user %s", acl::last_serror(),
				pop3_ip_.c_str(), auth_account_.c_str());
			return false;
		}
		logger("UIDL: %s", line.c_str());

		if (line == ".")
			break;
		char* ptr = line.c_str();
		char* p1 = acl_mystrtok(&ptr, " \t");
		if (ptr == NULL || *ptr == 0)
		{
			logger_error("invalid UIDL's reply(%s) from server %s, "
				"user %s", p1, pop3_ip_.c_str(),
				auth_account_.c_str());
			return false;
		}
		out.push_back(ptr);
	}

	meter_.total_uidl = out.size();
	return true;
}

bool pop3_client::pop3_list(acl::socket_stream& conn, std::vector<size_t>& out)
{
	if (conn.puts("LIST") == -1)
	{
		logger_error("send LIST to pop3 server(%s) error %s, "
			"user: %s", pop3_ip_.c_str(), acl::last_serror(),
			auth_account_.c_str());
		return false;
	}

	acl::string line;
	if (conn.gets(line) == false)
	{
		logger_error("gets LIST's reply from server(%s) error(%s), "
			"user: %s", pop3_ip_.c_str(), acl::last_serror(),
			auth_account_.c_str());
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("LIST's reply(%s) error from server %s, user: %s",
			line.c_str(), pop3_ip_.c_str(), auth_account_.c_str());
		return false;
	}

	logger("LIST's first reply: %s", line.c_str());

	while (true)
	{
		if (conn.gets(line) == false)
		{
			logger_error("LIST's reply error %s, from server %s, "
				"user %s", acl::last_serror(),
				pop3_ip_.c_str(), auth_account_.c_str());
			return false;
		}

		logger("LIST: %s", line.c_str());

		if (line == ".")
			break;
		char* ptr = line.c_str();
		char* p1 = acl_mystrtok(&ptr, " \t");
		if (ptr == NULL || *ptr == 0)
		{
			logger_error("invalid LIST's reply(%s) from server %s, "
				"user %s", p1, pop3_ip_.c_str(),
				auth_account_.c_str());
			return false;
		}
		int len = atoi(ptr);
		if (len <= 0)
		{
			logger_error("invalid LIST size(%d) from server %s, "
				"user %s", len, pop3_ip_.c_str(),
				auth_account_.c_str());
			return false;
		}
		out.push_back((size_t) len);
		meter_.total_size += len;
	}
	meter_.total_list = out.size();
	return true;
}

bool pop3_client::pop3_retr(acl::socket_stream& conn,
	const std::vector<size_t>& size_list)
{
	if (recv_limit_ == 0)
		return true;
	if (recv_limit_ < 0 || recv_limit_ > (int) size_list.size())
		meter_.recved_limit = size_list.size();
	else
		meter_.recved_limit = recv_limit_;

	recv_begin_ = time(NULL);
	time_t now, inter;
	for (size_t i = 1; i <= meter_.recved_limit; i++)
	{
		if (pop3_retr_one(conn, i) == false)
			return false;

		time(&now);
		inter = now - recv_begin_;
		if (inter > 0)
			meter_.recved_speed = meter_.recved_size / (size_t) inter;
		UP_CTX* up = new UP_CTX;
		up->curr = i;
		up->total = meter_.recved_limit;
		up->msg.format("�����ʼ�(%d/%d), �յ� %d �ֽ�, %d �ֽ�/��",
			(int) meter_.recved_count,
			(int) meter_.recved_limit,
			(int) meter_.recved_size,
			(int) meter_.recved_speed);
		rpc_signal(up);
	}

	return true;
}

bool pop3_client::pop3_retr_one(acl::socket_stream& conn, size_t idx)
{
	if (conn.format("RETR %d\r\n", idx) == -1)
	{
		logger_error("send RETR %d to pop3 server(%s) error %s, "
			"user: %s", pop3_ip_.c_str(), idx,
			acl::last_serror(), auth_account_.c_str());
		return false;
	}

	acl::string line;

	if (conn.gets(line) == false)
	{
		logger_error("gets RETR's reply from server(%s) error(%s), "
			"user: %s", pop3_ip_.c_str(), acl::last_serror(),
			auth_account_.c_str());
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("RETR's reply(%s) error from server %s, user: %s",
			line.c_str(), pop3_ip_.c_str(), auth_account_.c_str());
		return false;
	}

	time_t last, now;
	time(&last);

	static int __count = 0;
	__count++;
	acl::string filepath;
	filepath.format("%d.eml", __count);
	acl::ofstream out;
	if (resv_save_ && out.open_write(filepath) == false)
	{
		logger_error("open file %s error %s", 
			filepath.c_str(), acl::last_serror());
		return false;
	}
	while (true)
	{
		line.clear();
		if (conn.gets(line, false) == false)
		{
			logger_error("gets mail data error(%s) from server %s, "
				"user: %s, idx: %d", acl::last_serror(),
				pop3_ip_.c_str(), auth_account_.c_str(), idx);
			return false;
		}
		if (line == ".\n" || line == ".\r\n")
			break;

		if (out.write(line) == -1)
			logger_error("write to %s error %s, %d",
				filepath.c_str(), acl::last_serror());

		meter_.recved_size += line.length();

		time(&now);
		if (now - last >= 1)
		{
			UP_CTX* up = new UP_CTX;
			up->curr = meter_.recved_count;
			up->total = meter_.recved_limit;
			meter_.recved_speed = meter_.recved_size
				/ (int) (now - recv_begin_);
			up->msg.format("�����ʼ�(%d/%d), �յ� %d �ֽ�, "
				"%d �ֽ�/��",
				(int) meter_.recved_count,
				(int) meter_.recved_limit,
				(int) meter_.recved_size,
				(int) meter_.recved_speed);
			rpc_signal(up);
			time(&last);
		}
	}

	meter_.recved_count++;
	return true;
}

bool pop3_client::pop3_quit(acl::socket_stream& conn)
{
	if (conn.puts("QUIT") == -1)
	{
		logger_error("send QUIT to pop3 server(%s) error %s, "
			"user: %s", pop3_ip_.c_str(),
			acl::last_serror(), auth_account_.c_str());
		return false;
	}

	acl::string line;

	if (conn.gets(line) == false)
	{
		logger_error("gets QUIT's reply from server(%s) error(%s), "
			"user: %s", pop3_ip_.c_str(), acl::last_serror(),
			auth_account_.c_str());
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("QUIT's reply(%s) error from server %s, user: %s",
			line.c_str(), pop3_ip_.c_str(), auth_account_.c_str());
		return false;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
