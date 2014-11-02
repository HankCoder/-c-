#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/locker.hpp"
#include "acl_cpp/hsocket/hsclient.hpp"
#include "acl_cpp/hsocket/hspool.hpp"

namespace acl
{

hspool::hspool(const char* addr_rw, const char* addr_rd,
	bool cache_enable /* = true */, bool retry_enable /* = true */)
: cache_enable_(cache_enable)
, retry_enable_(retry_enable)
{
	acl_assert(addr_rw);
	addr_rw_ = acl_mystrdup(addr_rw);
	if (addr_rd != NULL)
		addr_rd_ = acl_mystrdup(addr_rd);
	else
		addr_rd_ = addr_rw_;
	locker_ = NEW locker(true);
}

hspool::~hspool()
{
	if (addr_rd_ != addr_rw_)
		acl_myfree(addr_rd_);
	acl_myfree(addr_rw_);

	std::list<hsclient*>::iterator it = pool_.begin();
	for (; it != pool_.end(); ++it)
		delete (*it);
	delete locker_;
}

hsclient* hspool::peek(const char* dbn, const char* tbl,
	const char* idx, const char* flds, bool readonly /* = false */)
{
	hsclient* client;
	const char* addr;

	if (readonly)
		addr = addr_rd_;
	else
		addr = addr_rw_;

	locker_->lock();

	// ��˳���ѯ���ϱ��ֶ����������Ӷ���
	std::list<hsclient*>::iterator it = pool_.begin();
	for (; it != pool_.end(); ++it)
	{
		// �����ַ��ƥ�����������ַ����ƥ��
		if (strcmp((*it)->get_addr(), addr) != 0)
			continue;

		// ���Ѿ��򿪵ı���ѯ���ֶ��Ƿ����
		if ((*it)->open_tbl(dbn, tbl, idx, flds, false))
		{
			client = *it;
			pool_.erase(it);
			locker_->unlock();
			return (client);
		}
	}

	// ��ѯ��ַƥ������Ӷ����������һ��ƥ������Ӷ�����
	// ���µı�
	it = pool_.begin();
	std::list<hsclient*>::iterator it_next = it;

	for (; it != pool_.end(); it = it_next)
	{
		++it_next;
		// �����ַ��ƥ�����������ַ����ƥ��
		if (strcmp((*it)->get_addr(), addr) != 0)
			continue;

		client = *it;
		pool_.erase(it); // �����ӳ���ɾ��

		// ���µı�
		if (client->open_tbl(dbn, tbl, idx, flds, true))
		{
			locker_->unlock();
			return (client);
		}

		// ��ʧ�ܣ�����Ҫɾ�������Ӷ���
		delete client;
	}

	locker_->unlock();

	client = NEW hsclient(addr, cache_enable_, retry_enable_);

	if (client->open_tbl(dbn, tbl, idx, flds) == false)
	{
		delete client;
		return (NULL);
	}

	return (client);
}

void hspool::put(hsclient* client)
{
	acl_assert(client);
	locker_->lock();
	pool_.push_back(client);
	locker_->unlock();
}

}  // namespace acl
