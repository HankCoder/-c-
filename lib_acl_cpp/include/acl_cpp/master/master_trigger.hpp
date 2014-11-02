#pragma once
#include "acl_cpp/master/master_base.hpp"

namespace acl {

/**
 * acl_master ����������д�������ģ���࣬�������ֻ����һ��ʵ������
 */
class ACL_CPP_API master_trigger : public master_base
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
	 * @param path {const char*} �����ļ�ȫ·��
	 * @param count {int} ����ֵ > 0 ʱ������յ����Ӵ����ﵽ��ֵ�����
	 *  �󣬸ú��������أ�����һֱѭ������Զ������
	 * @param interval {int} ������ʱ����(��)
	 */
	void run_alone(const char* path = NULL, int count = 1, int interval = 1);
protected:
	master_trigger();
	virtual ~master_trigger();

	/**
	 * ��������ʱ�䵽ʱ���ô˺���
	 */
	virtual void on_trigger() = 0;
private:
	// ��������ʱ�䵽ʱ�� acl_master ��ܻص��˺���
	static void service_main(char*, int, char*, char**);

	// �������л��û���ݺ���õĻص�����
	static void service_pre_jail(char*, char**);

	// �������л��û���ݺ���õĻص�����
	static void service_init(char*, char**);

	// �������˳�ʱ���õĻص�����
	static void service_exit(char*, char**);
};

}  // namespace acl
