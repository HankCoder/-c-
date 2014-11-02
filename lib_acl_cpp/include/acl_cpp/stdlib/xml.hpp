#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <list>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/pipe_stream.hpp"

struct ACL_XML;
struct ACL_XML_NODE;
struct ACL_XML_ATTR;
struct ACL_TOKEN;
struct ACL_ITER;

/**
 * �� ACL ���� XML ������ķ�װ������ C++ �û�ʹ�ã������̫ע���������أ�
 * ����ֱ��ʹ�ø��࣬����ڷ����ִ���ҷǳ�ע�����ܣ�����ֱ��ʹ�� ACL ���
 * XML ����������Ϊ����Ҳ�ǵ����� ACL ���е� XML �������̣������ж��ο���
 * ���̣����ܻ���΢Ӱ��һЩ���ܣ�������һ���Ӧ�����Ӱ����΢�������
 */

namespace acl {

class xml;
class xml_node;

class ACL_CPP_API xml_attr
{
public:
	/**
	 * ���������
	 * @return {const char*} ����������Զ���᷵�ؿ�ָ�룬����ֵ
	 *  �п���Ϊ "\0"
	 */
	const char* get_name(void) const;

	/**
	 * �������ֵ
	 * @return {const char*} ����ֵ����Զ���᷵�ؿ�ָ�룬����ֵ
	 *  �п���Ϊ "\0"
	 */
	const char* get_value(void) const;
private:
	friend class xml_node;

	xml_node* node_;
	ACL_XML_ATTR* attr_;

	xml_attr(void);
	~xml_attr(void);
};

class ACL_CPP_API xml_node
{
public:
	/**
	 * ȡ�ñ� XML ���ı�ǩ��
	 * @return {const char*} ���� XML ����ǩ����������ؿգ���˵��
	 *  �����ڱ�ǩ��xxxx���Է���һ����������Ҫ�жϷ���ֵ
	 */
	const char* tag_name(void) const;

	/**
	 * ����� XML ���� ID �����Բ����ڣ��򷵻ؿ�ָ��
	 * @return {const char*} �� ID ���Դ���ʱ���ض�Ӧ��ֵ�����򷵻ؿ�
	 */
	const char* id(void) const;

	/**
	 * ���ظ� XML ������������
	 * @return {const char*} ���ؿ�˵��û����������
	 */
	const char* text(void) const;

	/**
	 * ���ظ� XML ����ĳ������ֵ
	 * @param name {const char*} ������
	 * @return {const char*} ����ֵ��������ؿ���˵�������Բ�����
	 */
	const char* attr_value(const char* name) const;

	/**
	 * ���ظ� XML ����ĳ������ֵ�ı��д��
	 * @param name {const char*} ������
	 * @return {const char*} ����ֵ��������ؿ���˵�������Բ�����
	 */
	const char* operator[](const char* name) const;

	/**
	 * ����������������ʱ����Ҫ���ô˺�������õ�һ�����Զ���
	 * @return {const xml_attr*} ���ص�һ�����Զ�����Ϊ�գ����ʾ
	 *  �ý��û������
	 */
	const xml_attr* first_attr(void) const;

	/**
	 * ����������������ʱ�����ñ����������һ�����Զ���
	 * @return {const xml_attr*} ������һ�����Զ�����Ϊ�գ����ʾ
	 *  �������
	 */
	const xml_attr* next_attr(void) const;

	/**
	 * ��� XML �������
	 * @param name {const char*} ������
	 * @param value {const char*} ����ֵ
	 * @return {xml_node&}
	 */
	xml_node& add_attr(const char* name, const char* value);

	/**
	 * ��� XML �������
	 * @param name {const char*} ������
	 * @param n {char} ����ֵ
	 * @return {xml_node&}
	 */
	xml_node& add_attr(const char* name, char n);

	/**
	 * ��� XML �������
	 * @param name {const char*} ������
	 * @param n {int} ����ֵ
	 * @return {xml_node&}
	 */
	xml_node& add_attr(const char* name, int n);

	/**
	 * ��� XML �������
	 * @param name {const char*} ������
	 * @param n {size_t} ����ֵ
	 * @return {xml_node&}
	 */
	xml_node& add_attr(const char* name, size_t n);

	/**
	 * ��� XML �������
	 * @param name {const char*} ������
	 * @param n {acl_int64} ����ֵ
	 * @return {xml_node&}
	 */
#ifdef	WIN32
	xml_node& add_attr(const char* name, __int64 n);
#else
	xml_node& add_attr(const char* name, long long int n);
#endif

	/**
	 * ���� xml �����ı�����
	 * @param str {const char*} �ַ�������
	 * @return {xml_node&}
	 */
	xml_node& set_text(const char* str);

	/**
	 * ���� xml ������ xml_node �ӽ�����
	 * @param child {xml_node*} �ӽ�����
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {xml_node&} return_child Ϊ true �����ӽ������ã�
	 *  ���򷵻ر� xml �������
	 */
	xml_node& add_child(xml_node* child, bool return_child = false);

	/**
	 * ���� xml ������ xml_node �ӽ�����
	 * @param child {xml_node&} �ӽ�����
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {xml_node&} return_child Ϊ true �����ӽ������ã�
	 *  ���򷵻ر� xml �������
	 */
	xml_node& add_child(xml_node& child, bool return_child = false);

	/**
	 * ���� xml ������ xml_node �ӽ�����
	 * @param tag {const char* tag} �ӽ�����ı�ǩ��
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @param str {const char*} �ı��ַ���
	 * @return {xml_node&} return_child Ϊ true �����ӽ������ã�
	 *  ���򷵻ر� xml �������
	 */
	xml_node& add_child(const char* tag, bool return_child = false,
		const char* str = NULL);

	/**
	 * ��ñ����ĸ��������������
	 * @return {xml_node&}
	 */
	xml_node& get_parent(void) const;

	/**
	 * ��ñ����ĵ�һ���ӽ�㣬��Ҫ�����ӽ��ʱ�������ȵ��ô˺���
	 * @return {xml_node*} ���ؿձ�ʾû���ӽ��
	 */
	xml_node* first_child(void);

	/**
	 * ��ñ�������һ���ӽ��
	 * @return {xml_node*} ���ؿձ�ʾ�������̽���
	 */
	xml_node* next_child(void);

	/**
	 * ���ظ� XML ��������� XML ���е����
	 * @return {int}
	 */
	int   depth(void) const;

	/**
	 * ���ظ� xml ������һ���ӽ��ĸ���
	 * @return {int} ��Զ >= 0
	 */
	int   children_count(void) const;

	/**
	 * ��� xml ���������
	 * @return {xml&}
	 */
	xml& get_xml(void) const;

	/**
	 * ȡ����Ӧ�� ACL ���е� XML ������
	 * @return {ACL_XML_NODE*} ���ؽ�����ע���ý���û����ܵ����ͷ�
	 */
	ACL_XML_NODE* get_xml_node() const;

private:
	friend class xml;

	/**
	 * xml ��㹹�캯��
	 * @param node {ACL_XML_NODE*} �μ� acl �� acl_xml.h�е�ͷ�ļ����ǿ�
	 * @param xml_ptr {xml*} xml �����󣬷ǿ�
	 */
	xml_node(ACL_XML_NODE* node, xml* xml_ptr);

	/**
	 * Ҫ��ö�������Ƕ�̬������
	 */
	~xml_node(void);

	/**
	 * ���� xml ���
	 * @param node {ACL_XML_NODE*}
	 */
	void set_xml_node(ACL_XML_NODE* node);

private:
	ACL_XML_NODE *node_;
	xml* xml_;
	xml_node* parent_;
	xml_node* parent_saved_;
	xml_node* child_;
	ACL_ITER* child_iter_;
	xml_attr* attr_;
	ACL_ITER* attr_iter_;
};

class string;

class ACL_CPP_API xml : public pipe_stream
{
public:
	/**
	 * ���캯��
	 * @param data {const char*} xml ��ʽ���ַ�����������������
	 *  xml �ַ�����Ҳ�����ǲ��ֵ� xml �ַ�����Ҳ�����ǿ�ָ�룬
	 *  ������Σ��û���Ȼ�����ò��ֻ������� xml �ַ������� update
	 *  �������ڵ��� update �����н��� xml����ʵ�������캯����
	 *  �� data �����ǿ�ʱ����Ҳ����� update
	 */
	xml(const char* data = NULL);
	~xml(void);

	xml& part_word(bool on);
	xml& ignore_slash(bool on);

	/**
	 * ����ʽ��ʽѭ�����ñ�������� XML ���ݣ�Ҳ����һ�������
	 * ������ XML ���ݣ�������ظ�ʹ�ø� XML ������������� XML
	 * ������Ӧ���ڽ�����һ�� XML ����ǰ���� reset() ��������
	 * ����һ�εĽ������
	 * @param data {const char*} xml ����
	 */
	void update(const char* data);

	/**
	 * ���� XML ������״̬���� XML ������������Զ�� XML ����
	 * ���н������ڷ���ʹ�ñ� XML ������ǰ����Ҫ���ñ���������
	 * �ڲ� XML ������״̬�������һ�εĽ������
	 */
	void reset(void);

	/**
	 * �ӽ����� XML ԭʼ�����н���ȡ�ı�����
	 * @return {const string&} ���ؽ�������������ã�����������
	 *  ���������û�����Ҫ�ͷ�
	 */
	const string& getText(void);

	/**
	 * �� XML ������ȡ��ĳ����ǩ�������н�㼯��
	 * @param tag {const char*} ��ǩ��(�����ִ�Сд)
	 * @return {const std::vector<xml_node*>&} ���ؽ�����Ķ������ã�
	 *  �����ѯ���Ϊ�գ���ü���Ϊ�գ�����empty() == true
	 *  ע�����ص������е� xml_node ������ݿ����޸ģ�������ɾ���ý�㣬
	 *  ��Ϊ�ÿ��ڲ����Զ�ɾ���Ļ���
	 */
	const std::vector<xml_node*>& getElementsByTagName(const char* tag) const;

	/**
	 * �� xml �����л�ö�Ӧ��ǩ���ĵ�һ�� xml ������
	 * @param tag {const char*} ��ǩ��(�����ִ�Сд)
	 * @return {const xml_node*} ���ؿձ����ñ�ǩ��Ӧ�� xml ��㲻����
	 */
	const xml_node* getFirstElementByTag(const char* tag) const;

	/**
	 * �� xml �����л�����е�������༶��ǩ����ͬ�� xml ���ļ���
	 * @param tags {const char*} �༶��ǩ������ '/' �ָ�������ǩ��������� xml ���ݣ�
	 *  <root> <first> <second> <third name="test1"> text1 </third> </second> </first> ...
	 *  <root> <first> <second> <third name="test2"> text2 </third> </second> </first> ...
	 *  <root> <first> <second> <third name="test3"> text3 </third> </second> </first> ...
	 *  ����ͨ���༶��ǩ����root/first/second/third һ���Բ�����з��������Ľ��
	 * @return {const std::vector<xml_node*>&} ���������� xml ��㼯��, 
	 *  �����ѯ���Ϊ�գ���ü���Ϊ�գ�����empty() == true
	 *  ע�����ص������е� xml_node ������ݿ����޸ģ�������ɾ���ý�㣬
	 *  ��Ϊ�ÿ��ڲ����Զ�ɾ���Ļ���
	 */
	const std::vector<xml_node*>& getElementsByTags(const char* tags) const;

	/**
	 * �� xml �����л��ָ���༶��ǩ���ĵ�һ�� xml ���
	 * @param tags {const char*} �༶��ǩ������ '/' �ָ�������ǩ��������� xml ���ݣ�
	 *  <root> <first> <second> <third name="test1"> text1 </third> </second> </first> ...
	 *  <root> <first> <second> <third name="test2"> text2 </third> </second> </first> ...
	 *  <root> <first> <second> <third name="test3"> text3 </third> </second> </first> ...
	 *  ����ͨ���༶��ǩ����root/first/second/third һ���Բ�����з��������Ľ��
	 * @return {const xml_node*} ���ؿձ�ʾ������
	 */
	const xml_node* getFirstElementByTags(const char* tags) const;

	/**
	 * �� xml �����л�����е������������ name ������ֵ��ͬ�� xml ���Ԫ�ؼ���
	 * @param value {const char*} ������Ϊ name ������ֵ
	 * @return {const std::vector<xml_node*>&} ���ؽ�����Ķ������ã�
	 *  �����ѯ���Ϊ�գ���ü���Ϊ�գ�����empty() == true
	 *  ע�����ص������е� xml_node ������ݿ����޸ģ�������ɾ���ý�㣬
	 *  ��Ϊ�ÿ��ڲ����Զ�ɾ���Ļ���
	 */
	const std::vector<xml_node*>& getElementsByName(const char* value) const;

	/**
	 * �� xml �����л�����и���������������ֵ�� xml ���Ԫ�ؼ���
	 * @param name {const char*} ������
	 * @param value {const char*} ����ֵ
	 * @return {const std::vector<xml_node*>&} ���ؽ�����Ķ������ã�
	 *  �����ѯ���Ϊ�գ���ü���Ϊ�գ�����empty() == true
	 */
	const std::vector<xml_node*>& getElementsByAttr(
		const char* name, const char* value) const;

	/**
	 * �� xml �����л��ָ�� id ֵ�� xml ���Ԫ��
	 * @param id {const char*} id ֵ
	 * @return {const xml_node*} xml ���Ԫ��, ������ NULL ���ʾû�з���
	 *  ������ xml ���, ����ֵ����Ҫ�ͷ�
	 */
	const xml_node* getElementById(const char* id) const;

	/**
	 * ȡ�� acl ���е� ACL_XML ����
	 * @return {ACL_XML*} ��ֵ������Ϊ�գ�ע���û������޸ĸö����ֵ��
	 *  ���������ͷŸö���
	 */
	ACL_XML* get_xml(void) const;

	/**
	 * ����һ�� xml_node ������
	 * @param tag {const char*} ��ǩ��
	 * @param text {const char*} �ı��ַ���
	 * @return {xml_node*} �²����� xml_node ������Ҫ�û��ֹ��ͷţ���Ϊ��
	 *  xml �����ͷ�ʱ��Щ�����Զ����ͷţ���Ȼ�û�Ҳ�����ڲ���ʱ����
	 *  reset ���ͷ���Щ xml_node ������
	 */
	xml_node& create_node(const char* tag, const char* text = NULL);

	/**
	 * ��ø������󣬵���Ҫע�⣬�ý��Ϊ���㣬���治����κ����ݣ�
	 * �������� xml ���������㸸����
	 * @return {xml_node&}
	 */
	xml_node& get_root();

	/**
	 * ��ʼ������ xml ���󲢻�õ�һ�����
	 * @return {xml_node*} ���ؿձ�ʾ�� xml ����Ϊ�ս��
	 *  ע�����صĽ������û������ֹ��ͷţ���Ϊ�ö���
	 *  �ڲ����Զ��ͷ�
	 */
	xml_node* first_node(void);

	/**
	 * ������ xml �������һ�� xml ���
	 * @return {xml_node*} ���ؿձ�ʾ�������
	 *  ע�����صĽ������û������ֹ��ͷţ���Ϊ�ö���
	 *  �ڲ����Զ��ͷ�
	 */
	xml_node* next_node(void);

	/**
	 * �� xml ������ת���ַ���
	 * @param out {string&} �洢ת������Ļ�����
	 */
	void build_xml(string& out);

	// pipe_stream �麯������

	virtual int push_pop(const char* in, size_t len,
		string* out, size_t max = 0);
	virtual int pop_end(string* out, size_t max = 0);
	virtual void clear(void);

private:
	ACL_XML *xml_;
	xml_node* node_;
	xml_node* root_;
	std::vector<xml_node*> elements_;
	string* buf_;
	//bool dummyRootAdded_;

	ACL_TOKEN* m_pTokenTree;
	std::list<xml_node*> nodes_;
	ACL_ITER* iter_;
};

} // namespace acl
