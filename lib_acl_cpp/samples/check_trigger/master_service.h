#pragma once

class master_service : public acl::master_trigger
{
public:
	master_service();
	~master_service();

protected:
	/**
	 * ��������ʱ�䵽ʱ���ô˺���
	 */
	virtual void on_trigger();

	/**
	 * �������л��û���ݺ���õĻص��������˺���������ʱ������
	 * ��Ȩ��Ϊ��ͨ���޼���
	 */
	virtual void proc_on_init();

	/**
	 * �������˳�ǰ���õĻص�����
	 */
	virtual void proc_on_exit();

protected:
	std::vector<acl::string> url_list_;
};
