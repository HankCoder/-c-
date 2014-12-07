#include "stdafx.h"
#include <assert.h>
#include "gc_timer.h"
#include "rpc_stats.h"
#include "rpc_timer.h"
#include "rpc_manager.h"
#include "http_client.h"
#include "master_service.h"

////////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_rpc_addr;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "rpc_addr", "127.0.0.1:0", &var_cfg_rpc_addr },

	{ 0, 0, 0 }
};

int   var_cfg_preread;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "preread", 1, &var_cfg_preread },

	{ 0, 0, 0 }
};

int   var_cfg_nthreads_limit;
int   var_cfg_echo_length;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "nthreads_limit", 4, &var_cfg_nthreads_limit, 0, 0 },
	{ "echo_length", 1024, &var_cfg_echo_length, 0, 0 },

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
	// ������������߳���Ԥ��������������Ԥ����־λ
	if (var_cfg_preread)
	{
		ACL_VSTREAM* vstream = client->get_vstream();
		vstream->flag |= ACL_VSTREAM_FLAG_PREREAD;
	}

	// �����첽�ͻ������Ļص���������첽�����а�
	http_client* callback = new http_client(client);

	// ע���첽���Ķ��ص�����
	client->add_read_callback(callback);

	// ע���첽����д�ص�����
	client->add_write_callback(callback);

	// ע���첽���Ĺرջص�����
	client->add_close_callback(callback);

	// ע���첽���ĳ�ʱ�ص�����
	client->add_timeout_callback(callback);

	rpc_read_wait_add();

	// ����첽���Ƿ�ɶ�
	client->read_wait(0);

	return true;
}

void master_service::proc_on_init()
{
	rpc_stats_init();

	// get aio_handle from master_aio
	acl::aio_handle* handle = get_handle();
	assert(handle != NULL);

	// init rpc service
	rpc_manager::get_instance().init(handle, var_cfg_nthreads_limit,
		var_cfg_rpc_addr);

	// start one timer to logger the rpc status
	rpc_timer* timer = new rpc_timer(*handle);
	timer->start(1);

	if (var_mem_slice_on)
	{
		gc_timer* gt = new gc_timer(*handle);
		gt->start(2);
	}
}

void master_service::proc_on_exit()
{
	rpc_manager::get_instance().finish();
}
