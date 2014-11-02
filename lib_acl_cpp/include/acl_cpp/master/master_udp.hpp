#pragma once
#include "acl_cpp/master/master_base.hpp"

namespace acl {

class ACL_CPP_API master_udp : public master_base
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
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool run_alone(const char* addrs, const char* path = NULL,
		unsigned int count = 1);

protected:
	// ���಻��ֱ�ӱ�ʵ����
	master_udp();
	virtual ~master_udp();

	/**
	 * ���麯������ UDP �������ݿɶ�ʱ�ص�����˺���
	 * @param stream {socket_stream*}
	 */
	virtual void on_read(socket_stream* stream) = 0;

	/**
	 * ��ñ��ؼ������׽ӿ������󼯺�
	 * @return {const std::vector<socket_stream*>&}
	 */
	const std::vector<socket_stream*>& get_sstreams() const
	{
		return sstreams_;
	}

private:
	std::vector<socket_stream*> sstreams_;

	void close_sstreams(void);

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
	static void read_callback(int event_type, ACL_EVENT*,
		ACL_VSTREAM*, void* context);
};

} // namespace acl
