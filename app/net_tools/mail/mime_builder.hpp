#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/mime/mime_head.hpp"

class mime_builder
{
public:
	mime_builder();
	~mime_builder();

	/**
	 * ����ʼ�ͷ
	 * @return {acl::mime_head&}
	 */
	acl::mime_head& primary_header(void)
	{
		return (header_);
	}

	/**
	 * �����ʼ��������ݣ�Ϊ���ı���ʽ
	 * @param src {const char*) ��������
	 * @param len {size_t} src ���ݳ���
	 * @return {mime_builder&}
	 */
	mime_builder& set_body_text(const char* src, size_t len);

	/**
	 * �����ʼ��������ݣ�Ϊ HTML ��ʽ
	 * @param src {const char*) ��������
	 * @param len {size_t} src ���ݳ���
	 * @return {mime_builder&}
	 */
	mime_builder& set_body_html(const char* src, size_t len);

	/**
	 * ��Ӹ���
	 * @param filepath {const char*} �����ļ�·��
	 * @return {mime_builder&}
	 */
	mime_builder& add_file(const char* filepath);

	/**
	 * �����ʼ����ݣ���ת�����ļ���
	 * @param to {const char*} Ŀ���ļ�
	 * @return {bool}
	 */
	bool save_as(const char* to);

	/**
	 * �����ʼ����ݣ���ת�����ļ���
	 * @param fp {acl::ofstream&} Ŀ���ļ����
	 * @return {bool}
	 */
	bool save_as(acl::ofstream& fp);

private:
	acl::mime_head header_;
	char* body_text_;
	char* body_html_;
	std::vector<char*> attachs_;
	acl::string delimeter_;

	bool add_body(acl::ofstream& fp);
	bool add_boundary(acl::ofstream& fp, bool end = false);
	bool add_attach(acl::ofstream& fp, const char* filepath);
};
