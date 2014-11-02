#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#include "acl_cpp/master/master_aio.hpp"

namespace acl
{

static master_aio* __ma = NULL;
static aio_handle* __handle = NULL;

master_aio::master_aio()
{
	// ȫ�־�̬����
	acl_assert(__ma == NULL);
	__ma = this;
}

master_aio::~master_aio()
{
	if (daemon_mode_ == false)
		delete __handle;
	__handle = NULL;
}

aio_handle* master_aio::get_handle() const
{
	acl_assert(__handle);
	return __handle;
}

static bool has_called = false;

void master_aio::run_daemon(int argc, char** argv)
{
#ifndef WIN32
	// ÿ������ֻ����һ��ʵ��������
	acl_assert(has_called == false);
	has_called = true;
	daemon_mode_ = true;

	// ���� acl ��������ܵĵ��̷߳�����ģ��
	acl_aio_server2_main(argc, argv, service_main,
		ACL_MASTER_SERVER_PRE_INIT, service_pre_jail,
		ACL_MASTER_SERVER_POST_INIT, service_init,
		ACL_MASTER_SERVER_EXIT, service_exit,
		ACL_MASTER_SERVER_BOOL_TABLE, conf_.get_bool_cfg(),
		ACL_MASTER_SERVER_INT64_TABLE, conf_.get_int64_cfg(),
		ACL_MASTER_SERVER_INT_TABLE, conf_.get_int_cfg(),
		ACL_MASTER_SERVER_STR_TABLE, conf_.get_str_cfg(),
		ACL_MASTER_SERVER_END);
#else
	logger_fatal("no support win32 yet!");
#endif
}

//////////////////////////////////////////////////////////////////////////

static void close_all_listener(std::vector<aio_listen_stream*>& sstreams)
{
	std::vector<aio_listen_stream*>::iterator it = sstreams.begin();
	for (; it != sstreams.end(); ++it)
		(*it)->close();
}

bool master_aio::run_alone(const char* addrs, const char* path /* = NULL */,
	aio_handle_type ht /* = ENGINE_SELECT */)
{
	acl_assert(__handle == NULL);
	daemon_mode_ = false;
#ifdef WIN32
	acl_init();
#endif
	std::vector<aio_listen_stream*> sstreams;
	ACL_ARGV* tokens = acl_argv_split(addrs, ";,| \t");
	ACL_ITER iter;

	// ��ʼ�����ò���
	conf_.load(path);

	__handle = NEW aio_handle(ht);

	ACL_AIO* aio = __handle->get_handle();
	acl_assert(aio);
	ACL_EVENT* eventp = acl_aio_event(aio);
	set_event(eventp);  // ���û�����¼����

	acl_foreach(iter, tokens)
	{
		const char* addr = (const char*) iter.data;
		aio_listen_stream* sstream = NEW aio_listen_stream(__handle);
		// ����ָ���ĵ�ַ
		if (sstream->open(addr) == false)
		{
			logger_error("listen %s error: %s", addr, last_serror());
			close_all_listener(sstreams);
			// XXX: Ϊ�˱�֤�ܹرռ�������Ӧ�ڴ˴��� check һ��
			__handle->check();
			acl_argv_free(tokens);
			return (false);
		}
		sstream->add_accept_callback(this);
	}
	acl_argv_free(tokens);

	service_pre_jail(NULL);
	service_init(NULL);
	while (true)
	{
		// ������� false ���ʾ���ټ�������Ҫ�˳�
		if (__handle->check() == false)
		{
			logger("aio_server stop now ...");
			break;
		}
	}
	close_all_listener(sstreams);
	__handle->check();
	service_exit(NULL);
	return true;
}

void master_aio::stop()
{
	acl_assert(__handle);
	__handle->stop();
}

bool master_aio::accept_callback(aio_socket_stream* client)
{
	return on_accept(client);
}

//////////////////////////////////////////////////////////////////////////

class aio_close_callback : public aio_callback
{
public:
	aio_close_callback(aio_socket_stream* ss)
	{
		stream_ = ss->get_astream();
	}

	~aio_close_callback() {}

protected:
	void close_callback()
	{
#ifndef WIN32
		// ͨ���������֪ͨ���������Ŀǰ�Ѿ���������Ӹ���������
		// ��������ܰ�פ������
		acl_aio_server_on_close(stream_);
#endif // !WIN32
		delete this;
	}

private:
	ACL_ASTREAM *stream_;
};

//////////////////////////////////////////////////////////////////////////

void master_aio::service_pre_jail(void*)
{
	acl_assert(__ma != NULL);

#ifndef WIN32
	if (__ma->daemon_mode_)
	{
		acl_assert(__handle == NULL);

		ACL_EVENT* eventp = acl_aio_server_event();
		__ma->set_event(eventp);

		ACL_AIO *aio = acl_aio_server_handle();
		acl_assert(aio);
		__handle = NEW aio_handle(aio);
	}
#endif

	__ma->proc_pre_jail();
}

void master_aio::service_init(void* ctx acl_unused)
{
	acl_assert(__ma != NULL);

	__ma->proc_inited_ = true;
	__ma->proc_on_init();
}

void master_aio::service_exit(void* ctx acl_unused)
{
	acl_assert(__ma != NULL);
	__ma->proc_on_exit();
}

void master_aio::service_main(ACL_SOCKET fd, void*)
{
	acl_assert(__handle);
	acl_assert(__ma);

	aio_socket_stream* stream = NEW aio_socket_stream(__handle, fd);

	aio_close_callback* callback = NEW aio_close_callback(stream);
	stream->add_close_callback(callback);

	__ma->on_accept(stream);
}

}  // namespace acl
