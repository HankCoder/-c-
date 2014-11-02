#include "stdafx.h"
#include "ServerConnection.h"
#include "ClientConnection.h"
#include "ManagerTimer.h"
#include "master_service.h"

////////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_backend_service;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "backend_service", "dispatch.sock", &var_cfg_backend_service },

	{ 0, 0, 0 }
};

acl::master_bool_tbl var_conf_bool_tab[] = {
	{ 0, 0, 0 }
};

int   var_cfg_manager_timer;
int   var_cfg_conn_expired;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "manager_timer", 1, &var_cfg_manager_timer, 0, 0 },
	{ "conn_expired", 10, &var_cfg_conn_expired, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

acl::master_int64_tbl var_conf_int64_tab[] = {
	{ 0, 0 , 0 , 0, 0 }
};

////////////////////////////////////////////////////////////////////////////////

master_service::master_service()
{
}

master_service::~master_service()
{
}

bool master_service::on_accept(acl::aio_socket_stream* client)
{
	if (0)
	logger("connect from %s, fd %d", client->get_peer(true),
		client->sock_handle());

	acl_non_blocking(client->sock_handle(), ACL_BLOCKING);

	// ���ݿͻ������ӷ���˿ںŵĲ�ͬ�����ֲ�ͬ�ķ���Ӧ��Э��
	const char* local = client->get_local(true);
	if (acl_strrncasecmp(local, var_cfg_backend_service,
		strlen(var_cfg_backend_service)) == 0)
	{
		// ������������������ں�˷���ģ�������
		IConnection* conn = new ServerConnection(client);

		conn->run();
		return true;
	}
	else
	{
		// ����������������ǰ�˿ͻ���ģ�������
		IConnection* conn = new ClientConnection(client, var_cfg_conn_expired);

		conn->run();
		return true;
	}

	return true;
}

void master_service::proc_on_init()
{
	if (var_cfg_manager_timer <= 0)
		var_cfg_manager_timer = 1;

	// ������̨��ʱ������������δ�����ǰ�˿ͻ�������

	timer_ = new ManagerTimer();
	timer_->keep_timer(true);
	timer_->set_task(1, var_cfg_manager_timer * 1000000);

	// ���û��෽�����ö�ʱ������
	proc_set_timer(timer_);
}

void master_service::proc_on_exit()
{
}
