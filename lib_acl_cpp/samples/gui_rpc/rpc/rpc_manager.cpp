#include "stdafx.h"
#include "rpc_manager.h"

rpc_manager::rpc_manager(int max_threads /* = 10 */)
{
	// ��Ϊ����ʵ���ǵ��������ڳ��� main ֮ǰ�����ã�
	// ������Ҫ�ڴ����д���־
	logger_open("gui_rpc.log", "gui_rpc");
	// ������������ܾ���������� WIN32 ��Ϣģʽ��acl::ENGINE_WINMSG
	handle_ = new acl::aio_handle(acl::ENGINE_WINMSG);
	// ���� rpc �������
	service_ = new acl::rpc_service(max_threads);
	// ����Ϣ����
	if (service_->open(handle_) == false)
		logger_fatal("open service error: %s", acl::last_serror());
}

rpc_manager::~rpc_manager()
{
	delete service_;
	handle_->check();
	delete handle_;
	logger("rpc service destroy ok!");
}

void rpc_manager::fork(acl::rpc_request* req)
{
	service_->rpc_fork(req);
}
