#pragma once

/**                                                                            
 * ��˷���������״̬��ʱ�����ö�ʱ���ᶨ�ڻ��ܺ�˷�������ӵ�״̬��ͬʱ��    
 * ��Щ״̬ͳһ����״̬���ܷ�����                                              
 */
class StatusTimer : public acl::event_timer
{
public:
	StatusTimer();

	/**
	 * ���ٶ�̬����
	 */
	virtual void destroy();

protected:
	// �����麯��
	virtual void timer_callback(unsigned int id);

private:
	~StatusTimer();
};
