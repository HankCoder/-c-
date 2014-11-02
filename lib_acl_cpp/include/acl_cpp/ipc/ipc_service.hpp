#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/locker.hpp"
#include "acl_cpp/ipc/ipc_server.hpp"

struct acl_pthread_pool_t;

namespace acl {

class ipc_client;

class ACL_CPP_API ipc_request
{
public:
	ipc_request();
	virtual ~ipc_request();

	/**
	 * ��ӿڣ�����ʵ�ִ������ڴ����������񣬸ýӿ�����
	 * �ڲ��� IO ��Ϣ��ģʽ
	 */
	virtual void run(ipc_client* ipc);

#ifdef WIN32
	/**
	 * ��ӿڣ�����ʵ�ִ������ڴ����������񣬸ýӿ�����
	 * �ڲ��� WIN32 ��Ϣ��ģʽ
	 */
	virtual void run(HWND hWnd);

	/**
	 * ���� WIN32 ���ھ��
	 * @param hWnd {HWND} ���ھ��
	 */
	void set_hwnd(HWND hWnd)
	{
		hWnd_ = hWnd;
	}

	/**
	 * ��� WIN32 ���ھ��
	 * @return {HWND} ���ھ��
	 */
	HWND get_hwnd(void) const
	{
		return (hWnd_);
	}
#endif
private:
#ifdef WIN32
	HWND  hWnd_;
#endif
};

class ACL_CPP_API ipc_service : public ipc_server
{
public:
	/**
	 * ���캯��
	 * @param nthread {int} �����ֵ > 1 ���ڲ��Զ������̳߳أ�����
	 *  ����һ������һ���߳�
	 * @param ipc_keep {bool} �ڲ� IPC ��Ϣ���Ƿ񱣳ֳ����ӣ����ֳ�
	 *  ���������������Ϣ���ݵ�Ч��
	 */
	ipc_service(int nthread, bool ipc_keep = true);

	virtual ~ipc_service();

#ifdef WIN32
	/**
	 * ������ WIN32 ��Ϣģʽʱ��������Ҫʵ�ִ���ӿ����ڴ�������
	 * ��Ϣ���̣��������ʵ�ָýӿ�
	 * @param hWnd {HWND} ���ھ��
	 * @param msg {UINT} �û��Զ�����Ϣ��
	 * @param wParam {WPARAM} ����
	 * @param lParam {LPARAM} ����
	 */
	virtual void win32_proc(HWND hWnd, UINT msg,
		WPARAM wParam, LPARAM lParam);
#endif

	/**
	 * �� ipc ��Ϣ�����ӳ���ȡ��һ������
	 * @return {ipc_client*} ���� NULL ��ʾ�޷�������Ϣ������
	 */
	ipc_client* peek_conn();

	/**
	 * ������� ipc ��Ϣ���ӷŻ����ӳ���
	 * @param conn {ipc_client*} ipc ��Ϣ������
	 */
	void push_conn(ipc_client* conn);
protected:
#ifdef WIN32
	__int64   magic_;
#else
	long long int magic_;
#endif

	/**
	 * ������ô˺��������������
	 * @param req {ipc_request*}
	 */
	void request(ipc_request* req);
private:
	bool  ipc_keep_;
	acl_pthread_pool_t* thread_pool_;
#ifdef WIN32
	HWND hWnd_;
	HINSTANCE hInstance_;

	/**
	 * �����麯����Windows ��Ϣ��ʽ�£��������ش��ھ��
	 */
	virtual bool create_window(void);

	/**
	 * �����麯����Windows ��Ϣ��ʽ�£��ر����ش��ھ��
	 */
	virtual void close_window(void);
#endif

	locker lock_;
	std::list<ipc_client*> conn_pool_;
};

}  // namespace acl
