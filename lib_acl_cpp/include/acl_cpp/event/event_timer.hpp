#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>

namespace acl
{

class event_task;

class ACL_CPP_API event_timer
{
public:
	/**
	 * ���캯��
	 * @param keep {bool} �ö�ʱ���Ƿ������Զ�����
	 */
	event_timer(bool keep = false);
	virtual ~event_timer();

	/**
	 * ����ʱ�����������Ϊ��ʱ�Ļص�������
	 * ��������������ͷţ�һ���ú��������ã�
	 * ����ζ�Ÿö�ʱ�������е����ж�ʱ���񶼴�
	 * ��ʱ�������б�ɾ��
	 * �ú�����������������������
	 * 1) ��ʱ�����е�������Ϊ 0 ʱ
	 * 2) ��û�������ظ���ʱ���Ҹö�ʱ������һ����ʱ���񱻴�����
	 * 3) �� del_timer(event_timer*) �����ú�
	 */
	virtual void destroy(void) {}

	/**
	 * ��ʱ����������Ƿ�Ϊ��
	 * @return {bool}
	 */
	bool empty(void) const
	{
		return tasks_.empty();
	}

	/**
	 * ��ʱ������������
	 * @return {size_t}
	 */
	size_t length(void) const
	{
		return length_;
	}

	/**
	 * �ö�ʱ���Ƿ����Զ�������
	 * @param on {bool}
	 */
	void keep_timer(bool on);

	/**
	 * �жϸö�ʱ���Ƿ����Զ�������
	 * @return {bool}
	 */
	bool keep_timer(void) const
	{
		return keep_;
	}

	/**
	 * ��ն�ʱ����Ķ�ʱ����
	 * @return {int} ������Ķ�ʱ����ĸ���
	 */
	int clear(void);

	/**
	 * �������ʵ�ִ˻ص�������ע�����������߽�ֹ��
	 * timer_callback �ڲ����� event_timer ������
	 * ���������򽫻���ɴ��
	 * @param id {unsigned int} ��Ӧĳ������� ID ��
	 */
	virtual void timer_callback(unsigned int id) = 0;

	/****************************************************************/
	/*        ������Ե������º������һЩ�µĶ�ʱ������ ID ��      */
	/****************************************************************/
#ifdef WIN32
	/**
	 * ��Ա���ʱ�������µ�����ID�ţ����������ͨ��һ����ʱ������
	 * �����ʱ����
	 * @param id {unsigned int} ��ʱ����ʱ����ID��
	 * @param delay {__int64} ÿ������Զ������ö�ʱ����ͬʱ����Ӧ�Ķ�ʱ����ʱ
	 *  ����ID�Ŵ���(΢�뼶)
	 * @return {__int64} ���뱾��ʱ���ĵ�һ�����ᴥ���Ķ�ʱ����ID�����(΢�뼶)
	 */
	__int64 set_task(unsigned int id, __int64 delay);

	/**
	 * ɾ����ʱ����ĳ����ϢID��Ӧ�Ķ�ʱ����
	 * @param {unsigned int} ��ʱ����ID
	 * @return {__int64} ���뱾��ʱ���ĵ�һ�����ᴥ���Ķ�ʱ����ID�����(΢�뼶)
	 */
	__int64 del_task(unsigned int id);
#else
	long long int set_task(unsigned int id, long long int delay);
	long long int del_task(unsigned int id);
#endif

	//////////////////////////////////////////////////////////////////////

	/**
	 * �������е�ʱ�Ķ�ʱ����
	 * @return {long lont int} ��һ����Ҫ��ʱ������
	 */
#ifdef WIN32
	__int64 trigger(void);
	__int64 min_delay(void) const
	{
		return min_delay_;
	}
#else
	long long int trigger(void);
	long long int min_delay(void) const
	{
		return min_delay_;
	}
#endif

protected:
#ifdef WIN32
	__int64 min_delay_;
	__int64 present_;
#else
	long long int min_delay_;
	long long int present_;
#endif

private:
	size_t length_;
	std::list<event_task*> tasks_;
	bool keep_;  // �ö�ʱ���Ƿ������Զ�����
#ifdef WIN32
	__int64 set_task(event_task* task);
#else
	long long int set_task(event_task* task);
#endif

	/**
	 * ���õ�ǰ��ʱ����ʱ���
	 */
	void set_time(void);
};

}  // namespace acl
