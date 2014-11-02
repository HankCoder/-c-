#pragma once

class http_job : public acl::thread_job
{
public:
	http_job(acl::thread_pool& thrpool, const char* url,
		const char* dns_ip, int dns_port);

private:
	// Ҫ����ʵ�������Ƕ�̬����
	~http_job();

protected:
	// ���ി�麯��
	virtual void* run();
	
private:
	acl::thread_pool& thrpool_;
	acl::string url_;
	acl::string dns_ip_;
	int   dns_port_;

private:
	bool dns_lookup(const char* domain, std::vector<acl::string>& ips);
};
