#pragma once
#include "acl_cpp/master/master_base.hpp"
#include "acl_cpp/master/master_threads2.hpp"

struct ACL_VSTREAM;
struct ACL_EVENT;

namespace acl {

#if 1
typedef class master_threads2 master_threads;
#else

class socket_stream;

/**
 * �̳߳ط���������࣬����Ϊ�����࣬������Ҫʵ�����еĴ��麯����
 * ÿһ�����̽�����һ���������ʵ�����������ᱻ��ֹ
 */
class ACL_CPP_API master_threads : public master_base
{
public:
	/**
	 * ��ʼ���У����øú�����ָ�÷���������� acl_master ������
	 * ����֮�����У�һ������������״̬
	 * @param argc {int} �� main �д��ݵĵ�һ����������ʾ��������
	 * @param argv {char**} �� main �д��ݵĵڶ�������
	 */
	void run_daemon(int argc, char** argv);

	/**
	 * �ڵ�������ʱ�Ĵ��������û����Ե��ô˺�������һЩ��Ҫ�ĵ��Թ���
	 * @param addrs {const char*} ���������ַ�б���ʽ��IP:PORT, IP:PORT...
	 * @param path {const char*} �����ļ�ȫ·��
	 * @param count {unsigned int} ѭ������Ĵ������ﵽ��ֵ�����Զ����أ�
	 *  ����ֵΪ 0 ���ʾ����һֱѭ���������������������
	 * @param threads_count {int} ����ֵ���� 1 ʱ��ʾ�Զ������̳߳ط�ʽ��
	 *  ��ֵֻ�е� count != 1 ʱ����Ч������ count == 1 �������һ�ξͷ���
	 *  �Ҳ��������̴߳���ͻ�������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool run_alone(const char* addrs, const char* path = NULL,
		unsigned int count = 1, int threads_count = 1);

protected:
	// ���಻��ֱ�ӱ�ʵ����
	master_threads();
	virtual ~master_threads();

	/**
	 * ���麯������ĳ���ͻ������������ݿɶ���رջ����ʱ���ô˺���
	 * @param stream {socket_stream*}
	 * @return {bool} ���� false ���ʾ���������غ���Ҫ�ر����ӣ�
	 *  �����ʾ��Ҫ���ֳ����ӣ��������������Ӧ��Ӧ�÷��� false
	 */
	virtual bool thread_on_read(socket_stream* stream) = 0;

	/**
	 * ���̳߳��е�ĳ���̻߳��һ������ʱ�Ļص�������
	 * ���������һЩ��ʼ������
	 * @param stream {socket_stream*}
	 * @return {bool} ������� false ���ʾ����Ҫ��ر����ӣ�����
	 *  �ؽ��������ٴ����� thread_main ����
	 *  ע�������������� false ���ر�ʱ�������� thread_on_close ����
	 */
	virtual bool thread_on_accept(socket_stream* stream)
	{
		(void) stream;
		return true;
	}

	/**
	 * ��ĳ���������ӵ� IO ��д��ʱʱ�Ļص�����������ú������� true ��
	 * ��ʾ�����ȴ���һ�ζ�д��������ϣ���رո�����
	 * @param stream {socket_stream*}
	 * @return {bool} ������� false ���ʾ����Ҫ��ر����ӣ�������Ҫ��
	 *  ��������������
	 */
	virtual bool thread_on_timeout(socket_stream* stream)
	{
		(void) stream;
		return false;
	}

	/**
	 * ����ĳ���̰߳󶨵����ӹر�ʱ�Ļص�����
	 * @param stream {socket_stream*}
	 * ע������ thread_on_accept ���� false �����ر�ʱ�ú���������
	 * ������
	 */
	virtual void thread_on_close(socket_stream* stream) { (void) stream; }

	/**
	 * ���̳߳���һ�����̱߳�����ʱ�Ļص�����
	 */
	virtual void thread_on_init() {}

	/**
	 * ���̳߳���һ���߳��˳�ʱ�Ļص�����
	 */
	virtual void thread_on_exit() {}

private:
	// �߳̿�ʼ������Ļص�����
	static int thread_begin(void* arg);

	// �߳̽�������ǰ�Ļص�����
	static void thread_finish(void* arg);

	// ���߳�����µĴ�����
	static void thread_run(void* arg);

	// ������һ��
	static void run_once(ACL_VSTREAM* client);

	// �����ױ��ص��ĺ���
	static void listen_callback(int event_type, ACL_EVENT*,
		ACL_VSTREAM*, void *context);

	//////////////////////////////////////////////////////////////////

	// �����յ�һ���ͻ�������ʱ�ص��˺���
	static int service_main(ACL_VSTREAM*, void*);

	// �����յ�һ���ͻ�����ʱ�Ļص����������Խ���һЩ��ʼ��
	static int service_on_accept(ACL_VSTREAM*);

	// ���ͻ������Ӷ�д��ʱʱ�Ļص�����
	static int service_on_timeout(ACL_VSTREAM*, void*);

	// ���ͻ������ӹر�ʱ�Ļص�����
	static void service_on_close(ACL_VSTREAM*, void*);

	// �������л��û���ݺ���õĻص�����
	static void service_pre_jail(void*);

	// �������л��û���ݺ���õĻص�����
	static void service_init(void*);

	// �������˳�ʱ���õĻص�����
	static void service_exit(void*);

	// ���̴߳�������õĻص�����
	static void thread_init(void*);

	// ���߳��˳�ǰ���õĻص�����
	static void thread_exit(void*);
};

#endif

} // namespace acl
