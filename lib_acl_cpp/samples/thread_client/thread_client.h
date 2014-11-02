#pragma once

class thread_client : public acl::thread
{
public:
	thread_client(const char* server_addr, bool keep_alive,
		int count, int length);
	~thread_client();

protected:
	virtual void* run();		// �����麯���������߳��б�����

private:
	acl::string server_addr_;	// ��������ַ
	bool  keep_alive_;		// �Ƿ���ó����ӷ�ʽ
	int   count_;			// IO �Ự����
	int   length_;			// ÿ�� IO �����ݳ���
};
