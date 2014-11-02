#pragma once

namespace acl
{

/**
 * ���麯�����߳������࣬����ʵ���� run �����������߳��б�ִ�е�
 */
class ACL_CPP_API thread_job
{
public:
	thread_job() {}
	virtual ~thread_job() {}

	/**
	 * ���麯�����������ʵ�ִ˺������ú��������߳���ִ��
	 * @return {void*} �߳��˳�ǰ���صĲ���
	 */
	virtual void* run() = 0;
};

/**
 * �̴߳����࣬����Ľӿڶ��������� Java �Ľӿڶ��壬������Ҫʵ��
 * ����Ĵ��麯����ʹ����ͨ������ thread::start() �����̹߳���
 */
class ACL_CPP_API thread : public thread_job
{
public:
	thread();
	virtual ~thread();

	/**
	 * ��ʼ�����̹߳��̣�һ���ú��������ã������������һ���µ�
	 * ���̣߳������߳���ִ�л��� thread_job::run ����
	 * @return {bool} �Ƿ�ɹ������߳�
	 */
	bool start();

	/**
	 * �������߳�ʱΪ�� detachable ״̬���������ô˺����ȴ��߳̽�����
	 * �������߳�ʱΪ detachable ״̬ʱ����ֹ���ñ�����
	 * @param out {void**} ���ò����ǿ�ָ��ʱ���ò����������
	 *  �߳��˳�ǰ���صĲ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool wait(void** out = NULL);

	/**
	 * �ڵ��� start ǰ���ô˺������������������߳��Ƿ�Ϊ
	 * ���� (detachable) ״̬�����δ���ô˺�������������
	 * ���߳�Ĭ��Ϊ����״̬
	 * @param yes {bool} �Ƿ�Ϊ����״̬
	 * @return {thread&}
	 */
	thread& set_detachable(bool yes);

	/**
	 * �ڵ��� start ǰ���ô˺������������������̵߳Ķ�ջ��С
	 * @param size {size_t} �̶߳�ջ��С������ֵΪ 0 ��δ
	 *  ���ô˺����������������̶߳�ջ��СΪϵͳ��Ĭ��ֵ
	 * @return {thread&}
	 */
	thread& set_stacksize(size_t size);

	/**
	 * �ڵ��� start ����ô˺������Ի���������̵߳� id ��
	 * @return {unsigned long}
	 */
	unsigned long thread_id() const;

	/**
	 * ��ǰ�����������̵߳��߳� id ��
	 * @return {unsigned long}
	 */
	static unsigned long thread_self();

private:
	bool detachable_;
	size_t stack_size_;
#ifdef WIN32
	void* thread_;
	unsigned long thread_id_;
#else
	pthread_t thread_;
	unsigned long thread_id_;
#endif
	void* return_arg_;
	static void* thread_run(void* arg);
};

} // namespace acl
