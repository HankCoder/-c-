#include "stdafx.h"
#include "http_client.h"
#include "master_service.h"

////////////////////////////////////////////////////////////////////////////////
// ����������

int  var_cfg_preread;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "preread", 1, &var_cfg_preread },

	{ 0, 0, 0 }
};

int  var_cfg_buf_size;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "buf_size", 1024, &var_cfg_buf_size, 0, 0 },

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
	//acl_tcp_nodelay(client->get_socket(), 1);

	// ������������߳���Ԥ��������������Ԥ����־λ
	if (var_cfg_preread)
	{
		ACL_VSTREAM* vstream = client->get_vstream();
		vstream->flag |= ACL_VSTREAM_FLAG_PREREAD;
	}

	// �����첽�ͻ������Ļص���������첽�����а�
	http_client* callback = new http_client(client, var_cfg_buf_size);

	// ע���첽���Ķ��ص�����
	client->add_read_callback(callback);

	// ע���첽����д�ص�����
	client->add_write_callback(callback);

	// ע���첽���Ĺرջص�����
	client->add_close_callback(callback);

	// ע���첽���ĳ�ʱ�ص�����
	client->add_timeout_callback(callback);

	client->keep_read(true);

	// ����첽���Ƿ�ɶ�
	client->read_wait(0);

	return true;
}

void master_service::proc_on_init()
{
}

void master_service::proc_on_exit()
{
}
