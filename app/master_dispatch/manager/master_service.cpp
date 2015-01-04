#include "stdafx.h"
#include "access_list.h"
#include "client_servlet.h"
#include "pull_mode/server_manager.h"
#include "push_mode/server_servlet.h"
#include "push_mode/status_timer.h"
#include "master_service.h"

//////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_servers;
char *var_cfg_index_page;
char *var_cfg_login_page;
char *var_cfg_server_domain;
char *var_cfg_html_charset;
char *var_cfg_pop_server;
char *var_cfg_allow_users;
char *var_cfg_allow_clients;
char *var_cfg_allow_servers;
char *var_cfg_memcache_addr;
char *var_cfg_session_key;
char *var_cfg_path_info;
char *var_cfg_status_service;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "index_page", "index.htm", &var_cfg_index_page },
	{ "login_page", "login.htm", &var_cfg_login_page },
	{ "server_domain", "", &var_cfg_server_domain },
	{ "html_charset", "utf-8", &var_cfg_html_charset },
	{ "session_key", "dispatch_manager_id", &var_cfg_session_key },
	{ "memcache_addr", "127.0.0.1:11211", &var_cfg_memcache_addr },
	{ "path_info", "/dispatch_collect", &var_cfg_path_info },

	{ "pop_server", "", &var_cfg_pop_server },

	{ "allow_users", "all", &var_cfg_allow_users },
	{ "allow_clients", "all", &var_cfg_allow_clients },
	{ "allow_servers", "127.0.0.1:127.0.0.1, 192.168.0.0:192.168.255.255",
		&var_cfg_allow_servers },

	{ "server_list", "", &var_cfg_servers },
	{ "status_service", "", &var_cfg_status_service },

	{ 0, 0, 0 }
};

int   var_cfg_auth_enable;
int   var_cfg_pull_data;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "auth_enable", 0, &var_cfg_auth_enable },
	{ "pull_data", 0, &var_cfg_pull_data },

	{ 0, 0, 0 }
};

int   var_cfg_conn_timeout;
int   var_cfg_rw_timeout;
int   var_cfg_dns_ttl;
int   var_cfg_server_port;
int   var_cfg_session_ttl;
int   var_cfg_status_timer;
int   var_cfg_status_ttl;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "conn_timeout", 30, &var_cfg_conn_timeout, 0, 0 },
	{ "rw_timeout", 30, &var_cfg_rw_timeout, 0, 0 },
	{ "dns_ttl", 30, &var_cfg_dns_ttl, 0, 0 },
	{ "server_port", 10081, &var_cfg_server_port, 0, 0 },
	{ "session_ttl", 3600, &var_cfg_session_ttl, 0, 0 },
	{ "status_timer", 1, &var_cfg_status_timer, 0, 0 },
	{ "status_ttl", 10, &var_cfg_status_ttl, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

acl::master_int64_tbl var_conf_int64_tab[] = {

	{ 0, 0 , 0 , 0, 0 }
};

//////////////////////////////////////////////////////////////////////////

master_service::master_service()
: status_timer_(NULL)
{
}

master_service::~master_service()
{
	status_timer_->destroy();
}

bool master_service::thread_on_read(acl::socket_stream* conn)
{
	acl::HttpServlet* servlet = (acl::HttpServlet*) conn->get_ctx();
	if (servlet == NULL)
		logger_fatal("servlet null!");

	return servlet->doRun(var_cfg_memcache_addr, conn);
}

bool master_service::thread_on_accept(acl::socket_stream* conn)
{
	const char* local = conn->get_local(true);
	const char* peer = conn->get_peer(false);

	if (local == NULL || *local == 0 || peer == NULL || *peer == 0)
	{
		logger_warn("invalid client, local: %s, peer: %s",
			local ? local : "null", peer ? peer : "null");
		return false;
	}

	logger_debug(DEBUG_CONN, 2, "connect from %s, fd: %d",
		conn->get_peer(true), conn->sock_handle());

	// ���ö�д��ʱʱ��
	conn->set_rw_timeout(var_cfg_rw_timeout);

	acl::HttpServlet* servlet;

	// ���������ʵķ����ַ���ַ��ʵ���������

	// �������������ں�˷�������Ⱥʱ
	if (acl_strrncasecmp(local, var_cfg_status_service,
		strlen(var_cfg_status_service)) == 0)
	{
		// ������� IP ����Ȩ��
		if (access_list::get_instance().check_server(peer) == false)
		{
			logger_warn("Denied from server ip: %s", peer);
			return false;
		}

		servlet = new server_servlet();
		// ��Ϊ������������ JSON/XML ���ݣ����Բ�Ҫ�����
		servlet->setParseBody(false);
	}

	// ��������������ǰ��ʱ�����ͻ��� IP ����Ȩ��
	else if (access_list::get_instance().check_client(peer) == false)
	{
		logger_warn("Denied from client ip: %s", peer);
		return false;
	}
	// ����Ϊǰ�˿ͻ����ṩ����ķ������
	else
		servlet = new client_servlet(var_cfg_server_domain,
			var_cfg_server_port);

	conn->set_ctx(servlet);
	return true;
}

bool master_service::thread_on_timeout(acl::socket_stream* conn)
{
	logger_debug(DEBUG_CONN, 2, "read timeout from %s, fd: %d",
		conn->get_peer(), conn->sock_handle());

	return false;
}

void master_service::thread_on_close(acl::socket_stream* conn)
{
	logger_debug(DEBUG_CONN, 2, "disconnect from %s, fd: %d",
		conn->get_peer(), conn->sock_handle());

	acl::HttpServlet* servlet = (acl::HttpServlet*) conn->get_ctx();
	delete servlet;
}

void master_service::thread_on_init()
{
}

void master_service::thread_on_exit()
{
}

void master_service::proc_on_init()
{
	// ��ʼ���û��� IP ���ʰ�����
	if (var_cfg_allow_clients && *var_cfg_allow_clients)
	{
		access_list::get_instance()
			.set_allow_clients(var_cfg_allow_clients);
	}

	/*----------------- ������ģʽ�µ�����ѡ�� ------------------------*/

	// ��ʼ�� DNS ��ѯ���� acl ���еĻ���ʱ��
	// acl_netdb_cache_init(var_cfg_dns_ttl, 1);

	// ��ʹ�������ļ��еķ�������
	if (var_cfg_servers && *var_cfg_servers && server_manager
		::get_instance().init(var_cfg_servers) == false)
	{
		logger("Add static server: none!");
	}

	// ��ʼ���û����ʰ�����
	if (var_cfg_allow_users && *var_cfg_allow_users)
	{
		access_list::get_instance()
			.set_allow_users(var_cfg_allow_users);
	}

	/*----------------- ������ģʽ�µ�����ѡ�� ------------------------*/
	// ��ʼ����˷��� IP ���ʰ�����
	if (var_cfg_allow_servers && *var_cfg_allow_servers)
	{
		access_list::get_instance()
			.set_allow_servers(var_cfg_allow_servers);
	}

	// ���ö�ʱ����������ʱ��δ�㱨״̬�ķ�������Ϣ
	if (var_cfg_status_ttl > 0)
	{
		status_timer_ = new status_timer();
		status_timer_->keep_timer(true);
		status_timer_->set_task(1, 1000000);
		proc_set_timer(status_timer_);
	}
}

void master_service::proc_on_exit()
{
}
