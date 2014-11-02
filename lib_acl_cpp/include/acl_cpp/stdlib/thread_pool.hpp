#pragma once

struct acl_pthread_pool_t;
struct acl_pthread_pool_attr_t;

namespace acl
{

class thread_job;

/**
 * �̳߳ع����࣬�����ڹ�����̳߳��е��߳��ǰ�פ����(�����߳̿���һ��ʱ���
 * �Զ��˳�)�������������Ǵ��麯����thread_on_init(�̳߳��е�ĳ���̵߳�һ��
 * ����ʱ�����ȵ��ô˺���)��thread_on_exit(�̳߳��е�ĳ���߳��˳�ʱ���ô˺���)
 */
class ACL_CPP_API thread_pool
{
public:
	thread_pool();
	virtual ~thread_pool();

	/**
	 * �����̳߳أ��ڴ����̳߳ض���󣬱������ȵ��ô˺����������̳߳�
	 */
	void start();

	/**
	 * ֹͣ�������̳߳أ����ͷ��̳߳���Դ�����ô˺�������ʹ�������߳��˳���
	 * �������ͷű�ʵ�����������ʵ���Ƕ�̬��������û�Ӧ�����ͷ���ʵ����
	 * �ڵ��ñ�����������������̳߳ع��̣���������µ��� start ����
	 */
	void stop();

	/**
	 * �ȴ��̳߳��е������̳߳�ִ������������
	 */
	void wait();

	/**
	 * ��һ�����񽻸��̳߳��е�һ���߳�ȥִ�У��̳߳��е�
	 * �̻߳�ִ�и������е� run ����
	 * @param job {thread_job*} �߳�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool run(thread_job* job);

	/**
	 * ��һ�����񽻸��̳߳��е�һ���߳�ȥִ�У��̳߳��е�
	 * �̻߳�ִ�и������е� run �������ú��������� run ������ȫ��ͬ��ֻ��Ϊ��
	 * ʹ JAVA ����Ա��������Ϊ��Ϥ���ṩ�˴˽ӿ�
	 * @param job {thread_job*} �߳�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool execute(thread_job* job);

	/**
	 * �ڵ��� start ǰ���ô˺������������������̵߳Ķ�ջ��С
	 * @param size {size_t} �̶߳�ջ��С������ֵΪ 0 ��δ
	 *  ���ô˺����������������̶߳�ջ��СΪϵͳ��Ĭ��ֵ
	 * @return {thread&}
	 */
	thread_pool& set_stacksize(size_t size);

	/**
	 * �����̳߳�����̸߳�������
	 * @param max {size_t} ����߳�������������ô˺��������ڲ�ȱʡֵΪ 100
	 * @return {thread_pool&}
	 */
	thread_pool& set_limit(size_t max);

	/**
	 * �����̳߳��п����̵߳ĳ�ʱ�˳�ʱ��
	 * @param ttl {int} ���г�ʱʱ��(��)����������ô˺��������ڲ�ȱʡΪ 0
	 * @return {thread_pool&}
	 */
	thread_pool& set_idle(int ttl);

	/**
	 * ��õ�ǰ�̳߳������̵߳�����
	 * @return {int} �����̳߳������̵߳����������δͨ������ start
	 *  �����̳߳ع��̣���ú������� -1
	 */
	int threads_count() const;

	/**
	 * ��õ�ǰ�̳߳���δ���������������
	 * @return {int} ���̳߳ػ�δ������(��δ���� start)���Ѿ������򷵻� -1
	 */
	int task_qlen() const;

protected:
	/**
	 * ���̳߳��е����̵߳�һ�α�����ʱ�����麯���������ã�
	 * �û��������Լ���ʵ������һЩ��ʼ������
	 * @return {bool} ��ʼ���Ƿ�ɹ�
	 */
	virtual bool thread_on_init() { return true; }

	/**
	 * ���̳߳��е����߳��˳�ʱ�����麯���������ã��û�����
	 * ���Լ���ʵ�� ����һЩ��Դ�ͷŹ���
	 */
	virtual void thread_on_exit() {}

private:
	size_t stack_size_;
	size_t threads_limit_;
	int    thread_idle_;

	acl_pthread_pool_t* thr_pool_;
	acl_pthread_pool_attr_t* thr_attr_;

	static void thread_run(void* arg);
	static int  thread_init(void* arg);
	static void thread_exit(void* arg);
};

} // namespace acl
