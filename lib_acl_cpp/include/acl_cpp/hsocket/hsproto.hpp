#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>

namespace acl {

class string;
class hsrow;

class ACL_CPP_API hsproto
{
public:
	hsproto(bool cache_enable);
	~hsproto();

	/**
	 * ���������ݿ�����������Э������
	 * @param out {string&} �洢����Э����
	 * @param id {int} ��Ӧ�������ı�ID��
	 * @param dbn {const char*} ���ݿ�����
	 * @param tbl {const char*} ���ݿ����
	 * @param idx {const char*} �����ֶ���
	 * @param flds {const char*} Ҫ�򿪵������ֶ������ϣ���ʽΪ
	 *  �ɷָ��� ",; \t" �ָ����ֶ����ƣ��磺user_id,user_name,user_mail
	 * @return {bool} �Ƿ�ɹ�
	 */
	static bool build_open(string& out, int id,
		const char* dbn, const char* tbl,
		const char* idx, const char* flds);

	/**
	 * ������ѯ���ݿ��¼������Э������
	 * @param out {string&} �洢����Э����
	 * @param id {int} ��Ӧ�������ı�ID��
	 * @param values {const char*[]} ƥ���ֶ�ֵ���飬�ֶ�ֵ�ļ���˳��Ӧ������� 
	 *  �и����ֶε�˳����ͬ
	 * @param num {int} values ���鳤�ȣ���ֵ��Ӧ�����ڴ�����ʱ���ֶθ���
	 * @param cond {const char*} ƥ������������Ϊ��
	 *  = ����; >= ���ڵ���; > ����; < С��; <= С�ڵ���
	 * @param nlimit {int} ������������ƣ�0 ��ʾ�����Ƹ���
	 * @param noffset {int} �������ʼλ��(0��ʾ�ӵ�һ�������ʼ)
	 * @return {bool} �Ƿ�ɹ�
	 */
	static bool build_get(string& out, int id,
		const char* values[], int num,
		const char* cond = "=", int nlimit = 0, int noffset = 0);

	/**
	 * ������ѯ���ݿ��¼������Э������
	 * @param out {string&} �洢����Э������
	 * @param id {int} ��Ӧ�������ı�ID��
	 * @param nfld {int} �ڴ�����ʱ�ֶθ���
	 * @param first_value {const char*} ��һ������
	 * @param ... {const char*} �����б����һ������Ϊ NULL ��ʾ����
	 * @return {bool} �Ƿ�ɹ�
	 */
	static bool ACL_CPP_PRINTF(4, 5) build_get(string& out, int id,
		int nfld, const char* first_value, ...);

	/**
	 * �����޸����ݿ��¼������Э������
	 * @param out {string&} �洢����Э������
	 * @param id {int} ��Ӧ�������ı�ID��
	 * @param values {const char*[]} ƥ���ֶ�ֵ���飬�ֶ�ֵ�ļ���˳��Ӧ������� 
	 *  �и����ֶε�˳����ͬ
	 * @param num {int} values ���鳤�ȣ���ֵ��Ӧ�����ڴ�����ʱ���ֶθ���
	 * @param to_values {cosnt *[]} ƥ���ֶ���ֵ���ֶ�ֵ��˳��Ӧ�� open ������
	 *  ���ֶ�˳����ͬ
	 * @param to_num {int} to_values ���鳤��
	 * @param cond {const char*} ƥ������������Ϊ��
	 * @param nlimit {int} ������������ƣ�0 ��ʾ�����Ƹ���
	 * @param noffset {int} �������ʼλ��(0��ʾ�ӵ�һ�������ʼ)
	 * @return {bool} �Ƿ�ɹ�
	 */
	static bool build_mod(string& out, int id,
		const char* values[], int num,
		const char* to_values[], int to_num,
		const char* cond = "=", int nlimit = 0, int noffset = 0);

	/**
	 * ����ɾ�����ݿ��¼������Э������
	 * @param out {string&} �洢����Э������
	 * @param id {int} ��Ӧ�������ı�ID��
	 * @param values {const char*[]} ƥ���ֶ�ֵ���飬�ֶ�ֵ�ļ���˳��Ӧ������� 
	 *  �и����ֶε�˳����ͬ
	 * @param num {int} values ���鳤�ȣ���ֵ��Ӧ�����ڴ�����ʱ���ֶθ���
	 * @param cond {const char*} ƥ������������Ϊ��
	 * @param nlimit {int} ������������ƣ�0 ��ʾ�����Ƹ���
	 * @param noffset {int} �������ʼλ��(0��ʾ�ӵ�һ�������ʼ)
	 * @return {bool} �Ƿ�ɹ�
	 */
	static bool build_del(string& out, int id, const char* values[],
		int num, const char* cond = "=",
		int nlimit = 0, int noffset = 0);

	/**
	 * ����ɾ�����ݿ��¼������Э������
	 * @param out {string&} �洢����Э������
	 * @param id {int} ��Ӧ�������ı�ID��
	 * @param nfld {int} �ڴ�����ʱ�ֶθ���
	 * @param first_value {const char*} ��һ������
	 * @param ... {const char*} �����б����һ������Ϊ NULL ��ʾ����
	 * @return {bool} �Ƿ�ɹ�
	 */
	static bool ACL_CPP_PRINTF(4, 5) build_del(string& out, int id,
		int nfld, const char* first_value, ...);

	/**
	 * ����������ݿ��¼������Э������
	 * @param out {string&} �洢����Э������
	 * @param id {int} ��Ӧ�������ı�ID��
	 * @param values {const char*[]} ƥ���ֶ�ֵ���飬�ֶ�ֵ�ļ���˳��Ӧ������� 
	 *  �и����ֶε�˳����ͬ
	 * @param num {int} values ���鳤�ȣ���ֵ��Ӧ�����ڴ�����ʱ���ֶθ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	static bool build_add(string& out, int id,
		const char* values[], int num);

	/**
	 * ����������ݿ��¼������Э������
	 * @param out {string&} �洢����Э������
	 * @param id {int} ��Ӧ�������ı�ID��
	 * @param nfld {int} �ڴ�����ʱ�ֶθ���
	 * @param first_value {const char*} ��һ������
	 * @param ... {const char*} �����б����һ������Ϊ NULL ��ʾ����
	 * @return {bool} �Ƿ�ɹ�
	 */
	static bool ACL_CPP_PRINTF(4, 5) build_add(string& out, int id,
		int nfld, const char* first_value, ...);

	/**
	 * ͨ�õĴ������ݿ⴦�������Э������
	 * @param out {string&} �洢����Э������
	 * @param id {int} ��Ӧ�������ı�ID��
	 * @param oper {const char*} ������ʽ����Ӧ�Ĳ�����Ϊ��
	 *  ���: +
	 *  ��ѯ: =, >, >=, <, <=
	 *  �޸�: =, >, >=, <, <=
	 *  ɾ��: =, >, >=, <, <=
	 * @param values {const char*[]} ƥ���ֶ�ֵ���飬�ֶ�ֵ�ļ���˳��Ӧ������� 
	 *  �и����ֶε�˳����ͬ
	 * @param num {int} values ���鳤�ȣ���ֵ��Ӧ�����ڴ�����ʱ���ֶθ���
	 * @param limit_offset {const char*} Ҫ��Ĳ�ѯ��Χ
	 * @param mop {char} �����ɾ�����޸Ĳ�����Ч�����Ӧ�Ĳ������ֱ�Ϊ:
	 *  D: ɾ��, U: �޸�
	 * @param to_values {const char*[]} Ŀ��ֵָ������
	 * @param to_num {int} to_values ����ĳ���
	 */
	static void build_request(string& out, int id, const char* oper,
		const char* values[], int num,
		const char* limit_offset, char mop,
		const char* to_values[], int to_num);

	/**
	 * �������ݿ�ķ�������
	 * @param nfld {int} �򿪵ı��Ԫ�ظ���
	 * @param in {string&} �����ݿ������������, β��Ӧ�ò����� "\r\n"
	 * @param errnum_out {int&} �洢������̵ĳ���ţ��μ�: hserror.hpp
	 * @param serror_out {const char*&} �洢������̵ĳ���������Ϣ
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool parse_respond(int nfld, string& in,
                int& errnum_out, const char*& serror_out);

	/**
	 * ��ִ�в�ѯ���ʱ������ͨ���˺�����ò�ѯ�Ľ����
	 * @return {const std::vector<hsrow*>&}
	 */
	const std::vector<hsrow*>& get();

	/**
	 * ���û������ѯ���������еڶ��β�ѯʱ���ô˺��������ϴβ�ѯ���
	 */
	void reset();
private:
	bool  debugOn_;
	bool  cache_enable_;
	//int   nfld_;
	int   ntoken_;
	char* buf_ptr_;

	// ��ѯ�����
	std::vector<hsrow*> rows_;

	// �м�¼���󻺴棬���м�¼���������Ϊ�˱�֤�ڴ�
	// ���ã�������ʹ�õ��м�¼������л��棬�Ա�����
	std::vector<hsrow*> rows_cache_;

	// ����л�����󼯺�
	void clear_cache();

	// �����һ����ѯ���
	hsrow* get_next_row();
};

}  // namespace acl
