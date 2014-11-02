#include "stdafx.h"
#include "rpc_manager.h"

rpc_manager::rpc_manager()
: handle_(NULL)
{

}

rpc_manager::~rpc_manager()
{
	delete service_;
	if (handle_ != NULL)
	{
		handle_->check();
		if (internal_handle_)
			delete handle_;
	}
	logger("rpc service destroy ok!");
}

void rpc_manager::init(acl::aio_handle* handle, int max_threads /* = 10 */,
	acl::aio_handle_type type /* = acl::ENGINE_SELECT */,
	const char* addr /* = NULL */)
{
	// ��Ϊ����ʵ���ǵ��������ڳ��� main ֮ǰ�����ã�
	// ������Ҫ�ڴ����д���־
	// ������������ܾ��
	if (handle == NULL)
	{
		logger("create new handle, max_threads: %d", max_threads);
		handle_ = new acl::aio_handle(type);
		internal_handle_ = true;
	}
	else
	{
		logger("use input handle, max_threads: %d", max_threads);
		handle_ = handle;
		internal_handle_ = false;
	}
	// ���� rpc �������
	service_ = new acl::rpc_service(max_threads);
	// ����Ϣ����
	if (service_->open(handle_, addr && *addr ? addr : NULL) == false)
		logger_fatal("open service error: %s", acl::last_serror());
	else
		logger("open service ok, listening: %s!", service_->get_addr());
}

void rpc_manager::fork(acl::rpc_request* req)
{
	service_->rpc_fork(req);
}
