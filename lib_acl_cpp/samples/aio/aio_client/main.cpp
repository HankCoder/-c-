#include <iostream>
#include <assert.h>
#include "lib_acl.h"
#include "acl_cpp/acl_cpp_init.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"

#ifdef WIN32
# ifndef snprintf
#  define snprintf _snprintf
# endif
#endif

using namespace acl;

typedef struct
{
	char  addr[64];
	aio_handle* handle;
	int   connect_timeout;
	int   read_timeout;
	int   nopen_limit;
	int   nopen_total;
	int   nwrite_limit;
	int   nwrite_total;
	int   nread_total;
	int   id_begin;
	bool  debug;
} IO_CTX;

static bool connect_server(IO_CTX* ctx, int id);

/**
 * �ͻ����첽�������ص�������
 */
class client_io_callback : public aio_open_callback
{
public:
	/**
	 * ���캯��
	 * @param ctx {IO_CTX*}
	 * @param client {aio_socket_stream*} �첽������
	 * @param id {int} ������ID��
	 */
	client_io_callback(IO_CTX* ctx, aio_socket_stream* client, int id)
		: client_(client)
		, ctx_(ctx)
		, nwrite_(0)
		, id_(id)
	{
	}

	~client_io_callback()
	{
		std::cout << ">>>ID: " << id_ << ", io_callback deleted now!" << std::endl;
	}

	/**
	 * �����麯��, ���첽��������Ҫ�������ʱ���ô˻ص�����
	 * @param data {char*} ���������ݵ�ַ
	 * @param len {int�� ���������ݳ���
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool read_callback(char* data, int len)
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
		if (acl::strncasecmp_(data, "quit", 4) == 0)
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

	/**
	 * �����麯��, ���첽��д�ɹ�ʱ���ô˻ص�����
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool write_callback()
	{
		ctx_->nwrite_total++;
		nwrite_++;

		// �ӷ�������һ������
		client_->gets(ctx_->read_timeout, false);
		return (true);
	}

	/**
	 * �����麯��, �����첽���ر�ʱ���ô˻ص�����
	 */
	void close_callback()
	{
		if (client_->is_opened() == false)
		{
			std::cout << "Id: " << id_ << " connect "
				<< ctx_->addr << " error: "
				<< acl::last_serror();

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

		/* ����첽�������ܼ�ص��첽������ */
		int nleft = client_->get_handle().length();
		if (ctx_->nopen_total == ctx_->nopen_limit && nleft == 1)
		{
			std::cout << "Id: " << id_ << " stop now! nstream: "
				<< nleft << std::endl;
			/* ����첽��������������Ϊ�˳�״̬ */
			client_->get_handle().stop();
		}

		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
		delete this;
	}

	/**
	 * �����麯�������첽����ʱʱ���ô˺���
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool timeout_callback()
	{
		std::cout << "Connect " << ctx_->addr << " Timeout ..." << std::endl;
		client_->close();
		return (false);
	}

	/**
	 * �����麯��, ���첽���ӳɹ�����ô˺���
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool open_callback()
	{
		// ���ӳɹ�������IO��д�ص�����
		client_->add_read_callback(this);
		client_->add_write_callback(this);
		ctx_->nopen_total++;

		acl::assert_(id_ > 0);
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

protected:
private:
	aio_socket_stream* client_;
	IO_CTX* ctx_;
	int   nwrite_;
	int   id_;
};

static bool connect_server(IO_CTX* ctx, int id)
{
	// ��ʼ�첽����Զ�̷�����
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
	client_io_callback* callback = new client_io_callback(ctx, stream, id);

	// ������ӳɹ��Ļص�������
	stream->add_open_callback(callback);

	// �������ʧ�ܺ�ص�������
	stream->add_close_callback(callback);

	// ������ӳ�ʱ�Ļص�������
	stream->add_timeout_callback(callback);
	return (true);
}

static void usage(const char* procname)
{
	printf("usage: %s -h[help] -l server_addr -c nconnect"
		" -n io_max -k[use kernel event: epoll/kqueue/devpoll"
		" -t connect_timeout -d[debug]\n", procname);
}

int main(int argc, char* argv[])
{
	bool use_kernel = false;
	int   ch;
	IO_CTX ctx;

	memset(&ctx, 0, sizeof(ctx));
	ctx.connect_timeout = 5;
	ctx.nopen_limit = 10;
	ctx.id_begin = 1;
	ctx.nwrite_limit = 10;
	ctx.debug = false;
	snprintf(ctx.addr, sizeof(ctx.addr), "127.0.0.1:9001");

	while ((ch = getopt(argc, argv, "hc:n:kl:dt:")) > 0)
	{
		switch (ch)
		{
		case 'c':
			ctx.nopen_limit = atoi(optarg);
			if (ctx.nopen_limit <= 0)
				ctx.nopen_limit = 10;
			break;
		case 'n':
			ctx.nwrite_limit = atoi(optarg);
			if (ctx.nwrite_limit <= 0)
				ctx.nwrite_limit = 10;
			break;
		case 'h':
			usage(argv[0]);
			return (0);
		case 'k':
			use_kernel = true;
			break;
		case 'l':
			snprintf(ctx.addr, sizeof(ctx.addr), "%s", optarg);
			break;
		case 'd':
			ctx.debug = true;
			break;
		case 't':
			ctx.connect_timeout = atoi(optarg);
			break;
		default:
			break;
		}
	}

	acl::meter_time(__FUNCTION__, __LINE__, "-----BEGIN-----");
	acl::acl_cpp_init();

	aio_handle handle(use_kernel ? ENGINE_KERNEL : ENGINE_SELECT);
	ctx.handle = &handle;

	if (connect_server(&ctx, ctx.id_begin) == false)
	{
		std::cout << "enter any key to exit." << std::endl;
		getchar();
		return (1);
	}

	std::cout << "Connect " << ctx.addr << " ..." << std::endl;

	while (true)
	{
		// ������� false ���ʾ���ټ�������Ҫ�˳�
		if (handle.check() == false)
			break;
	}

	acl::string buf;

	buf << "total open: " << ctx.nopen_total
		<< ", total write: " << ctx.nwrite_total
		<< ", total read: " << ctx.nread_total;

	acl::meter_time(__FUNCTION__, __LINE__, buf.c_str());

	return (0);
}

