#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/ipc/rpc.hpp"

namespace acl
{

class connect_monitor;
class check_client;

class check_rpc : public rpc_request
{
public:
	check_rpc(connect_monitor& monitor, check_client& checker);

protected:
	// ���̴߳�����
	virtual void rpc_run();

	// ���̴߳�����̣��յ����߳�������ɵ���Ϣ
	virtual void rpc_onover();

private:
	connect_monitor& monitor_;
	check_client& checker_;

	~check_rpc();
};

} // namespace acl
