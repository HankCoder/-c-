#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <list>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/mime/mime_attach.hpp"
#include "acl_cpp/http/http_type.hpp"

struct MIME_STATE;
struct MIME_NODE;

namespace acl {

/**
 * http mime ����࣬�̳й�ϵ��
 *   http_mime_node : mime_attach : mime_node
 * ���ú������ܣ�
 * http_mime_node::get_mime_type ��øý�������
 * mime_node::get_name: ��øý�������
 * mime_attach::get_filename: �����Ϊ�ϴ��ļ�����ʱ���˺���
 *   ����ϴ��ļ����ļ���
 * http_mime_node::get_value: �����Ϊ��������ʱ���˺������
 *   ����ֵ
 */
class ACL_CPP_API http_mime_node : public mime_attach
{
public:
	/**
	 * @param path {const char*} ԭʼ�ļ����·��������Ϊ��
	 * @param node {MIME_NODE*} ��Ӧ�� MIME ��㣬�ǿ�
	 * @param decodeIt {bool} �Ƿ�� MIME ����ͷ������
	 *  �����������ݽ��н���
	 * @param toCharset {const char*} �������ַ���
	 * @param off {off_t} ƫ������λ��
	 */
	http_mime_node(const char* path, const MIME_NODE* node,
		bool decodeIt = true, const char* toCharset = "gb2312",
		off_t off = 0);
	~http_mime_node(void);

	/**
	 * ��øý�������
	 * @return {http_mime_t}
	 */
	http_mime_t get_mime_type(void) const;

	/**
	 * �� get_mime_type ���ص�����Ϊ HTTP_MIME_PARAM ʱ������
	 * ���ô˺�����ò���ֵ������������ͨ������� get_name() ���
	 * @return {const char*} ���� NULL ��ʾ����������
	 */
	const char* get_value(void) const;
protected:
private:
	http_mime_t mime_type_;
	char* param_value_;

	void load_param(const char* path);
};

//////////////////////////////////////////////////////////////////////////

/**
 * http mime ���������ý�����Ϊ��ʽ���������û���ʹ��ʱ����ÿ�ν�����
 * �������ݸ� update ���������ú������� true ʱ��ʾ��������ҽ�����ȷ
 */
class ACL_CPP_API http_mime
{
public:
	/**
	 * ��������
	 * @param boundary {const char*} �ָ���������Ϊ��
	 * @param local_charset {const char*} �����ַ������ǿ�ʱ���Զ���
	 *  ��������תΪ�����ַ���
	 */
	http_mime(const char* boundary, const char* local_charset  = "gb2312");
	~http_mime(void);

	/**
	 * ���� MIME ���ݵĴ洢·������������ MIME ���ݺ������Ҫ
	 * ������ȡ���ݣ����������� MIME ��ԭʼ���ݵĴ洢λ�ã�����
	 * �޷������Ӧ���ݣ��� save_xxx/get_nodes/get_node �������޷�
	 * ����ʹ��
	 * @param path {const char*} �ļ�·����, ����ò���Ϊ��, ����
	 *  �������������, Ҳ���ܵ��� save_xxx ��صĽӿ�
	 */
	void set_saved_path(const char* path);

	/**
	 * ���ô˺���������ʽ��ʽ��������������
	 * @param data {const char*} ������(����������ͷҲ������������, 
	 *  ���Ҳ�����������������)
	 * @param len {size_t} data ���ݳ���
	 * @return {bool} ��� multipart ����, ���� true ��ʾ�������;
	 *  ���ڷ� multipart �ļ�, �÷���ֵ��ԶΪ false, û���κ�����, 
	 *  ��Ҫ�������Լ��ж�������Ľ���λ��
	 * ע��: ������˺�����һ����Ҫ���� update_end ����֪ͨ������
	 * �������
	 */
	bool update(const char* data, size_t len);

	/**
	 * ������е� MIME ���
	 * @return {const std::list<http_mimde_node*>&}
	 */
	const std::list<http_mime_node*>& get_nodes(void) const;

	/**
	 * ���ݱ�����ȡ�� HTTP MIME ���
	 * @param name {const char*} ������
	 * @return {const http_mime_node*} ���ؿ���˵����Ӧ�������Ľ��
	 *  ������
	 */
	const http_mime_node* get_node(const char* name) const;
protected:
private:
	char* boundary_;
	char* save_path_;
	off_t off_;
	MIME_STATE* mime_state_;
	std::list<http_mime_node*> mime_nodes_;
	char  local_charset_[32];
	bool  decode_on_;
	bool  parsed_;
};

} // namespace acl
