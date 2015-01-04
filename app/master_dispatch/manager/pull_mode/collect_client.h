#pragma once

class message_manager;

class collect_client : public acl::thread
{
public:
	collect_client(message_manager& manager, const char* server);
	~collect_client() {}

protected:
	// ���� acl::thread ���鷽��
	void* run();

private:
	message_manager& manager_;
	acl::string server_;
};
