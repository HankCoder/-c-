#include "acl_stdafx.hpp"
#include <map>
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/ipc/rpc.hpp"
#include "acl_cpp/connpool/connect_manager.hpp"
#include "acl_cpp/connpool/connect_monitor.hpp"
#include "acl_cpp/connpool/check_client.hpp"
#include "check_timer.hpp"
#include "check_rpc.hpp"

namespace acl
{

connect_monitor::connect_monitor(connect_manager& manager)
: stop_(false)
, stop_graceful_(true)
, handle_(ENGINE_KERNEL)
, manager_(manager)
, check_inter_(1)
, conn_timeout_(10)
, rpc_service_(NULL)
{
}

connect_monitor::~connect_monitor()
{
}

connect_monitor& connect_monitor::open_rpc_service(int max_threads,
	const char* addr /* = NULL */)
{
	if (rpc_service_ != NULL)
		return *this;

	rpc_service_ = NEW rpc_service(max_threads);
	if (rpc_service_->open(&handle_, addr) == false)
		logger_fatal("open rpc_service error: %s", last_serror());

	return *this;
}

connect_monitor& connect_monitor::set_check_inter(int n)
{
	check_inter_ = n;
	return *this;
}

connect_monitor& connect_monitor::set_conn_timeout(int n)
{
	conn_timeout_ = n;
	return *this;
}

void connect_monitor::stop(bool graceful)
{
	stop_ = true;
	stop_graceful_ = graceful;
}

void* connect_monitor::run()
{
	// �����������״̬��ʱ��
	check_timer timer(*this, handle_, conn_timeout_);

	timer.keep_timer(true);  // ���ֶ�ʱ��
	handle_.set_timer(&timer, check_inter_ * 1000000);

	while (!stop_)
		handle_.check();

	// �ȴ���ʱ������
	while (!timer.finish(stop_graceful_))
		handle_.check();

	// ��� rpc_service_ ����ǿ���ɾ��֮
	delete rpc_service_;

	// ����ټ��һ�Σ��Ծ����ͷſ��ܴ��ڵ��첽����
	handle_.check();

	return NULL;
}

void connect_monitor::on_open(check_client& checker)
{
	// ���δ���� rpc �������������첽 IO ������
	if (rpc_service_ == NULL)
	{
		checker.set_blocked(false);
		nio_check(checker, checker.get_conn());
	}
	else
	{
		// ���ü�����Ϊ����ģʽ
		checker.set_blocked(true);

		// ���� rpc ������󣬽�������̳߳������У��������� IO ����
		check_rpc* req = new check_rpc(*this, checker);
		rpc_service_->rpc_fork(req);
	}
}

void connect_monitor::nio_check(check_client& checker, aio_socket_stream&)
{
	// ����״̬�����������Ǵ���
	checker.set_alive(true);

	// �첽�ر����Ӽ�����
	checker.close();
}

void connect_monitor::sio_check(check_client& checker, socket_stream&)
{
	checker.set_alive(true);
}

} // namespace acl
