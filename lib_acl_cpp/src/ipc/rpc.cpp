#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/ipc/ipc_client.hpp"
#include "acl_cpp/ipc/rpc.hpp"

namespace acl
{

// ������Ϣ���Ͷ���
enum
{
	RPC_MSG,
	RPC_SIG
};

#ifdef WIN32
#define RPC_WIN32_MSG	(WM_USER + 100)
#define RPC_WIN32_SIG	(WM_USER + 101)
#endif

// IPC ͨ��ʱ�����߳�ͨ�����ʹ�����֪ͨ���߳���������
/*
struct IPC_DAT 
{
	rpc_request* req;
	void* ctx;
};
*/

rpc_request::rpc_request()
: ipc_(NULL)
, wait_timedout_(false)
{
	dat_.req = this;
	dat_.ctx = NULL;

	cond_count_ = 0;
	cond_ = (acl_pthread_cond_t*) acl_mycalloc(
		1, sizeof(acl_pthread_cond_t));
	acl_pthread_cond_init(cond_, NULL);

	lock_ = (acl_pthread_mutex_t*) acl_mycalloc(
		1, sizeof(acl_pthread_mutex_t));
	acl_pthread_mutex_init(lock_, NULL);
}

rpc_request::~rpc_request()
{
	acl_pthread_mutex_destroy(lock_);
	acl_myfree(lock_);

	acl_pthread_cond_destroy(cond_);
	acl_myfree(cond_);
}

// �ú��������߳��б�����
void rpc_request::run(ipc_client* ipc)
{
	ipc_ = ipc;
	rpc_run();
	/*
	IPC_DAT data;
	data.req = this;
	data.ctx = NULL;
	// �����̷߳��ͽ��
	ipc->send_message(RPC_MSG, &data, sizeof(data));
	*/
	dat_.ctx = NULL;

	// �����̷߳��ͽ��
	ipc->send_message(RPC_MSG, &dat_, sizeof(RPC_DAT));
}

// �ú��������߳��б�����
#ifdef WIN32
void rpc_request::run(HWND hWnd)
{
	rpc_run();
	// �򴰿ھ������Ϣ������ǰ����ĵ�ַ�������߳�
	::PostMessage(hWnd, RPC_WIN32_MSG, 0, (LPARAM) this);
}
#endif

// �ú��������߳��б�����
void rpc_request::rpc_signal(void* ctx)
{
#ifdef WIN32
	HWND hWnd = get_hwnd();
	if (hWnd != NULL)
	{
		// �򴰿ھ������Ϣ������ǰ����ĵ�ַ�������߳�
		::PostMessage(hWnd, RPC_WIN32_SIG, (WPARAM) ctx, (LPARAM) this);
		return;
	}
#endif
	acl_assert(ipc_ != NULL);
	/*
	IPC_DAT data;
	data.req = this;
	data.ctx = ctx;
	// �����̷߳��ͽ��
	ipc_->send_message(RPC_SIG, &data, sizeof(data));
	*/
	dat_.ctx = ctx;
	// �����̷߳��ͽ��
	ipc_->send_message(RPC_SIG, &dat_, sizeof(RPC_DAT));
}

bool rpc_request::cond_wait(int timeout /* = -1 */)
{
	int status;

	status = acl_pthread_mutex_lock(lock_);
	if (status != 0)
	{
		logger_error("pthread_mutex_lock error: %d", status);
		return false;
	}

	if (--cond_count_ >= 0)
	{
		status = acl_pthread_mutex_unlock(lock_);
		if (status != 0)
		{
			logger_error("pthread_mutex_unlock error: %d", status);
			return false;
		}
		return true;
	}

	if (timeout < 0)
	{
		status = acl_pthread_cond_wait(cond_, lock_);
		if (status != 0)
		{
			logger_error("pthread_cond_wait error: %d", status);
			status = acl_pthread_mutex_unlock(lock_);
			if (status != 0)
				logger_error("pthread_mutex_unlock error: %d", status);
			return false;
		}
		status = acl_pthread_mutex_unlock(lock_);
		if (status != 0)
		{
			logger_error("pthread_mutex_unlock error: %d", status);
			return false;
		}
		return true;
	}

	struct  timeval   tv;
	struct	timespec  when_ttl;
	gettimeofday(&tv, NULL);
	when_ttl.tv_sec = tv.tv_sec + timeout / 1000;
	when_ttl.tv_nsec = tv.tv_usec * 1000 + (timeout % 1000) * 1000;
	wait_timedout_ = false;

	status = acl_pthread_cond_timedwait(cond_, lock_, &when_ttl);
	if (status != 0)
	{
		if (status == ACL_ETIMEDOUT)
			wait_timedout_ = true;
		else
			logger_error("pthread_cond_timedwait error: %d", status);
		status = acl_pthread_mutex_unlock(lock_);
		if (status != 0)
			logger_error("pthread_mutex_unlock error: %d", status);
		return false;
	}
	else
	{
		status = acl_pthread_mutex_unlock(lock_);
		if (status != 0)
		{
			logger_error("pthread_mutex_unlock error: %d", status);
			return false;
		}
		return true;
	}
}

bool rpc_request::cond_signal()
{
	int status;

	status = acl_pthread_mutex_lock(lock_);
	if (status != 0)
	{
		logger_error("pthread_mutex_lock error: %d", status);
		return false;
	}

	cond_count_++;

	status = acl_pthread_cond_signal(cond_);
	if (status != 0)
	{
		(void) acl_pthread_mutex_unlock(lock_);
		logger_error("pthread_cond_signal error: %d", status);
		return false;
	}

	status = acl_pthread_mutex_unlock(lock_);
	if (status != 0)
	{
		logger_error("pthread_mutex_unlock error: %d", status);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

// ����ʵ�������߳�������

class rpc_client : public ipc_client
{
public:
	rpc_client(acl_int64 magic) : ipc_client(magic) {}
	~rpc_client(void) {}

protected:
	/**
	 * ������ӿڣ����յ���Ϣʱ�Ļص�����
	 * @param nMsg {int} �û���ӵ��Զ�����Ϣֵ
	 * @param data {void*} ��Ϣ����
	 * @param dlen {int} ��Ϣ���ݵĳ���
	 */
	virtual void on_message(int nMsg, void* data, int dlen)
	{
		acl_assert(data && dlen == sizeof(RPC_DAT));
		RPC_DAT* dat = (RPC_DAT*) data;
		acl_assert(dat->req);

		if (nMsg == RPC_MSG)
			dat->req->rpc_onover();
		else if (nMsg == RPC_SIG)
			dat->req->rpc_wakeup(dat->ctx);
	}

	// ������ӿ�
	virtual void on_close(void)
	{
		delete this;
	}
private:
};

#ifdef WIN32
#include <process.h>
#endif

rpc_service::rpc_service(int nthread, bool ipc_keep /* = true */)
: ipc_service(nthread, ipc_keep)
{
#ifdef WIN32
	magic_ = _getpid() + time(NULL);
#else
	magic_ = getpid() + time(NULL);
#endif
}

void rpc_service::on_accept(aio_socket_stream* client)
{
	// �����������������߳���Ϣ�� IPC ���Ӷ���
	ipc_client* ipc = new rpc_client(magic_);
	ipc->open(client);

	// �����Ϣ�ص�����
	ipc->append_message(RPC_MSG);
	ipc->append_message(RPC_SIG);
	ipc->wait();
}

#ifdef WIN32
void rpc_service::win32_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == RPC_WIN32_MSG)
	{
		rpc_request* req = (rpc_request*) lParam;
		acl_assert(req);
		req->rpc_onover();
	}
	else if (msg == RPC_WIN32_SIG)
	{
		rpc_request* req = (rpc_request*) lParam;
		acl_assert(req);
		void* ctx = (void*) wParam;
		req->rpc_wakeup(ctx);
	}
}
#endif

void rpc_service::rpc_fork(rpc_request* req)
{
	request(req);
}

} // namespace acl
