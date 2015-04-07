#include <iostream>
#include <assert.h>
#include "lib_acl.h"
#include "acl_cpp/acl_cpp_init.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/polarssl_conf.hpp"
#include "acl_cpp/stream/polarssl_io.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_istream.hpp"
#include "acl_cpp/stream/aio_listen_stream.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"

static int   __max = 0;
static int   __timeout = 0;
static int   __max_used = 0;
static int   __cur_used = 0;

// SSL ģʽ�µ� SSL ���ö���
static acl::polarssl_conf* __ssl_conf;

/**
 * �ӳٶ��ص�������
 */
class timer_reader: public acl::aio_timer_reader
{
public:
	timer_reader(int delay)
	{
		delay_ = delay;
		printf("timer_reader init, delay: %d\r\n", delay);
	}

	~timer_reader()
	{
	}

	// aio_timer_reader ������������� destroy ����
	void destroy()
	{
		printf("timer_reader delete, delay: %d\r\n", delay_);
		delete this;
	}

	// ���ػ���ص�����
	virtual void timer_callback(unsigned int id)
	{
		printf("timer_reader(%d): delay: %d\r\n", id, delay_);

		// ���û���Ĵ������
		aio_timer_reader::timer_callback(id);
	}

private:
	int   delay_;
};

/**
 * �ӳ�д�ص�������
 */
class timer_writer: public acl::aio_timer_writer
{
public:
	timer_writer(int delay)
	{
		delay_ = delay;
		printf("timer_writer init, delay: %d\r\n", delay);
	}

	~timer_writer()
	{
	}

	// aio_timer_reader ������������� destroy ����
	void destroy()
	{
		printf("timer_writer delete, delay: %d\r\n", delay_);
		delete this;
	}

	// ���ػ���ص�����
	virtual void timer_callback(unsigned int id)
	{
		printf("timer_writer(%d): timer_callback, delay: %d\r\n",
			id, delay_);

		// ���û���Ĵ������
		aio_timer_writer::timer_callback(id);
	}

private:
	int   delay_;
};

/**
 * �첽�ͻ������Ļص��������
 */
class io_callback : public acl::aio_callback
{
public:
	io_callback(acl::aio_socket_stream* client)
		: client_(client)
		, i_(0)
	{
	}

	~io_callback()
	{
		printf("delete io_callback now ...\r\n");
		__cur_used++;
	}

	/**
	 * ���ص��麯�����ûص��������������� aio_istream ʵ���е�
	 * gets/read �Ŀɶ������󱻵��ã����첽����ڲ���������������
	 * �ݶ�����ֱ�Ӵ��ݸ��û�������
	 * @param data {char*} ���������ݵ�ָ���ַ
	 * @param len {int} ���������ݳ���(> 0)
	 * @return {bool} �ú������� false ֪ͨ�첽����رո��첽��
	 */
	bool read_wakeup()
	{
		acl::polarssl_io* hook = (acl::polarssl_io*) client_->get_hook();
		if (hook == NULL)
		{
			// �� SSL ģʽ���첽��ȡһ������
			client_->gets(__timeout, false);
			return true;
		}

		// ���Խ��� SSL ����
		if (hook->handshake() == false)
		{
			printf("ssl handshake failed\r\n");
			return false;
		}

		// ��� SSL �����Ѿ��ɹ�����ʼ���ж�����
		if (hook->handshake_ok())
		{
			// �� reactor ģʽתΪ proactor ģʽ���Ӷ�ȡ��
			// read_wakeup �ص�����
			client_->disable_read();

			// �첽��ȡһ��
			client_->gets(__timeout, false);
			return true;
		}

		// SSL ���ֻ�δ��ɣ��ȴ��������ٴα�����
		return true;
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
		//if (i_ < 10)
		//	std::cout << ">>gets(i:" << i_ << "): "
		//		<< data << std::endl;

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
	acl::aio_socket_stream* client_;
	int   i_;
};

/**
 * �첽�������Ļص��������
 */
class io_accept_callback : public acl::aio_accept_callback
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
	bool accept_callback(acl::aio_socket_stream* client)
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

		// SSL ģʽ�£��ȴ��ͻ��˷���������Ϣ
		if (__ssl_conf != NULL)
		{
			// ע�� SSL IO ���̵Ĺ���
			acl::polarssl_io* ssl = new
				acl::polarssl_io(*__ssl_conf, true, true);

			if (client->setup_hook(ssl) == ssl)
			{
				std::cout << "setup_hook error" << std::endl;
				ssl->destroy();
				return false;
			}

			// ���ͻ������ڶ�����״̬�Դ��� read_wakeup �ص����̣�
			// SSL ���ֹ��̽��� read_wakeup �����
			client->read_wait(__timeout);
		}

		// �� SSL ģʽ�£����첽����һ������
		else
			client->gets(__timeout, false);

		return (true);
	}
};

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"	-l server_addr[ip:port, default: 127.0.0.1:9001]\r\n"
		"	-L line_max_length\r\n"
		"	-t timeout\r\n"
		"	-n conn_used_limit\r\n"
		"	-k[use kernel event: epoll/iocp/kqueue/devpool]\r\n"
		"	-K ssl_key_file -C ssl_cert_file [in SSL mode]\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	// �¼������Ƿ�����ں��еĸ�Чģʽ
	bool use_kernel = false;
	acl::string key_file, cert_file;
	acl::string addr("127.0.0.1:9001");
	int  ch;

	while ((ch = getopt(argc, argv, "l:hkL:t:K:C:n:")) > 0)
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
		case 'K':
			key_file = optarg;
			break;
		case 'C':
			cert_file = optarg;
			break;
		case 'n':
			__max_used = atoi(optarg);
			break;
		default:
			break;
		}
	}

	acl::log::stdout_open(true);

	// ��˽Կ��֤�鶼����ʱ�Ų��� SSL ͨ�ŷ�ʽ
	if (!key_file.empty() && !cert_file.empty())
	{
		__ssl_conf = new acl::polarssl_conf();

		// �������˵� SSL �Ự���湦��
		__ssl_conf->enable_cache(true);

		// ��ӱ��ط����֤��
		if (__ssl_conf->add_cert(cert_file.c_str()) == false)
		{
			delete __ssl_conf;
			__ssl_conf = NULL;
			std::cout << "add_cert error: " << cert_file.c_str()
				<< std::endl;
		}

		// ��ӱ��ط�����Կ
		else if (__ssl_conf->set_key(key_file.c_str()) == false)
		{
			delete __ssl_conf;
			__ssl_conf = NULL;
			std::cout << "set_key error: " << key_file.c_str()
				<< std::endl;
		}
		else
			std::cout << "Load cert&key OK!" << std::endl;
	}

	// �����첽���������
	acl::aio_handle handle(use_kernel ? acl::ENGINE_KERNEL : acl::ENGINE_SELECT);

	// ���������첽��
	acl::aio_listen_stream* sstream = new acl::aio_listen_stream(&handle);

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

		if (__max_used > 0 && __cur_used >= __max_used)
			break;
	}

	// �رռ��������ͷ�������
	sstream->close();

	// XXX: Ϊ�˱�֤�ܹرռ�������Ӧ�ڴ˴��� check һ��
	handle.check();

	// ɾ�� acl::polarssl_conf ��̬����
	delete __ssl_conf;

	return (0);
}
