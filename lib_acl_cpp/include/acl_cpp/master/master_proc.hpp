#pragma once
#include "acl_cpp/master/master_base.hpp"

struct ACL_VSTREAM;
struct ACL_EVENT;

namespace acl {

class socket_stream;

/**
 * acl_master ����������н��̷�ʽ��ģ���࣬�������ֻ����һ��ʵ������
 */
class ACL_CPP_API master_proc : public master_base
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
	 * @param count {int} ����ֵ > 0 ʱ������յ����Ӵ����ﵽ��ֵ�����
	 *  �󣬸ú��������أ�����һֱѭ������Զ������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool run_alone(const char* addrs, const char* path = NULL, int count = 1);

protected:
	master_proc();
	virtual ~master_proc();

	/**
	 * ���麯���������յ�һ���ͻ�������ʱ���ô˺���
	 * @param stream {aio_socket_stream*} �½��յ��Ŀͻ����첽������
	 * ע���ú������غ������ӽ��ᱻ�رգ��û���Ӧ�����رո���
	 */
	virtual void on_accept(socket_stream* stream) = 0;

private:
	// �����յ�һ���ͻ�������ʱ�ص��˺���
	static void service_main(ACL_VSTREAM *stream, char *service, char **argv);

	// �������л��û���ݺ���õĻص�����
	static void service_pre_jail(char* service, char** argv);

	// �������л��û���ݺ���õĻص�����
	static void service_init(char* service, char** argv);

	// �������˳�ʱ���õĻص�����
	static void service_exit(char* service, char** argv);

private:
	// �ڵ������з�ʽ�£��ú����������׽����������ӵ���ʱ������
	static void listen_callback(int event_type, ACL_EVENT*,
		ACL_VSTREAM*, void* context);
};

}  // namespace acl
