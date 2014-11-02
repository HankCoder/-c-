#ifndef ACL_XML_INCLUDE_H
#define ACL_XML_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib/acl_array.h"
#include "stdlib/acl_ring.h"
#include "stdlib/acl_vstream.h"
#include "stdlib/acl_htable.h"
#include "stdlib/acl_vstring.h"
#include "stdlib/acl_iterator.h"
#include "stdlib/acl_slice.h"

typedef struct ACL_XML	ACL_XML;
typedef struct ACL_XML_NODE	ACL_XML_NODE;
typedef struct ACL_XML_ATTR	ACL_XML_ATTR;

struct ACL_XML_ATTR {
	ACL_XML_NODE *node;         /**< ������� */
	ACL_VSTRING *name;          /**< ������ */
	ACL_VSTRING *value;         /**< ����ֵ */

	/* private */
	int   quote;                /**< �� 0 ��ʾ ' �� " */
	int   backslash;            /**< ת���ַ� \ */
	int   part_word;            /**< ������ֵ���������־λ */
	int   slash;                /**< �Ƿ��� '/' ��־λ�趨 */
};

struct ACL_XML_NODE {
	ACL_VSTRING *ltag;          /**< ���ǩ�� */
	ACL_VSTRING *rtag;          /**< �ұ�ǩ�� */
	const ACL_VSTRING *id;      /**< ID��ʶ��, ֻ���� xml->id_table ���ڵĽ��� id �ŷǿ� */
	ACL_VSTRING *text;          /**< �ı���ʾ���� */
	ACL_ARRAY *attr_list;       /**< ����(ACL_XML_ATTR)�б� */
	ACL_XML_NODE *parent;       /**< ����� */
	ACL_RING children;          /**< �ӽ�㼯�� */
	int  depth;                 /**< ��ǰ������� */

	/* private */
	ACL_XML *xml;               /**< xml ���� */
	ACL_RING node;              /**< ��ǰ��� */
	ACL_XML_ATTR *curr_attr;    /**< ��ǰ���ڽ��������� */
	int   quote;                /**< �� 0 ��ʾ ' �� " */
	int   last_ch;              /**< ����¼������ǰһ���ֽ�ֵ */
	int   nlt;                  /**< '<' ���� */
	char  meta[3];              /**< Ԫ������ʱ������ */
	unsigned int flag;
#define	ACL_XML_F_META_QM	(1 << 0)    /**< '?' flag */
#define	ACL_XML_F_META_CM	(1 << 1)    /**< '!--' flag */
#define	ACL_XML_F_META_EM	(1 << 2)    /**< only '!' flag */
#define ACL_XML_F_SELF_CL	(1 << 3)    /**< self closed flag */
#define	ACL_XML_F_LEAF		(1 << 4)    /**< leaf node has no child node */

/**< �Ƿ���Ԫ���� */
#define	ACL_XML_F_META		\
	(ACL_XML_F_META_QM | ACL_XML_F_META_CM | ACL_XML_F_META_EM)

	int   status;               /**< ״̬����ǰ����״̬ */
#define ACL_XML_S_NXT	0       /**< ��һ����� */
#define ACL_XML_S_LLT	1       /**< ��� '<' */
#define ACL_XML_S_LGT	2       /**< �ұ� '>' */
#define	ACL_XML_S_LCH	3       /**< ��� '<' ���һ���ֽ� */
#define ACL_XML_S_LEM	4       /**< ��� '<' ��� '!' */
#define ACL_XML_S_LTAG	5       /**< ��ߵı�ǩ�� */
#define ACL_XML_S_RLT	6       /**< �ұߵ� '<' */
#define ACL_XML_S_RGT	7       /**< �ұߵ� '>' */
#define ACL_XML_S_RTAG	8       /**< �ұߵı�ǩ�� */
#define ACL_XML_S_ATTR	9       /**< ��ǩ������ */
#define ACL_XML_S_AVAL	10      /**< ��ǩ����ֵ */
#define ACL_XML_S_TXT	11      /**< ����ı� */
#define ACL_XML_S_MTAG	12      /**< Ԫ���ݱ�ǩ */
#define ACL_XML_S_MTXT	13      /**< Ԫ�����ı� */
#define ACL_XML_S_MCMT	14      /**< Ԫ����ע�� */
#define ACL_XML_S_MEND	15      /**< Ԫ���ݽ��� */

	/* public: for acl_iterator, ͨ�� acl_foreach �г��ý���һ���ӽ�� */

	/* ȡ������ͷ���� */
	ACL_XML_NODE *(*iter_head)(ACL_ITER*, ACL_XML_NODE*);
	/* ȡ��������һ������ */
	ACL_XML_NODE *(*iter_next)(ACL_ITER*, ACL_XML_NODE*);
	/* ȡ������β���� */
	ACL_XML_NODE *(*iter_tail)(ACL_ITER*, ACL_XML_NODE*);
	/* ȡ��������һ������ */
	ACL_XML_NODE *(*iter_prev)(ACL_ITER*, ACL_XML_NODE*);
};

struct ACL_XML {
	/* public */
	int   depth;                /**< ������ */
	int   node_cnt;             /**< �������, ���� root ��� */
	ACL_XML_NODE *root;         /**< XML ����� */

	/* private */
	ACL_HTABLE *id_table;       /**< id ��ʶ����ϣ�� */
	ACL_XML_NODE *curr_node;    /**< ��ǰ���ڴ���� XML ��� */
	ACL_SLICE_POOL *slice;      /**< �ڴ�ض��� */

	ACL_ARRAY *node_cache;      /**< XML��㻺��� */
	int   max_cache;            /**< XML��㻺��ص�������� */
	unsigned flag;              /**< ��־λ: ACL_XML_FLAG_xxx */ 
#define	ACL_XML_FLAG_PART_WORD		(1 << 0) /**< �Ƿ���ݺ�������Ϊת��� '\' ����� */
#define	ACL_XML_FLAG_IGNORE_SLASH	(1 << 1) /**< �Ƿ���ݵ������û�� '/' ��� */

	/* public: for acl_iterator, ͨ�� acl_foreach �����г������ӽ�� */

	/* ȡ������ͷ���� */
	ACL_XML_NODE *(*iter_head)(ACL_ITER*, ACL_XML*);
	/* ȡ��������һ������ */
	ACL_XML_NODE *(*iter_next)(ACL_ITER*, ACL_XML*);
	/* ȡ������β���� */
	ACL_XML_NODE *(*iter_tail)(ACL_ITER*, ACL_XML*);
	/* ȡ��������һ������ */
	ACL_XML_NODE *(*iter_prev)(ACL_ITER*, ACL_XML*);
};

#define	ACL_XML_IS_COMMENT(x)	(((x)->flag & ACL_XML_F_META_CM))

/***************************************************************************/
/*                  ���������ӿڣ��û����Է���ʹ�øýӿڼ�                 */
/***************************************************************************/

/*----------------------------- in acl_xml.c ------------------------------*/

/**
 * �ж� xml �����Ƿ�պϵ�, ���Ƿ��������������Ƿ�����, ����� xml �������
 * xml ���Ԫ��Ϊ��, ��Ҳ��Ϊ���Ǳպϵ�
 * @param xml {ACL_XML*} xml ����
 * @return {int} 0: ��; 1: ��
 */
ACL_API int acl_xml_is_closure(ACL_XML *xml);

/**
 * ����ָ���ı�ǩ���ж� xml �����Ѿ����, ���ñ�ǩ�� xml ������ root һ���ӽ��
 * �е����һ�� xml ���ı�ǩ��ͬʱ, ����Ϊ xml �������, Ϊ�˱�֤�жϵ���ȷ��,
 * ����ԴӦ��֤�����ĸ����ֻ��һ��, �� xml->root ��һ���ӽ��ֻ��һ��, ����
 * ���������
 * @param xml {ACL_XML*} xml ����
 * @param tag {const char*} �û�������ǩ��, �ڲ���ƥ��ʱ�����ִ�Сд
 * @return {int} 0: ��; 1: ��
 */
ACL_API int acl_xml_is_complete(ACL_XML *xml, const char *tag);

/**
 * ����һ�� xml ����
 * @return {ACL_XML*} �´����� xml ����
 */
ACL_API ACL_XML *acl_xml_alloc(void);

/**
 * ����һ�� xml ���󣬸� xml �������е��ڲ��ڴ���䶼�ڸ��ڴ���Ͻ��з���
 * @param slice {ACL_SLICE_POOL*} �ڴ�ض��󣬿���Ϊ��ָ�룬���������ڴ��
 * @return {ACL_XML*} �´����� xml ����
 */
ACL_API ACL_XML *acl_xml_alloc1(ACL_SLICE_POOL *slice);

/**
 * ��ĳһ�� ACL_XML_NODE �����Ϊһ�� XML ����ĸ���㣬�Ӷ����Է���ر�������
 * ���ĸ����ӽ��(�ڱ��������е����н�㲻�����������)���ñ�����ʽ�б��ڵ���
 * ����ĳһ�� ACL_XML_NODE ���ʱ���ܱ�����һ���ӽ�������
 * @param xml {ACL_XML*} xml ����
 * @param node {ACL_XML_NODE*} AXL_XML_NODE ���
 */
ACL_API void acl_xml_foreach_init(ACL_XML *xml, ACL_XML_NODE *node);

/**
 * ���� XML ���������, �Ƿ��������û�� /, ��:
 * <test id=111>, <test id=111 />, ����������û�� / ��������д��
 * ���ǺϷ��ģ�����ֻ�еڶ���д���ǺϷ��ģ�����������ּ����ԣ���
 * �����һ����������ʧ
 * @param xml {ACL_XML*} xml ����
 * @param ignore {int} ����� 0 ��ʾ���������� /
 */
ACL_API void acl_xml_slash(ACL_XML *xml, int ignore);

/**
 * �򿪻�ر�XML�Ļ��湦�ܣ������� ACL_XML ����ʱ��XML�Ľ�㻺�湦���������Ч��
 * @param xml {ACL_XML*} xml ����
 * @param max_cache {int} ��������ֵ������ֵ > 0 ʱ��� xml �������� xml ����
 *  ���湦�ܣ������ر� xml �������� xml ���Ļ��湦��
 */
ACL_API void acl_xml_cache(ACL_XML *xml, int max_cache);

/**
 * �ͷ� XML ����� XML ������
 * @param xml {ACL_XML*} xml ����
 */
ACL_API void acl_xml_cache_free(ACL_XML *xml);

/**
 * �ͷ�һ�� xml ����, ͬʱ�ͷŸö��������ɵ����� xml ���
 * @param xml {ACL_XML*} xml ����
 */
ACL_API int acl_xml_free(ACL_XML *xml);

/**
 * ���� XML ����������
 * @param xml {ACL_XML*} xml ����
 */
ACL_API void acl_xml_reset(ACL_XML *xml);

/*------------------------- in acl_xml_parse.c ----------------------------*/

/**
 * ���� xml ����, ���������Զ����� xml �����
 * @param xml {ACL_XML*} xml ����
 * @param data {const char*} �� '\0' ��β�������ַ���, ������������ xml ����;
 *  Ҳ�����ǲ������� xml ����, ����ѭ�����ô˺���, �����������ݳ���������
 */
ACL_API void acl_xml_update(ACL_XML *xml, const char *data);
#define	acl_xml_parse	acl_xml_update

/*------------------------- in acl_xml_util.c -----------------------------*/

/**
 * ��ʼ�������� input, br, hr �ȵ��Ապϱ�ǩ, �γ��Ապϱ�ǩ��, �Ա���
 * acl_xml_tag_selfclosed ��ѯ����, ���������ǩ�Ƿ��Ǳ������Ապϱ�ǩ,
 * �ú���ֻ�ܱ���ʼ��һ��, Ҳ���Բ���ʼ��
 */
ACL_API void acl_xml_tag_init(void);

/**
 * �����û��Լ����һЩ���Ապϵı�ǩ
 * @param tag {const char*} ��ǩ����ע���ǩ���Ȳ��ô��� 254 ���ֽ�
 */
ACL_API void acl_xml_tag_add(const char *tag);

/**
 * ������ acl_xml_tag_init ��ʼ���������Ապϱ�ǩ����, ���Ե��ô˺����ж�����
 * ��ǩ�Ƿ������Ապϱ�ǩ, ���δ���� acl_xml_tag_init, ��ú�����Զ���� 0
 * @parma tag {const char*} ��ǩ����
 * @return {int} 0: ��ʾ��, 1: ��ʾ��
 */
ACL_API int  acl_xml_tag_selfclosed(const char *tag);

/**
 * �жϱ�ǩ���� xml ����Ƿ���Ҷ���, Ҷ���û���ӽ��
 * @param tag {const char*} ��ǩ��
 * @return {int} 0: ����Ҷ���; 1: ��Ҷ���
 */
ACL_API int  acl_xml_tag_leaf(const char *tag);

/**
 * �ͷ��� acl_xml_getElementsByTagName, acl_xml_getElementsByName,
 * acl_xml_getElementsByAttr �Ⱥ������صĶ�̬�������, ��Ϊ�ö�̬�����е�
 * Ԫ�ض��� ACL_XML ������Ԫ�ص�����, �����ͷŵ��ö�̬�����, ֻҪ ACL_XML
 * �����ͷ�, ��ԭ�����ڸ������е�Ԫ����Ȼ����ʹ��.
 * �������ͷ������ xml ���Ԫ��
 * @param a {ACL_ARRAY*} ��̬�������
 */
ACL_API void acl_xml_free_array(ACL_ARRAY *a);

/**
 * �� xml �����л�����е���������ǩ����ͬ�� xml ���ļ���
 * @param xml {ACL_XML*} xml ����
 * @param tag {const char*} ��ǩ����
 * @return {ACL_ARRAY*} ���������� xml ��㼯��, ���� ��̬������, ������ NULL ��
 *  ��ʾû�з��������� xml ���, �ǿ�ֵ��Ҫ���� acl_xml_free_array �ͷ�
 */
ACL_API ACL_ARRAY *acl_xml_getElementsByTagName(ACL_XML *xml, const char *tag);

/**
 * �� xml �����л�����е�������༶��ǩ����ͬ�� xml ���ļ���
 * @param xml {ACL_XML*} xml ����
 * @param tags {const char*} �༶��ǩ������ '/' �ָ�������ǩ��������� xml ���ݣ�
 *  <root> <first> <second> <third name="test1"> text1 </third> </second> </first>
 *  <root> <first> <second> <third name="test2"> text2 </third> </second> </first>
 *  <root> <first> <second> <third name="test3"> text3 </third> </second> </first>
 *  ����ͨ���༶��ǩ����root/first/second/third һ���Բ�����з��������Ľ��
 * @return {ACL_ARRAY*} ���������� xml ��㼯��, ���� ��̬������, ������ NULL ��
 *  ��ʾû�з��������� xml ���, �ǿ�ֵ��Ҫ���� acl_xml_free_array �ͷ�
 */
ACL_API ACL_ARRAY *acl_xml_getElementsByTags(ACL_XML *xml, const char *tags);

/**
 * �� xml �����л�����е������������ name ������ֵ��ͬ�� xml ���Ԫ�ؼ���
 * @param xml {ACL_XML*} xml ����
 * @param value {const char*} ������Ϊ name ������ֵ
 * @return {ACL_ARRAY*} ���������� xml ��㼯��, ���� ��̬������, ������ NULL ��
 *  ��ʾû�з��������� xml ���, �ǿ�ֵ��Ҫ���� acl_xml_free_array �ͷ�
 */
ACL_API ACL_ARRAY *acl_xml_getElementsByName(ACL_XML *xml, const char *value);

/**
 * �� xml �����л�����и���������������ֵ�� xml ���Ԫ�ؼ���
 * @param xml {ACL_XML*} xml ����
 * @param name {const char*} ������
 * @param value {const char*} ����ֵ
 * @return {ACL_ARRAY*} ���������� xml ��㼯��, ���� ��̬������, ������ NULL ��
 *  ��ʾû�з��������� xml ���, �ǿ�ֵ��Ҫ���� acl_xml_free_array �ͷ�
 */
ACL_API ACL_ARRAY *acl_xml_getElementsByAttr(ACL_XML *xml,
	const char *name, const char *value);

/**
 * �� xml �����л��ָ�� id ֵ�� xml ���Ԫ�ص�ĳ�����Զ���
 * @param xml {ACL_XML*} xml ����
 * @param id {const char*} id ֵ
 * @return {ACL_XML_ATTR*} ĳ xml ����ĳ�����Զ���, ������ NULL ���ʾ
 *  û�з�������������, ����ֵ����Ҫ�ͷ�
 */
ACL_API ACL_XML_ATTR *acl_xml_getAttrById(ACL_XML *xml, const char *id);

/**
 * �� xml �����л��ָ�� id ֵ�� xml ���Ԫ�ص�ĳ������ֵ
 * @param xml {ACL_XML*} xml ����
 * @param id {const char*} id ֵ
 * @return {const char*} ĳ xml ����ĳ������ֵ, ������ NULL ���ʾû�з���
 *  ����������
 */
ACL_API const char *acl_xml_getAttrValueById(ACL_XML *xml, const char *id);

/**
 * �� xml �����л��ָ�� id ֵ�� xml ���Ԫ��
 * @param xml {ACL_XML*} xml ����
 * @param id {const char*} id ֵ
 * @return {ACL_XML_NODE*} xml ���Ԫ��, ������ NULL ���ʾû�з���
 *  ������ xml ���, ����ֵ����Ҫ�ͷ�
 */
ACL_API ACL_XML_NODE *acl_xml_getElementById(ACL_XML *xml, const char *id);

/**
 * �� xml ��������ȡ���� ? ! �ȿ�ͷ�Ľ��
 * @param xml {ACL_XML*} xml ����
 * @param tag {const char*} ��ǩ��
 * @return {ACL_XML_NODE*} xml ���Ԫ��, ������ NULL ���ʾû�з���
 *  ������ xml ���, ����ֵ����Ҫ�ͷ�
 */
ACL_API ACL_XML_NODE *acl_xml_getElementMeta(ACL_XML *xml, const char *tag);

/**
 * ��� xml ���ַ��������ʽ
 * @param xml {ACL_XML*} xml ����
 * @return {const char*} �����ַ��������ʽ������ NULL ʱ��ʾû�и�����
 */
ACL_API const char *acl_xml_getEncoding(ACL_XML *xml);

/**
 * ��� xml ���ݵ����ͣ��磺text/xsl
 * @param xml {ACL_XML*} xml ����
 * @return {const char*} ���� NULL ��ʾû�и�����
 */
ACL_API const char *acl_xml_getType(ACL_XML *xml);

/**
 * �� xml ����л��ָ�������������Զ���
 * @param node {ACL_XML_NODE*} xml ���
 * @param name {const char*} ��������
 * @return {ACL_XML_ATTR*} ���Զ���, Ϊ�ձ�ʾ������, ����ֵ����Ҫ�ͷ�
 */
ACL_API ACL_XML_ATTR *acl_xml_getElementAttr(ACL_XML_NODE *node, const char *name);

/**
 * �� xml ����л��ָ��������������ֵ
 * @param node {ACL_XML_NODE*} xml ���
 * @param name {const char*} ��������
 * @return {const char*} ����ֵ, Ϊ�ձ�ʾ������
 */
ACL_API const char *acl_xml_getElementAttrVal(ACL_XML_NODE *node, const char *name);

/**
 * �� xml ���ɾ��ĳ�����Զ���, ���������Ϊ id ����, ��ͬʱ��� xml->id_table ��ɾ��
 * @param node {ACL_XML_NODE*} xml ���
 * @param name {const char*} ��������
 * @return {int} 0 ��ʾɾ���ɹ�, -1: ��ʾɾ��ʧ��(�п����Ǹ����Բ�����)
 */
ACL_API int acl_xml_removeElementAttr(ACL_XML_NODE *node, const char *name);

/**
 * �� xml ����������, ������������Ѵ���, �����µ�����ֵ�滻������ֵ, ����
 * ����������µ����Զ���
 * @param node {ACL_XML_NODE*} xml ���
 * @param name {const char*} ��������
 * @param value {const char*} ����ֵ
 * @return {ACL_XML_ATTR*} ���ظ����Զ���(�п�����ԭ����, Ҳ�п������µ�), 
 *  ����ֵ�����ͷ�
 */
ACL_API ACL_XML_ATTR *acl_xml_addElementAttr(ACL_XML_NODE *node,
        const char *name, const char *value);

/**
 * ����ǩ��������ı���Ϊ�������� xml ��㣬�ú�����Ҫ���ڹ��� xml ����ʱ
 * @param xml {ACL_XML*} xml ���󣬸ö���Ӧ������ acl_xml_alloc ������
 * @param tagname {const char*} ��ǩ��������ǿ����ַ������ȴ��� 0
 * @param text {const char*} �����ı����ݣ�����Ϊ��
 * @return {ACL_XML_NODE*} �´����� xml ��㣬�÷�����Զ���طǿգ��������
 *  �����Ƿ���ᵼ���ڲ��Զ���������
 */
ACL_API ACL_XML_NODE *acl_xml_create_node(ACL_XML *xml,
	const char* tagname, const char* text);

/**
 * ��һ�� xml ���������ԣ��ú�����Ҫ���ڹ��� xml ����ʱ
 * @param node {ACL_XML_NODE*} �� acl_xml_create_node �����Ľ��
 * @param name {const char*} ������������Ϊ�ǿ��ַ������ַ������ȴ��� 0
 * @param value {const char*} ����ֵ������Ϊ��
 * @return {ACL_XML_ATTR*} xml �������Զ��󣬵���������Ƿ�ʱ�ú���
 *  �ڲ��Զ���������
 */
ACL_API ACL_XML_ATTR *acl_xml_node_add_attr(ACL_XML_NODE *node,
	const char *name, const char *value);

/**
 * ��һ�� xml ������һ�����ԣ��ú�����Ҫ���ڹ��� xml ����ʱ
 * @param node {ACL_XML_NODE*} �� acl_xml_create_node �����Ľ��
 * @param ... һ�����ԣ����� NULL ʱ��ʾ�������磺
 *  {name1}, {value1}, {name2}, {value2}, ... NULL
 */
ACL_API void acl_xml_node_add_attrs(ACL_XML_NODE *node, ...);

/**
 * ��һ�� xml �������ı����ݣ��ú�����Ҫ���ڹ��� xml ����ʱ
 * @param node {ACL_XML_NODE*} �� acl_xml_create_node �����Ľ��
 * @param text {const char*} �ı�����
 */
ACL_API void acl_xml_node_set_text(ACL_XML_NODE *node, const char *text);

/**
 * �� xml ����ת���ַ�������
 * @param xml {ACL_XML*} xml ����
 * @param buf {ACL_VSTRING*} �洢������Ļ����������ò���Ϊ��ʱ�����ڲ���
 *  �Զ�����һ�λ�������Ӧ���������Ҫ�ͷŵ����ǿպ����ڲ���ֱ�ӽ�����洢����
 * @return {ACL_VSTRING*} xml ����ת�����ַ�����Ĵ洢���������÷���ֵ��Զ�ǿգ�
 *  ʹ���߿���ͨ�� ACL_VSTRING_LEN(x) �����ж������Ƿ�Ϊ�գ����ص� ACL_VSTRING
 *  ָ�����Ϊ�ú����ڲ������ģ����û��������� acl_vstring_free �����ͷ�
 */
ACL_API ACL_VSTRING* acl_xml_build(ACL_XML* xml, ACL_VSTRING *buf);

/**
 * �� xml ����ת����ָ�����У�ע����ת����Ϣ��Ϊ�����õ�����
 * @param xml {ACL_XML*} xml ����
 * @param fp {ACL_VSTREAM*} ������
 */
ACL_API void acl_xml_dump(ACL_XML *xml, ACL_VSTREAM *fp);

/**
 * �� xml ����ת����ָ���������У�ע����ת����Ϣ��Ϊ�����õ�����
 * @param xml {ACL_XML*} xml ����
 * @param buf {ACL_VSTRING*} ������, ��Ҫ�û��Լ�����ռ�
 */
ACL_API void acl_xml_dump2(ACL_XML *xml, ACL_VSTRING *buf);

/***************************************************************************/
/*          ����Ϊ��Ϊ�ͼ��Ľӿ�, �û����Ը�����Ҫ�������½ӿ�             */
/***************************************************************************/

/*----------------------------- in acl_xml.c ------------------------------*/

/**
 * ���� xml ��������
 * @param node {ACL_XML_NODE*} xml ���
 * @return {ACL_XML_ATTR*} �´����Ľ������
 */
ACL_API ACL_XML_ATTR *acl_xml_attr_alloc(ACL_XML_NODE *node);

/**
 * �ͷ� xml ����������ռ�ڴ�, ���øú���ǰ, ����ע���Ѿ���������
 * �����������Ľ����ɾ��
 * @param attr {ACL_XML_ATTR*} xml ��������
 */
ACL_API void acl_xml_attr_free(ACL_XML_ATTR *attr);

/**
 * ����һ�� xml ���
 * @param xml {ACL_XML*} xml ����
 * @return {ACL_XML_NODE*} xml ������
 */
ACL_API ACL_XML_NODE *acl_xml_node_alloc(ACL_XML *xml);

/**
 * ��ĳ�� xml ��㼰���ӽ��� xml ������ɾ��, ���ͷŸý�㼰���ӽ����ռ�ռ�
 * �������ͷŸ� xml �����ռ�ڴ�
 * @param node {ACL_XML_NODE*} xml ���
 * @return {int} ����ɾ���Ľ�����
 */
ACL_API int acl_xml_node_delete(ACL_XML_NODE *node);

/**
 * ��ĳ�� xml �������ֵܽ��(���ֵܽ������Ƕ����� xml ���)
 * @param node1 {ACL_XML_NODE*} �򱾽����� xml ���
 * @param node2 {ACL_XML_NODE*} ����ӵ��ֵ� xml ���
 */
ACL_API void acl_xml_node_append(ACL_XML_NODE *node1, ACL_XML_NODE *node2);

/**
 * ��ĳ�� xml �����Ϊ�ӽ�����ĳ�� xml �����
 * @param parent {ACL_XML_NODE*} �����
 * @param child {ACL_XML_NODE*} �ӽ��
 */
ACL_API void acl_xml_node_add_child(ACL_XML_NODE *parent, ACL_XML_NODE *child);

/**
 * ���ĳ�� xml ���ĸ����
 * @param node {ACL_XML_NODE*} xml ���
 * @return {ACL_XML_NODE*} �����, ���Ϊ NULL ���ʾ�丸��㲻����
 */
ACL_API ACL_XML_NODE *acl_xml_node_parent(ACL_XML_NODE *node);

/**
 * ���ĳ�� xml ���ĺ�һ���ֵܽ��
 * @param node {ACL_XML_NODE*} xml ���
 * @return {ACL_XML_NODE*} ���� xml ���ĺ�һ���ֵܽ��, ��ΪNULL���ʾ������
 */
ACL_API ACL_XML_NODE *acl_xml_node_next(ACL_XML_NODE *node);

/**
 * ���ĳ�� xml ����ǰһ���ֵܽ��
 * @param node {ACL_XML_NODE*} xml ���
 * @return {ACL_XML_NODE*} ���� xml ����ǰһ���ֵܽ��, ��ΪNULL���ʾ������
 */
ACL_API ACL_XML_NODE *acl_xml_node_prev(ACL_XML_NODE *node);

#ifdef __cplusplus
}
#endif
#endif
