#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stdlib/string.hpp"

namespace acl {

/**
 * ���ݿ��ѯ��������м�¼���Ͷ���
 */
class ACL_CPP_API db_row
{
public:
	/**
	 * ���캯��
	 * @param names {const std::vector<const char*>&} ���ݿ�����ֶ����б�
	 */
	db_row(const std::vector<const char*>& names);
	~db_row();

	/**
	 * ȡ�����ݱ��е�ĳ����Ӧ�±�ֵ���ֶ���
	 * @param ifield {size_t} �±�ֵ
	 * @return {const char*} ���ؿ�˵�����±�ֵԽ��
	 */
	const char* field_name(size_t ifield) const;

	/**
	 * �Ӳ�ѯ����ļ�¼���и����ֶ���ȡ����Ӧ���ֶ�ֵ
	 * @param name {const char*} ���ݱ���ֶ���
	 * @return {const char*} ��Ӧ���ֶ�ֵ��Ϊ�����ʾ�ֶ�ֵ�����ڻ�
	 *  �ֶ����Ƿ�
	 */
	const char* field_value(const char* name) const;

	/**
	 * �Ӳ�ѯ����ļ�¼���и����ֶ���ȡ����Ӧ���ֶ�ֵ��
	 * ������ field_value ��ͬ
	 * @param name {const char*} ���ݱ���ֶ���
	 * @return {const char*} ��Ӧ���ֶ�ֵ��Ϊ�����ʾ�ֶ�ֵ�����ڻ�
	 *  �ֶ����Ƿ�
	 */
	const char* operator[](const char* name) const;

	/**
	 * �Ӳ�ѯ����ļ�¼����ȡ�ö�Ӧ�±���ֶ�ֵ
	 * @param ifield {size_t} �±�ֵ����ֵӦ < �ֶ����ĸ���
	 * @return {const char*} ��Ӧ���ֶ�ֵ��Ϊ�����ʾ�±�ֵ�Ƿ���
	 *  �ֶ�ֵ������
	 */
	const char* field_value(size_t ifield) const;

	/**
	 * �Ӳ�ѯ����ļ�¼����ȡ�ö�Ӧ�±���ֶ�ֵ�������� field_value ��ͬ
	 * @param ifield {size_t} �±�ֵ����ֵӦ < �ֶ����ĸ���
	 * @return {const char*} ��Ӧ���ֶ�ֵ��Ϊ�����ʾ�±�ֵ�Ƿ���
	 *  �ֶ�ֵ������
	 */
	const char* operator[](size_t ifield) const;

	/**
	 * �Ӳ�ѯ����ļ�¼����ȡ�ö�Ӧ�±���������͵��ֶ�ֵ
	 * @param ifield {size_t} �±�ֵ
	 * @param null_value {int} �����Ϊ��ʱ�����ش�ֵ��ʾδ����Ӧ���
	 * @return {int} ������ֵ���û������ null_value ֵ��ͬʱ����û�в鵽���
	 */
	int field_int(size_t ifield, int null_value = 0) const;

	/**
	 * �Ӳ�ѯ����ļ�¼����ȡ���ֶ������������͵��ֶ�ֵ
	 * @param name {const char*} �±�ֵ
	 * @param null_value {int} �����Ϊ��ʱ�����ش�ֵ��ʾδ����Ӧ���
	 * @return {int} ������ֵ���û������ null_value ֵ��ͬʱ����û�в鵽���
	 */
	int field_int(const char* name, int null_value = 0) const;

	
	/**
	 * �Ӳ�ѯ����ļ�¼����ȡ�ö�Ӧ�±���������͵��ֶ�ֵ
	 * @param ifield {size_t} �±�ֵ
	 * @param null_value {acl_int64} �����Ϊ��ʱ�����ش�ֵ��ʾδ����Ӧ���
	 * @return {acl_int64} ������ֵ���û������ null_value ֵ��ͬʱ����û�в鵽���
	 */
#ifdef WIN32
	__int64 field_int64(size_t ifield, __int64 null_value = 0) const;
#else
	long long int field_int64(size_t ifield, long long int null_value = 0) const;
#endif

	/**
	 * �Ӳ�ѯ����ļ�¼����ȡ���ֶ������������͵��ֶ�ֵ
	 * @param name {const char*} �±�ֵ
	 * @param null_value {acl_int64} �����Ϊ��ʱ�����ش�ֵ��ʾδ����Ӧ���
	 * @return {acl_int64} ������ֵ���û������ null_value ֵ��ͬʱ����û�в鵽���
	 */
#ifdef WIN32
	__int64 field_int64(const char* name, __int64 null_value = 0) const;
#else
	long long int field_int64(const char* name, long long int null_value = 0) const;
#endif

	/**
	 * �Ӳ�ѯ����ļ�¼����ȡ�ö�Ӧ�±���ַ������͵��ֶ�ֵ
	 * @param ifield {size_t} �±�ֵ
	 * @return {const char*} ������ֵ NULL ʱ����û�в鵽���
	 */
	const char* field_string(size_t ifield) const;

	/**
	 * �Ӳ�ѯ����ļ�¼����ȡ���ֶ������ַ������͵��ֶ�ֵ
	 * @param name {const char*} �±�ֵ
	 * @return {const char*} ������ֵ NULL ʱ����û�в鵽���
	 */
	const char* field_string(const char* name) const;

	/**
	 * ���¼�����һ���ֶ�ֵ������ֶ�ֵ��˳��Ӧ�����ֶ�����˳��һ��
	 * @param value {const char*} ���м�¼��ĳ���ֶ�ֵ
	 */
	void push_back(const char* value);

	/**
	 * �м�¼���ֶ�ֵ�ĸ���
	 * @return {size_t}
	 */
	size_t length() const;
protected:
private:
	// ���ݱ���ֶ������ϵ�����
	const std::vector<const char*>& names_;

	// ���ݽ���е��ֶμ���
	std::vector<const char*> values_;
};

/**
 * ���ݿ��ѯ������м�¼�������Ͷ���
 */
class ACL_CPP_API db_rows
{
public:
	db_rows();
	virtual ~db_rows();

	/**
	 * �Ӳ�ѯ���м�¼�����и��ݱ��ֶ�����Ӧ���ֶ�ֵȡ�������¼����
	 * @param name {const char*} ���ݱ��ֶ���(�����ִ�Сд)
	 * @param value {const char*} ���ݱ��ֶ�ֵ(���ִ�Сд)
	 * @return {const std::vector<const db_row*>&} �����м�¼�������Ͷ���
	 *  ����ͨ������ db_rows.empty() ���жϽ���Ƿ�Ϊ��
	 */
	const std::vector<const db_row*>& get_rows(
		const char* name, const char* value);

	/**
	 * ȡ�����еĲ�ѯ�����
	 * @return {const std::vector<db_row*>&} �����м�¼�������Ͷ���
	 *  ����ͨ������ db_rows.empty() ���жϽ���Ƿ�Ϊ��
	 */
	const std::vector<db_row*>& get_rows() const;

	/**
	 * �Ӳ�ѯ���м�¼�����и��������±�ȡ�ö�Ӧ��ĳ�м�¼
	 * @param idx {size_t} �����±꣬��ֵӦ�� < �������С
	 * @return {const db_row*} ���ؿձ�ʾ�����±�ֵ�Ƿ����ֶ�ֵ����
	 *  Ϊ��
	 */
	const db_row* operator[](size_t idx) const;

	/**
	 * �жϽ�����Ƿ�Ϊ��
	 * @return {bool} �Ƿ�Ϊ��
	 */
	bool empty() const;

	/**
	 * ��������м�¼����
	 * @return {size_t} �м�¼����
	 */
	size_t length() const;
protected:
	// ���ݱ��ֶ���
	std::vector<const char*> names_;

	// ��ѯ����м��ϣ����е�Ԫ�� db_row �����Ƕ�̬��ӽ�ȥ�ģ�
	// ��Ϊ�ڱ����������ʱ���Զ� delete rows_ �е�����Ԫ�ض���
	std::vector<db_row*> rows_;

	// ��ʱ����м���
	std::vector<const db_row*> rows_tmp_;
private:
};

class db_pool;
class query;

/**
 * ���ݿ���������������
 */
class ACL_CPP_API db_handle
{
public:
	db_handle(void);
	virtual ~db_handle(void);

	/**
	 * �������ݿ����������
	 * @return {const char*}
	 */
	virtual const char* dbtype() const = 0;

	/**
	 * ����ϴ����ݿ�����ĳ�������
	 * @return {int}
	 */
	virtual int get_errno() const
	{
		return -1;
	}

	/**
	 * ����ϴ����ݿ�����ĳ��������
	 * @return {const char*}
	 */
	virtual const char* get_error() const
	{
		return "unkonwn error";
	}

	/**
	 * ����ӿڣ��������ʵ�ִ˽ӿ����ڴ����ݿ�
	 * @param local_charset {const char*} �����ַ���(GBK, UTF8, ...)
	 * @return {bool} ���Ƿ�ɹ�
	 */
	virtual bool open(const char* local_charset = "GBK") = 0;

	/**
	 * ���ݿ��Ƿ��Ѿ�����
	 * @return {bool} ���� true �������ݿ��Ѿ�����
	 */
	virtual bool is_opened() const = 0;

	/**
	 * ����ӿڣ��������ʵ�ִ˽ӿ������ж����ݱ��Ƿ����
	 * @return {bool} �Ƿ����
	 */
	virtual bool tbl_exists(const char* tbl_name) = 0;

	/**
	 * ����ӿڣ��������ʵ�ִ˽ӿ����ڹر����ݿ�
	 * @return {bool} �ر��Ƿ�ɹ�
	 */
	virtual bool close() = 0;

	/**
	 * ����ӿڣ��������ʵ�ִ˽ӿ�����ִ�� SQL ���
	 * @param sql {const char*} ��׼�� SQL ��䣬�ǿգ�����һ����Ҫע���
	 *  SQL �����뾭��ת�崦���Է�ֹ SQL ע�빥��
	 * @return {bool} ִ���Ƿ�ɹ�
	 */
	virtual bool sql_select(const char* sql) = 0;

	/**
	 * ����ӿڣ��������ʵ�ִ˽ӿ�����ִ�� SQL ���
	 * @param sql {const char*} ��׼�� SQL ��䣬�ǿգ�����һ����Ҫע���
	 *  SQL �����뾭��ת�崦���Է�ֹ SQL ע�빥��
	 * @return {bool} ִ���Ƿ�ɹ�
	 */
	virtual bool sql_update(const char* sql) = 0;

	/**
	 * ����ȫ���õĲ�ѯ���̣����ô˺������ܵ�ͬ�� sql_select��ֻ�ǲ�ѯ���� query
	 * ������ sql ����ǰ�ȫ�ģ����Է�ֹ sql ע��
	 * @param query {query&}
	 * @return {bool} ִ���Ƿ�ɹ�
	 */
	bool exec_select(query& query);

	/**
	 * ����ȫ���õĸ��¹��̣����ô˺������ܵ�ͬ�� sql_update��ֻ�ǲ�ѯ���� query
	 * ������ sql ����ǰ�ȫ�ģ����Է�ֹ sql ע��
	 * @param query {query&}
	 * @return {bool} ִ���Ƿ�ɹ�
	 */
	bool exec_update(query& query);

	/**
	 * ��ӿڣ�Ϊ��ֹ sql ע�룬�û�Ӧ����ַ����ֶε��ô˺�����һЩ����
	 * �ַ�����ת�壬�ýӿڶԳ����������ַ�������ת�壬����Ҳ����ʵ���Լ�
	 * ��ת�巽��
	 * @param in {const char*} �����ַ���
	 * @param len {size_t} �ַ�������
	 * @param out {string&} �洢ת�����
	 * @return {string&} ����Ӧ�÷�������Ļ����������ã��Ա����û���ƴ��
	 *  SQL ��ʱ��ȽϷ���
	 */
	virtual string& escape_string(const char* in, size_t len, string& out);

	/**
	 * �ϴ� sql ����Ӱ��ļ�¼����
	 * @return {int} Ӱ���������-1 ��ʾ����
	 */
	virtual int affect_count() const = 0;

	/////////////////////////////////////////////////////////////////////

	/**
	 * ���ִ�� SQL ����Ľ��
	 * @return {const db_rows*}�����ؽ�����ǿգ����������Ҫ����
	 *  free_result() ���ͷŽ������
	 */
	const db_rows* get_result() const;

	/**
	 * �Ӳ�ѯ���м�¼�����и��ݱ��ֶ�����Ӧ���ֶ�ֵȡ�������¼����
	 * @param name {const char*} ���ݱ��ֶ���(�����ִ�Сд)
	 * @param value {const char*} ���ݱ��ֶ�ֵ(���ִ�Сд)
	 * @return {const std::vector<const db_row*>*} �����м�¼�������Ͷ���
	 *  �����ؽ�����ǿգ��������� free_result() ���ͷŽ������
	 */
	const std::vector<const db_row*>* get_rows(
		const char* name, const char* value);

	/**
	 * ȡ�����еĲ�ѯ�����
	 * @return {const std::vector<db_row*>*} �����м�¼�������Ͷ���
	 *  �����ؽ�����ǿգ��������� free_result() ���ͷŽ������
	 */
	const std::vector<db_row*>* get_rows() const;

	/**
	 * ���ִ�� SQL ����ĵ�һ�н���������Ψһ�������ݲ�ѯʱ�ԵñȽϱ��Щ
	 * @return {const db_row*} ���ؿձ�ʾ��ѯ���Ϊ�գ�������ؽ���ǿգ������
	 *  ���� free_result() �������ͷ��м�Ľ���ڴ棬����������ڴ�й¶
	 */
	const db_row* get_first_row() const;

	/**
	 * �ͷ��ϴβ�ѯ�Ľ��������ѯ��ɺ󣬱�����øú������ͷ�
	 * ���β�ѯ�Ľ�����ú�������ε��ò��޺�������Ϊ����һ��
	 * ����ʱ���Զ����ڲ����� result_ �ÿ�
	 */
	void free_result();

	/**
	 * ���ĳ����Ӧ�±�ֵ���м�¼
	 * @param idx {size_t} �±�ֵ������С�ڲ�ѯ���������
	 * @return {const db_row*} ��������Ϊ�գ����п������±�Խ�磬
	 *  Ҳ�п����ǽ��Ϊ��
	 */
	const db_row* operator[](size_t idx) const;

	/**
	 * ȡ�ò�ѯ(sql_select)������м�¼��
	 * @return {size_t} ����м�¼������Ϊ 0 ���ʾ���Ϊ��
	 */
	size_t length() const;

	/**
	 * ��ѯ(sql_select)ִ��������Ƿ�Ϊ��
	 * @return {bool} ���� true ��ʾ��ѯ���Ϊ��
	 */
	bool empty() const;

	/**
	 * ������ݿ��ѯ���
	 * @param max {size_t} �������Ļ���м�¼�������ֵ���ƣ������ֵΪ 0
	 *  ��������еĽ����
	 */
	void print_out(size_t max = 0) const;

	/////////////////////////////////////////////////////////////////
	/**
	 * ���ñ�ʵ����Ψһ ID
	 * @param id {const char*} Ψһ ID
	 * @return {db_handle&}
	 */
	db_handle& set_id(const char* id);

	/**
	 * ��ñ�ʵ����Ψһ ID
	 * @return {const char*} Ϊ��ʱ����ʾδ�����ù�ΨһID
	 */
	const char* get_id() const
	{
		return id_;
	}

	/**
	 * ���ñ����ݿ����Ӿ����ǰ��ʹ�õ�ʱ��
	 * @param now {time_t}
	 * @return {db_handle&}
	 */
	db_handle& set_when(time_t now);

	/**
	 * ��ø����Ӿ���ϴα�ʹ�õ�ʱ��
	 * @return {time_t}
	 */
	time_t get_when() const
	{
		return when_;
	}
protected:
	// ��ʱ�������
	db_rows* result_;

	// ʵ��Ψһ ID
	char* id_;

	// �����ݿ����Ӿ�������ʹ�õ�ʱ��
	time_t when_;
};

} // namespace acl
