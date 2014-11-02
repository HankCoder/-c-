#pragma once

class db_store;

class http_thread : public acl::thread
{
public:
	http_thread(const char* domain, const char* ip, int port,
		const char* url, double spent_dns);
	~http_thread();

protected:
	virtual void* run();

private:
	acl::string domain_;
	acl::string ip_;
	int   port_;

private:
	friend class db_store;

	acl::string url_;
	double spent_dns_;
	double spent_connect_;
	double spent_http_;
	double spent_total_;
	int    length_;
	bool   success_;
	char   addr_[256];

private:
	// ���ӷ�����
	acl::socket_stream* connect_server();

	// ���� HTTP �������
	bool http_request(acl::socket_stream* conn, const char* host);
};
