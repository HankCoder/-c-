#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>

namespace acl
{

class ACL_CPP_API redis_slot
{
public:
	/**
	 * ���캯��
	 * constructor
	 * @param slot_min {size_t} ��С��ϣ��ֵ
	 *  the min hash-slot
	 * @param slot_max {size_t} ����ϣ��ֵ
	 *  the max hash-slot
	 * @param ip {const char*} ��ǰ redis-server �� IP ��ַ
	 *  the given redis-server's ip
	 * @param port {int} ��ǰ redis-server �ļ����˿�
	 *  the listening port of the given redis-server
	 * 
	 */
	redis_slot(size_t slot_min, size_t slot_max,
		const char* ip, int port);
	redis_slot(const redis_slot& node);

	~redis_slot();

	/**
	 * ��һ�� redis ��ϣ�۴ӽ���������ǰ�����
	 * add a slave slot node to the current node
	 * @param node {redis_slot*} һ���洢��ϣ�۵Ĵӽ��
	 *  the slave slot node
	 */
	redis_slot& add_slave(redis_slot* node);

	/**
	 * ��õ�ǰ��ϣ�۽������дӽ��
	 * get the slave nodes of the current node
	 * @return {const std::vector<redis_slot*>&}
	 */
	const std::vector<redis_slot*>& get_slaves() const
	{
		return slaves_;
	}

	/**
	 * ��õ�ǰ���� IP ��ַ
	 * get the ip of the current node
	 * @return {const char*}
	 */
	const char* get_ip() const
	{
		return ip_;
	}

	/**
	 * ��õ�ǰ���Ķ˿ں�
	 * get the port of the current node
	 * @return {int}
	 */
	int get_port() const
	{
		return port_;
	}

	/**
	 * ��õ�ǰ��ϣ�۽�����Сֵ
	 * get the min hash slot of the current node
	 * @return {size_t}
	 */
	size_t get_slot_min() const
	{
		return slot_min_;
	}

	/**
	 * ��õ�ǰ��ϣ�۽������ֵ
	 * get the max hash slot of the current node
	 * @return {size_t}
	 */
	size_t get_slot_max() const
	{
		return slot_max_;
	}

private:
	size_t slot_min_;
	size_t slot_max_;
	char ip_[128];
	int port_;

	std::vector<redis_slot*> slaves_;
};

} // namespace acl
