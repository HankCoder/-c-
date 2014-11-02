#pragma once
#include <list>

namespace acl_min
{

class connect_manager;
class connect_client;
class locker;

/**
 * �ͻ������ӳ��࣬ʵ�ֶ����ӳصĶ�̬��������Ϊ�����࣬��Ҫ����ʵ��
 * ���麯�� create_connect ���ڴ��������˵�һ�����ӣ����������
 * ��������ͨ�� set_delay_destroy() �����ӳ�����ʱ�����������ʵ��
 * �����Ƕ�̬����
 */
class connect_pool
{
public:
	/**
	 * ���캯��
	 * @param addr {const char*} ������������ַ����ʽ��ip:port(domain:port)
	 * @param count {int} ���ӳ�������Ӹ�������
	 * @param idx {size_t} �����ӳض����ڼ����е��±�λ��(�� 0 ��ʼ)
	 */
	connect_pool(const char* addr, int count, size_t idx = 0);

	/**
	 * ���൱������������ʱ����ʵ��ӦΪ��̬����
	 */
	virtual ~connect_pool();

	/**
	 * �������ӳ��쳣������ʱ����
	 * @param retry_inter {int} �����ӶϿ��������ٴδ����ӵ�ʱ����(��)��
	 *  ����ֵ <= 0 ʱ��ʾ�������ӶϿ����������������������볬����ʱ�������
	 *  ����Ͽ�������δ���ñ�����ʱ���ڲ�ȱʡֵΪ 1 ��
	 * @retrun {connect_pool&}
	 */
	connect_pool& set_retry_inter(int retry_inter);

	/**
	 * �������ӳ��п������ӵĿ�����������
	 * @param ttl {time_t} �������ӵ��������ڣ�����ֵ < 0 ���ʾ�������Ӳ����ڣ�
	 *  == 0 ʱ��ʾ���̹��ڣ�> 0 ��ʾ���и�ʱ��κ󽫱��ͷ�
	 * @return {connect_pool&}
	 */
	connect_pool& set_idle_ttl(time_t ttl);

	/**
	 * �����ӳ��г����Ի�ȡһ�����ӣ��������������á����ϴη���������쳣ʱ����
	 * δ���ڻ����ӳ����Ӹ����ﵽ���������򽫷��� NULL��������һ���µ����������
	 * ����ʱʧ�ܣ�������ӳػᱻ��Ϊ������״̬
	 * @return {connect_client*} ���Ϊ�����ʾ�÷��������ӳض��󲻿���
	 */
	connect_client* peek();

	/**
	 * �ͷ�һ�����������ӳ��У��������ӳض�Ӧ�ķ����������û������ϣ���رո�����ʱ��
	 * ������ӽ��ᱻֱ���ͷ�
	 * @param conn {redis_client*}
	 * @param keep {bool} �Ƿ���Ը����ӱ��ֳ�����
	 */
	void put(connect_client* conn, bool keep = true);

	/**
	 * ������ӳ��п��е����ӣ������ڵ������ͷŵ�
	 * @param ttl {time_t} ����ʱ����������ֵ�����ӽ����ͷ�
	 * @param exclusive {bool} �ڲ��Ƿ���Ҫ����
	 * @return {int} ���ͷŵĿ������Ӹ���
	 */
	int check_idle(time_t ttl, bool exclusive = true);

	/**
	 * �������ӳصĴ��״̬
	 * @param ok {bool} ���ø������Ƿ�����
	 */
	void set_alive(bool ok /* true | false */);

	/**
	 * �жϱ����ӳ��Ƿ����
	 * @return {bool}
	 */
	bool aliving(void) const
	{
		return alive_;
	}

	/**
	 * ��ȡ���ӳصķ�������ַ
	 * @return {const char*} ���طǿյ�ַ
	 */
	const char* get_addr() const
	{
		return addr_;
	}

	/**
	 * ��ȡ���ӳ��������������
	 * @return {int}
	 */
	int get_count() const
	{
		return count_;
	}

	/**
	 * ��ø����ӳض��������ӳؼ����е��±�λ��
	 * @return {size_t}
	 */
	size_t get_idx() const
	{
		return idx_;
	}

	/**
	 * ����ͳ�Ƽ�����
	 * @param inter {int} ͳ�Ƶ�ʱ����
	 */
	void reset_statistics(int inter);

	/**
	 * ��ȡ�����ӳ��ܹ���ʹ�õĴ���
	 */
	unsigned long long get_total_used() const
	{
		return total_used_;
	}

	/**
	 * ��ȡ�����ӳص�ǰ��ʹ�ô���
	 * @return {unsigned long long}
	 */
	unsigned long long get_current_used() const
	{
		return current_used_;
	}

protected:
	virtual connect_client* create_connect() = 0;

	friend class connect_manager;

	/**
	 * ���ø����ӳض���Ϊ�ӳ������٣����ڲ��������ü���Ϊ 0 ʱ����������
	 */
	void set_delay_destroy();

protected:
	bool  alive_;				// �Ƿ�������
	bool  delay_destroy_;			// �Ƿ��������ӳ�������
	// ������ķ������Ŀ������Ե�ʱ���������������ӳض����ٴα����õ�ʱ����
	int   retry_inter_;
	time_t last_dead_;			// �����ӳض����ϴβ�����ʱ��ʱ���

	size_t idx_;				// �����ӳض����ڼ����е��±�λ��
	char  addr_[256];			// ���ӳض�Ӧ�ķ�������ַ��IP:PORT
	int   max_;				// ���������
	int   count_;				// ��ǰ��������
	time_t idle_ttl_;			// �������ӵ���������
	time_t last_check_;			// �ϴμ��������ӵ�ʱ���
	int   check_inter_;			// ���������ӵ�ʱ����

	locker* lock_;				// ���� pool_ ʱ�Ļ�����
	unsigned long long total_used_;		// �����ӳص����з�����
	unsigned long long current_used_;	// ĳʱ����ڵķ�����
	time_t last_;				// �ϴμ�¼��ʱ���
	std::list<connect_client*> pool_;	// ���ӳؼ���
};

} // namespace acl_min
