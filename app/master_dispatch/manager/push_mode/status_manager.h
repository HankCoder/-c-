#pragma once
#include <vector>
#include <set>

class server_ttl
{
public:
	server_ttl(const char* key)
		: key_(key)
	{
		when_ = time(NULL) + var_cfg_status_ttl;
	}

	~server_ttl() {}

	time_t get_when() const
	{
		return when_;
	}

	const char* get_key() const
	{
		return key_.c_str();
	}

private:
	time_t when_;
	acl::string key_;
};

class server_ttl_comp
{
public:
	bool operator()(const server_ttl* left, const server_ttl* right)
	{
		return left->get_when() < right->get_when();
	}
};

class server_status
{
public:
	server_status(server_ttl& ttl, const char* key, const char* data)
		: ttl_(ttl)
		, key_(key)
		, data_(data)
	{
	}

	~server_status() {}

	const char* get_data() const
	{
		return data_.c_str();
	}

	const char* get_key() const
	{
		return key_.c_str();
	}

	server_ttl& get_ttl() const
	{
		return ttl_;
	}

private:
	server_ttl& ttl_;
	acl::string key_;
	acl::string data_;
};

class status_manager : public acl::singleton<status_manager>
{
public:
	status_manager();
	~status_manager();

	/**
	 * ��õ�ǰ��������Ⱥ�����н�������״̬
	 * @param out {acl::string&} �洢������ݣ��ڲ�����������ӷ�ʽ����
	 *  ��������� out ԭʼ������
	 * @return {acl::string&}
	 */
	acl::string& get_status(acl::string& out);

	/**
	 * ����ĳ�����������ĵ�ǰ״̬����
	 * @param key {const char*} ��ʶĳ��������
	 * @param data {const char*} �÷������״̬����
	 */
	void set_status(const char* key, const char* data);

	/**
	 * ɾ��ĳ������������״̬����
	 * @param key {const char*} ��ʶĳ��������
	 */
	void del_status(const char* key);

	/**
	 * �����ڵ��������
	 * @return {int} ���������������
	 */
	int check_timeout();

private:
	acl::locker lock_;
	std::map<acl::string, server_status*> servers_status_;
	std::multiset<server_ttl*, server_ttl_comp> servers_ttl_;

	void del_server_ttl(server_ttl& ttl);
};
