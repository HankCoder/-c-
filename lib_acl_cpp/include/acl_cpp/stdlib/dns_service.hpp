#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#include "acl_cpp/ipc/ipc_service.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_delay_free.hpp"

namespace acl
{

class ACL_CPP_API dns_res
{
public:
	dns_res(const char* domain) : domain_(domain) {}
	~dns_res() { ips_.clear(); }

	string domain_;
	std::list<string> ips_;
protected:
private:
};

class ACL_CPP_API dns_result_callback
{
public:
	dns_result_callback(const char* domain) : domain_(domain) {}

	/**
	 * ����������ϻ����ʱ���ڲ�������̻��Զ����� destroy �ӿڣ�
	 * ��������ڸýӿ��ڽ���һЩ�ͷŹ��̣����䵱�ö����Ƕ�̬����ʱ��
	 * ����Ӧ���ڸú����� delete this ��ɾ���Լ�����Ϊ�ú������տ϶�
	 * �ᱻ���ã��������಻Ӧ�������ط�������������
	 */
	virtual void destroy(void) {}

	/**
	 * ����ʵ�ִ˽ӿڣ��Ի�ò�ѯ�������� res.ips_.size() == 0
	 * ��˵����ѯ���Ϊ��
	 * @param domain {const char*} �û�����Ĳ�ѯ������
	 * @param res {const dns_res&} ��ѯ�����
	 *  ע���ڸûص��в���ɾ�� dns_service ���󣬷��򽫻����
	 *      �ڴ�Ƿ����ʣ���Ϊ�ûص����� dns_service �б����õģ�
	 *      �ڸú������غ� dns_service ���󻹻����ʹ��
	 */
	virtual void on_result(const char* domain, const dns_res& res) = 0;

	/**
	 * ����ڹ��캯�������õ�����ֵ
	 */
	const string& get_domain() const { return (domain_); }
protected:
	virtual ~dns_result_callback() {}
private:
	string domain_;
};

class ipc_client;

class ACL_CPP_API dns_service
	: public ipc_service
	, public aio_delay_free
{
public:
	/**
	 * ���캯��
	 * @param nthread {int} �����ֵ > 1 ���ڲ��Զ������̳߳أ�����
	 *  ����һ������һ���߳�
	 * @param win32_gui {bool} �Ƿ��Ǵ��������Ϣ������ǣ����ڲ���
	 *  ͨѶģʽ�Զ�����Ϊ���� WIN32 ����Ϣ��������Ȼ����ͨ�õ��׽�
	 *  ��ͨѶ��ʽ
	 */
	dns_service(int nthread = 1, bool win32_gui = false);
	~dns_service();

	/**
	 * ��ʼ������������
	 * @param callback {dns_result_callback*} ��������Ϻ�ص������
	 *  �Ļص����� on_result
	 */
	void lookup(dns_result_callback* callback);

	/**
	 * ����ѯ�߳���������������֪ͨ���̵߳Ĳ�ѯ���󣬸ò�ѯ�����
	 * ���ñ��ص�����֪ͨ�����ѯ���
	 * @param res {const dns_res&} ��ѯ�����
	 */
	void on_result(const dns_res& res);
protected:
	/**
	 * �����麯�������������ӵ���ʱ����ص��˺���
	 * @param client {aio_socket_stream*} ���յ����µĿͻ�������
	 */
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
private:
	std::list<dns_result_callback*> callbacks_;
};

}
