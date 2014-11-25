#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#include <vector>
#include "acl_cpp/stdlib/pipe_stream.hpp"

struct ACL_JSON_NODE;
struct ACL_JSON;
struct ACL_ITER;

/**
 * �� ACL ���� json ������ķ�װ������ C++ �û�ʹ�ã������̫ע���������أ�
 * ����ֱ��ʹ�ø��࣬����ڷ����ִ���ҷǳ�ע�����ܣ�����ֱ��ʹ�� ACL ���
 * json ����������Ϊ����Ҳ�ǵ����� ACL ���е� json �������̣������ж��ο���
 * ���̣����ܻ���΢Ӱ��һЩ���ܣ�������һ���Ӧ�����Ӱ����΢�������
 */

namespace acl {

class string;
class json;

/**
 * json ��㣬������������ json.create_node() ��ʽ����
 */
class ACL_CPP_API json_node
{
public:
	/**
	 * ȡ�ñ� json ���ı�ǩ��
	 * @return {const char*} ���� json ����ǩ����������ؿգ���˵��
	 *  ��������Ҫ�жϷ���ֵ
	 */
	const char* tag_name(void) const;

	/**
	 * ���ظ� json �����ı���ǩֵ������ֵΪ�����ͻ���ֵ��ʱ�����߿�
	 * ���н���ת��
	 * @return {const char*} ���ؿ�˵��û���ı���ǩֵ
	 */
	const char* get_text(void) const;

	/**
	 * ���� json �������ӽ��ʱ�����ر� json ����ǩ��Ӧ�� json �ӽ��
	 * @param {const json_node*} ���� NULL ˵���������ӽ��
	 *  ע��get_text �� get_obj ����ͬʱ���ط� NULL
	 */
	json_node* get_obj(void) const;

	/**
	 * ���� json ����б�ǩʱ�������������µı�ǩֵ���Ǿɵı�ǩ��
	 * @param name {const char*} �µı�ǩֵ��Ϊ�ǿ��ַ���
	 * @return {bool} ���� false ��ʾ�ý��û�б�ǩ������մ���û�н����滻
	 */
	bool set_tag(const char* name);

	/**
	 * ���� json ���ΪҶ���ʱ�������������滻�����ı�ֵ
	 * @param text {const char*} �µ�Ҷ����ı�ֵ��Ϊ�ǿ��ַ���
	 * @return {bool} ���� false ��ʾ�ý���Ҷ��������Ƿ�
	 */
	bool set_text(const char* text);

	/**
	 * ����ǰ json ���ת���� json �ַ���(������ json ��㼰���ӽ��)
	 * @return {const char*}
	 */
	const string& to_string(void);

	/**
	 * ���� json ������ json_node �ӽ�����
	 * @param child {json_node*} �ӽ�����
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�����ӽ������ã�
	 *  ���򷵻ر� json �����������
	 */
	json_node& add_child(json_node* child, bool return_child = false);

	/**
	 * ���� json ������ json_node �ӽ�����
	 * @param child {json_node&} �ӽ�����
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�����ӽ������ã�
	 *  ���򷵻ر� json �����������
	 */
	json_node& add_child(json_node& child, bool return_child = false);

	/**
	 * ����һ�� json �����󣬲���֮���Ϊ�� json �����ӽ��
	 * @param as_array {bool} �Ƿ��������
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�������½������ã�
	 *  ���򷵻ر� json �����������
	 */
	json_node& add_array(bool return_child = false);
	json_node& add_child(bool as_array = false, bool return_child = false);

	/**
	 * ����һ���ַ������͵� json �����󣬲���֮���Ϊ�� json �����ӽ��
	 * @param tag {const char*} ��ǩ��
	 * @param value {const char*} ��ǩֵ
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�������½������ã�
	 *  ���򷵻ر� json �����������
	 * ע���˴��� add_text �� add_child ��ͬ���Ĺ���
	 */
	json_node& add_text(const char* tag, const char* value,
		bool return_child = false);

	json_node& add_child(const char* tag, const char* value,
		bool return_child = false);

	/**
	 * ����һ��int64 ���͵� json �����󣬲���֮���Ϊ�� json �����ӽ��
	 * @param tag {const char*} ��ǩ��
	 * @param value {int64} ��ǩֵ
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�������½������ã�
	 *  ���򷵻ر� json �����������
	 */
#ifdef WIN32
	json_node& add_number(const char* tag, __int64 value,
		bool return_child = false);
#else
	json_node& add_number(const char* tag, long long int value,
		bool return_child = false);
#endif

	/**
	 * ����һ���������͵� json �����󣬲���֮���Ϊ�� json �����ӽ��
	 * @param tag {const char*} ��ǩ��
	 * @param value {bool} ��ǩֵ
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�������½������ã�
	 *  ���򷵻ر� json �����������
	 */
	json_node& add_bool(const char* tag, bool value,
		bool return_child = false);

	/**
	 * ����һ�� json �ַ������󣬲���֮���Ϊ�� json �����ӽ��
	 * @param text {const char*} �ı��ַ���
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�������½������ã�
	 *  ���򷵻ر� json �����������
	 */
	json_node& add_array_text(const char* text, bool return_child = false);

	ACL_CPP_DEPRECATED_FOR("add_text")
	json_node& add_child(const char* text, bool return_child = false);

	/**
	 * ����һ�� json ���ֶ��󣬲���֮���Ϊ�� json �����ӽ��
	 * @param value {acl_int64} ����ֵ
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�������½������ã�
	 *  ���򷵻ر� json �����������
	 */
#ifdef WIN32
	json_node& add_array_number(__int64 value, bool return_child = false);
#else
	json_node& add_array_number(long long int value, bool return_child = false);
#endif

	/**
	 * ����һ�� json �������󣬲���֮���Ϊ�� json �����ӽ��
	 * @param value {bool} ����ֵ
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�������½������ã�
	 *  ���򷵻ر� json �����������
	 */
	json_node& add_array_bool(bool value, bool return_child = false);

	/**
	 * ����һ�� json �����󣬲���֮���Ϊ�� json �����ӽ��
	 * @param tag {const char*} ��ǩ��
	 * @param node {json_node*} ��ǩֵָ��
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�������½������ã�
	 *  ���򷵻ر� json �����������
	 */
	json_node& add_child(const char* tag, json_node* node,
		bool return_child = false);

	/**
	 * ����һ�� json �����󣬲���֮���Ϊ�� json �����ӽ��
	 * @param tag {const char*} ��ǩ��
	 * @param node {json_node&} ��ǩֵ����
	 * @param return_child {bool} �Ƿ���Ҫ�����������´������ӽ�������
	 * @return {json_node&} return_child Ϊ true ʱ�������½������ã�
	 *  ���򷵻ر� json �����������
	 */
	json_node& add_child(const char* tag, json_node& node,
		bool return_child = false);

	/**
	 * @return {json_node&} ���ر����ĸ��������
	 */
	json_node& get_parent(void) const;

	/**
	 * ��ñ����ĵ�һ���ӽ�㣬��Ҫ�����ӽ��ʱ�������ȵ��ô˺���
	 * @return {json_node*} ���ؿձ�ʾû���ӽ��
	 */
	json_node* first_child(void);

	/**
	 * ��ñ�������һ���ӽ��
	 * @return {json_node*} ���ؿձ�ʾ�������̽���
	 */
	json_node* next_child(void);

	/**
	 * ���ظ� json ��������� json ���е����
	 * @return {int}
	 */
	int   depth(void) const;

	/**
	 * ���ظ� json ������һ���ӽ��ĸ���
	 * @return {int} ��Զ >= 0
	 */
	int   children_count(void) const;

	/**
	 * ��� json ���������
	 * @return {json&}
	 */
	json& get_json(void) const;

	/**
	 * ȡ����Ӧ�� ACL ���е� json ������
	 * @return {ACL_JSON_NODE*} ���ؽ�����ע���ý���û����ܵ����ͷ�
	 */
	ACL_JSON_NODE* get_json_node() const;

private:
	friend class json;
	/**
	 * ���캯����Ҫ��ö������ֻ���� json ���󴴽�
	 * @param node {ACL_JSON_NODE*} ACL ���е� ACL_JSON_NODE �ṹ����
	 */
	json_node(ACL_JSON_NODE* node, json* json_ptr);

	/**
	 * Ҫ��ö�������Ƕ�̬������
	 */
	~json_node(void);

	/**
	 * ���� json ���
	 * @param node {ACL_JSON_NODE*}
	 */
	void set_json_node(ACL_JSON_NODE* node);

private:
	ACL_JSON_NODE* node_me_;
	json* json_;
	json_node* parent_;
	json_node* parent_saved_;
	json_node* child_;
	ACL_ITER* iter_;
	string* buf_;
	json_node* obj_;
};

class ACL_CPP_API json : public pipe_stream
{
public:
	/**
	 * ���캯���������ڽ��� json �ַ��������� json ����
	 * @param data {const char*} json ��ʽ���ַ�����������������
	 *  json �ַ�����Ҳ�����ǲ��ֵ� json �ַ�����Ҳ�����ǿ�ָ�룬
	 *  ������Σ��û���Ȼ�����ò��ֻ������� json �ַ������� update
	 *  �������ڵ��� update �����н��� json����ʵ�������캯����
	 *  �� data �����ǿ�ʱ����Ҳ����� update
	 */
	json(const char* data = NULL);

	/**
	 * ����һ�� json �����е�һ�� json ��㹹��һ���µ� json ����
	 * @param node {const json_node&} Դ json �����е�һ�� json ���
	 */
	json(const json_node& node);

	~json(void);

	/**
	 * �����Ƿ��ڽ���ʱ�Զ����������ֵ�����
	 * @param on {bool}
	 * @return {json&}
	 */
	json& part_word(bool on);

	/**
	 * ����ʽ��ʽѭ�����ñ�������� json ���ݣ�Ҳ����һ�������
	 * ������ json ���ݣ�������ظ�ʹ�ø� json ������������� json
	 * ������Ӧ���ڽ�����һ�� json ����ǰ���� reset() ��������
	 * ����һ�εĽ������
	 * @param data {const char*} json ����
	 */
	void update(const char* data);

	/**
	 * ���� json ������״̬���� json ������������Զ�� json ����
	 * ���н������ڷ���ʹ�ñ� json ������ǰ����Ҫ���ñ���������
	 * �ڲ� json ������״̬�������һ�εĽ������
	 */
	void reset(void);

	/**
	 * �� json ������ȡ��ĳ����ǩ�������н�㼯��
	 * @param tag {const char*} ��ǩ��(�����ִ�Сд)
	 * @return {const std::vector<json_node*>&} ���ؽ�����Ķ������ã�
	 *  �����ѯ���Ϊ�գ���ü���Ϊ�գ�����empty() == true
	 *  ע�����ص������е� json_node ������ݿ����޸ģ�������ɾ���ý�㣬
	 *  ��Ϊ�ÿ��ڲ����Զ�ɾ���Ļ���
	 */
	const std::vector<json_node*>& getElementsByTagName(const char* tag) const;

	/**
	 * �� json �����л�����е�������༶��ǩ����ͬ�� json ���ļ���
	 * @param tags {const char*} �༶��ǩ������ '/' �ָ�������ǩ����
	 *  ����� json ���ݣ�
	 *  { 'root': [
	 *      'first': { 'second': { 'third': 'test1' } },
	 *      'first': { 'second': { 'third': 'test2' } },
	 *      'first': { 'second': { 'third': 'test3' } }
	 *    ]
	 *  }
	 *  ����ͨ���༶��ǩ����root/first/second/third һ���Բ�����з���
	 *  �����Ľ��
	 * @return {const std::vector<json_node*>&} ���������� json ��㼯��, 
	 *  �����ѯ���Ϊ�գ���ü���Ϊ�գ�����empty() == true
	 *  ע�����ص������е� json_node ������ݿ����޸ģ�������ɾ���ý�㣬
	 *  ��Ϊ�ÿ��ڲ����Զ�ɾ���Ļ���
	 */
	const std::vector<json_node*>& getElementsByTags(const char* tags) const;

	/**
	 * ȡ�� acl ���е� ACL_JSON ����
	 * @return {ACL_JSON*} ��ֵ������Ϊ�գ�ע���û������޸ĸö����ֵ��
	 *  ���������ͷŸö���
	 */
	ACL_JSON* get_json(void) const;

	/**
	 * ����һ�� json_node Ҷ�����󣬸ý�����ĸ�ʽΪ��
	 * "tag_name": "tag_value"
	 * @param tag {const char*} ��ǩ��
	 * @param value {const char*} ��ǩֵ
	 * @return {json_node&} �²����� json_node ������Ҫ�û��ֹ��ͷţ�
	 *  ��Ϊ�� json �����ͷ�ʱ��Щ�����Զ����ͷţ���Ȼ�û�Ҳ������
	 *  ����ʱ���� reset ���ͷ���Щ json_node ������
	 */
	json_node& create_node(const char* tag, const char* value);

	/**
	 * ����һ�� json_node Ҷ�����󣬸ý�����ĸ�ʽΪ��
	 * "tag_name": tag_value
	 * @param tag {const char*} ��ǩ��
	 * @param value {int64} ��ǩֵ
	 * @return {json_node&} �²����� json_node ������Ҫ�û��ֹ��ͷţ�
	 *  ��Ϊ�� json �����ͷ�ʱ��Щ�����Զ����ͷţ���Ȼ�û�Ҳ������
	 *  ����ʱ���� reset ���ͷ���Щ json_node ������
	 */
#ifdef WIN32
	json_node& create_node(const char* tag, __int64 value);
#else
	json_node& create_node(const char* tag, long long int value);
#endif

	/**
	 * ����һ�� json_node Ҷ�����󣬸ý�����ĸ�ʽΪ��
	 * "tag_name": true|false
	 * @param tag {const char*} ��ǩ��
	 * @param value {bool} ��ǩֵ
	 * @return {json_node&} �²����� json_node ������Ҫ�û��ֹ��ͷţ�
	 *  ��Ϊ�� json �����ͷ�ʱ��Щ�����Զ����ͷţ���Ȼ�û�Ҳ������
	 *  ����ʱ���� reset ���ͷ���Щ json_node ������
	 */
	json_node& create_node(const char* tag, bool value);

	/**
	 * ����һ�� json_node Ҷ����ַ������󣬸ý�����ĸ�ʽΪ��"string"
	 * �� json �淶���ý��ֻ�ܼ��������������
	 * @param text {const char*} �ı��ַ���
	 * @return {json_node&} �²����� json_node ������Ҫ�û��ֹ��ͷţ�
	 *  ��Ϊ�� json �����ͷ�ʱ��Щ�����Զ����ͷţ���Ȼ�û�Ҳ������
	 *  ����ʱ���� reset ���ͷ���Щ json_node ������
	 */
	json_node& create_array_text(const char* text);

	ACL_CPP_DEPRECATED_FOR("create_array_text")
	json_node& create_node(const char* text);

	/**
	 * ����һ�� json_node Ҷ�����ֵ����
	 * �� json �淶���ý��ֻ�ܼ��������������
	 * @param value {acl_int64} ��ֵ
	 * @return {json_node&} �²����� json_node ������Ҫ�û��ֹ��ͷţ�
	 *  ��Ϊ�� json �����ͷ�ʱ��Щ�����Զ����ͷţ���Ȼ�û�Ҳ������
	 * ����ʱ���� reset ���ͷ���Щ json_node ������
	 */
#ifdef WIN32
	json_node& create_array_number(__int64 value);
#else
	json_node& create_array_number(long long int value);
#endif

	/**
	 * ����һ�� json_node Ҷ��㲼������
	 * �� json �淶���ý��ֻ�ܼ��������������
	 * @param value {bool} ����ֵ
	 * @return {json_node&} �²����� json_node ������Ҫ�û��ֹ��ͷţ�
	 *  ��Ϊ�� json �����ͷ�ʱ��Щ�����Զ����ͷţ���Ȼ�û�Ҳ������
	 * ����ʱ���� reset ���ͷ���Щ json_node ������
	 */
	json_node& create_array_bool(bool value);

	/**
	 * ����һ�� json_node ����������󣬸ö���û�б�ǩ,
	 * �ý�����ĸ�ʽΪ��"{}" ��������� "[]"
	 * @param as_array {bool} �Ƿ��������
	 * @return {json_node&} �²����� json_node ������Ҫ�û��ֹ��ͷţ�
	 *  ��Ϊ�� json �����ͷ�ʱ��Щ�����Զ����ͷţ���Ȼ�û�Ҳ������
	 *  ����ʱ���� reset ���ͷ���Щ json_node ������
	 */
	json_node& create_node(bool as_array = false);
	json_node& create_array();

	/**
	 * ����һ�� json_node �����󣬸ý�����ĸ�ʽΪ��tag_name: {}
	 * �� tag_name: []
	 * @param tag {const char*} ��ǩ��
	 * @param node {json_node*} json ��������Ϊ��ǩֵ
	 * @return {json_node&} �²����� json_node ������Ҫ�û��ֹ��ͷţ�
	 *  ��Ϊ�� json �����ͷ�ʱ��Щ�����Զ����ͷţ���Ȼ�û�Ҳ������
	 *  ����ʱ���� reset ���ͷ���Щ json_node ������
	 */
	json_node& create_node(const char* tag, json_node* node);

	/**
	 * ����һ�� json_node �����󣬸ý�����ĸ�ʽΪ��tag_name: {}
	 * �� tag_name: []
	 * @param tag {const char*} ��ǩ��
	 * @param node {json_node&} json ��������Ϊ��ǩֵ
	 * @return {json_node&} �²����� json_node ������Ҫ�û��ֹ��ͷţ�
	 *  ��Ϊ�� json �����ͷ�ʱ��Щ�����Զ����ͷţ���Ȼ�û�Ҳ������
	 *  ����ʱ���� reset ���ͷ���Щ json_node ������
	 */
	json_node& create_node(const char* tag, json_node& node);

	/**
	 * ��һ�� json �����е�һ�� json ��㸴������һ json �����е�һ��
	 * �� json ����в����ظ��µ� json ���
	 * @param node {json_node*} Դ json �����һ�� json ���
	 * @return {json_node&} ��ǰĿ�� json �������´����� json ���
	 */
	json_node& duplicate_node(const json_node* node);

	/**
	 * ��һ�� json �����е�һ�� json ��㸴������һ json �����е�һ��
	 * �� json ����в����ظ��µ� json ���
	 * @param node {json_node&} Դ json �����һ�� json ���
	 * @return {json_node&} ��ǰĿ�� json �������´����� json ���
	 */
	json_node& duplicate_node(const json_node& node);

	/**
	 * ��ø�������
	 * @return {json_node&}
	 */
	json_node& get_root();

	/**
	 * ��ʼ������ json ���󲢻�õ�һ�����
	 * @return {json_node*} ���ؿձ�ʾ�� json ����Ϊ�ս��
	 *  ע�����صĽ������û������ֹ��ͷţ���Ϊ�ö���
	 *  �ڲ����Զ��ͷ�
	 */
	json_node* first_node(void);

	/**
	 * ������ json �������һ�� json ���
	 * @return {json_node*} ���ؿձ�ʾ�������
	 *  ע�����صĽ������û������ֹ��ͷţ���Ϊ�ö���
	 *  �ڲ����Զ��ͷ�
	 */
	json_node* next_node(void);

	/**
	 * �� json ������ת���ַ���
	 * @param out {string&} �洢ת������Ļ�����
	 */
	void build_json(string& out);

	/**
	 * �� json ������ת���� json �ַ���
	 * @return {const string&}
	 */
	const string& to_string();

	// pipe_stream �麯������

	virtual int push_pop(const char* in, size_t len,
		string* out, size_t max = 0);
	virtual int pop_end(string* out, size_t max = 0);
	virtual void clear(void);
private:
	// ��Ӧ�� acl ���е� ACL_JSON ����
	ACL_JSON *json_;
	// json �������еĸ�������
	json_node* root_;
	// �ɸ� json ��������� json ��㼯��
	std::list<json_node*> nodes_;
	// ��ʱ�� json ������
	json_node* node_tmp_;
	// ��ʱ�� json ����ѯ�����
	std::vector<json_node*> nodes_tmp_;
	// ������
	string* buf_;
	ACL_ITER* iter_;
};

} // namespace acl
