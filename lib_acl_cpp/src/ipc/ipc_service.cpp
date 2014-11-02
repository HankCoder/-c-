#include "acl_stdafx.hpp"
#include <assert.h>
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/ipc/ipc_client.hpp"
#include "acl_cpp/ipc/ipc_server.hpp"
#include "acl_cpp/ipc/ipc_service.hpp"

namespace acl
{

//////////////////////////////////////////////////////////////////////////

ipc_request::ipc_request()
{
#ifdef WIN32
	hWnd_ = NULL;
#endif
}

ipc_request::~ipc_request()
{

}

void ipc_request::run(ipc_client* ipc acl_unused)
{
	logger_fatal("ipc_request::run(ipc_client*) be called now");
}

#ifdef WIN32
void ipc_request::run(HWND hWnd acl_unused)
{
	logger_fatal("ipc_request::run(HWND) be called now");
}
#endif
//////////////////////////////////////////////////////////////////////////

struct REQ_CTX
{
	acl_int64 magic;
	char  addr[256];
	ipc_request* req;
	ipc_service* service;
};

static void thread_pool_main(REQ_CTX* ctx)
{

#ifdef WIN32
	HWND hWnd = ctx->req->get_hwnd();
	if (hWnd != NULL)
		ctx->req->run(hWnd);
	else
#endif
	if (ctx->service)
	{
		ipc_client* ipc = ctx->service->peek_conn();
		if (ipc == NULL)
		{
			logger_error("peek connect to %s error: %s",
				ctx->addr, last_serror());
		}
		else
		{
			ctx->req->run(ipc);

			// �������������Ȼ��������������ӳ���
			if (ipc->active())
				ctx->service->push_conn(ipc);

			// �������ͷŶ�̬����
			else
				delete ipc;
		}
	}
	else
	{
		// IO ��Ϣģʽ

		ipc_client* ipc = NEW ipc_client(ctx->magic);

		// ������Ϣ������, ����ͬ��IPCͨ����ʽ
		if (ipc->open(ctx->addr, 0) == false)
			logger_error("open %s error(%s)",
				ctx->addr, last_serror());

		// ��������������������
		else
			ctx->req->run(ipc);

		// ���� IPC ��
		delete ipc;
	}

	// �ͷ������߳��з���Ķ���
	acl_myfree(ctx);
}

static void* thread_once_main(REQ_CTX* ctx)
{
#ifdef WIN32
	HWND hWnd = ctx->req->get_hwnd();
	if (hWnd != NULL)
		ctx->req->run(hWnd);
	else
#endif
	if (ctx->service)
	{
		ipc_client* ipc = ctx->service->peek_conn();
		if (ipc == NULL)
		{
			logger_error("peek connect to %s error: %s",
				ctx->addr, last_serror());
		}
		else
		{
			ctx->req->run(ipc);

			// �������������Ȼ��������������ӳ���
			if (ipc->active())
				ctx->service->push_conn(ipc);

			// �������ͷŶ�̬����
			else
				delete ipc;
		}
	}
	else
	{
		// IO ��Ϣģʽ

		ipc_client* ipc = NEW ipc_client(ctx->magic);

		// ������Ϣ������
		if (ipc->open(ctx->addr, 0) == false)
			logger_error("open %s error(%s)",
				ctx->addr, acl_last_serror());

		// ��������������������
		else
			ctx->req->run(ipc);
		// ���� IPC ��
		delete ipc;
	}

	// �ͷ������߳��з���Ķ���
	acl_myfree(ctx);
	return (NULL);
}

///////////////////////////////////////////////////////////////////////////

#ifdef WIN32

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	ipc_service* service = (ipc_service*)
		GetWindowLongPtr(hWnd, GWL_USERDATA);
	if (service == NULL)
		return (DefWindowProc(hWnd, msg, wParam, lParam));

	// �����������Ϣ������̣���Ϣ�ű����� >= WM_USER
	if (msg >= WM_USER)
		service->win32_proc(hWnd, msg, wParam, lParam);
	return (DefWindowProc(hWnd, msg, wParam, lParam));
}

static BOOL InitApplication(const char *class_name, HINSTANCE hInstance)
{
	const char *myname = "InitApplication";
	WNDCLASSEX wcx;

	if (GetClassInfoEx(hInstance, class_name, &wcx))
	{
		// class already registered
		logger_warn("class(%s) already registered", class_name);
		return TRUE;
	}

	// Fill in the window class structure with parameters
	// that describe the main window.

	memset(&wcx, 0, sizeof(wcx));

	wcx.cbSize = sizeof(wcx);          // size of structure
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = WndProc;         // points to window procedure
	wcx.cbClsExtra = 0;                // no extra class memory
	wcx.cbWndExtra = 0;                // no extra window memory
	wcx.hInstance = hInstance;         // handle to instance

	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);    // predefined app. icon
	wcx.hCursor = (HCURSOR) LoadCursor(NULL, IDC_ARROW);      // predefined arrow
	wcx.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); // white background brush
	wcx.lpszMenuName =  NULL;          // name of menu resource
	wcx.lpszClassName = class_name;       // name of window class
	wcx.hIconSm = (HICON) LoadImage(hInstance, // small class icon
		MAKEINTRESOURCE(5),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		(UINT) LR_DEFAULTCOLOR);
		// Register the window class. 
	if (RegisterClassEx(&wcx) == 0) {
		logger_error("RegisterClassEx error(%d, %s)",
			acl_last_error(), acl_last_serror());
		return (FALSE);
	} else
		return (TRUE);
}

static HWND InitInstance(const char *class_name, HINSTANCE hInstance)
{
	const char *myname = "InitInstance";
	HWND hWnd;
	CREATESTRUCT cs;

	cs.dwExStyle = 0;
	cs.lpszClass = class_name;
	cs.lpszName = "WIN32 IPC Notification";
	cs.style = WS_OVERLAPPED;
	cs.x = 0;
	cs.y = 0;
	cs.cx = 0;
	cs.cy = 0;
	cs.hwndParent = NULL;
	cs.hMenu = NULL;
	cs.hInstance = hInstance;
	cs.lpCreateParams = NULL;
	hWnd = CreateWindowEx(cs.dwExStyle, cs.lpszClass,
		cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy,
		cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);
	if (hWnd == NULL)
		logger_fatal("create window error: %s", acl_last_serror());
		return (hWnd);
}

static const char *__class_name = "__IpcEventsMainWClass";

bool ipc_service::create_window(void)
{
	hInstance_ = GetModuleHandle(NULL);
	if (InitApplication(__class_name, hInstance_) == FALSE)
		logger_fatal("InitApplication %s error(%s)",
			__class_name, acl_last_serror());
	hWnd_ = InitInstance(__class_name, hInstance_);
	if (hWnd_ == NULL)
		logger_fatal("create %s window error(%s)",
			__class_name, acl_last_serror());

	// ��Ӵ��ھ���Ĺ�������
	SetWindowLongPtr(hWnd_, GWL_USERDATA, (LONG) this);

	// �������ദ�����
	on_open("win32 gui message");
	return true;
}

void ipc_service::close_window(void)
{
	if (hWnd_ == NULL)
		return;
	WNDCLASSEX wcx;
	DestroyWindow(hWnd_);
	if (__class_name && GetClassInfoEx(hInstance_, __class_name, &wcx))
	{
		logger("unregister ipc_service class: %s", __class_name);
		UnregisterClass(__class_name, hInstance_);
	}
}

#endif

ipc_service::ipc_service(int nthread, bool ipc_keep /* true */)
: magic_(-1)
{
#ifdef WIN32
	hWnd_ = NULL;
#endif
	ipc_keep_ = ipc_keep;
	if (nthread > 1)
		thread_pool_ = acl_thread_pool_create(nthread, 30);
	else
		thread_pool_ = NULL;
}

ipc_service::~ipc_service()
{
#ifdef WIN32
	if (hWnd_ != NULL)
		close_window();
#endif
	if (thread_pool_)
		acl_pthread_pool_destroy(thread_pool_);
	std::list<ipc_client*>::iterator it = conn_pool_.begin();
	for (; it != conn_pool_.end(); ++it)
		delete (*it);
	logger("delete service ipc_service");
}

#ifdef WIN32
void ipc_service::win32_proc(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	// �������ʵ�ָýӿ�
	logger_fatal("ipc_service::win32_proc be called");
}
#endif

void ipc_service::request(ipc_request* req)
{
	REQ_CTX* req_ctx = (REQ_CTX*) acl_mycalloc(1, sizeof(REQ_CTX));
#ifdef WIN32
	if (hWnd_ != NULL)
		req->set_hwnd(hWnd_);
	else
		ACL_SAFE_STRNCPY(req_ctx->addr,
			get_addr(), sizeof(req_ctx->addr));
#else
	ACL_SAFE_STRNCPY(req_ctx->addr, get_addr(), sizeof(req_ctx->addr));
#endif
	req_ctx->magic = magic_;
	req_ctx->req = req;
	if (ipc_keep_)
		req_ctx->service = this;
	else
		req_ctx->service = NULL;

	if (thread_pool_)
		acl_pthread_pool_add(thread_pool_, (void (*)(void*))
			thread_pool_main, req_ctx);
	else
	{
		acl_pthread_t tid;
		acl_pthread_attr_t attr;

		acl_pthread_attr_init(&attr);
		acl_pthread_attr_setdetachstate(&attr, 1);
		acl_pthread_create(&tid, &attr, (void* (*)(void*))
			thread_once_main, req_ctx);
	}
}

ipc_client* ipc_service::peek_conn()
{
	ipc_client* ipc;

	// �ȴ����ӳ��в����Ƿ��п��õ�����

	lock_.lock();

	std::list<ipc_client*>::iterator it = conn_pool_.begin();
	if (it != conn_pool_.end())
	{
		ipc = *it;
		conn_pool_.pop_front();
	}
	else
		ipc = NULL;

	lock_.unlock();

	if (ipc)
		return ipc;

	// �����µ� IO ��Ϣ��

	ipc = NEW ipc_client(magic_);

	const char* addr = get_addr();
	// ������Ϣ������, ����ͬ��IPCͨ����ʽ
	if (ipc->open(addr, 0) == false)
	{
		logger_error("open %s error(%s)", addr, acl_last_serror());
		delete ipc;
		return NULL;
	}
	else
		return ipc;
}

void ipc_service::push_conn(ipc_client* conn)
{
	lock_.lock();
	conn_pool_.push_back(conn);
	lock_.unlock();
}

}
