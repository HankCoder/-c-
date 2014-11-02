#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>
#include <vector>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/hsocket/hsproto.hpp"

struct ACL_ARGV;

namespace acl {

class hsrow;
class hstable;

class ACL_CPP_API hsclient
{
public:
	/**
	 * ���캯��
	 * @param addr {const char*} handlersocket ����� Mysql �ϵļ�����ַ��
	 * @param cache_enable {bool} �ڲ��Ƿ������л��湦��
	 * @param retry_enable {bool} ����Ϊ����ԭ�����ʱ�Ƿ���Ҫ����
	 *  ��ʽΪ��ip:port
	 */
	hsclient(const char* addr, bool cache_enable = true, bool retry_enable = true);
	~hsclient();

	/**
	 * ��ѯ�������ֶ�ֵƥ��Ľ��
	 * @param values {const char*[]} ƥ���ֶ�ֵ���飬�ֶ�ֵ�ļ���˳��Ӧ�� open
	 *  ������ flds �и����ֶε�˳����ͬ
	 * @param num {int} values ���鳤�ȣ���ֵ��Ӧ�������캯���� flds �����е�
	 *  �ֶθ���
	 * @param cond {const char*} ƥ������������Ϊ��
	 *  = ����; >= ���ڵ���; > ����; < С��; <= С�ڵ���
	 * @param nlimit {int} ������������ƣ�0 ��ʾ�����Ƹ���
	 * @param noffset {int} �������ʼλ��(0��ʾ�ӵ�һ�������ʼ)
	 * @return {const std::verctor<hsrow*>&} ���ؽ����
	 */
	const std::vector<hsrow*>& get(const char* values[], int num,
		const char* cond = "=", int nlimit = 0, int noffset = 0);

	/**
	 * ��ѯ�������ֶ�ֵƥ��Ľ��
	 * @param first_value {const char*} Ϊ��Ӧ�ڹ��캯���� flds �ֶμ���
	 *  �ĵ�һ���ֶε��ֶ�ֵ
	 * @param ... {const char*} �����б����һ������Ϊ NULL ��ʾ����
	 * @return {const std::verctor<hsrow*>&} ���ؽ����
	 */
	const std::vector<hsrow*>& get(const char* first_value, ...)
		ACL_CPP_PRINTF(2, 3);

	/**
	 * �������ݿ����ƥ���ֶε���ֵ
	 * @param values {const char*[]} ƥ���ֶ�ֵ���飬�ֶ�ֵ�ļ���˳��Ӧ�� open
	 *  ������ flds �и����ֶε�˳����ͬ
	 * @param num {int} values ���鳤�ȣ���ֵ��Ӧ�������캯���� flds �����е�
	 *  �ֶθ���
	 * @param to_values {cosnt *[]} ƥ���ֶ���ֵ���ֶ�ֵ��˳��Ӧ�� open ������
	 *  ���ֶ�˳����ͬ
	 * @param to_num {int} to_values ���鳤��
	 * @param cond {const char*} ƥ������������Ϊ��
	 *  = ����; >= ���ڵ���; > ����; < С��; <= С�ڵ���
	 * @param nlimit {int} ������������ƣ�0 ��ʾ�����Ƹ���
	 * @param noffset {int} �������ʼλ��(0��ʾ�ӵ�һ�������ʼ)
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool mod(const char* values[], int num,
		const char* to_values[], int to_num,
		const char* cond = "=", int nlimit = 0, int noffset = 0);

	/**
	 * ɾ�����ݿ����ƥ���ֶεļ�¼
	 * @param values {const char*[]} ƥ���ֶ�ֵ���飬�ֶ�ֵ�ļ���˳��Ӧ�� open
	 *  ������ flds �и����ֶε�˳����ͬ
	 * @param num {int} values ���鳤�ȣ���ֵ��Ӧ�������캯���� flds �����е�
	 *  �ֶθ���
	 * @param cond {const char*} ƥ������������Ϊ��
	 *  = ����; >= ���ڵ���; > ����; < С��; <= С�ڵ���
	 * @param nlimit {int} ɾ�����ݵĸ���, 0 ��ʾ������
	 * @param noffset {int} �������ʼλ��(0��ʾ�ӵ�һ�������ʼ)
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool del(const char* values[], int num, const char* cond = "=",
		int nlimit = 0, int noffset = 0);

	/**
	 * ɾ�����ݿ����ƥ���ֶεļ�¼
	 * @param first_value {const char*} Ϊ��Ӧ�ڹ��캯���� flds �ֶμ���
	 *  �ĵ�һ���ֶε��ֶ�ֵ
	 * @param ... {const char*} �����б����һ������Ϊ NULL ��ʾ����
	 * @return {bool} ��Ӽ�¼�Ƿ�ɹ�
	 */
	bool fmt_del(const char* first_value, ...) ACL_CPP_PRINTF(2, 3);

	/**
	 * �����ݿ�����¼�¼
	 * @param values {const char*[]} ƥ���ֶ�ֵ���飬�ֶ�ֵ�ļ���˳��Ӧ�빹��
	 *  ������ flds �и����ֶε�˳����ͬ
	 * @param num {int} values ���鳤�ȣ���ֵ��Ӧ�������캯���� flds �����е�
	 *  �ֶθ���
	 * @return {bool} ��Ӽ�¼�Ƿ�ɹ�
	 */
	bool add(const char* values[], int num);

	/**
	 * �����ݿ�������¼�¼
	 * @param first_value {const char*} Ϊ��Ӧ�ڹ��캯���� flds �ֶμ���
	 *  �ĵ�һ���ֶε��ֶ�ֵ
	 * @param ... {const char*} �����б����һ������Ϊ NULL ��ʾ����
	 * @return {bool} ��Ӽ�¼�Ƿ�ɹ�
	 */
	bool fmt_add(const char* first_value, ...) ACL_CPP_PRINTF(2, 3);

	/**
	 * �����Ƿ���е���
	 * @param on {bool} true ���ʾ���е��ԣ��ὫһЩ�м���Ϣ������־��
	 */
	void debug_enable(bool on);

	/**
	 * �����ݿ��
	 * @param dbn {const char*} ���ݿ�����
	 * @param tbl {const char*} ���ݿ����
	 * @param idx {const char*} �����ֶ���
	 * @param flds {const char*} Ҫ�򿪵������ֶ������ϣ���ʽΪ
	 *  �ɷָ��� ",; \t" �ָ����ֶ����ƣ��磺user_id,user_name,user_mail
	 * @param auto_open {bool} ����δ���Ƿ��Զ���
	 * @return {bool} true ��ʾ�����򿪣������ʾ�򿪱�ʧ��
	 */
	bool open_tbl(const char* dbn, const char* tbl,
		const char* idx, const char* flds, bool auto_open = true);

	/**
	 * ������ӵ�ַ
	 * @return {const char*} ����Ϊ��
	 */
	const char* get_addr() const;

	/**
	 * ��ó�������
	 * @return {int}
	 */
	int  get_error() const;

	/**
	 * ��ó��������Ϣ����
	 * @param errnum {int} �� get_error ��õĴ����
	 * @return {const char*}
	 */
	const char* get_serror(int errnum) const;

	/**
	 * ����ϴγ���ʱ�Ĵ���������Ϣ
	 * @return {const char*}
	 */
	const char* get_last_serror() const;

	/**
	 * ��õ�ǰ hsclient �������õ� id ��
	 * @return {int}
	 */
	int get_id() const;
private:
	bool   debugOn_;
	char*  addr_;
	hsproto  proto_;
	bool   retry_enable_;
	int    id_max_;
	hstable* tbl_curr_;
	string buf_;

	// ������������
	socket_stream stream_;
	std::map<string, hstable*> tables_;

	char   cond_def_[2];
	int    error_;
	const char* serror_;

	// �����ݿ�����
	bool open_tbl(const char* dbn, const char* tbl,
		const char* idx, const char* flds, const char* key);

	// ����д���ݿ�����������ʱ��Ҫ���ô˺������ر����������ͷ�
	// �����
	void close_stream();

	// �����ڲ��򿪵����ݿ��Ķ���
	void clear_tables();

	// �����ݿⷢ�Ͳ�ѯ���ȡ�ý������
	bool query(const char* oper, const char* values[], int num,
		const char* limit_offset, char mop,
		const char* to_values[], int to_num);
	bool chat();
};

} // namespace acl
