#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/master/master_udp.hpp"

namespace acl
{

static master_udp* __mu = NULL;

master_udp::master_udp(void)
{
	// ȫ�־�̬����
	acl_assert(__mu == NULL);
	__mu = this;
}

master_udp::~master_udp(void)
{
	close_sstreams();
}

void master_udp::close_sstreams(void)
{
	std::vector<socket_stream*>::iterator it = sstreams_.begin();
	for (; it != sstreams_.end(); ++it)
	{
		// ���� daemon ����ģʽ��socket_stream �е� ACL_VSTREAM
		// ������ lib_acl ���е� acl_udp_server �ڲ��رգ�����
		// �˴���Ҫ�� ACL_VSTREAM ������ socket_stream ����
		if (daemon_mode_)
			(*it)->unbind();
		delete *it;
	}

	// ������������ϣ���Ϊ�ú���������ʱ����������һ��
	sstreams_.clear();
}

static bool has_called = false;

void master_udp::run_daemon(int argc, char** argv)
{
#ifndef WIN32
	// ÿ������ֻ����һ��ʵ��������
	acl_assert(has_called == false);
	has_called = true;
	daemon_mode_ = true;

	// ���� acl ����������� UDP ������ģ��ӿ�
	acl_udp_server_main(argc, argv, service_main,
		ACL_MASTER_SERVER_PRE_INIT, service_pre_jail,
		ACL_MASTER_SERVER_POST_INIT, service_init,
		ACL_MASTER_SERVER_EXIT, service_exit,
		ACL_MASTER_SERVER_INT_TABLE, conf_.get_int_cfg(),
		ACL_MASTER_SERVER_STR_TABLE, conf_.get_str_cfg(),
		ACL_MASTER_SERVER_BOOL_TABLE, conf_.get_bool_cfg(),
		ACL_MASTER_SERVER_INT64_TABLE, conf_.get_int64_cfg(),
		0);
#endif
}

//////////////////////////////////////////////////////////////////////////

static int  __count_limit = 1;
static int  __count = 0;
static bool __stop = false;

void master_udp::read_callback(int, ACL_EVENT*, ACL_VSTREAM *sstream, void*)
{
	service_main(sstream, NULL, NULL);
	__count++;
	if (__count_limit > 0 && __count >= __count_limit)
		__stop = true;
}

bool master_udp::run_alone(const char* addrs, const char* path /* = NULL */,
	unsigned int count /* = 1 */)
{
	// ÿ������ֻ����һ��ʵ��������
	acl_assert(has_called == false);
	has_called = true;
	daemon_mode_ = false;
	__count_limit = count;
	acl_assert(addrs && *addrs);

#ifdef WIN32
	acl_init();
#endif
	ACL_EVENT* eventp = acl_event_new_select(1, 0);
	set_event(eventp);  // ���û�����¼����

	ACL_ARGV* tokens = acl_argv_split(addrs, ";,| \t");
	ACL_ITER iter;

	acl_foreach(iter, tokens)
	{
		const char* addr = (const char*) iter.data;
		ACL_VSTREAM* sstream = acl_vstream_bind(addr, 0);
		if (sstream == NULL)
		{
			logger_error("bind %s error %s",
				addr, last_serror());
			close_sstreams();
			acl_event_free(eventp);
			acl_argv_free(tokens);
			return false;
		}
		acl_event_enable_read(eventp, sstream, 0,
			read_callback, sstream);
		socket_stream* ss = NEW socket_stream();
		if (ss->open(sstream) == false)
			logger_fatal("open stream error!");
		sstream->context = ss;
		sstreams_.push_back(ss);
	}

	acl_argv_free(tokens);

	// ��ʼ�����ò���
	conf_.load(path);

	service_pre_jail(NULL, NULL);
	service_init(NULL, NULL);

	while (!__stop)
		acl_event_loop(eventp);

	service_exit(NULL, NULL);

	// �����ڵ��� acl_event_free ǰ���� close_sstreams����Ϊ�ڹر�
	// ����������ʱ��Ȼ�ж� ACL_EVENT �����ʹ��
	close_sstreams();
	acl_event_free(eventp);
	return true;
}

//////////////////////////////////////////////////////////////////////////

static void on_close(ACL_VSTREAM* stream, void* ctx)
{
	if (ctx && stream->context == ctx)
	{
		socket_stream* ss = (socket_stream*) ctx;
		delete ss;
	}
}

void master_udp::service_main(ACL_VSTREAM *stream, char*, char**)
{
	acl_assert(__mu != NULL);

	socket_stream* ss = (socket_stream*) stream->context;
	if (ss == NULL)
	{
		// ����������һ�α�����ʱ����Ҫ�� socket_stream ��
		ss = NEW socket_stream();
		if (ss->open(stream) == false)
			logger_fatal("open stream error!");
		stream->context = ss;
		acl_vstream_add_close_handle(stream, on_close, ss);
	}

#ifndef	WIN32
	if (__mu->daemon_mode_)
		acl_watchdog_pat();  // ����֪ͨ acl_master ���һ��
#endif
	__mu->on_read(ss);
}

void master_udp::service_pre_jail(char*, char**)
{
	acl_assert(__mu != NULL);

#ifndef WIN32
	ACL_EVENT* eventp = acl_udp_server_event();
	__mu->set_event(eventp);
#endif

	__mu->proc_pre_jail();
}

void master_udp::service_init(char*, char**)
{
	acl_assert(__mu != NULL);

#ifndef	WIN32
	if (__mu->daemon_mode_)
	{
		ACL_VSTREAM** streams = acl_udp_server_streams();
		for (int i = 0; streams[i] != NULL; i++)
		{
			socket_stream* ss = NEW socket_stream();
			if (ss->open(streams[i]) == false)
				logger_fatal("open stream error!");
			__mu->sstreams_.push_back(ss);
		}
	}
#endif

	__mu->proc_inited_ = true;
	__mu->proc_on_init();
}

void master_udp::service_exit(char*, char**)
{
	acl_assert(__mu != NULL);
	__mu->proc_on_exit();
}

}  // namespace acl
