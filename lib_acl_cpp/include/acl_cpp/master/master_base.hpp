#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/master/master_conf.hpp"

struct ACL_EVENT;

namespace acl
{

class event_timer;

class ACL_CPP_API master_base
{
public:
	/**
	 * ���� bool ���͵�������
	 * @param table {master_bool_tbl*}
	 */
	void set_cfg_bool(master_bool_tbl* table);

	/**
	 * ���� int ���͵�������
	 * @param table {master_int_tbl*}
	 */
	void set_cfg_int(master_int_tbl* table);

	/**
	 * ���� int64 ���͵�������
	 * @param table {master_int64_tbl*}
	 */
	void set_cfg_int64(master_int64_tbl* table);

	/**
	 * ���� �ַ��� ���͵�������
	 * @param table {master_str_tbl*}
	 */
	void set_cfg_str(master_str_tbl* table);

	/**
	 * �ж��Ƿ����� acl_master ���Ƶ� daemon ģʽ
	 * @return {bool}
	 */
	bool daemon_mode(void) const;

	/////////////////////////////////////////////////////////////////////
	
	/**
	 * ���ý��̼���Ķ�ʱ�����ú���ֻ�������̵߳����пռ� (���ں���
	 * proc_on_init) �б����ã����ö�ʱ������ִ����Ϻ���Զ���
	 * ����(���ڲ����Զ����� master_timer::destroy ����)
	 * @param timer {event_timer*} ��ʱ����
	 * @return {bool} ���ö�ʱ���Ƿ�ɹ�
	 */
	bool proc_set_timer(event_timer* timer);

	/**
	 * ɾ�����̼���ʱ��
	 * @param timer {event_timer*} �� proc_set_timer ���õĶ�ʱ����
	 */
	void proc_del_timer(event_timer* timer);

protected:
	bool daemon_mode_;
	bool proc_inited_;

	master_base();
	virtual ~master_base();

	/**
	 * �������л��û����ǰ���õĻص������������ڴ˺�������һЩ
	 * �û����Ϊ root ��Ȩ�޲���
	 */
	virtual void proc_pre_jail() {}

	/**
	 * �������л��û���ݺ���õĻص��������˺���������ʱ������
	 * ��Ȩ��Ϊ��ͨ���޼���
	 */
	virtual void proc_on_init() {}

	/**
	 * �������˳�ǰ���õĻص�����
	 */
	virtual void proc_on_exit() {}

	// ���ö���
	master_conf conf_;

protected:
	// ���������ñ����������¼�������
	void set_event(ACL_EVENT* event);

private:
	ACL_EVENT* event_;
};

}  // namespace acl
