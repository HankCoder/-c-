#include "StdAfx.h"
#include <iostream>
#include "lib_acl.h"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_listen_stream.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#include "AioServer.h"

using namespace acl;

/**
* �ӳٶ��ص�������
*/
class timer_reader: public aio_timer_reader
{
public:
	timer_reader()
	{
		std::cout << "timer_reader init now" << std::endl;
	}

	~timer_reader()
	{
	}

	// aio_timer_reader ������������� destroy ����
	void destroy()
	{
		std::cout << "timer_reader delete now" << std::endl;
		delete this;
	}

	// ���ػ���ص�����
	virtual void timer_callback(unsigned int id)
	{
		std::cout << "timer_reader: timer_callback now" << std::endl;

		// ���û���Ĵ������
		aio_timer_reader::timer_callback(id);
	}
};

/**
* �ӳ�д�ص�������
*/
class timer_writer: public aio_timer_writer
{
public:
	timer_writer()
	{
		std::cout << "timer_writer init now" << std::endl;
	}

	~timer_writer()
	{
	}

	// aio_timer_reader ������������� destroy ����
	void destroy()
	{
		std::cout << "timer_writer delete now" << std::endl;
		delete this;
	}

	// ���ػ���ص�����
	virtual void timer_callback(unsigned int id)
	{
		std::cout << "timer_writer: timer_callback now" << std::endl;

		// ���û���Ĵ������
		aio_timer_writer::timer_callback(0);
	}
};

CAcceptedClientCallback::CAcceptedClientCallback(aio_socket_stream* client)
: client_(client)
, i_(0)
{

}

CAcceptedClientCallback::~CAcceptedClientCallback()
{
	std::cout << "delete io_callback now ..." << std::endl;
}

bool CAcceptedClientCallback::read_callback(char* data, int len)
{
	i_++;
	if (i_ < 10)
		std::cout << ">>gets(i:" << i_ << "): " << data;

	// ���Զ�̿ͻ���ϣ���˳�����ر�֮
	if (strncasecmp(data, "quit", 4) == 0)
	{
		client_->format("Bye!\r\n");
		client_->close();
		return (true);
	}

	// ���Զ�̿ͻ���ϣ�������Ҳ�رգ�����ֹ�첽�¼�����
	else if (strncasecmp(data, "stop", 4) == 0)
	{
		client_->format("Stop now!\r\n");
		client_->close();  // �ر�Զ���첽��

		// ֪ͨ�첽����ر�ѭ������
		client_->get_handle().stop();
		return (true);
	}

	int   delay = 0;

	if (strncasecmp(data, "write_delay", strlen("write_delay")) == 0)
	{
		// �ӳ�д����

		const char* ptr = data + strlen("write_delay");
		delay = atoi(ptr);
		if (delay > 0)
		{
			std::cout << ">> write delay " << delay
				<< " second ..." << std::endl;
			timer_writer* timer = new timer_writer();
			client_->write(data, len, delay, timer);
			client_->gets(10, false);
			return (true);
		}
	}
	else if (strncasecmp(data, "read_delay", strlen("read_delay")) == 0)
	{
		// �ӳٶ�����

		const char* ptr = data + strlen("read_delay");
		delay = atoi(ptr);
		if (delay > 0)
		{
			client_->write(data, len);
			std::cout << ">> read delay " << delay
				<< " second ..." << std::endl;
			timer_reader* timer = new timer_reader();
			client_->gets(10, false, delay, timer);
			return (true);
		}
	}

	// ��Զ�̿ͻ��˻�д�յ�������
	client_->write(data, len);

	// ��������һ������
	client_->gets(10, false);
	return (true);
}

bool CAcceptedClientCallback::write_callback()
{
	return (true);
}

void CAcceptedClientCallback::close_callback()
{
	// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
	delete this;
}

bool CAcceptedClientCallback::timeout_callback()
{
	std::cout << "Timeout ..." << std::endl;
	return (true);
}

///////////////////////////////////////////////////////////////////////////////

CServerCallback::CServerCallback()
{

}

CServerCallback::~CServerCallback()
{

}

bool CServerCallback::accept_callback(aio_socket_stream* client)
{
	// �����첽�ͻ������Ļص���������첽�����а�
	CAcceptedClientCallback* callback = new CAcceptedClientCallback(client);

	// ע���첽���Ķ��ص�����
	client->add_read_callback(callback);

	// ע���첽����д�ص�����
	client->add_write_callback(callback);

	// ע���첽���Ĺرջص�����
	client->add_close_callback(callback);

	// ע���첽���ĳ�ʱ�ص�����
	client->add_timeout_callback(callback);

	// ���첽����һ������
	client->gets(10, false);
	return (true);
}
