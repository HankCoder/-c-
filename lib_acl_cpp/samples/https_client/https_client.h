#pragma once

class https_client : public acl::thread
{
public:
	https_client(const char* server_addr, const char* domain,
		bool keep_alive, int count, int length);
	~https_client();

	void set_ssl_conf(acl::polarssl_conf* conf);

protected:
	virtual void* run();		// �����麯���������߳��б�����

private:
	acl::string server_addr_;	// ��������ַ
	acl::string domain_;		// ����
	bool  keep_alive_;		// �Ƿ���ó����ӷ�ʽ
	int   count_;			// IO �Ự����
	int   length_;			// ÿ�� IO �����ݳ���
	acl::polarssl_conf* ssl_conf_;

	bool connect_server(acl::http_client& client);
	int http_request(int count);
};
