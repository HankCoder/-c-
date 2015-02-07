// http_server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include "rpc_manager.h"
#include "rpc_stats.h"
#include "http_rpc.h"

static int var_data_size = 1024;

//////////////////////////////////////////////////////////////////////////

/**
 * �첽�ͻ������Ļص��������
 */
class handle_io : public aio_callback
{
public:
	handle_io(aio_socket_stream* client)
		: client_(client)
	{
		http_ = new http_rpc(client_, (unsigned) var_data_size);
	}

	~handle_io()
	{
		delete http_;
		printf("delete io_callback now ...\r\n");
	}

	bool write_callback()
	{
		return (true);
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	 */
	void close_callback()
	{
		printf("Closed now.\r\n");

		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
		delete this;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	bool timeout_callback()
	{
		printf("Timeout ...\r\n");
		return (false);
	}

	virtual bool read_wakeup()
	{
		// ����״̬
		rpc_read_wait_del();
		rpc_add();

		// ���첽����������ȥ���Ը��첽���ļ��
		client_->disable_read();

		// ����һ�� http �Ự����
		rpc_manager::get_instance().fork(http_);

		return true;
	}

private:
	aio_socket_stream* client_;
	http_rpc* http_;
};

//////////////////////////////////////////////////////////////////////////

/**
 * �첽�������Ļص��������
 */
class handle_accept : public aio_accept_callback
{
public:
	handle_accept(bool preread)
 	: preread_(preread)
	{
	}

	~handle_accept()
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
		// ������������߳���Ԥ��������������Ԥ����־λ
		if (preread_)
		{
			ACL_VSTREAM* vstream = client->get_vstream();
			vstream->flag |= ACL_VSTREAM_FLAG_PREREAD;
		}

		// �����첽�ͻ������Ļص���������첽�����а�
		handle_io* callback = new handle_io(client);

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
		client->read_wait(10);

		return (true);
	}

private:
	bool preread_;
};

static void usage(const char* procname)
{
	printf("usage: %s \r\n"
		" -h[help]\r\n"
		" -p[preread in main thread]\r\n"
		" -l listen_addr[127.0.0.1:9001]\r\n"
		" -m[use mempool]\r\n"
		" -k[use kernel engine]\r\n"
		" -n data size response\r\n"
		" -N thread pool limit\r\n"
		" -r rpc_addr\r\n"
		" -v[enable stdout]\r\n", procname);
}

int main(int argc, char* argv[])
{
#ifdef WIN32
	acl_cpp_init();
#endif

	bool preread = false;
	char addr[32], rpc_addr[32], ch;
	bool use_mempool = false;
	bool use_kernel = false;
	bool enable_stdout = false;
	int  nthreads = 20;

	acl::safe_snprintf(addr, sizeof(addr), "127.0.0.1:9001");
	acl::safe_snprintf(rpc_addr, sizeof(rpc_addr), "127.0.0.1:0");

	while ((ch = getopt(argc, argv, "vkhpms:n:N:r:")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			acl::safe_snprintf(addr, sizeof(addr), "%s", optarg);
			break;
		case 'p':
			preread = true;
			break;
		case 'm':
			use_mempool = true;
			break;
		case 'k':
			use_kernel = true;
			break;
		case 'n':
			var_data_size = atoi(optarg);
			if (var_data_size <= 0)
				var_data_size = 1024;
			break;
		case 'N':
			nthreads = atoi(optarg);
			if (nthreads <= 0)
				nthreads = 10;
			break;
		case 'v':
			enable_stdout = true;
			break;
		case 'r':
			acl::safe_snprintf(rpc_addr, sizeof(rpc_addr), "%s", optarg);
			break;
		default:
			break;
		}
	}

	// �Ƿ�����ֲ߳̾��ڴ��
	if (use_mempool)
		acl_mem_slice_init(8, 1024, 100000,
			ACL_SLICE_FLAG_GC2 |
			ACL_SLICE_FLAG_RTGC_OFF |
			ACL_SLICE_FLAG_LP64_ALIGN);

	rpc_stats_init();

	// ������־��Ϣ�������Ļ
	if (enable_stdout)
		log::stdout_open(true);

	// �첽ͨ�ſ�ܾ�������� select ϵͳ api
	aio_handle* handle = new aio_handle(use_kernel ? ENGINE_KERNEL : ENGINE_SELECT);

	// ���������첽��
	aio_listen_stream* sstream = new aio_listen_stream(handle);

	// ����ָ���ĵ�ַ
	if (sstream->open(addr) == false)
	{
		printf("open %s error!\r\n", addr);
		sstream->close();
		// XXX: Ϊ�˱�֤�ܹرռ�������Ӧ�ڴ˴��� check һ��
		handle->check();
#ifdef WIN32
		getchar();
#endif
		return 1;
	}

	// ��ʼ���첽 RPC ͨ�ŷ�����
	rpc_manager::get_instance().init(handle, nthreads, rpc_addr);

	// �����ص�����󣬵��������ӵ���ʱ�Զ����ô������Ļص�����
	handle_accept callback(preread);
	// ���ص�������������첽��������
	sstream->add_accept_callback(&callback);

	printf("Listen: %s ok!\r\n", addr);

	time_t last = time(NULL), now;
	while (true)
	{
		// ������� false ���ʾ���ټ�������Ҫ�˳�
		if (handle->check() == false)
		{
			printf("aio_server stop now ...\r\n");
			break;
		}

		time(&now);
		if (now - last >= 1)
		{
			printf("\r\n------------------------------\r\n");
			rpc_out(); // �����ǰ rpc ���е�����
			rpc_req_out();
			rpc_read_wait_out();
			last = now;
		}
	}

	// �رռ��������ͷ�������
	sstream->close();

	// �ر� RPC ����
	rpc_manager::get_instance().finish();

	// XXX: Ϊ�˱�֤�ܹرռ�������Ӧ�ڴ˴��� check һ��
	handle->check();
	delete handle;

	rpc_stats_finish();

	if (use_mempool)
	{
		acl_mem_slice_gc();
		acl_mem_slice_destroy();
	}

	return 0;
}
