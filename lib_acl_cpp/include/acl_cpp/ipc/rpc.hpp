#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/ipc/ipc_service.hpp"
#ifdef WIN32
 struct acl_pthread_mutex_t;
 struct acl_pthread_cond_t;
#else
# include <pthread.h>
# ifndef	acl_pthread_mutex_t
#  define	acl_pthread_mutex_t	pthread_mutex_t
# endif
# ifndef	acl_pthread_cond_t
#  define	acl_pthread_cond_t	pthread_cond_t
# endif
#endif

namespace acl {

class ipc_client;
class rpc_client;
class rpc_service;

class rpc_request;
struct RPC_DAT
{
	rpc_request* req;
	void* ctx;
};

class ACL_CPP_API rpc_request : public ipc_request
{
public:
	rpc_request(void);
	virtual ~rpc_request(void);

protected:
	friend class rpc_client;
	friend class rpc_service;

	/**
	 * �����߳��б����ã��������ʵ�ִ˽ӿڣ�
	 * �����̴߳��������������ýӿڽ������ã����Ը������ֻ��
	 * �ǵ����ӿڵ��ú���ܱ��ͷţ���ֹ�ڵ��ñ��ӿ�ǰ�ͷű������
	 */
	virtual void rpc_onover(void) = 0;

	/**
	 * ��ӿڣ������̵߳��ñ������ rpc_signal ʱ�������߳��л�
	 * ���ñ��ӿڣ�֪ͨ������δ���ǰ(������ rpc_onover ǰ)�յ�
	 * ���߳����е��м�״̬��Ϣ���ڲ��Զ�֧���׽ӿڻ� WIN32 ����
	 * ��Ϣ��Ӧ�ó��������磬���� HTTP ����Ӧ�ã������߳��п���
	 * һ�����أ�һ�������̷߳���(���� rpc_signal ����)���ؽ��̣�
	 * �����̻߳���ñ���ʵ���Ĵ˷������������Ϣ
	 */
	virtual void rpc_wakeup(void* ctx) { (void) ctx; }

protected:
	/**
	 * �����߳��б����ã��������ʵ�ִ˽ӿڣ����ڴ����������
	 */
	virtual void rpc_run(void) = 0;

	/**
	 * �����߳��б����ã��ڲ��Զ�֧���׽ӿڻ� WIN32 ������Ϣ
	 * ����ʵ���� rpc_run �����п��Զ�ε��ô˷��������̵߳�
	 * ����ʵ��������Ϣ�����߳��е��ñ����� rpc_wakeup ����
	 * @param ctx {void*} ���ݵĲ���ָ�룬һ��Ӧ���Ƕ�̬��ַ
	 *  �ȽϺã��������Ա���ͬһ���������ظ����ǵ�����
	 */
	void rpc_signal(void* ctx);

	/**
	 * �����̵߳��� rpc_signal �����̺߳󣬵��ñ��������Եȴ�
	 * ���̷߳�����һ��ָ��
	 * @param timeout {int} �ȴ���ʱʱ��(����)������ֵΪ 0 ʱ
	 *  ����÷������ȴ�ģʽ������ֵΪ < 0 ʱ���������ȫ����
	 *  �ȴ�ģʽ(��һֱ�ȵ����̷߳��� cond_signal ֪ͨ)������
	 *  ֵ > 0 ʱ����ȴ������ʱʱ��Ϊ timeout ����
	 * @return {bool} ���� true ��ʾ�յ����̷߳�����֪ͨ�źţ�
	 *  ������Ҫ���� cond_wait_timeout �ж��Ƿ��ǳ�ʱ�����
	 */
	bool cond_wait(int timeout = -1);

	/**
	 * �� cond_wait ���� false ʱ��Ӧ��Ӧ�õ��ñ������ж��Ƿ�
	 * ����Ϊ�ȴ���ʱ�����
	 * @return {bool} �Ƿ��ǵȴ���ʱ
	 */
	bool cond_wait_timeout() const
	{
		return wait_timedout_;
	}

	/**
	 * �����̵߳��� cond_wait ʱ�������߳��е��ñ�����֪ͨ���߳�
	 * ��������
	 * @return {bool} �������̵߳��� cond_wait ʱ������֪ͨ���߳�
	 *  �����������ҷ��� true�����򷵻� false
	 */
	bool cond_signal(void);

private:
	RPC_DAT dat_;
	ipc_client* ipc_;
	int   cond_count_;
	acl_pthread_mutex_t* lock_;
	acl_pthread_cond_t* cond_;
	bool wait_timedout_;

	// ���� ipc_request �麯���������߳��б�����
	virtual void run(ipc_client* ipc);
#ifdef WIN32
	/**
	 * ��ӿڣ�����ʵ�ִ������ڴ����������񣬸ýӿ�����
	 * �ڲ��� WIN32 ��Ϣ��ģʽ
	 * @param hWnd {HWND} WIN2 ���ھ��
	 */
	virtual void run(HWND hWnd);
#endif
};

//////////////////////////////////////////////////////////////////////////

class aio_socket_stream;

class ACL_CPP_API rpc_service : public ipc_service
{
public:
	/**
	 * ���캯��
	 * @param nthread {int} �����ֵ > 1 ���ڲ��Զ������̳߳أ�����
	 *  ����һ������һ���߳�
	 * @param ipc_keep {bool} �ڲ� IPC ��Ϣ���Ƿ񱣳ֳ����ӣ����ֳ�
	 *  ���������������Ϣ���ݵ�Ч��
	 */
	rpc_service(int nthread, bool ipc_keep = true);

	~rpc_service(void) {}

	/**
	 * ���߳������У�����������������̳߳ص�������������У����̳߳�
	 * �е�һ�����߳̽��յ������������ rpc_request::rpc_run ������
	 * ������ķ���������������Ϻ�����̷߳���Ϣ�������߳����ٵ���
	 * rpc_request::rpc_callback
	 * @param req {rpc_request*} rpc_request ����ʵ�����ǿ�
	 */
	void rpc_fork(rpc_request* req);

private:
	// �����麯�������̶߳�����յ����߳���Ϣ��
	// ipc ��������ʱ�Ļص�����
	virtual void on_accept(aio_socket_stream* client);

#ifdef WIN32
	/**
	 * �����麯�������յ����������̵߳� win32 ��Ϣʱ�Ļص�����
	 * @param hWnd {HWND} ���ھ��
	 * @param msg {UINT} �û��Զ�����Ϣ��
	 * @param wParam {WPARAM} ����
	 * @param lParam {LPARAM} ����
	 */
	virtual void win32_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
};

} // namespace acl
