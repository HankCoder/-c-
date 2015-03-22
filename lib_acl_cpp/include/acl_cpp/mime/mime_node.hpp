#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <stdlib.h>
#include <map>
#include "acl_cpp/stdlib/string.hpp"

struct MIME_NODE;

namespace acl {

class pipe_manager;
class ostream;
class ifstream;

class ACL_CPP_API mime_node
{
public:
	/**
	 * ���캯��
	 * @param emailFile {const char*} �洢�ʼ����ݵ�Դ�ļ�������
	 *  Ϊ�գ�����Ϊ��ʱ�ڵ��� save_body ����ʱ������ָ��Դ�ļ�
	 * @param node {const MIME_NODE*} �ʼ��е�ĳ��������
	 * @param enableDecode {bool} ���ʼ�����Ϊ base64/qp �ȱ����ʽ
	 *  ʱ�Ƿ���Ҫ�Զ����н���
	 * @param toCharset {const char*} ȱʡ��Ŀ���ַ��������Ŀ��
	 *  �ַ�����Դ�ַ�����ͬ��������ַ���ת��
	 * @param off {off_t} �ʼ����������������е���ʼλ���и��ӵ�
	 *  ���ƫ�������Ա����û��������ʼ�����ǰ����Լ���˽������
	 */
	mime_node(const char* emailFile, const MIME_NODE* node,
		bool enableDecode = true, const char* toCharset = "gb2312",
		off_t off = 0);
	virtual ~mime_node();

	/**
	 * ��� MIME ����� Content-Type ֵ�е� name �ֶ�ֵ
	 * @return {const char*} ���Ϊ�����ʾû�и��ֶ�ֵ
	 */
	const char* get_name() const
	{
		if (m_name.empty())
			return (NULL);
		return (m_name.c_str());
	}

	/**
	 * ��� Content-Type �е������ͣ���: Content-Type: image/jpeg, ��
	 * �������� MIME_CTYPE_IMAGE (�� mime_define.hpp �ж���)
	 * @return {int} ���� mime_define.hpp �ж���� MIME_CTYPE_XXX
	 */
	int get_ctype() const
	{
		return (m_ctype);
	}

	/**
	 * ��� Content-Type �еĴ����ͣ���: Content-Type: image/jpeg, ��
	 * �������� MIME_STYPE_JPEG (�� mime_define.hpp �ж���)
	 * @return {int} ���� mime_define.hpp �ж���� MIME_STYPE_XXX
	 */
	int get_stype() const
	{
		return (m_stype);
	}

	/**
	 * ��ô���������� (��Ӧ�� Content-Transfer-Encoding)
	 * @return {int} ���� mime_define.hpp �ж���� MIME_ENC_XXX
	 */
	int get_encoding() const
	{
		return (m_encoding);
	}

	/**
	 * ��ý���ַ����ַ���(��Ӧ�� Content-Type �е� charset �ֶ�)
	 * @return {const char*} Ϊ�����ʾû�и��ֶ�
	 */
	const char* get_charset() const
	{
		return (m_charset);
	}

	/**
	 * ���Ŀ���ַ���, ���û��ڹ��캯���д���
	 * @return {const char*} Ϊ�����ʾ�û�δ����
	 */
	const char* get_toCharset() const
	{
		if (m_toCharset[0])
			return (m_toCharset);
		else
			return (NULL);
	}

	/**
	 * ��ñ�������ʼ��е���ʼƫ����
	 * @return {off_t}
	 */
	off_t get_bodyBegin() const
	{
		return (m_bodyBegin);
	}

	/**
	 * ��ñ�������ʼ��еĽ���ƫ����
	 * @return {off_t}
	 */
	off_t get_bodyEnd() const
	{
		return (m_bodyEnd);
	}

	/**
	 * ��ñ����ͷ����ĳ���ֶε�ֵ
	 * @param name {const char*} �ֶ���, ��: Content-Type
	 * @return {const char*} Ϊ�����ʾ������
	 */
	const char* header_value(const char* name) const;

	/**
	 * ȡ�øý�������ͷ���ֶμ���
	 * @return {const std::map<string, string>&}
	 */
	const std::map<string, string>& get_headers() const;

	/**
	 * ת�������������ָ���Ĺܵ�����
	 * @param out {pipe_manager&}
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save(pipe_manager& out) const;

	/**
	 * ת�������������ָ���Ĺܵ�����
	 * @param out {pipe_manager&}
	 * @param src {const char*} �ʼ����ݵ���ʼ��ַ�����Ϊ��ָ�룬
	 *  ��ӹ��캯�������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @param len {int} �ʼ����ݵ����ݳ��ȣ����Ϊ0����ӹ���
	 *  ���������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save(pipe_manager& out, const char* src, int len) const;

	/**
	 * ת�������������ָ�����������
	 * @param out {ostream&} ������
	 * @param src {const char*} �ʼ����ݵ���ʼ��ַ�����Ϊ��ָ�룬
	 *  ��ӹ��캯�������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @param len {int} �ʼ����ݵ����ݳ��ȣ����Ϊ0����ӹ���
	 *  ���������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save(ostream& out, const char* src = NULL, int len = 0) const;

	/**
	 * ת�������������ָ�����ļ���
	 * @param outFile {const char*} Ŀ���ļ���
	 * @param src {const char*} �ʼ����ݵ���ʼ��ַ�����Ϊ��ָ�룬
	 *  ��ӹ��캯�������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @param len {int} �ʼ����ݵ����ݳ��ȣ����Ϊ0����ӹ���
	 *  ���������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save(const char* outFile, const char* src = NULL, int len = 0) const;

	/**
	 * ת������������ڻ�������
	 * @param out {string&} ������
	 * @param src {const char*} �ʼ����ݵ���ʼ��ַ�����Ϊ��ָ�룬
	 *  ��ӹ��캯�������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @param len {int} �ʼ����ݵ����ݳ��ȣ����Ϊ0����ӹ���
	 *  ���������ṩ�� emailFile ���ļ�����ȡ�ʼ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save(string& out, const char* src, int len) const;

	/**
	 * ��ñ�����Ӧ�ĸ�������
	 * @return {mime_node*} Ϊ�����ʾ�����û�и����(��˵��
	 *  �����Ϊ�ʼ��ĸ����); �����򷵻صĸ������Ҫ�������
	 *  delete �����ͷ���Ӧ�ڴ�
	 */
	mime_node* get_parent() const;

	/**
	 * �жϱ�����Ƿ��и����
	 * @return {bool} true ���ʾ�и����, �����ʾû��
	 */
	bool has_parent() const;

	/**
	 * ��ø����������� (MIME_CTYPE_XXX), ���Ϊ MIME_CTYPE_OTHER
	 * ��˵������㲻���ڻ򸸽���������δ֪
	 * @return {int} MIME_CTYPE_XXX
	 */
	int parent_ctype() const;

	/**
	 * ��ø����Ĵ����� (MIME_STYPE_XXX), ���Ϊ MIME_STYPE_OTHER
	 * ��˵������㲻���ڻ򸸽��Ĵ�����δ֪
	 * @return {int} MIME_STYPE_XXX
	 */
	int parent_stype() const;

	/**
	 * ��ø����ı������� (MIME_ENC_XXX), �������ֵΪ MIME_ENC_OTHER
	 * ��˵������㲻���ڻ򸸽��ı�������δ֪
	 * @return {int} MIME_ENC_XXX
	 */
	int parent_encoding() const;

	/**
	 * ��ø������ַ�������, �������ֵΪ����˵������㲻���ڻ򸸽��
	 * ��û���ַ�������
	 * @return {const char*}
	 */
	char* parent_charset() const;

	/**
	 * ��ø�������������ʼƫ����
	 * @return {off_t} ����ֵΪ -1 ��ʾ����㲻����
	 */
	off_t parent_bodyBegin() const;

	/**
	 * ��ø��������������ƫ����
	 * @return {off_t} ����ֵΪ -1 ��ʾ����㲻����
	 */
	off_t parent_bodyEnd() const;

	/**
	 * ��ø����ͷ����ĳ���ֶ�����Ӧ���ֶ�ֵ, ��: Content-Type
	 * @param name {const char*} �ֶ���
	 * @return {const char*} �ֶ�ֵ, ���ؿ���˵������㲻����
	 *  �򸸽��ͷ���в����ڸ��ֶ�
	 */
	const char* parent_header_value(const char* name) const;

protected:
	bool  m_enableDecode;
	string m_name;
	string m_emailFile;
	int   m_ctype;		// mime_define.hpp
	int   m_stype;		// mime_define.hpp
	int   m_encoding;	// mime_define.hpp
	char  m_charset[32];
	char  m_toCharset[32];
	off_t m_bodyBegin;
	off_t m_bodyEnd;
	std::map<string, string>* m_headers_;
	const MIME_NODE* m_pMimeNode;
	mime_node* m_pParent;
};

} // namespace acl
