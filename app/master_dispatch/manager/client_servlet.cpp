#include "stdafx.h"
#include "access_list.h"
#include "pull_mode/server_manager.h"
#include "pull_mode/collect_client.h"
#include "pull_mode/message.h"
#include "pull_mode/message_manager.h"
#include "push_mode/status_manager.h"
#include "client_servlet.h"

client_servlet::client_servlet(const char* domain, int port)
: port_(port)
{
	if (domain && *domain)
		domain_ = domain;
}

client_servlet::~client_servlet(void)
{

}

bool client_servlet::reply(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res, const char* fmt, ...)
{
	acl::string buf;

	va_list ap;
	va_start(ap, fmt);
	buf.vformat(fmt, ap);
	va_end(ap);

	res.setStatus(200)
		.setKeepAlive(req.isKeepAlive())
		.setContentLength(buf.length());

	return res.write(buf) && req.isKeepAlive();
}

bool client_servlet::reply_status(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res, int status, const char* fmt, ...)
{
	acl::string buf;

	va_list ap;
	va_start(ap, fmt);
	buf.vformat(fmt, ap);
	va_end(ap);

	res.setStatus(status)
		.setKeepAlive(req.isKeepAlive())
		.setContentLength(buf.length());

	return res.write(buf) && req.isKeepAlive();
}

bool client_servlet::show_page(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res, const char* fmt, ...)
{
	acl::string buf;
	va_list ap;
	va_start(ap, fmt);
	buf.vformat(fmt, ap);
	va_end(ap);

	logger_error("%s", buf.c_str());

	acl::string page_buf;
	if (acl::ifstream::load(var_cfg_index_page, &page_buf) == false)
	{
		logger_error("load %s error: %s", var_cfg_index_page,
			acl::last_serror());
		return reply(req, res, "load html page failed!");
	}

	res.setStatus(200)
		.setKeepAlive(req.isKeepAlive())
		.setContentLength(page_buf.length());
	return res.write(page_buf) && req.isKeepAlive();
}

bool client_servlet::show_login(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	acl::string page_buf;
	if (acl::ifstream::load(var_cfg_login_page, &page_buf) == false)
	{
		logger_error("load %s error: %s", var_cfg_login_page,
			acl::last_serror());
		return reply(req, res, "load html page failed!");
	}

	res.setStatus(200)
		.setKeepAlive(req.isKeepAlive())
		.setContentLength(page_buf.length());
	return res.write(page_buf) && req.isKeepAlive();
}

bool client_servlet::doUnknown(acl::HttpServletRequest&,
	acl::HttpServletResponse& res)
{
	res.setStatus(400);
	res.setContentType("text/html; charset=utf-8");
	// ���� http ��Ӧͷ
	if (res.sendHeader() == false)
		return false;
	// ���� http ��Ӧ��
	acl::string buf("<root error='unkown request method' />\r\n");
	(void) res.getOutputStream().write(buf);
	return false;
}

bool client_servlet::doError(acl::HttpServletRequest&, acl::HttpServletResponse&)
{
	//logger_error("error happend");
	return false;
}

bool client_servlet::get_servers()
{
	servers_.clear();

	// �ȴ�������ȡ��������Ҫ��ѯ�ĵ�ַ
	const std::vector<acl::string>& addrs =
		server_manager::get_instance().get_addrs();
	std::vector<acl::string>::const_iterator cit = addrs.begin();
	for (; cit != addrs.end(); ++cit)
		servers_.push_back(*cit);

	// �ٸ����������в�ѯ
	lookup_dns();

	if (servers_.empty())
	{
		logger_error("no result for domain: %s", domain_.c_str());
		return false;
	}
	else
		return true;
}

void client_servlet::lookup_dns()
{
	if (domain_.empty())
	{
		logger("domain null, don't need to search dns");
		return;
	}

	int  err = 0;
	ACL_DNS_DB* res = acl_gethostbyname(domain_.c_str(), &err);
	if (res == NULL)
	{
		logger_error("gethostbyname failed, domain: %s, err: %d",
			domain_.c_str(), err);
		return;
	}

	ACL_ITER iter;

	char  addr[256];
	acl_foreach(iter, res)
	{
		const ACL_HOST_INFO* info = (const ACL_HOST_INFO*) iter.data;
		acl::safe_snprintf(addr, sizeof(addr), "%s:%d", info->ip, port_);
		servers_.push_back(addr);
	}

	acl_netdb_free(res);
}

bool client_servlet::doGet(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	return doPost(req, res);
}

// ʹ�� POP �˻����������֤
bool client_servlet::doLogin(const char* user, const char* pass)
{
	acl::socket_stream conn;
	if (conn.open(var_cfg_pop_server, var_cfg_conn_timeout,
		var_cfg_rw_timeout) == false)
	{
		logger_error("connect pop server: %s error, user: %s",
			var_cfg_pop_server, user);
		return false;
	}

	acl::string line;
	if (conn.gets(line) == false)
	{
		logger_error("gets banner from pop server: %s error, user: %s",
			var_cfg_pop_server, user);
		return false;
	}

	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("pop server: %s, banner: %s error, user: %s",
			var_cfg_pop_server, line.c_str(), user);
		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// for USER cmd
	//
	if (conn.format("USER %s\r\n", user) < 0)
	{
		logger_error("write USER cmd to pop server: %s error, user: %s",
			var_cfg_pop_server, user);
		return false;
	}

	line.clear();
	if (conn.gets(line) == false)
	{
		logger_error("gets USER's reply error, pop: %s, user: %s",
			var_cfg_pop_server, user);
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("USER's reply: %s error, pop: %s, user: %s",
			line.c_str(), var_cfg_pop_server, user);
		return false;
	}

	//////////////////////////////////////////////////////////////////////
	// for PASS cmd
	//
	if (conn.format("PASS %s\r\n", pass) < 0)
	{
		logger_error("write PASS cmd error, pop: %s, user: %s",
			var_cfg_pop_server, user);
		return false;
	}
	line.clear();
	if (conn.gets(line) == false)
	{
		logger_error("gets PASS's reply error, pop: %s, user: %s",
			var_cfg_pop_server, user);
		return false;
	}
	if (line.ncompare("+OK", 3, false) != 0)
	{
		logger_error("PASS's reply: %s error, pop: %s, user: %s",
			line.c_str(), var_cfg_pop_server, user);
		return false;
	}

	return true;
}

bool client_servlet::doPost(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	bool keep_alive = req.isKeepAlive();
	res.setContentType("text/html; charset=utf-8")	// ������Ӧ�ַ���
		.setKeepAlive(keep_alive);		// �����Ƿ񱣳ֳ�����

	const char* path = req.getPathInfo();
	if (path == NULL || *path == 0)
	{
		logger_error("getPathInfo null");
		return show_login(req, res);
	}

	if (strcasecmp(path, var_cfg_path_info) != 0
		&& strcasecmp(path, "/") != 0)
	{
		logger_error("invalid path: %s", path);
		return reply_status(req, res, 404, "not found: %s", path);
	}

	// ��������в���Ҫ�����֤����ֱ�ӽ��д���
	if (!var_cfg_auth_enable)
		return doAction(req, res);

	// ���濪ʼ�û������֤����

	// �ȼ�� session ��֤��Ϣ�Ƿ����
	const char* user = req.getSession().getAttribute(var_cfg_session_key);
	if (user && *user)
	{
		if (access_list::get_instance().check_user(user) == false)
		{
			(void) reply(req, res, "Permission denied!");
			logger_warn("user: %s was denied!", user);
			return false;
		}

		logger("user: %s session login, sid: %s", user,
			req.getSession().getSid());
		return doAction(req, res);
	}
	else
	{
		const char* ptr = req.getCookieValue("ACL_SESSION_ID");
		logger("no session for user, ACL_SESSION_ID cookie: %s",
			ptr ? ptr : "null");
	}

	// �ټ������������˺�/���벢ͨ�� POP ������������֤

	user = req.getParameter("user");
	if (user == NULL || *user == 0)
		return show_login(req, res);

	const char* pass = req.getParameter("pass");
	if (pass == NULL || *pass == 0)
		return show_login(req, res);

	if (access_list::get_instance().check_user(user) == false)
	{
		(void) reply(req, res, "Permission denied!");
		logger_warn("user: %s was denied!", user);
		return false;
	}

	// ���� POP �����֤����
	if (doLogin(user, pass) == false)
	{
		logger_error("login error, user: %s, pass: %s", user, pass);
		return show_login(req, res);
	}

	logger("user: %s pop login", user);

	// ��֤ͨ�������¼ session ��֤��Ϣ
	req.getSession().setMaxAge(var_cfg_session_ttl);

	if (req.getSession().setAttribute(var_cfg_session_key, user) == false)
	{
		logger_error("set session failed, user: %s, memcache: %s",
			user, var_cfg_memcache_addr);
		return show_login(req, res);
	}
	else
		logger("set session ok, user: %s, memcache: %s, sid: %s",
			user, var_cfg_memcache_addr, req.getSession().getSid());

	return doAction(req, res);
}

bool client_servlet::doAction(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	// ���ÿ鴫�䷽ʽ�����������Կ鴫�䷽ʽ�ظ�����
	res.setChunkedTransferEncoding(true)
		.setContentType("text/xml; charset=utf-8")
		.setKeepAlive(req.isKeepAlive());

	// ��дҳ�濪ʼ����
	if (res.format("<?xml version=\"1.0\"?><servers>") < 0)
	{
		logger_error("write head failed!");
		return false;
	}

	bool ok;

	if (var_cfg_pull_data)
		ok = doRequest(req, res);
	else
		ok = doResponse(req, res);

	if (!ok)
		return false;

	// ��� XML ��β���
	if (res.write("</servers>") == false)
	{
		logger_error("write html end failed");
		return false;
	}

	// HTTP �鴫��ģʽ�±�������Բ���Ϊ�ձ�ʾ������̽���
	if (res.write(NULL, 0) == false)
	{
		logger_error("write chunked end failed");
		return false;
	}

	return req.isKeepAlive();
}

bool client_servlet::doResponse(acl::HttpServletRequest&,
	acl::HttpServletResponse& res)
{
	acl::string buf;
	status_manager::get_instance().get_status(buf);

	if (!buf.empty() && res.write(buf) == false)
		return false;

	return true;
}

bool client_servlet::doRequest(acl::HttpServletRequest& req,
	acl::HttpServletResponse& res)
{
	// �������̣߳���������־��ѯ��������ѯ��־����

	if (get_servers() == false)
		return reply(req, res, "get all servers's addrs failed!");

	// ��ʼ�����̳߳أ������߳��������е� TCP �ַ���������з�������״̬��Ϣ
	std::vector<collect_client*> collecters;
	message_manager* manager = new message_manager;
	std::vector<acl::string>::const_iterator cit = servers_.begin();
	int  nthreads = 0;
	for (; cit != servers_.end(); ++cit)
	{
		// �����������߳̽��д���
		collect_client* collect = new collect_client(*manager,
			(*cit).c_str());
		collect->set_detachable(false);
		collecters.push_back(collect);
		nthreads++;
		// �������̴߳������
		collect->start();
	}

	// �ȴ��������̵߳Ĳ�ѯ���
	bool ok = wait_result(res, *manager, nthreads);

	// ������ɾ�����е����߳��������
	std::vector<collect_client*>::iterator it = collecters.begin();
	for (; it != collecters.end(); ++it)
	{
		(*it)->wait();
		delete (*it);
	}

	delete manager;
	return ok;
}

bool client_servlet::wait_result(acl::HttpServletResponse& res,
	message_manager& manager, int nthreads)
{
	bool disconnected = false;
	int  n = 0;

	// �첽�����������̵߳Ĳ�ѯ�������
	while (n < nthreads)
	{
		// ����Ϣ�����е������߳���ɵ�����
		message* msg = manager.pop();
		if (msg == NULL)
		{
			logger_error("pop message failed!");
			return false;
		}
		n++;

		if (!disconnected)
		{
			// ����������ĳ�����̵߳Ĳ�ѯ���
			if (reply(res, *msg) < 0)
				disconnected = true;
		}

		// ɾ����̬�������־��Ϣ����
		delete msg;
	}

	// ��������������������߳��������ʱ��˵����ѯ���
	logger("All threads over!");
	return !disconnected;
}

int client_servlet::reply(acl::HttpServletResponse& res, message& msg)
{
	//const acl::string& server = msg.get_server();
	const acl::string& buf = msg.get_result();
	if (buf.empty())
		return 0;
	else
		return res.format("%s", buf.c_str());
}
