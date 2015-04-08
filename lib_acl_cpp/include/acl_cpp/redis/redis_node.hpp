#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <utility>
#include "acl_cpp/stdlib/string.hpp"

namespace acl
{

/**
 * ������Ҫ���� redis_cluster �������ȡ�йؼ�Ⱥ redis �����Ϣ
 * this class is mainly used for redis_cluster command class to
 * get some information about the nodes in redis cluster
 */
class ACL_CPP_API redis_node
{
public:
	/**
	 * ���캯��
	 * constructor
	 * @param id {const char*} ��Ⱥ��ĳ�� redis ����Ψһ��ʶ��
	 *  the unique ID for one redis node in the redis cluster
	 * @param addr {const char*} ��Ⱥ��ĳ�� redis ���ļ�����ַ
	 *  the listening addr for one redis node in redis cluster
	 */
	redis_node(const char* id, const char* addr);
	redis_node(const redis_node& node);
	~redis_node();

	/**
	 * �������Ϊ�ӽ��ʱ�����õ�ǰ���������
	 * setting current slave node's master node
	 * @param master {const redis_node*} ��������
	 *  the redis master node of the current slave in cluster
	 */
	void set_master(const redis_node* master);

	/**
	 * �������Ϊ�ӽ��ʱ�����õ�ǰ����������ʶ��
	 * setting current node's master node when the node is slave node
	 * @param id {const char*} �����Ψһ��ʶ��
	 *  the unique ID of the master node
	 */
	void set_master_id(const char* id);

	/**
	 * �������Ϊ�����ʱ�����һ���ӽ��
	 * add one slave node to the current node if it's one master node
	 * @return {bool} ����Ƿ�ɹ������ӽ���Ѿ������ڵ�ǰ�����ʱ�򷵻� false
	 *  false will be returned when the slave to be added is already
	 *  existing in the current master node
	 */
	bool add_slave(redis_node* slave);

	/**
	 * �������Ϊ�����ʱ�����ݽ��Ψһ��ʶ��ɾ��һ���ӽ��
	 * when the current node is a master node, this function will
	 * remove one slave node by the unique ID
	 * @param id {const char*} redis ���Ψһ��ʶ��
	 *  the unique ID of the redis node
	 * @return {const redis_node*} ���ر�ɾ���Ĵӽ�㣬����������򷵻� NULL
	 *  the slave node according to the ID will be returned, and if
	 *  not exists NULL will be returned
	 */
	redis_node* remove_slave(const char* id);

	/**
	 * �������Ϊ�����ʱ����ձ��������дӽ��
	 * clear all the slave nodes in the current master node
	 * @param free_all {bool} �Ƿ���Ҫͬʱ�ͷ���Щ�ӽ��
	 *  if freeing the all slave nodes memory meanwhile
	 */
	void clear_slaves(bool free_all = false);

	/**
	 * �������Ϊ�����ʱ����ӹ�ϣ�۷�Χ
	 * add hash-slots range to the master node
	 * @param min {size_t} ��ϣ�۷�Χ����ʼֵ
	 *  the begin hash-slot of the slots range
	 * @param max {size_t} ��ϣ�۷�Χ�Ľ���ֵ
	 *  the end hash-slot of the slots range
	 */
	void add_slot_range(size_t min, size_t max);

	/**
	 * �������Ϊ�����ʱ�����������Ĺ�ϣ�۷�Χ����Ϊ�ӽ��ʱ�������Ӧ��
	 * �����Ĺ�ϣ�۷�Χ
	 * @return {const std::vector<std::pair<size_t, size_t> >&}
	 */
	const std::vector<std::pair<size_t, size_t> >& get_slots() const;

	/**
	 * �������Ϊ�ӽ��ʱ����øôӽ�����������
	 * get the current slave's master node
	 * @return {const redis_node*}
	 */
	const redis_node* get_master() const
	{
		return master_;
	}

	/**
	 * �������Ϊ�ӽ��ʱ����øôӽ���Ӧ�������� ID ��ʶ
	 * when the current node is slave, getting its master's ID
	 * @return {const char*}
	 */
	const char* get_master_id() const
	{
		return master_id_.c_str();
	}

	/**
	 * �������Ϊ�����ʱ����ø����������дӽ��
	 * getting all the slaves of the master
	 * @return {const std::vector<redis_node*>*}
	 */
	const std::vector<redis_node*>* get_slaves() const
	{
		return (master_ && master_ == this) ? &slaves_ : NULL;
	}

	/**
	 * �жϵ�ǰ����Ƿ�Ϊ��Ⱥ�е�һ�������
	 * check if the current node is a master in the redis cluster
	 * @return {bool}
	 */
	bool is_master() const
	{
		return master_ == this;
	}

	/**
	 * ��õ�ǰ���� ID ��ʶ
	 * get the unique ID of the current node, set in constructor
	 * @return {const char*}
	 */
	const char* get_id() const
	{
		return id_.c_str();
	}

	/**
	 * ��õ�ǰ���ļ�����ַ
	 * get the listening addr of the current node, set in constructor
	 * @reutrn {const char*}
	 */
	const char* get_addr() const
	{
		return addr_.c_str();
	}

private:
	string id_;
	string addr_;
	const redis_node* master_;
	string master_id_;
	std::vector<redis_node*> slaves_;
	std::vector<std::pair<size_t, size_t> > slots_;
};

} // namespace acl
