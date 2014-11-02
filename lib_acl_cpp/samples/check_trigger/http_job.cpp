#include "stdafx.h"
#include "util.h"
#include "http_thread.h"
#include "http_job.h"

http_job::http_job(acl::thread_pool& thrpool, const char* url,
	const char* dns_ip, int dns_port)
: thrpool_(thrpool)
, url_(url)
, dns_ip_(dns_ip)
, dns_port_(dns_port)
{
}

http_job::~http_job()
{
}

void* http_job::run()
{
	char  addr[256];
	if (acl::http_utils::get_addr(url_, addr, sizeof(addr)) == false)
	{
		logger_error("invalid url: %s", url_.c_str());
		return NULL;
	}

	char* domain = addr, *ptr = strchr(domain, ':');
	int   port;
	if (ptr == NULL)
		port = 80;
	else
	{
		*ptr++ = 0;
		port = atoi(ptr);
	}

	std::vector<acl::string> ips;

	struct timeval begin, end;

	gettimeofday(&begin, NULL);

	// ��ѯ������ IP �б�
	if (dns_lookup(domain, ips) == false)
	{
		delete this;
		return NULL;
	}

	gettimeofday(&end, NULL);

	// ���� DNS �Ĳ�ѯ��ʱ
	double spent = util::stamp_sub(&end, &begin);

	std::vector<acl::thread*> threads;

	// ���� IP ��ַ�б�ÿһ�� IP ����һ�� HTTP �ͻ����߳�
	std::vector<acl::string>::const_iterator cit = ips.begin();
	for (; cit != ips.end(); ++cit)
	{
		// ����������һ���̶߳���
		acl::thread* thr = new http_thread(domain, (*cit).c_str(),
				port, url_.c_str(), spent);
		thr->set_detachable(false);  // �����߳�Ϊ�Ƿ���״̬
		thr->start();
		threads.push_back(thr);
	}

	// �ȴ����� HTTP �����߳�ִ�����
	std::vector<acl::thread*>::iterator it = threads.begin();
	for (; it != threads.end(); ++it)
	{
		acl::thread* thr = (*it);
		thr->wait();
		delete thr;
	}

	delete this;  // ������
	return NULL;
}

bool http_job::dns_lookup(const char* domain, std::vector<acl::string>& ips)
{
	ACL_RES *res;
	ACL_DNS_DB *dns_db;

	res = acl_res_new(dns_ip_.c_str(), dns_port_);

	// ֱ���� DNS ���������� DNS ��ѯ���ݰ�
	dns_db = acl_res_lookup(res, domain);
	if (dns_db == NULL)
	{
		logger_error("acl_res_lookup failed, dns addr: %s:%d, domain: %s",
			dns_ip_.c_str(), dns_port_, domain);
		acl_res_free(res);
		return false;
	}

	ACL_ITER iter;
	acl_foreach(iter, dns_db)
	{
		ACL_HOST_INFO* info = (ACL_HOST_INFO*) iter.data;
		ips.push_back(info->ip);
	}

	acl_res_free(res);
	acl_netdb_free(dns_db);

	if (ips.empty())
	{
		logger_error("no ip for domain: %s", domain);
		return false;
	}

	return true;
}
