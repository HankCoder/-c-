#include <iostream>
#include <assert.h>
#include "lib_acl.h"
#include "acl_cpp/acl_cpp_init.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_istream.hpp"
#include "acl_cpp/stream/aio_listen_stream.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"

using namespace acl;

static int   __max = 0;
static int   __timeout = 0;

/**
 * �ӳٶ��ص�������
 */
class timer_reader: public aio_timer_reader
{
public:
	timer_reader(int delay)
	{
		delay_ = delay;
		std::cout << "timer_reader init, delay: " << delay << std::endl;
	}

	~timer_reader()
	{
	}

	// aio_timer_reader ������������� destroy ����
	void destroy()
	{
		std::cout << "timer_reader delete, delay: "  << delay_ << std::endl;
		delete this;
	}

	// ���ػ���ص�����
	virtual void timer_callback(unsigned int id)
	{
		std::cout << "timer_reader(" << id
			<< "): timer_callback, delay: " << delay_ << std::endl;

		// ���û���Ĵ������
		aio_timer_reader::timer_callback(id);
	}

private:
	int   delay_;
};

/**
 * �ӳ�д�ص�������
 */
class timer_writer: public aio_timer_writer
{
public:
	timer_writer(int delay)
	{
		delay_ = delay;
		std::cout << "timer_writer init, delay: " << delay << std::endl;
	}

	~timer_writer()
	{
	}

	// aio_timer_reader ������������� destroy ����
	void destroy()
	{
		std::cout << "timer_writer delete, delay: " << delay_ << std::endl;
		delete this;
	}

	// ���ػ���ص�����
	virtual void timer_callback(unsigned int id)
	{
		std::cout << "timer_writer(" << id << "): timer_callback, delay: "
			<< delay_ << std::endl;

		// ���û���Ĵ������
		aio_timer_writer::timer_callback(id);
	}

private:
	int   delay_;
};

/**
 * �첽�ͻ������Ļص��������
 */
class io_callback : public aio_callback
{
public:
	io_callback(aio_socket_stream* client)
		: client_(client)
		, i_(0)
	{
	}

	~io_callback()
	{
		std::cout << "delete io_callback now ..." << std::endl;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������Ķ��ɹ��ص�����
	 * @param data {char*} ���������ݵ�ַ
	 * @param len {int} ���������ݳ���
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool read_callback(char* data, int len)
	{
		i_++;
		if (i_ < 10)
			std::cout << ">>gets(i:" << i_ << "): "
				<< data << std::endl;

		// ���Զ�̿ͻ���ϣ���˳�����ر�֮
		if (strncasecmp(data, "quit", 4) == 0)
		{
			client_->format("Bye!\r\n");
			client_->close();
		}

		// ���Զ�̿ͻ���ϣ�������Ҳ�رգ�����ֹ�첽�¼�����
		else if (strncasecmp(data, "stop", 4) == 0)
		{
			client_->format("Stop now!\r\n");
			client_->close();  // �ر�Զ���첽��

			// ֪ͨ�첽����ر�ѭ������
			client_->get_handle().stop();
		}

		// ��Զ�̿ͻ��˻�д�յ�������

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
				timer_writer* timer = new timer_writer(delay);
				client_->write(data, len, delay * 1000000, timer);
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
				timer_reader* timer = new timer_reader(delay);
				client_->gets(10, false, delay * 1000000, timer);
				return (true);
			}
		}

		client_->write(data, len);
		//client_->gets(10, false);
		return (true);
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ�������д�ɹ��ص�����
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool write_callback()
	{
		return (true);
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	 */
	void close_callback()
	{
		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
		delete this;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool timeout_callback()
	{
		std::cout << "Timeout, delete it ..." << std::endl;
		return (false);
	}

private:
	aio_socket_stream* client_;
	int   i_;
};

/**
 * �첽�������Ļص��������
 */
class io_accept_callback : public aio_accept_callback
{
public:
	io_accept_callback() {}
	~io_accept_callback()
	{
		printf(">>io_accept_callback over!\n");
	}

	/**
	 * �����麯�������������ӵ������ô˻ص�����
	 * @param client {aio_socket_stream*} �첽�ͻ�����
	 * @return {bool} ���� true ��֪ͨ��������������
	 */
	bool accept_callback(aio_socket_stream* client)
	{
		// �����첽�ͻ������Ļص���������첽�����а�
		io_callback* callback = new io_callback(client);

		// ע���첽���Ķ��ص�����
		client->add_read_callback(callback);

		// ע���첽����д�ص�����
		client->add_write_callback(callback);

		// ע���첽���Ĺرջص�����
		client->add_close_callback(callback);

		// ע���첽���ĳ�ʱ�ص�����
		client->add_timeout_callback(callback);

		// ���޶�����������󳤶�ʱ
		if (__max > 0)
			client->set_buf_max(__max);

		// ���첽����һ������
		client->gets(__timeout, false);
		return (true);
	}
};

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"	-l ip:port\r\n"
		"	-L line_max_length\r\n"
		"	-t timeout\r\n"
		"	-k[use kernel event: epoll/iocp/kqueue/devpool]\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	bool use_kernel = false;
	int  ch;
	acl::string addr(":9001");

	while ((ch = getopt(argc, argv, "l:hkL:t:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return (0);
		case 'l':
			addr = optarg;
			break;
		case 'k':
			use_kernel = true;
			break;
		case 'L':
			__max = atoi(optarg);
			break;
		case 't':
			__timeout = atoi(optarg);
			break;
		default:
			break;
		}
	}

	acl::log::stdout_open(true);

	// �����첽���������
	aio_handle handle(use_kernel ? ENGINE_KERNEL : ENGINE_SELECT);

	// ���������첽��
	aio_listen_stream* sstream = new aio_listen_stream(&handle);

	// ��ʼ��ACL��(��������WIN32��һ��Ҫ���ô˺�������UNIXƽ̨�¿ɲ�����)
	acl::acl_cpp_init();

	// ����ָ���ĵ�ַ
	if (sstream->open(addr.c_str()) == false)
	{
		std::cout << "open " << addr.c_str() << " error!" << std::endl;
		sstream->close();
		// XXX: Ϊ�˱�֤�ܹرռ�������Ӧ�ڴ˴��� check һ��
		handle.check();

		getchar();
		return (1);
	}

	// �����ص�����󣬵��������ӵ���ʱ�Զ����ô������Ļص�����
	io_accept_callback callback;
	sstream->add_accept_callback(&callback);
	std::cout << "Listen: " << addr.c_str() << " ok!" << std::endl;

	while (true)
	{
		// ������� false ���ʾ���ټ�������Ҫ�˳�
		if (handle.check() == false)
		{
			std::cout << "aio_server stop now ..." << std::endl;
			break;
		}
	}

	// �رռ��������ͷ�������
	sstream->close();

	// XXX: Ϊ�˱�֤�ܹرռ�������Ӧ�ڴ˴��� check һ��
	handle.check();

	return (0);
}
