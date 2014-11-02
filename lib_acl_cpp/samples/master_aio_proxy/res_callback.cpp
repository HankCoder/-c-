#include "stdafx.h"
#include "req_callback.h"
#include "res_callback.h"

res_callback::res_callback(req_callback* req, acl::ofstream* res_fp)
: conn_(NULL)
, req_(req)
, res_fp_(res_fp)
{

}

res_callback::~res_callback()
{
	acl_assert(req_ == NULL);
}

bool res_callback::read_callback(char* data, int len)
{
	if (req_ == NULL)
	{
		logger_warn("client peer disconnected!");
		return false;
	}

	// ȡ�ÿͻ����������������ӷ���˷���������д��
	acl::aio_socket_stream& peer = req_->get_conn();
	peer.write(data, len);

	// ������ͬʱд�뱾����Ӧ�����ļ�
	if (res_fp_)
		res_fp_->write(data, len);

	return true;
}

void res_callback::close_callback()
{
	if (req_)
	{
		req_callback* req = req_;
		req_ = NULL;
		req->disconnect();
	}

	// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶  
	delete this;  
}

bool res_callback::open_callback()
{
	// ���ӳɹ�������IO��д�ص�����
	conn_->add_read_callback(this);
	conn_->add_write_callback(this);

	// �ȴ��ӷ�������ȡ����
	conn_->read();

	// ֪ͨ�������Ѿ����������������
	acl_assert(req_);
	req_->on_connected();
	return true;
}

bool res_callback::start(acl::aio_handle& handle, const char* server_addr)
{
	conn_ = acl::aio_socket_stream::open(&handle, server_addr, 0);
	if (conn_ == NULL)
	{
		logger_error("connect server %s error %s",
			server_addr, acl::last_serror());
		return false;
	}

	conn_->add_open_callback(this);
	conn_->add_timeout_callback(this);
	conn_->add_close_callback(this);

	return true;
}

acl::aio_socket_stream& res_callback::get_conn()
{
	acl_assert(conn_);
	return *conn_;
}

void res_callback::disconnect()
{
	if (conn_)
	{
		req_ = NULL;
		conn_->close();
	}
	else
		delete this;
}
