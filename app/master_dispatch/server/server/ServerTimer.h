#pragma once

/**                                                                            
 * ��˷���������״̬��ʱ�����ö�ʱ���ᶨ�ڻ��ܺ�˷�������ӵ�״̬��ͬʱ��    
 * ��Щ״̬ͳһ����״̬���ܷ�����                                              
 */
class ServerTimer : public acl::event_timer
{
public:
	ServerTimer() {}

	/**
	 * ���ٶ�̬����
	 */
	virtual void destroy();

protected:
	// �����麯��
	virtual void timer_callback(unsigned int id);

private:
	~ServerTimer() {}
};
