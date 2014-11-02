#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>

namespace acl {

class string;

struct URL_NV
{
	char* name;
	char* value;
};

class ACL_CPP_API url_coder
{
public:
	/**
	 * ���캯��
	 * @param nocase {bool} ��Ϊ true ʱ��ʾ�������������Сд
	 */
	url_coder(bool nocase = true);

	/**
	 * ���캯����ͨ����ʵ��������
	 * @param coder {const url_coder&}
	 */
	url_coder(const url_coder& coder);

	~url_coder();

	/**
	 * ���洢�� params_ �����е����ݽ��� url ����
	 * @param buf {string&} �洢�����Ľ��
	 * @param clean {bool} �Ƿ���մ���� buf ������
	 */
	void encode(string& buf, bool clean = true) const;

	/**
	 * ��ý��������ת��Ϊ�������ַ�������
	 * @return {const string&}
	 */
	const string& to_string() const;

	/**
	 * ������ URL ������ַ���
	 * @param str {const char*} url ������ʽ���ַ���
	 */
	void decode(const char* str);
	
	/**
	 * ���� url ����ʱ�����ô˺�����ӱ���
	 * @param name {const char*} ������
	 * @param value ����ֵ
	 * @param override {bool} �������ͬ�������Ƿ�ֱ�Ӹ���
	 * @return ���� url_coder ���������
	 */
	url_coder& set(const char* name, const char* value,
		bool override = true);
	url_coder& set(const char* name, int value, bool override = true);
	url_coder& set(const char* name, bool override, const char* fmt, ...)
		ACL_CPP_PRINTF(4, 5);
	url_coder& set(const char* name, const char* fmt, va_list ap,
		bool override = true);

	/**
	 * ��� URL ����� params_ ������ĳ����������ֵ
	 * @param name {const char*} ������
	 * @return {const char*} ���� NULL ��ʾ������
	 */
	const char* get(const char* name) const;

	/**
	 * ��� URL ����� params_ ������ĳ����������ֵ
	 * @param name {const char*} ������
	 * @return {const char*} ���� NULL ��ʾ������
	 */
	const char* operator[](const char* name) const;

	/**
	 * URL ����������Ŀ���
	 * @param coder {const url_coder&} URL Դ����������
	 * @return {const url_coder&}
	 */
	const url_coder& operator =(const url_coder& coder);

	/**
	 * ��ò����������
	 * @return {std::vector<URL_NV*>&}
	 */
	const std::vector<URL_NV*>& get_params() const
	{
		return params_;
	}

	/**
	 * �� params_ ����������ɾ��ĳ������
	 * @param name {const char*} ������
	 * @return {bool} ���� true ��ʾɾ���ɹ��������ʾ������
	 */
	bool del(const char* name);

	/**
	 * ���ý�����״̬������ڲ�����
	 */
	void reset();

private:
	bool nocase_;
	std::vector<URL_NV*> params_;
	string*  buf_;

	void free_param(URL_NV*);
};

} // namespace acl end
