#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

/**
 * ��Ҫ���ӳ��ͷŵ���̳д���󣬿��Ե��� aio_handle:delay_free ������
 * �ӳ����ٵ�Ŀ�ģ��������ڵݹ�����б������ͷ�ʱ�Ķ�����ǰ�ͷŵ�����
 */
class ACL_CPP_API aio_delay_free
{
public:
	aio_delay_free(void);
	virtual ~aio_delay_free(void);

	/**
	 * �ж���ʱ���Ƿ�����������״̬����������״̬�Ķ�ʱ����
	 * ���ܱ�ɾ���ģ����������ڴ����ش���
	 * @return {bool} �Ƿ�������״̬����������״̬�Ķ�����
	 *  ���������ٵ�
	 */
	bool locked(void) const;

	/**
	 * ���������������������������������ڶ�ʱ����������оͲ���
	 * �Զ����������������ٹ���
	 */
	void set_locked(void);

	/**
	 * ��������ȡ������������״̬
	 */
	void unset_locked(void);

	/**
	 * ���ٺ��������ڲ��� aio_timer_delay_free �����ж���Ҫ���ӳ��ͷ�
	 * �����������
	 */
	virtual void destroy(void) {}

private:
	bool locked_;
	bool locked_fixed_;
};

} // namespace acl
