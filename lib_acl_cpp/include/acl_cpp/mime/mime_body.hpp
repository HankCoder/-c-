#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/mime/mime_node.hpp"

struct MIME_NODE;

namespace acl {

class pipe_manager;
class ostream;
class pipe_string;
class string;

class ACL_CPP_API mime_body : public mime_node
{
public:
	/**
	 * ���캯��
	 * @param emailFile {const char*} �洢�ʼ����ݵ�Դ�ļ�������
	 *  Ϊ�գ�����Ϊ��ʱ�ڵ��� save_body ����ʱ������ָ��Դ�ļ�
	 * @param node {const MIME_NODE*} �ʼ��е�ĳ��������
	 * @param htmlFirst {bool} �Ƿ�����ȡ����ʱ������ȡ HTML ����
	 * @param enableDecode {bool} ���ʼ�����Ϊ base64/qp �ȱ����ʽ
	 *  ʱ�Ƿ���Ҫ�Զ����н���
	 * @param toCharset {const char*} ȱʡ��Ŀ���ַ��������Ŀ��
	 *  �ַ�����Դ�ַ�����ͬ��������ַ���ת��
	 * @param off {off_t} �ʼ����������������е���ʼλ���и��ӵ�
	 *  ���ƫ�������Ա����û��������ʼ�����ǰ����Լ���˽������
	 */
	mime_body(const char* emailFile, const MIME_NODE* node,
		bool htmlFirst = true, bool enableDecode = true,
		const char* toCharset = "gb2312", off_t off = 0)
		: mime_node(emailFile, node, enableDecode, toCharset, off)
		, m_htmlFirst(htmlFirst)
	{
	}

	~mime_body() {}

	/**
	 * �����Ƿ����ȡ HTML ����, ���Ϊ true ��������ȡ HTML ����,
	 * �������� HTML ����ʱ�Ż���ȡ���ı�����; ���Ϊ false ������
	 * ��ȡ���ı�����, ������� HTML ����ʱ���Ӹ� HTML �����г�
	 * ȡ�����ı�����
	 * @param htmlFirst {bool}
	 */
	void set_status(bool htmlFirst)
	{
		m_htmlFirst = htmlFirst;
	}

	/**
	 * ת���ʼ����������ڹܵ�����
	 * @param out {pipe_manager&} �ܵ���������
	 * @param src {const char*} �ʼ����ݵ���ʼ��ַ�����Ϊ��ָ�룬
	 *  ��ӹ��캯�������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @param len {int} �ʼ����ݵ����ݳ��ȣ����Ϊ0����ӹ���
	 *  ���������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save_body(pipe_manager& out, const char* src = NULL,
		int len = 0);

	/**
	 * ת���ʼ������������������
	 * @param out {ostream&} �����
	 * @param src {const char*} �ʼ����ݵ���ʼ��ַ�����Ϊ��ָ�룬
	 *  ��ӹ��캯�������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @param len {int} �ʼ����ݵ����ݳ��ȣ����Ϊ0����ӹ���
	 *  ���������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save_body(ostream& out, const char* src = NULL,
		int len = 0);

	/**
	 * ת���ʼ�����������Ŀ���ļ���
	 * @param file_path {const char*} Ŀ���ļ���
	 * @param src {const char*} �ʼ����ݵ���ʼ��ַ�����Ϊ��ָ�룬
	 *  ��ӹ��캯�������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @param len {int} �ʼ����ݵ����ݳ��ȣ����Ϊ0����ӹ���
	 *  ���������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save_body(const char* file_path, const char* src = NULL,
		int len = 0);

	/**
	 * ת���ʼ����������ڹܵ���������
	 * @param out {pipe_string&} �ܵ�������
	 * @param src {const char*} �ʼ����ݵ���ʼ��ַ�����Ϊ��ָ�룬
	 *  ��ӹ��캯�������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @param len {int} �ʼ����ݵ����ݳ��ȣ����Ϊ0����ӹ���
	 *  ���������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save_body(pipe_string& out, const char* src = NULL,
		int len = 0);

	/**
	 * ת���ʼ����������ڻ�������
	 * @param out {string&} ������
	 * @param src {const char*} �ʼ����ݵ���ʼ��ַ�����Ϊ��ָ�룬
	 *  ��ӹ��캯�������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @param len {int} �ʼ����ݵ����ݳ��ȣ����Ϊ0����ӹ���
	 *  ���������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save_body(string& out, const char* src = NULL,
		int len = 0);

	/**
	 * �жϽ��ͷ�������еĴ������Ƿ� MIME_STYPE_HTML ����
	 * @return {bool}
	 */
	bool html_stype() const;
private:
	bool m_htmlFirst;
};

} // namespace acl
