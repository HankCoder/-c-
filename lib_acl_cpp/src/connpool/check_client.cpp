#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#include "acl_cpp/connpool/connect_monitor.hpp"
#include "acl_cpp/connpool/connect_manager.hpp"
#include "acl_cpp/connpool/check_client.hpp"
#include "check_timer.hpp"

namespace acl
{

static double stamp_sub(const struct timeval& from, const struct timeval& sub)
{
	struct timeval res;

	memcpy(&res, &from, sizeof(struct timeval));

	res.tv_usec -= sub.tv_usec;
	if (res.tv_usec < 0) {
		--res.tv_sec;
		res.tv_usec += 1000000;
	}
	res.tv_sec -= sub.tv_sec;

	return res.tv_sec * 1000.0 + res.tv_usec/1000.0;
}

check_client::check_client(check_timer& timer, const char* addr,
	aio_socket_stream& conn, struct timeval& begin)
: blocked_(true)
, aliving_(false)
, timer_(timer)
, conn_(conn)
, addr_(addr)
{
	memcpy(&begin_, &begin, sizeof(begin_));
}

void check_client::set_alive(bool yesno)
{
	aliving_ = yesno;
}

void check_client::set_blocked(bool on)
{
	blocked_ = on;
}

void check_client::close()
{
	conn_.close();
}

bool check_client::open_callback()
{
	set_alive(true);
	timer_.get_monitor().on_open(*this);
	return true;
}

void check_client::close_callback()
{
	struct timeval end;
	gettimeofday(&end, NULL);
	double spent = stamp_sub(end, begin_);

	if (!aliving_)
		logger_warn("server: %s dead, spent: %.2f ms",
			addr_.c_str(), spent);
	//else
	//	logger("server: %s alive, spent: %.2f ms",
	//		addr_.c_str(), spent);

	// ���ڴ˴��ĺô��Ǳ�֤�˵�ǰ���϶����ڹرչ����У�ͬʱ����������
	// �� timer_ ��ɾ���Լ��� timer_ �м�����ĸ���
	timer_.get_monitor().get_manager().set_pools_status(addr_, aliving_);
	timer_.remove_client(addr_, this);

	delete this;
}

bool check_client::timeout_callback()
{
	struct timeval end;
	gettimeofday(&end, NULL);
	double spent = stamp_sub(end, begin_);

	logger_warn("server: %s dead, timeout, spent: %.2f ms",
		addr_.c_str(), spent);

	// ���ӳ�ʱ����ֱ�ӷ���ʧ��
	return false;
}

} // namespace acl
