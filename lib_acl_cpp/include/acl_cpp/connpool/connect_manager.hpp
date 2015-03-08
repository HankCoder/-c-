#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/locker.hpp"
#include <vector>

struct ACL_EVENT;

namespace acl
{

class connect_pool;
class connect_monitor;

/**
 * connect pool ������������л�ȡ���ӳصȹ���
 */
class ACL_CPP_API connect_manager
{
public:
	connect_manager();
	virtual ~connect_manager();

	/**
	 * ��ʼ�����з����������ӳأ��ú����ڲ����� set �������ÿ����������ӳ�
	 * @param default_addr {const char*} ȱʡ�ķ�������ַ������ǿգ�
	 *  ���ڲ�ѯʱ����ʹ�ô˷�����
	 * @param addr_list {const char*} ���з������б�����Ϊ��
	 *  ��ʽ: IP:PORT:COUNT;IP:PORT:COUNT;IP:PORT;IP:PORT ...
	 *    ��  IP:PORT:COUNT,IP:PORT:COUNT,IP:PORT;IP:PORT ...
	 *  �磺127.0.0.1:7777:50;192.168.1.1:7777:10;127.0.0.1:7778
	 * @param default_count {int} �� addr_list �зָ���ĳ������û��
	 *  COUNT ��Ϣʱ���ô�ֵ
	 *  ע��default_addr �� addr_list ����ͬʱΪ��
	 */
	void init(const char* default_addr, const char* addr_list,
		int default_count);

	/**
	* ��ӷ������Ŀͻ������ӳأ��ú��������ڳ�������ʱ�����ã��ڲ��Զ�����
	 * @param addr {const char*} ��������ַ(ip:port)
	 * @param count {int} ���ӳ���������
	 * @return {connect_pool&} ��������ӵ����ӳض���
	 */
	connect_pool& set(const char* addr, int count);

	/**
	 * �������ӳ�ʧ�ܺ����Ե�ʱ��ʱ������룩���ú��������ڳ�������ʱ�����ã��ڲ��Զ�����
	 * @param n {int} ����ֵ <= 0 ʱ�������ӳس��������������������
	 * @return {void}
	 */
	void set_retry_inter(int n);

	/**
	 * �����ӳؼ�Ⱥ��ɾ��ĳ����ַ�����ӳأ��ú��������ڳ������й�����
	 * �����ã���Ϊ�ڲ����Զ�����
	 * @param addr {const char*} ��������ַ(ip:port)
	 */
	void remove(const char* addr);

	/**
	 * ���ݷ���˵�ַ��ø÷����������ӳ�
	 * @param addr {const char*} redis ��������ַ(ip:port)
	 * @param exclusive {bool} �Ƿ���Ҫ����������ӳ����飬����Ҫ��̬
	 *  �������ӳؼ�Ⱥʱ����ֵӦΪ true
	 * @param restore {bool} ���÷����㱻��Ϊ������ʱ���ò��������Ƿ��Զ�
	 *  ��֮�ָ�Ϊ����״̬
	 * @return {connect_pool*} ���ؿձ�ʾû�д˷���
	 */
	connect_pool* get(const char* addr, bool exclusive = true,
		bool restore = false);

	/**
	 * �����ӳؼ�Ⱥ�л��һ�����ӳأ��ú���������ѭ��ʽ�����ӳؼ����л�ȡһ��
	 * ��˷����������ӳأ��Ӷ���֤����ȫ�ľ����ԣ��ú����ڲ����Զ������ӳع���
	 * ���м���
	 * ���⣬�ú���Ϊ��ӿڣ���������ʵ���Լ�����ѭ��ʽ
	 * @return {connect_pool*} ����һ�����ӳأ�����ָ����Զ�ǿ�
	 */
	virtual connect_pool* peek();

	/**
	 * �����ӳؼ�Ⱥ�л��һ�����ӳأ��ú������ù�ϣ��λ��ʽ�Ӽ����л�ȡһ��
	 * ��˷����������ӳأ�����������ش��麯���������Լ��ļ�Ⱥ��ȡ��ʽ
	 * ���麯���ڲ�ȱʡ���� CRC32 �Ĺ�ϣ�㷨��
	 * @param key {const char*} ��ֵ�ַ����������ֵΪ NULL�����ڲ�
	 *  �Զ��л�����ѭ��ʽ
	 * @param exclusive {bool} �Ƿ���Ҫ����������ӳ����飬����Ҫ��̬
	 *  �������ӳؼ�Ⱥʱ����ֵӦΪ true
	 * @return {connect_pool*} ����һ�����õ����ӳأ�����ָ����Զ�ǿ�
	 */
	virtual connect_pool* peek(const char* key, bool exclusive = true);

	/**
	 * ���û������� peek ����ʱ�����Ե��ô˺��������ӳع�����̼���
	 */
	void lock();

	/**
	 * ���û������� peek ����ʱ�����Ե��ô˺��������ӳع�����̼���
	 */
	void unlock();

	/**
	 * ������еķ����������ӳأ������ӳ��а���ȱʡ�ķ������ӳ�
	 * @return {std::vector<connect_pool*>&}
	 */
	std::vector<connect_pool*>& get_pools()
	{
		return pools_;
	}

	/**
	 * ������ӳؼ��������ӳض���ĸ���
	 * @return {size_t}
	 */
	size_t size() const
	{
		return pools_.size();
	}

	/**
	 * ���ȱʡ�ķ��������ӳ�
	 * @return {connect_pool*} ������ init ������ default_addr Ϊ��ʱ
	 *  �ú������� NULL
	 */
	connect_pool* get_default_pool()
	{
		return default_pool_;
	}

	/**
	 * ��ӡ��ǰ���� redis ���ӳصķ�����
	 */
	void statistics();

	/**
	 * ������̨����������̼߳���������ӳ�����״̬
	 * @param check_inter {int} ����ʱ����(��)
	 * @param conn_timeout {int} ���ӷ������ĳ�ʱʱ��(��)
	 */
	void start_monitor(int check_inter = 1, int conn_timeout = 10);

	/**
	 * ֹͣ��̨����߳�
	 * @param graceful {bool} �Ƿ��ڹرռ���߳�ʱ��Ҫ�ȴ����еļ�����ӹرպ�
	 *  �ŷ��أ������ӳؼ�Ⱥ����Ϊ���̿ռ��ڲ����η������ͷ�ʱ�����ֵ������Ϊ false
	 *  �Ӷ�ʹ����߳̿����˳�������Ӧ�õȴ����м�����ӹرպ���ʹ����߳��˳�
	 */
	void stop_monitor(bool graceful = true);

	/**
	 * ����ĳ�����ӳط���Ĵ��״̬���ڲ����Զ�����
	 * @param addr {const char*} ��������ַ����ʽ��ip:port
	 * @param alive {bool} �÷������Ƿ�����
	 */
	void set_pools_status(const char* addr, bool alive);

protected:
	/**
	 * ���麯�����������ʵ�ִ˺��������������ӳض���
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 * @param addr {const char*} ������������ַ����ʽ��ip:port
	 * @param count {int} ���ӳصĴ�С����
	 * @return {connect_pool*} ���ش��������ӳض���
	 */
	virtual connect_pool* create_pool(const char* addr,
		int count, size_t idx) = 0;

private:
	string default_addr_;			// ȱʡ�ķ����ַ
	connect_pool* default_pool_;		// ȱʡ�ķ������ӳ�
	std::vector<connect_pool*> pools_;	// ���еķ������ӳ�
	size_t service_idx_;			// ��һ��Ҫ���ʵĵ��±�ֵ
	locker lock_;				// ���� pools_ ʱ�Ļ�����
	int  stat_inter_;			// ͳ�Ʒ������Ķ�ʱ�����
	int  retry_inter_;			// ���ӳ�ʧ�ܺ����Ե�ʱ����
	connect_monitor* monitor_;		// ��̨����߳̾��

	// ���ó�ȱʡ����֮��ķ�������Ⱥ
	void set_service_list(const char* addr_list, int count);
};

} // namespace acl
