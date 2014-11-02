#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#include "acl_cpp/stream/aio_delay_free.hpp"

namespace acl
{

class aio_timer_task;
class aio_handle;

/**
 * ��ʱ���Ļص���
 */
class ACL_CPP_API aio_timer_callback : public aio_delay_free
{
public:
	/**
	 * ���캯��
	 * @param keep {bool} �ö�ʱ���Ƿ������Զ�����
	 */
	aio_timer_callback(bool keep = false);
	virtual ~aio_timer_callback(void);

	/**
	 * ����ʱ�����������Ϊ��ʱ�Ļص�������
	 * ��������������ͷţ�һ���ú��������ã�
	 * ����ζ�Ÿö�ʱ�������е����ж�ʱ���񶼴�
	 * ��ʱ�������б�ɾ��
	 * �ú�����������������������
	 * 1) ��ʱ�����е�������Ϊ 0 ʱ(�磬
	 *    del_timer(aio_timer_callback*, unsigned int) ��
	 *    ������������Ϊ 0 ʱ)
	 * 2) �� aio_handle û�������ظ���ʱ���Ҹö�ʱ����
	 *    ��һ����ʱ���񱻴�����
	 * 3) �� del_timer(aio_timer_callback*) �����ú�
	 */
	virtual void destroy(void) {}

	/**
	 * ��ʱ����������Ƿ�Ϊ��
	 * @return {bool}
	 */
	bool empty(void) const;

	/**
	 * ��ʱ������������
	 * @return {size_t}
	 */
	size_t length(void) const;

	/**
	 * �ö�ʱ���Ƿ����Զ�������
	 * @param on {bool}
	 */
	void keep_timer(bool on);

	/**
	 * �жϸö�ʱ���Ƿ����Զ�������
	 * @return {bool}
	 */
	bool keep_timer(void) const;

	/**
	 * ��ն�ʱ����Ķ�ʱ����
	 * @return {int} ������Ķ�ʱ����ĸ���
	 */
	int clear(void);

protected:
	friend class aio_handle;

	/**
	 * �������ʵ�ִ˻ص�������ע�����������߽�ֹ��
	 * timer_callback �ڲ����� aio_timer_callback ������
	 * ���������򽫻���ɴ��
	 * @param id {unsigned int} ��Ӧĳ������� ID ��
	 */
	virtual void timer_callback(unsigned int id) = 0;

	/****************************************************************/
	/*        ������Ե������º������һЩ�µĶ�ʱ������ ID ��              */
	/****************************************************************/
#ifdef WIN32
	__int64 present_;

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
	long long int present_;
	long long int set_task(unsigned int id, long long int delay);
	long long int del_task(unsigned int id);
#endif

	/**
	 * ���õ�ǰ��ʱ����ʱ���
	 */
	void set_time(void);

private:
	aio_handle* handle_;
	size_t length_;
	std::list<aio_timer_task*> tasks_;
	bool keep_;  // �ö�ʱ���Ƿ������Զ�����
	bool destroy_on_unlock_;  // �������Ƿ� destroy
#ifdef WIN32
	__int64 set_task(aio_timer_task* task);
	__int64 trigger(void);
#else
	long long int set_task(aio_timer_task* task);
	long long int trigger(void);
#endif
};

} // namespace acl
