#pragma once
#include <stdarg.h>
#include <vector>
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/socket_stream.hpp"

struct ACL_ARGV;

namespace acl {

/**
 * ��Ϣ ID �Ŵ� 1 ��ʼ����(�μ� beanstalkd �� job.c Դ�����е��������ݣ�
 *     static uint64 next_id = 1; �� make_job_with_id() �е�
 *     if (id) {
 *         j->r.id = id;
 *         if (id >= next_id) next_id = id + 1;
 *     } else {
 *         j->r.id = next_id++;
 *     }
 * ��Ϣ���ȼ� pri ��ȡֵ��ΧΪ 0 ~ 4,294,968,295(����޷�������ֵ)��ֵԽС
 * �����ȼ���Խ�ߣ���߼���Ϊ 0 ��
 * ��Ϣ��Ĭ����󳤶�Ϊ 65,535(����޷��� short ֵ)����ֵ���������� beanstalkd ָ��
 * ����������ο�����Ŀ doc/ Ŀ¼�µ� <beanstalkЭ�����.pdf>
 * �����е���������ڲ����Զ��������Ӳ����������������У����֮ǰ������ watch �� use
 * ���У�����Զ�������Щ������̣�����һ����˵������ʽ���� open ���̣����û�������
 * close �����󣬲����Ͽ����� beanstalkd �����������ӣ�ͬʱ�������������д洢��
 * use �� watch ����
 */
class ACL_CPP_API beanstalk
{
public:
	/**
	 * ���캯��
	 * @param addr {const char*} beanstalkd ��ַ����ʽ��ip:port/domain:port
	 * @param conn_timeout {int} ���ӷ������ĳ�ʱʱ��(��)
	 * @param retry {bool} ������Ӷ����Ƿ��Զ�����
	 */
	beanstalk(const char* addr, int conn_timeout, bool retry = true);
	~beanstalk();

	/////////////////////////////////////////////////////////////////////
	// �����ߵ��õĽӿ�

	/**
	 * ѡ�����õķ��͹ܵ�
	 * @param tube {const char*} �ܵ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool use(const char* tube);

	/**
	 * ����ѡ�ܵ���ȱʡ�����з�����Ϣ
	 * @param data {const void*} ��Ϣ���ݵ�ַ�������Ƕ�����
	 * @param len {size_t} data �����峤��
	 * @param pri {unsigned} ���ȼ���ֵԽС�����ȼ�Խ��
	 * @param delay {unsigned} ��ʾ��job����ready������Ҫ�ȴ�������
	 * @param ttr {unsigned} ��ʾ����һ��workerִ�и���Ϣ������
	 * @return {unsigned long long} �����������Ϣ����Ϣ�ţ�
	 *  �������ֵ > 0 ���ʾ��ӳɹ����� == 0 ���ʾ���ʧ��
	 *  (�鿴 beanstalkd Դ�룬���Կ�����Ϣ�Ŵ� 1 ��ʼ����)
	 */
	unsigned long long put(const void* data, size_t len,
		unsigned pri = 1024, unsigned delay = 0, unsigned ttr = 60);

	/**
	 * �Ը�ʽ�ַ�����ʽ����ѡ�ܵ���ȱʡ�����з�����Ϣ
	 * @param pri {unsigned} ���ȼ���ֵԽС�����ȼ�Խ��
	 * @param delay {unsigned} ��ʾ��job����ready������Ҫ�ȴ�������
	 * @param ttr {unsigned} ��ʾ����һ��workerִ�и���Ϣ������
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @return {unsigned long long} �����������Ϣ����Ϣ�ţ�
	 *  �������ֵ > 0 ���ʾ��ӳɹ����� == 0 ���ʾ���ʧ��
	 *  (�鿴 beanstalkd Դ�룬���Կ�����Ϣ�Ŵ� 1 ��ʼ����)
	 */
	unsigned long long format_put(unsigned pri, unsigned delay, unsigned ttr,
		const char* fmt, ...) ACL_CPP_PRINTF(5, 6);

	unsigned long long vformat_put(const char* fmt, va_list ap,
		unsigned pri = 1024, unsigned delay = 0, unsigned ttr = 60);

	/**
	 * �Ը�ʽ�ַ�����ʽ����ѡ�ܵ���ȱʡ�����з�����Ϣ�����е�
	 * �� pri, delay, ttr ����Ĭ��ֵ
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @return {unsigned long long} �����������Ϣ����Ϣ�ţ�
	 *  �������ֵ > 0 ���ʾ��ӳɹ����� == 0 ���ʾ���ʧ��
	 *  (�鿴 beanstalkd Դ�룬���Կ�����Ϣ�Ŵ� 1 ��ʼ����)
	 */
	unsigned long long format_put(const char* fmt, ...) ACL_CPP_PRINTF(2, 3);

	/////////////////////////////////////////////////////////////////////
	// �����ߵ��õĽӿ�

	/**
	 * ѡ���ȡ��Ϣ�Ĺܵ�����������ع����б��У�
	 * �����ñ���������ʹ��ȱʡ�Ĺܵ�(default)
	 * @param tube {const char*} ��Ϣ�ܵ�����
	 * @return {unsigned} ����ֵΪ��ע����Ϣ�ܵ���, ����ֵ > 0 ���ʾ�ɹ�
	 */
	unsigned watch(const char* tube);

	/**
	 * ȡ����ע(watch)һ��������Ϣ�Ĺܵ�(tube)
	 * @param tube {const char*} ��Ϣ�ܵ�����
	 * @return {unsigned} ����ֵΪʣ�����Ϣ��ע�ܵ���, ����ֵ > 0 ���ʾ
	 *  �ɹ�(������Ҫ��עһ��ȱʡ��Ϣ�ܵ���������ȷ����¸÷���ֵ����Ϊ 1)��
	 *  �������ֵΪ 0 ��˵������Ĺܵ���δ����ע��ȡ����עʧ��
	 */
	unsigned ignore(const char* tube);

	/**
	 * ȡ����ע���еĽ�����Ϣ�Ĺܵ�
	 * @return {unsigned} ����ֵΪʣ�����Ϣ��ע�ܵ���, ����ֵ > 0 ���ʾ
	 *  �ɹ�(������Ҫ��עһ��ȱʡ��Ϣ�ܵ���������ȷ����¸÷���ֵ����Ϊ 1)��
	 *  ���� 0 ��ʾȡ����עʧ��
	 */
	unsigned ignore_all();

	/**
	 * ����Ϣ����ܵ��л�ȡһ����Ϣ��������ɾ����Ϣ����������
	 * �ȴ���ʱ�������Ϊ -1 ����Զ�����ȴ���Ϣ����
	 * @param buf {string&} �洢��õ�һ����Ϣ�������ڲ�������ոû�����
	 * @param timeout {int} �ȴ����з�����������Ϣ�ĳ�ʱֵ����Ϊ -1
	 *  ʱ���������ڵȴ����� > 0 ʱ�����ڸ�ʱ������û����Ϣ���򷵻أ�
	 *  �� == 0 ʱ������������һ����Ϣ�򷵻س�ʱ
	 * @return {unsigned long long} ������ȡ�õ���Ϣ�ţ�������ֵ > 0
	 *  ��ʾ��ȷȡ��һ����Ϣ������˵�������ʱû����Ϣ���ã����е�
	 *  ���� 0 ʱ��������� get_error() ��õ�����Ϊ TIMED_OUT ���ʾ
	 *  ��ʱ�ˣ���Ϊ DEADLINE_SOON ʱ���ʾ����Ϣ�Ѿ�����ȡ���ڹ涨�� ttr
	 *  (����ʱ����) û�б� delete_id
	 */
	unsigned long long reserve(string& buf, int timeout = -1);

	/**
	 * �Ӷ��з�������ɾ��ָ�� ID �ŵ���Ϣ
	 * @param id {unsigned long long} ��Ϣ��
	 * @return {bool} �Ƿ�ɹ�ɾ��
	 */
	bool delete_id(unsigned long long id);

	/**
	 * ��һ���Ѿ�����ȡ����Ϣ���·Ż�ready����(����job״̬��Ϊ "ready")��
	 * �ø���Ϣ���Ա��������ӻ��
	 * @param id {unsigned long long} ��Ϣ��
	 * @param pri {unsigned} ���ȼ���
	 * @param delay {unsigned} �ڸ���Ϣ������ready����֮ǰ��Ҫ�ȴ�������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool release(unsigned long long id, unsigned pri = 1024,
		unsigned delay = 0);

	/**
	 * ��һ����Ϣ��״̬��Ϊ "buried", Buried ��Ϣ������һ��FIFO�������У�
	 * �ڿͻ��˵���kick����֮ǰ����Щ��Ϣ�����ᱻ����˴���
	 * @param id {unsigned long long} ��Ϣ��
	 * @param pri {unsigned int} ���ȼ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool bury(unsigned long long id, unsigned pri = 1024);

	/**
	 * ����һ��worker������һ����Ϣ��ȡ����ִ�е�ʱ�䡣�������Щ��Ҫ
	 * ��ʱ����ɵ���Ϣ�Ƿǳ����õģ���ͬʱҲ��������TTR�����ƽ�һ����Ϣ
	 * ��һ���޷���ɹ�����worker�����ߡ�һ��worker����ͨ��������������
	 * �����������ִ�и�job (���磺���յ�DEADLINE_SOON�ǿ��Է���������)
	 * @param id {unsigned long long} ��Ϣ��
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool touch(unsigned long long id);

	/////////////////////////////////////////////////////////////////////
	// �����ӿ�

	/**
	 * ���� beanstalkd ��������ͨ������²���Ҫ��ʾ�ص��øú�������������
	 * ���Զ�������Ҫ�Զ����ñ�����
	 * @return {bool}  ��ɹ�
	 */
	bool open();

	/**
	 * ��ʾ�ر��� beanstalkd �����ӣ�������ʵ������ʱ�᳢�Ե��ùرչ��̣�
	 * ���ñ�������������ڲ��� tube_used_ �� tubes_watched_ �ᱻ�ͷ�
	 */
	void close();

	/**
	 * ��ʾ֪ͨ beanstalkd �������˳�����(�������յ��������������ر�����)
	 */
	void quit();

	/**
	 * ��ȡ��Ϣ������ָ������Ϣ�ŵ�����
	 * @param buf {string&} �����Ϣ������洢������Ϣ�������ڲ�������ոû�����
	 * @param id {unsigned long long} ָ������Ϣ��
	 * @return {unsigned long long} ����ȡ�õ� ready ״̬��Ϣ�ţ�
	 *  ������ֵ > 0 ��˵��ȡ����һ����Ϣ�������ʾû����Ϣ����
	 */
	unsigned long long peek(string& buf, unsigned long long id);

	/**
	 * ��õ�ǰ��ע (watch) �ܵ��е�һ�� ready ״̬��Ϣ��
	 * �����Ϣ������Ҳ��������
	 * @param buf {string&} �����Ϣ������洢������Ϣ�������ڲ�������ոû�����
	 * @return {unsigned long long} ����ȡ�õ� ready ״̬��Ϣ�ţ�
	 *  ������ֵ > 0 ��˵��ȡ����һ����Ϣ�������ʾû����Ϣ����
	 */
	unsigned long long peek_ready(string& buf);

	/**
	 * ��õ�ǰ��ע (watch) �ܵ��е�һ�� delayed ״̬��Ϣ��
	 * �����Ϣ������Ҳ��������
	 * @param buf {string&} �����Ϣ������洢������Ϣ�������ڲ�������ոû�����
	 * @return {unsigned long long} ����ȡ�õ� delayed ״̬��Ϣ�ţ�
	 *  ������ֵ > 0 ��˵��ȡ����һ����Ϣ�������ʾû����Ϣ����
	 */
	unsigned long long peek_delayed(string& buf);

	/**
	 * ��õ�ǰ��ע (watch) �ܵ��е�һ�� buried ״̬��Ϣ��
	 * �����Ϣ������Ҳ��������
	 * @param buf {string&} �����Ϣ������洢������Ϣ�������ڲ�������ոû�����
	 * @return {unsigned long long} ����ȡ�õ� buried ״̬��Ϣ�ţ�
	 *  ������ֵ > 0 ��˵��ȡ����һ����Ϣ�������ʾû����Ϣ����
	 */
	unsigned long long peek_buried(string& buf);

	/**
	 * ������ֻ����Ե�ǰ����ʹ�õ�tubeִ�У����� buried
	 * ���� delayed ״̬����Ϣ�ƶ��� ready ����
	 * @param n {unsigned} ��ʾÿ�� kick ��Ϣ�����ޣ�
	 *  ����˽���� kick ����Ϣ����
	 * @return {int} ��ʾ����kick����������Ϣ����Ŀ������ -1 ��ʾ����
	 */
	int  kick(unsigned n);

	/**
	 * ��ÿͻ���ǰ����ʹ�õ���Ϣ�ܵ�
	 * @param buf {string&} �洢��ǰʹ�õ���Ϣ�ܵ��������ڲ�������ոû�����
	 * @return {bool} �Ƿ�ɹ����
	 */
	bool list_tube_used(string&buf);

	/**
	 * ����Ѿ����ڵ�������Ϣ�ܵ�(tube)���б���
	 * @param buf {string&} �洢����������ڲ�������ոû�����
	 * @return {bool} �Ƿ�ɹ����
	 */
	bool list_tubes(string& buf);

	/**
	 * ��õ�ǰ��ע(watch)����Ϣ�ܵ��ļ���
	 * @param buf {string&} �洢����������ڲ�������ոû�����
	 * @return {bool} �Ƿ�ɹ����
	 */
	bool list_tubes_watched(string& buf);

	/**
	 * ����ʱ������ͣ��ָ����Ϣ�ܵ�(tube)�л�ȡ��Ϣ
	 * @param tube {const char*} ��Ϣ�ܵ���
	 * @param delay {unsigned} ָ��ʱ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool pause_tube(const char* tube, unsigned delay);

	/////////////////////////////////////////////////////////////////////
	// �����ӿ�
	const char* get_error() const
	{
		return errbuf_.c_str();
	}

	socket_stream& get_conn()
	{
		return conn_;
	}

	/**
	 * ���ع��캯���� beanstalkd �ķ�������ַ����ʽ��ip:port
	 * @return {const char*} ��Զ���طǿյ� beanstalkd ��������ַ
	 */
	const char* get_addr() const
	{
		return addr_;
	}

private:
	char* addr_;
	int   timeout_;
	bool  retry_;
	string  errbuf_;
	char* tube_used_;
	std::vector<char*> tubes_watched_;
	socket_stream conn_;
	unsigned long long peek_fmt(string& buf, const char* fmt, ...)
		ACL_CPP_PRINTF(3, 4);
	bool list_tubes_fmt(string& buf, const char* fmt, ...)
		ACL_CPP_PRINTF(3, 4);

	unsigned ignore_one(const char* tube);
	bool beanstalk_open();
	bool beanstalk_use();
	unsigned beanstalk_watch(const char* tube);
	ACL_ARGV* beanstalk_request(const string& cmdline,
		const void* data = NULL, size_t len = 0);
};

} // namespace acl
