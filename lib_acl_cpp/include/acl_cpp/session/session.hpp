#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>
#include "acl_cpp/stdlib/string.hpp"

namespace acl {

// �����洢����ֵ�Ļ����������壬����Ҫ��Ϊ�˼�������ֵ
// ����Ϊ�����Ƶ����ζ����ӵĽṹ����
typedef enum
{
	TODO_NUL,
	TODO_SET,
	TODO_DEL
} todo_t;

struct VBUF
{
	size_t len;
	size_t size;
	todo_t todo;
	char  buf[1];
};

/**
 * session �࣬����ʹ�� memcached �洢 session ����
 */
class ACL_CPP_API session
{
public:
	/**
	 * �����캯���Ĳ��� sid �ǿ�ʱ����� session ����ʹ�ø�
	 * sid�������ڲ����Զ�����һ�� sid���û�Ӧ��ͨ�� get_sid()
	 * �������Զ����ɵ� sid �Ա���ÿ�β�ѯ�� sid ��Ӧ������
	 * @param ttl {time_t} ָ�� session ����������(��)
	 * @param sid {const char*} �ǿ�ʱ���� session �� sid ʹ
	 *  ��ֵ�������ڲ������һ������� session sid���������
	 *  sid ����ͨ������ get_sid() ��ã���Ȼ��ʹ�ù����У��û�
	 *  Ҳ����ͨ�� set_sid() �޸ı������ session sid��
	 *  ���⣬����� sid Ϊ�գ�������û������ĳ�� sid ��Ӧ��
	 *  ���ݣ����û������ȵ��� set_sid()
	 */
	session(time_t ttl = 0, const char* sid = NULL);
	virtual ~session(void);
	
	/**
	 * �����ڲ�״̬�������һЩ��ʱ����
	 */
	void reset(void);

	/**
	 * ��ñ� session �����Ψһ ID ��ʶ
	 * @return {const char*} �ǿ�
	 */
	virtual const char* get_sid(void) const;

	/**
	 * ���ñ� session �����Ψһ ID ��ʶ
	 * @param sid {const char*} �ǿ�
	 * ע�����ñ������󣬻��Զ����֮ǰ���м仺������
	 */
	void set_sid(const char* sid);

	/**
	 * ������ session ��� set/set_ttl ʱ��������һ������ delay Ϊ true��
	 * �����ͨ�����ñ������������������и���
	 * @return {bool} ���ݸ����Ƿ�ɹ�
	 */
	bool flush();

	/**
	 * �� session ������µ��ַ������ԣ�ͬʱ���ø�
	 * session �Ĺ���ʱ����(��)
	 * @param name {const char*} session �����ǿ�
	 * @param value {const char*} session ֵ���ǿ�
	 * @param delay {bool} ��Ϊ true ʱ�����ӳٷ��͸���ָ���˵�
	 *  ��������������û������� session::flush ���ٽ������ݸ��£���
	 *  ��������ߴ���Ч�ʣ���Ϊ false ʱ�������̸�������
	 * @return {bool} ���� false ��ʾ����
	 */
	bool set(const char* name, const char* value, bool delay = false);

	/**
	 * �� session ������µ����Զ���ͬʱ���ø�
	 * session �Ĺ���ʱ����(��)
	 * @param name {const char*} session ���������ǿ�
	 * @param value {const char*} session ����ֵ���ǿ�
	 * @param len {size_t} value ֵ����
	 * @param delay {bool} ��Ϊ true ʱ�����ӳٷ��͸���ָ���˵�
	 *  ��������������û������� session::flush ���ٽ������ݸ��£���
	 *  ��������ߴ���Ч�ʣ���Ϊ false ʱ�������̸�������
	 * @return {bool} ���� false ��ʾ����
	 */
	bool set(const char* name, const void* value, size_t len, bool delay = false);
	
	/**
	 * �� session ��ȡ���ַ�����������ֵ
	 * @param name {const char*} session ���������ǿ�
	 * @param local_cached {bool} ���� session ����Ӻ�� cache ������
	 *  ȡ��һ�����ݺ�����ò���Ϊ true�����������ñ�����ʱ����������
	 *  �Ǳ����󻺴�����ݣ�������ÿ�ζ�Ҫ�Ӻ��ȡ��������ÿ���ñ�����
	 *  �����Ӻ�� cache ������ȡ����
	 * @return {const char*} session ����ֵ�����ص�ָ���ַ��Զ�ǿգ��û�
	 *  ����ͨ���жϷ��ص��Ƿ��ǿմ�(��: "\0")���жϳ���򲻴���
	 *  ע���ú������طǿ����ݺ��û�Ӧ�����̱����˷���ֵ����Ϊ�´�
	 *      �������������ÿ��ܻ��������ʱ��������
	 */
	const char* get(const char* name, bool local_cached = false);

	/**
	 * �� session ��ȡ�ö������������͵�����ֵ
	 * @param name {const char*} session ���������ǿ�
	 * @param local_cached {bool} ���� session ����Ӻ�� cache ������
	 *  ȡ��һ�����ݺ�����ò���Ϊ true�����������ñ�����ʱ����������
	 *  �Ǳ����󻺴�����ݣ�������ÿ�ζ�Ҫ�Ӻ��ȡ��������ÿ���ñ�����
	 *  �����Ӻ�� cache ������ȡ����
	 * @return {const VBUF*} session ����ֵ�����ؿ�ʱ��ʾ����򲻴���
	 *  ע���ú������طǿ����ݺ��û�Ӧ�����̱����˷���ֵ����Ϊ�´�
	 *      �������������ÿ��ܻ��������ʱ��������
	 */
	const VBUF* get_vbuf(const char* name, bool local_cached = false);

	/**
	 * �� session ��ɾ��ָ������ֵ�������еı�����ɾ��
	 * ʱ�Ὣ��������� memcached ��ɾ��
	 * @param name {const char*} session ���������ǿ�
	 * @param delay {bool} ��Ϊ true ʱ�����ӳٷ��͸���ָ���˵�
	 *  ��������������û������� session::flush ���ٽ������ݸ��£���
	 *  ��������ߴ���Ч�ʣ���Ϊ false ʱ�������̸�������
	 * @return {bool} true ��ʾ�ɹ�(�����������)��false ��ʾɾ��ʧ��
	 */
	bool del(const char* name, bool delay = false);

	/**
	 * �������� session �ڻ���������ϵĻ���ʱ��
	 * @param ttl {time_t} ��������(��)
	 * @param delay {bool} ��Ϊ true ʱ�����ӳٷ��͸���ָ���˵�
	 *  ��������������û������� session::flush ���ٽ������ݸ��£���
	 *  ��������ߴ���Ч�ʣ���Ϊ false ʱ�������̸�������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool set_ttl(time_t ttl, bool delay = true);

	/**
	 * ��ñ� session �����м�¼�� session �������ڣ���ֵ�п���
	 * �������洢�ڻ����������ʱ�䲻һ�£���Ϊ�п���������ʵ��
	 * ���������� session �ڻ���������ϵ���������
	 * @return {time_t}
	 */
	time_t get_ttl(void) const;

	/**
	 * ʹ session �ӷ���˵Ļ�����ɾ����ʹ session ʧЧ
	 * @return {bool} �Ƿ�ʹ session ʧЧ
	 */
	bool remove(void);

protected:
	// ��ö�Ӧ sid ������
	virtual bool get_data(const char* sid, string& buf) = 0;

	// ���ö�Ӧ sid ������
	virtual bool set_data(const char* sid, const char* buf,
		size_t len, time_t ttl) = 0;

	// ɾ����Ӧ sid ������
	virtual bool del_data(const char* sid) = 0;

	// ���ö�Ӧ sid ���ݵĹ���ʱ��
	virtual bool set_timeout(const char* sid, time_t ttl) = 0;

private:
	time_t ttl_;
	VBUF* sid_;

	// �ñ�����Ҫ���� set_ttl �����У�����Ʋ�� sid_ ֻ���²�����
	// �һ�û���ں�� cache ����˴洢���� set_ttl �����������º��
	// �� cache ������
	bool sid_saved_;
	bool dirty_;
	std::map<string, VBUF*> attrs_;
	std::map<string, VBUF*> attrs_cache_;

	// �� session �������л�
	static void serialize(const std::map<string, VBUF*>& attrs, string& out);

	static void serialize(const char* name, const void* value,
		size_t len, string& buf);

	// �� session ���ݷ����л�
	static void deserialize(string& buf, std::map<string, VBUF*>& attrs);

	// ��� session ���Լ���
	static void attrs_clear(std::map<string, VBUF*>& attrs);

	// �����ڴ����
	static VBUF* vbuf_new(const void* str, size_t len, todo_t todo);

	// ���ڴ����ֵ������ڴ����ռ䲻���������·���
	// �ڴ棬�����߱����÷���ֵ��Ϊ�µ��ڴ���󣬸ö���
	// ������ԭ�е��ڴ����Ҳ�п������µ��ڴ����
	static VBUF* vbuf_set(VBUF* buf, const void* str, size_t len, todo_t todo);

	// �ͷ��ڴ����
	static void vbuf_free(VBUF* buf);
};

} // namespace acl
