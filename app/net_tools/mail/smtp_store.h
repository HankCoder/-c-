#pragma once

struct SMTP_METER;
class smtp_callback;
class smtp_store : public acl::rpc_request
{
public:
	smtp_store(const char* user, const char* smtp_ip,
		const SMTP_METER& meter, smtp_callback& callback);
protected:
	~smtp_store();

	// �����麯�������̴߳�����
	virtual void rpc_run();

	// �����麯�������̴߳�����̣��յ����߳�������ɵ���Ϣ
	virtual void rpc_onover();

private:
	bool ok_;
	char* user_;
	char* smtp_ip_;
	SMTP_METER* meter_;
	smtp_callback& callback_;
	acl::string dbpath_;
	bool create_tbl(acl::db_handle& db);
	void insert_tbl(acl::db_handle& db);
};