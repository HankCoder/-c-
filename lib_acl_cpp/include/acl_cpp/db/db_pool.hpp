#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>

namespace acl {

class db_handle;
class locker;

class ACL_CPP_API db_pool
{
public:
	/**
	 * ���ݿ⹹�캯��
	 * @param dblimit {int} ���ݿ����ӳص��������������
	 */
	db_pool(int dblimit = 64);
	virtual ~db_pool();

	/**
	 * �����ݿ����ӳػ��һ�����ݿ���󣬸ú����������ݿ�����û������
	 * db_handle::open �����ݿ����ӣ�Ȼ����ʹ�ø����ݿ����Ӷ���
	 * ����������� db_pool->put(db_handle*) �����ӹ黹�����ݿ����ӳأ�
	 * �ɸú�����õ����Ӿ������ delete�������������ӳص��ڲ�����������
	 * @return {db_handle*} ���ؿգ����ʾ����
	 */
	db_handle* peek();

	/**
	 * �����ݿ����ӳػ��һ�����ݿ���󣬲���Ҫ������ݿ����ӣ����û�����
	 * ��ʽ���ٵ��� db_handle::open ���̣�
	 * ����������� db_pool->put(db_handle*) �����ӹ黹�����ݿ����ӳأ�
	 * �ɸú�����õ����Ӿ������ delete�������������ӳص��ڲ�����������
	 * @param charset {const char*} �����ݿ�ʱʹ�õ��ַ���
	 * @return {db_handle*} ���ݿ����Ӷ��󣬷��ؿձ�ʾ����
	 */
	db_handle* peek_open(const char* charset = "utf8");

	/**
	 * �����ݿ����ӷŻ������ӳ��У��������ݿ����ӳ��л������
	 * ��������Ӧ��ͨ���ú����Żأ�����ֱ�� delete����Ϊ����
	 * �ᵼ�����ӳص��ڲ�������������
	 * @param conn {db_handle*} ���ݿ����Ӿ���������Ӿ������
	 *  ���� peek �����ģ�Ҳ���Ե�����̬������
	 * @param keep {bool} �黹�����ӳص����ݿ����Ӿ���Ƿ����
	 *  �������ӣ���������ڲ����Զ�ɾ�������Ӿ��
	 */
	void put(db_handle* conn, bool keep = true);

	/**
	 * �����ݿ����ӳ��еĹ��������ͷŵ����Լ��ٶԺ�����ݿ��������
	 * @param ttl {time_t} �����ݿ����ӵĿ���ʱ����ڵ��ڴ�ֵʱ
	 *  �����ӽ����ͷţ��� idle Ϊ 0 ʱ����Ҫ�ͷ����е����ݿ�����;
	 *  ��Ϊ -1 ʱ�򲻽����ͷ����Ӳ���
	 * @param exclusive {bool} �ڲ��Ƿ���Ҫ�ӻ�����
	 * @return {int} ���ͷŵ����ݿ����ӵĸ���(>= 0)
	 */
	int check_idle(time_t ttl, bool exclusive = true);

	/**
	 * �������ӳ��п������ӵ��������ڣ���ͨ�����������������ݿ��������
	 * ���������ں�(idle >= 0)�����û����� db_pool::put ʱ���Զ����
	 * �������Ӳ��رգ������ڲ���������һֱ����
	 * @param idle {int} ���ӳ��п������ӵ����Ӵ��ʱ�䣬����ֵ
	 *  Ϊ -1 ʱ����ʾ������������ӣ�Ϊ 0 ʱ��ʾ�ڲ��������κγ�����
	 * @return {db_pool&}
	 */
	db_pool& set_idle(int idle);

	/**
	 * �����Զ����������ӵ�ʱ������ȱʡֵΪ 30 ��
	 * @param n {int} ʱ����
	 * @return {db_pool&}
	 */
	db_pool& set_check_inter(int n);

	/**
	 * ��õ�ǰ���ݿ����ӳص��������������
	 * @return {int}
	 */
	int get_dblimit() const
	{
		return dblimit_;
	}

	/**
	 * ��õ�ǰ���ݿ����ӳص�ǰ��������
	 * @return {int}
	 */
	int get_dbcount() const
	{
		return dbcount_;
	}
protected:
	/**
	 * ���麯�������� DB �ķ���
	 * @return {db_handle*}
	 */
	virtual db_handle* create() = 0;
private:
	std::list<db_handle*> pool_;
	int   dblimit_;  // ���ӳص��������������
	int   dbcount_;  // ��ǰ�Ѿ��򿪵�������
	locker* locker_;
	char  id_[128];  // ����ʵ����Ψһ ID ��ʶ
	time_t ttl_;     // ���ӳ��п������ӱ��ͷŵĳ�ʱֵ
	time_t last_check_;  // �ϴμ��������ӵ�ʱ���
	int    check_inter_; // ���������ӵ�ʱ����

	// ���ñ�ʵ����Ψһ ID ��ʶ
	void set_id();
};

} // namespace acl
