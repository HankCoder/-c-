#pragma once
#include <vector>

class domain_info;
class nslookup_callback;
class dns_store : public acl::rpc_request
{
public:
	dns_store(std::vector<domain_info*>* domain_list,
		nslookup_callback& callback);
protected:
private:
	~dns_store();

	// �����麯�������̴߳�����
	virtual void rpc_run();

	// �����麯�������̴߳�����̣��յ����߳�������ɵ���Ϣ
	virtual void rpc_onover();

private:
	bool ok_;
	std::vector<domain_info*>* domain_list_;
	nslookup_callback& callback_;
	acl::string dbpath_;
	bool create_tbl(acl::db_handle& db);
	void insert_tbl(acl::db_handle& db);
	void insert_one(acl::db_handle& db, const domain_info* info);
};