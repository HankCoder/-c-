#pragma once
#include <vector>
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/thread.hpp"
#include "acl_cpp/stream/aio_handle.hpp"

namespace acl
{

class aio_handle;
class check_client;
class connect_manager;
class rpc_service;
class socket_stream;
class aio_socket_stream;

class ACL_CPP_API connect_monitor : public thread
{
public:
	/**
	 * ���캯��
	 * @param manager {connect_manager&}
	 */
	connect_monitor(connect_manager& manager);

	virtual ~connect_monitor();

	/**
	 * ��ϣ����������ʽ�����������ʱ����Ҫ�ȵ��ñ������� acl::rpc_service
	 * �����ӿڴ����������ڳ�ʼ��ʱ�����ñ�����������÷�������ʽ���� IO ���
	 * @param max_threads {int} rpc_service �����̳߳������е�����߳���
	 * @param addr {const char*} ϣ�� rpc_service ��������ı�����ַ������
	 *  Ϊ�����Ļص�ַ���� UNIX ƽ̨��ʹ�����׽ӿڵ�ַ
	 * @return {connect_monitor&}
	 */
	connect_monitor& open_rpc_service(int max_threads,
		const char* addr = NULL);

	/**
	 * ���ü�ⶨʱ��������ʱ����
	 * @param n {int} ʱ�������룩
	 * @return {connect_mointor&}
	 */
	connect_monitor& set_check_inter(int n);

	/**
	 * �������ӱ����������ĳ�ʱʱ��
	 * @param n {int} ��ʱʱ�䣨�룩
	 * @return {connect_monitor&}
	 */
	connect_monitor& set_conn_timeout(int n);

	/**
	 * ֹͣ����߳�
	 * @param graceful {bool} �Ƿ������عرռ����̣����Ϊ true
	 *  �������еļ�����ӹرպ����̲߳ŷ��أ�������ֱ�Ӽ���߳�
	 *  ֱ�ӷ��أ����ܻ����һЩ���ڼ�������δ���ͷš�������ˣ����
	 *  ���ӳؼ�Ⱥ��������ǽ�����ȫ�ֵģ����Խ��˲�����Ϊ false�����
	 *  ���ӳؼ�Ⱥ������������й�������Ҫ����δ������ͷţ���Ӧ����Ϊ true
	 */
	void stop(bool graceful);

	/**
	 * ��� connect_manager ���ö���
	 * @return {connect_manager&}
	 */
	connect_manager& get_manager() const
	{
		return manager_;
	}

	/**
	 * �麯��������������ر�����������һ���жϸ������Ƿ��Ǵ��ģ��ûص�����������
	 * �ռ�Ϊ��ǰ����������̵߳����пռ䣬����ڸûص������в������������̣�����
	 * ��������������������߳�
	 * @param checker {check_client&} ��������ӵļ����󣬿���ͨ��
	 *  check_client ���еķ������£�
	 *  1) get_conn ��÷��������Ӿ��
	 *  2) get_addr ��÷���˵�ַ
	 *  3) set_alive ���������Ƿ���
	 *  4) close �ر�����
	 */
	virtual void nio_check(check_client& checker, aio_socket_stream& conn);

	/**
	 * ͬ�� IO ����麯�����ú������̳߳ص�ĳ�����߳̿ռ������У�����������ر�����
	 * �Լ��ʵ��Ӧ�õ��������Ӵ��״̬�������ڱ������������� IO ����
	 * @param checker {check_client&} ��������ӵļ�����
	 *  check_client ����������õķ������£�
	 *  1) get_addr ��÷���˵�ַ
	 *  2) set_alive ���������Ƿ���
	 *  check_client ���н�ֹ���õķ������£�
	 *  1) get_conn ��÷��������Ӿ��
	 *  2) close �ر�����
	 */
	virtual void sio_check(check_client& checker, socket_stream& conn);

public:
	// ��Ȼ����ĺ����� public �ģ���ֻ���ڲ�ʹ��
	/**
	 * �������˽������Ӻ���ô˺���
	 * @param checker {check_client&}
	 */
	void on_open(check_client& checker);

protected:
	// ���ി�麯��
	virtual void* run();

private:
	bool stop_;
	bool stop_graceful_;
	aio_handle handle_;			// ��̨����̵߳ķ��������
	connect_manager& manager_;		// ���ӳؼ��Ϲ������
	int   check_inter_;			// ������ӳ�״̬��ʱ����(��)
	int   conn_timeout_;			// ���ӷ������ĳ�ʱʱ��
	rpc_service* rpc_service_;		// �첽 RPC ͨ�ŷ�����
};

} // namespace acl
