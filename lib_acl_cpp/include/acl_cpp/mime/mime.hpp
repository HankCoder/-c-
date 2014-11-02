#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#include <string>
#include "acl_cpp/mime/mime_head.hpp"

struct MIME_STATE;

namespace acl {

class string;
class mime_node;
class mime_attach;
class mime_body;
class mime_image;
class ifstream;
class fstream;

class ACL_CPP_API mime
{
public:
	mime();
	~mime();

	/////////////////////////////////////////////////////////////////////
	// ���ʼ�������صĺ���

	/**
	 * ����ͬһ��MIME�������Զ���ʼ�����ʱ, ��Ҫ���ô˺�������֮ǰ
	 * ���������в������ڴ�, ��Ȼ��ε��øú����޺�, ��Ϊ�˲�Ӱ��
	 * Ч��, ��������øý�����������һ���ʼ�ǰ���øú���
	 */
	mime& reset(void);

	/**
	 * �����߿����ֹ����ô˺����Դ��������ʼ�ͷ����
	 */
	void primary_head_finish(void);

	/**
	 * ������ʽ����ʱ�ô˺����ж��ʼ�ͷ�Ƿ�������
	 * @return {bool} �Ƿ��ʼ�ͷ�������
	 */
	bool primary_head_ok() const;

	/**
	 * ��ʼ������ʽ��������, �ú����ڲ����Զ����� reset() ���������ý���
	 * ��״̬
	 * @param path {const char*} �ʼ��ļ�·����, ����ò���Ϊ��, ����
	 *  ����ʼ�������, Ҳ���ܵ��� save_xxx ��صĽӿ�
	 */
	void update_begin(const char* path);

	/**
	 * ���ô˺���������ʽ��ʽ�����ʼ�����, �����������ʼ�ͷ, ������ô�
	 * �ӿڽ������ʼ�ͷ����� update_end() �ӿڼ���, �����Ҫ����������һ
	 * ���ʼ�, ����Ҫ���ϵص��ô˺���ֱ������������ true ��ʾ multipart ��ʽ
	 * ���ʼ��������; ������� multipart ��ʽ�ʼ�, ��˺��������ܻ�ط� true,
	 * ��������Ҫ�����ж��ʼ��Ľ���λ��
	 * @param data {const char*} �ʼ�����(�������ʼ�ͷҲ�������ʼ���, ����
	 *  ������������������)
	 * @param len {size_t} data ���ݳ���
	 * @return {bool} ��� multipart �ʼ�, ���� true ��ʾ�÷��ʼ��������;
	 *  ���ڷ� multipart �ʼ�, �÷���ֵ��ԶΪ false, û���κ�����, ��Ҫ����
	 *  ���Լ��ж��ʼ��Ľ���λ��
	 * ע��: ������˺�����һ����Ҫ���� update_end ����֪ͨ�������������
	 */
	bool update(const char* data, size_t len);

	/**
	 * �ڲ�����ʽ���������������ô˺���
	 */
	void update_end(void);

	/**
	 * ���ô˺������������ϵ�һ���ʼ�
	 * @param file_path {const char*} �ʼ��ļ�·��
	 * @return {bool} ������� false ˵��Դ�ʼ��ļ��޷���
	 */
	bool parse(const char* file_path);

	/**
	 * ���ʼ�����������Ϊ��һ���ļ���
	 * @param out {fstream&} Ŀ��������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save_as(fstream& out);

	/**
	 * ���ʼ�����������Ϊ��һ���ļ���
	 * @param file_path {const char*} Ŀ���ļ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save_as(const char* file_path);

	/**
	 * �ʼ�������Ϻ󣬰��ͻ���ʾ�ķ�ʽ��������������ڴ��̣�
	 * �û�����ʹ��������򿪸� html ҳ��
	 * @param path {const char*} ҳ�汣��·��
	 * @param filename {const char*} Ŀ���ļ���
	 * @param enableDecode {bool} ת��ʱ�Ƿ��Զ����н���
	 * @param toCharset {const char*} Ŀ���ַ���
	 * @param off {off_t} ������ϣ�����ʼ���㸽�ӵ����ƫ����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool save_mail(const char* path, const char* filename,
		bool enableDecode = true, const char* toCharset = "gb2312",
		off_t off = 0);

	/**
	 * ����ʼ����Ľ��
	 * @param htmlFirst {bool} ���Ȼ��HTML��ʽ���ı����������Ȼ��
	 *  ���ı��������ֻ��HTML�ı���ת��Ϊ���ı�
	 * @param enableDecode {bool} ת��ʱ�Ƿ��ԭ�Ľ��н���
	 * @param toCharset {const char*} Ŀ���ַ���
	 * @param off {off_t} ������ϣ�����ʼ����㸽�ӵ����ƫ����
	 * @return {mime_body*} ��δ�ҵ����������򷵻� NULL
	 */
	mime_body* get_body_node(bool htmlFirst, bool enableDecode = true,
                const char* toCharset = "gb2312", off_t off = 0);

	/**
	 * ������е� mime ����б�
	 * @param enableDecode {bool} ת��ʱ�Ƿ��Զ����н���
	 * @param toCharset {const char*} Ŀ���ַ���
	 * @param off {off_t} ������ϣ�����ʼ���㸽�ӵ����ƫ����
	 * @return {const std::list<mime_node*>&}
	 */
	const std::list<mime_node*>& get_mime_nodes(bool enableDecode = true,
		const char* toCharset = "gb2312", off_t off = 0);

	/**
	 * ��ø����б�
	 * @param enableDecode {bool} ת��ʱ�Ƿ��Զ����н���
	 * @param toCharset {const char*} Ŀ���ַ���
	 * @param off {off_t} ������ϣ�����ʼ���㸽�ӵ����ƫ����
	 * @return {const std::list<mime_attach*>&}
	 */
	const std::list<mime_attach*>& get_attachments(bool enableDecode = true,
		const char* toCharset = "gb2312", off_t off = 0);

	/**
	 * ���ͼƬ�б�
	 * @param enableDecode {bool} ת��ʱ�Ƿ��Զ����н���
	 * @param toCharset {const char*} Ŀ���ַ���
	 * @param off {off_t} ������ϣ�����ʼ���㸽�ӵ����ƫ����
	 * @return {const std::list<mime_image*>&}
	 */
	const std::list<mime_image*>& get_images(bool enableDecode = true,
		const char* toCharset = "gb2312", off_t off = 0);
	mime_image* get_image(const char* cld, bool enableDecode = true,
		const char* toCharset = "gb2312", off_t off = 0);

	/**
	 * ����MIME�������
	 * @param save_path {const char*} �洢 MIME ���������·��
	 * @param decode {bool} �Ƿ��ԭ�Ľ��н���
	 */
	void mime_debug(const char* save_path, bool decode = true);

	/////////////////////////////////////////////////////////////////////
        // ���ʼ�ͷ��صĺ���

	/**
	 * ���÷�����
	 * @param addr {const char*} �ʼ���ַ
	 * @return {mime&}
	 */
	mime& set_sender(const char* addr)
	{
		m_primaryHeader.set_returnpath(addr);
		return (*this);
	}

	/**
	 * ���÷�����: From: zhengshuxin@51iker.com
	 * @param addr {const char*} �ʼ���ַ
	 * @return {mime&}
	 */
	mime& set_from(const char* addr)
	{
		m_primaryHeader.set_from(addr);
		return (*this);
	}

	/**
	 * �����ʼ��ص�ַ: Reply-To: zhengshuxin@51iker.com
	 * @param addr {const char*} �ʼ���ַ
	 * @return {mime&}
	 */
	mime& set_replyto(const char* addr)
	{
		m_primaryHeader.set_replyto(addr);
		return (*this);
	}

	/**
	 * �����ʼ�������ַ Return-Path: <zhengshuxin@51iker.com>
	 * @param addr {const char*} �ʼ���ַ
	 * @return {mime&}
	 */
	mime& set_returnpath(const char* addr)
	{
		m_primaryHeader.set_returnpath(addr);
		return (*this);
	}

	/**
	 * �����ʼ�����: Subject: test
	 * @param s {const char*} �ʼ�����
	 * @return {mime&}
	 */
	mime& set_subject(const char* s)
	{
		m_primaryHeader.set_subject(s);
		return (*this);
	}

	/**
	 * ����ʼ�������: To: <zhengshuxin@51iker.com>
	 * @param addr {const char*} �ʼ���ַ
	 * @return {mime&}
	 */
	mime& add_to(const char* addr)
	{
		m_primaryHeader.add_to(addr);
		return (*this);
	}

	/**
	 * ����ʼ�������: CC: <zhengshuxin@51iker.com>
	 * @param addr {const char* addr} �ʼ���ַ
	 * @return {mime&}
	 */
	mime& add_cc(const char* addr)
	{
		m_primaryHeader.add_cc(addr);
		return (*this);
	}

	/**
	 * ����ʼ�������: BCC: <zhengshuxin@51iker.com>
	 * @param addr {const char* addr} �ʼ���ַ
	 * @return {mime&}
	 */
	mime& add_bcc(const char* addr)
	{
		m_primaryHeader.add_bcc(addr);
		return (*this);
	}

	/**
	 * ����ʼ�������: CC: <zhengshuxin@51iker.com>
	 * @param addr {const char* addr} �ʼ���ַ
	 * @return {mime&}
	 */
	mime& add_rcpt(const char* addr)
	{
		m_primaryHeader.add_rcpt(addr);
		return (*this);
	}

	/**
	 * ����ʼ�ͷ���ֶ�
	 * @param name {const char*} �ֶ���
	 * @param value {const char*} �ֶ�ֵ
	 * @return {mime&}
	 */
	mime& add_header(const char* name, const char* value)
	{
		m_primaryHeader.add_header(name, value);
		return (*this);
	}

	/**
	 * �����ʼ�ͷ����������: Content-Type: text/plain
	 * @param ctype {size_t} ������
	 * @param stype {size_t} ������
	 * @return {mime&}
	 */
	mime& set_type(size_t ctype, size_t stype)
	{
		m_primaryHeader.set_type(ctype, stype);
		return (*this);
	}

	/**
	 * �����ʼ�ͷ���ķָ���
	 * @param s {const char*} �ָ���
	 * @return {mime&}
	 */
	mime& set_boundary(const char* s)
	{
		m_primaryHeader.set_boundary(s);
		return (*this);
	}

	/**
	 * ��÷�����
	 * @return {const string&} ������ض��������Ϊ��
	 *  (���� string::empty()) ���ʾû�д��ֶ�
	 */
	const string& sender(void) const
	{
		return (m_primaryHeader.sender());
	}

	/**
	 * ��÷�����
	 * @return {const string&} ������ض��������Ϊ��
	 *  (���� string::empty()) ���ʾû�д��ֶ�
	 */
	const string& from(void) const
	{
		return (m_primaryHeader.from());
	}

	/**
	 * ��ûظ��ʼ���ַ
	 * @return {const string&} ������ض��������Ϊ��
	 *  (���� string::empty()) ���ʾû�д��ֶ�
	 */
	const string& replyto(void) const
	{
		return (m_primaryHeader.replyto());
	}

	/**
	 * ��ûظ��ʼ���ַ
	 * @return {const string&} ������ض��������Ϊ��
	 *  (���� string::empty()) ���ʾû�д��ֶ�
	 */
	const string& returnpath(void) const
	{
		return (m_primaryHeader.returnpath());
	}

	/**
	 * ����ʼ�����
	 * @return {const string&} ������ض��������Ϊ��
	 *  (���� string::empty()) ���ʾû�д��ֶ�
	 */
	const string& subject(void) const
	{
		return (m_primaryHeader.subject());
	}

	/**
	 * ����ռ����б�: To: xxx@xxx.com
	 * @return {const std::list<char*>&) ������ض��������Ϊ��
	 *  (���� std::list<char*>::empty()) ���ʾû�д��ֶ�
	 */
	const std::list<char*>& to_list(void) const
	{
		return (m_primaryHeader.to_list());
	}

	/**
	 * ��ó������б�: To: xxx@xxx.com
	 * @return {const std::list<char*>&) ������ض��������Ϊ��
	 *  (���� std::list<char*>::empty()) ���ʾû�д��ֶ�
	 */
	const std::list<char*>& cc_list(void) const
	{
		return (m_primaryHeader.cc_list());
	}

	/**
	 * ��ð������б�: To: xxx@xxx.com
	 * @return {const std::list<char*>&) ������ض��������Ϊ��
	 *  (���� std::list<char*>::empty()) ���ʾû�д��ֶ�
	 */
	const std::list<char*>& bcc_list(void) const
	{
		return (m_primaryHeader.bcc_list());
	}

	/**
	 * ����ռ����б�:
	 * To: xxx@xxx.xxx, CC: xxx@xxx.xxx, BCC: xxx@xxx.xxx
	 * @return {const std::list<char*>&) ������ض��������Ϊ��
	 *  (���� std::list<char*>::empty()) ���ʾû�д��ֶ�
	 */
	const std::list<char*>& rcpt_list(void) const
	{
		return (m_primaryHeader.rcpt_list());
	}

	/**
	 * ����ʼ�ͷ�ĸ����ֶ��б�
	 * @return {const std::list<HEADER*>&)
	 */
	const std::list<HEADER*>& header_list(void) const
	{
		return (m_primaryHeader.header_list());
	}	

	/**
	 * ��ѯ�ʼ�ͷ��Ӧ�ֶ������ֶ�ֵ
	 * @param name {const char*} �ֶ���
	 * @return {const char*} �ֶ�ֵ, Ϊ��ʱ��ʾ������
	 */
	const char* header_value(const char* name) const
	{
		return (m_primaryHeader.header_value(name));
	}

	/**
	 * ��ѯ�ʼ�ͷ��Ӧ�ֶ������ֶ�ֵ����
	 * @param name {const char*} �ֶ���
	 * @param values {std::list<const char*>*} �洢��Ӧ�Ľ����
	 * @return {int} �ֶ�ֵ���ϵĸ���
	 */
	int header_values(const char* name, std::list<const char*>* values) const
	{
		return (m_primaryHeader.header_values(name, values));
	}

	/**
	 * ����ʼ�ͷ�й��� Content-Type: text/html �е� text �ֶ�
	 * @return {const char*} ��Զ���طǿ�ֵ
	 */
	const char* get_ctype() const
	{
		return m_primaryHeader.get_ctype();
	}

	/**
	 * ����ʼ�ͷ�й��� Content-Type: text/html �е� html �ֶ�
	 * @return {const char*} ��Զ���طǿ�ֵ
	 */
	const char* get_stype() const
	{
		return m_primaryHeader.get_stype();
	}

	/**
	 * ����ʼ�ͷ
	 * @return {const mime_head&}
	 */
	const mime_head& primary_header(void) const
	{
		return (m_primaryHeader);
	}

private:
	mime_head m_primaryHeader;

	MIME_STATE* m_pMimeState;
	bool m_bPrimaryHeadFinish;
	char* m_pFilePath;
	mime_body* m_pBody;
	std::list<mime_node*>* m_pNodes;
	std::list<mime_attach*>* m_pAttaches;
	std::list<mime_image*>* m_pImages;
};

} // namespace acl
