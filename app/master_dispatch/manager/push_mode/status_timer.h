#pragma once

class status_timer : public acl::event_timer
{
public:
	status_timer();

	/**
	 * ��ʱ��������ʱ�Ļص����� 
	 */
	virtual void destroy();

protected:
	// ��ʱ��ʱ�䵽����ʱ�Ļص����� 
	virtual void timer_callback(unsigned int id);

private:
	~status_timer();
};
