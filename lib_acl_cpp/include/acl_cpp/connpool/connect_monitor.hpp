#pragma once
#include <vector>
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/thread.hpp"
#include "acl_cpp/stream/aio_handle.hpp"

namespace acl
{

class aio_handle;
class connect_manager;

class connect_monitor : public thread
{
public:
	connect_monitor(connect_manager& manager,
		int check_inter = 1, int conn_timeout = 10);
	~connect_monitor();

	/**
	 * ֹͣ�߳�
	 * @param graceful {bool} �Ƿ������عرռ����̣����Ϊ true
	 *  �������еļ�����ӹرպ����̲߳ŷ��أ�������ֱ�Ӽ���߳�
	 *  ֱ�ӷ��أ����ܻ����һЩ���ڼ�������δ���ͷš�������ˣ����
	 *  ���ӳؼ�Ⱥ��������ǽ�����ȫ�ֵģ����Խ��˲�����Ϊ false�����
	 *  ���ӳؼ�Ⱥ������������й�������Ҫ����δ������ͷţ���Ӧ����Ϊ true
	 */
	void stop(bool graceful);

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
};

} // namespace acl
