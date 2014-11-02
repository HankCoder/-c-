#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/master/master_trigger.hpp"

namespace acl
{

static master_trigger* __mt = NULL;

master_trigger::master_trigger()
{
	acl_assert(__mt == NULL);
	__mt = this;
}

master_trigger::~master_trigger()
{

}

static bool has_called = false;

void master_trigger::run_daemon(int argc, char** argv)
{
#ifdef WIN32
	logger_fatal("not support WIN32!");
#else
	// ÿ������ֻ����һ��ʵ��������
	acl_assert(has_called == false);
	has_called = true;
	daemon_mode_ = true;

	acl_trigger_server_main(argc, argv, service_main,
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

void master_trigger::run_alone(const char* path /* = NULL */,
	int count /* = 1 */, int interval /* = 1 */)
{
	// ÿ������ֻ����һ��ʵ��������
	acl_assert(has_called == false);
	has_called = true;
	daemon_mode_ = false;
#ifdef WIN32
	acl_init();
#endif
	if (interval <= 0)
		interval = 1;

	// ��ʼ�����ò���
	conf_.load(path);

	service_pre_jail(NULL, NULL);
	service_init(NULL, NULL);

	int   i = 0;
	while (true)
	{
		sleep(interval);
		service_main(NULL, 0, NULL, NULL);
		if (count > 0 && ++i >= count)
			break;
	}

	service_exit(NULL, NULL);
}

//////////////////////////////////////////////////////////////////////////

void master_trigger::service_main(char*, int, char*, char**)
{
	acl_assert(__mt != NULL);
#ifndef	WIN32
	if (__mt->daemon_mode_)
		acl_watchdog_pat();
#endif
	__mt->on_trigger();
}

void master_trigger::service_pre_jail(char*, char**)
{
	acl_assert(__mt != NULL);

#ifndef WIN32
	if (__mt->daemon_mode())
	{
		ACL_EVENT* eventp = acl_trigger_server_event();
		__mt->set_event(eventp);  // ���û�����¼�������
	}
#endif

	__mt->proc_pre_jail();
}

void master_trigger::service_init(char*, char**)
{
	acl_assert(__mt != NULL);

	__mt->proc_inited_ = true;
	__mt->proc_on_init();
}

void master_trigger::service_exit(char*, char**)
{
	acl_assert(__mt != NULL);
	__mt->proc_on_exit();
}

}  // namespace acl
