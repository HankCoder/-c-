#include "StdAfx.h"
#include "lib_acl.h"
#include <iostream>
#include <assert.h>
#include "acl_cpp/stream/aio_handle.hpp"
#include "AioClient.h"

#ifdef WIN32
# ifndef snprintf
#  define snprintf _snprintf
# endif
#endif

using namespace acl;

CConnectClientCallback::CConnectClientCallback(IO_CTX* ctx,
	aio_socket_stream* client, int id)
: client_(client)
, ctx_(ctx)
, nwrite_(0)
, id_(id)
{
}

CConnectClientCallback::~CConnectClientCallback()
{
	std::cout << ">>>ID: " << id_ << ", CConnectClientCallback deleted now!"
		<< std::endl;
}

bool CConnectClientCallback::read_callback(char* data, int len)
{
	(void) data;
	(void) len;

	ctx_->nread_total++;

	if (ctx_->debug)
	{
		if (nwrite_ < 10)
			std::cout << "gets(" << nwrite_ << "): " << data;
		else if (nwrite_ % 2000 == 0)
			std::cout << ">>ID: " << id_ << ", I: "
			<< nwrite_ << "; "<<  data;
	}

	// ����յ����������˳���Ϣ����ҲӦ�˳�
	if (strncasecmp(data, "quit", 4) == 0)
	{
		// ���������������
		client_->format("Bye!\r\n");
		// �ر��첽������
		client_->close();
		return (true);
	}

	if (nwrite_ >= ctx_->nwrite_limit)
	{
		if (ctx_->debug)
			std::cout << "ID: " << id_
			<< ", nwrite: " << nwrite_
			<< ", nwrite_limit: " << ctx_->nwrite_limit
			<< ", quiting ..." << std::endl;

		// ������������˳���Ϣ
		client_->format("quit\r\n");
		client_->close();
	}
	else
	{
		char  buf[256];
		snprintf(buf, sizeof(buf), "hello world: %d\n", nwrite_);
		client_->write(buf, (int) strlen(buf));

		// ���������������
		//client_->format("hello world: %d\n", nwrite_);
	}

	return (true);
}

bool CConnectClientCallback::write_callback()
{
	ctx_->nwrite_total++;
	nwrite_++;

	// �ӷ�������һ������
	client_->gets(ctx_->read_timeout, false);
	return (true);
}

void CConnectClientCallback::close_callback()
{
	if (client_->is_opened() == false)
	{
		std::cout << "Id: " << id_ << " connect "
			<< ctx_->addr << " error: "
			<< acl_last_serror();

		// ����ǵ�һ�����Ӿ�ʧ�ܣ����˳�
		if (ctx_->nopen_total == 0)
		{
			std::cout << ", first connect error, quit";
			/* ����첽��������������Ϊ�˳�״̬ */
			client_->get_handle().stop();
		}
		std::cout << std::endl;
		delete this;
		return;
	}

	// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
	delete this;
}

bool CConnectClientCallback::timeout_callback()
{
	std::cout << "Connect " << ctx_->addr << " Timeout ..." << std::endl;
	client_->close();
	return (false);
}

bool CConnectClientCallback::open_callback()
{
	// ���ӳɹ�������IO��д�ص�����
	client_->add_read_callback(this);
	client_->add_write_callback(this);
	ctx_->nopen_total++;

	acl_assert(id_ > 0);
	if (ctx_->nopen_total < ctx_->nopen_limit)
	{
		// ��ʼ������һ�����ӹ���
		if (connect_server(ctx_, id_ + 1) == false)
			std::cout << "connect error!" << std::endl;
	}

	// �첽���������������
	//client_->format("hello world: %d\n", nwrite_);
	char  buf[256];
	snprintf(buf, sizeof(buf), "hello world: %d\n", nwrite_);
	client_->write(buf, (int) strlen(buf));

	// �첽�ӷ�������ȡһ������
	client_->gets(ctx_->read_timeout, false);

	// ��ʾ�����첽����
	return (true);
}

bool CConnectClientCallback::connect_server(IO_CTX* ctx, int id)
{
	// ��ʼ�첽����Զ�̷�����
	// const char* addr = "221.194.139.155:18887";
	// ctx->connect_timeout = 1;
	aio_socket_stream* stream = aio_socket_stream::open(ctx->handle,
		ctx->addr, ctx->connect_timeout);
	if (stream == NULL)
	{
		std::cout << "connect " << ctx->addr << " error!" << std::endl;
		std::cout << "stoping ..." << std::endl;
		if (id == 0)
			ctx->handle->stop();
		return (false);
	}

	// �������Ӻ�Ļص�������
	CConnectClientCallback* callback = new CConnectClientCallback(ctx, stream, id);

	// ������ӳɹ��Ļص�������
	stream->add_open_callback(callback);

	// �������ʧ�ܺ�ص�������
	stream->add_close_callback(callback);

	// ������ӳ�ʱ�Ļص�������
	stream->add_timeout_callback(callback);
	return (true);
}
