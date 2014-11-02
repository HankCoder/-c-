#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <time.h>
#include "acl_cpp/connpool/connect_client.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/mime/rfc2047.hpp"

namespace acl {

class socket_stream;

typedef class memcache mem_cache;

/**
 * memcached �ͻ���ͨ��Э��⣬֧�ֳ��������Զ�����
 */
class ACL_CPP_API memcache : public connect_client
{
public:
	/**
	* ���캯��
	* @param addr {const char*} memcached ������������ַ����ʽΪ��
	*  ip:port����: 127.0.0.1:11211
	* @param conn_timeout {int} ���ӳ�ʱʱ��(��)
	* @param rw_timeout {int} IO ��д��ʱʱ��(��)
	*/
	memcache(const char* addr = "127.0.0.1:11211",
		int conn_timeout = 180, int rw_timeout = 300);

	~memcache();

	/**
	 * ���� key ��ǰ׺����ʵ�ʵ� key ���� ��ǰ׺+ԭʼkey ��ɣ�ȱʡʱ����ǰ׺��
	 * �����Ӧ�ù���ͬһ�� memcached ����ʱ������Ӧ����������� key ǰ׺������
	 * ���Ա���������Ӧ�õ� key �����ظ�����
	 * @param keypre {const char*} �ǿ�ʱ���� key ǰ׺������ȡ�� key ǰ׺
	 * @return {memcache&}
	 */
	memcache& set_prefix(const char* keypre);

	/**
	 * �ڱ��ֵĳ������ж�ʱ�Ƿ�Ҫ���Զ�������ȱʡΪ�Զ�����
	 * @param onoff {bool} Ϊ true ʱ��ʾ����������Ͽ����Զ�����
	 * @return {memcache&}
	 */
	memcache& auto_retry(bool onoff);

	/**
	 * �����Ƿ���� KEY ��ֵ���б��룬ȱ��ʱ���� key ���룬��Ӧ�õ� key �п���
	 * ���������ַ��������ֵʱ��������ô˺����� key ���б���
	 * @param onoff {bool} Ϊ true ��ʾ�ڲ���Ҫ�� key ���б���
	 * @return {memcache&}
	 */
	memcache& encode_key(bool onoff);

	/**
	* �� memcached ���޸Ļ�����µ����ݻ������
	* @param key {const char*} ��ֵ
	* @param klen {size_t} key ��ֵ����
	* @param dat {const void*} ����
	* @param dlen {size_t} data ���ݳ���
	* @param timeout {time_t} ���泬ʱʱ��(��)
	* @param flags {unsigned short} �����ı�־λ
	* @return {bool} �Ƿ�ɹ�
	*/
	bool set(const char* key, size_t klen,
		const void* dat, size_t dlen,
		time_t timeout = 0, unsigned short flags = 0);

	/**
	* �� memcached ���޸Ļ�����µ����ݻ������
	* @param key {const char*} �ַ�����ֵ
	* @param dat {const void*} ����
	* @param dlen {size_t} data ���ݳ���
	* @param timeout {time_t} ���泬ʱʱ��(��)
	* @param flags {unsigned short} �����ı�־λ
	* @return {bool} �Ƿ�ɹ�
	*/
	bool set(const char* key, const void* dat, size_t dlen,
		time_t timeout = 0, unsigned short flags = 0);

	/**
	* ���� memcached ���Ѿ����ڵļ��Ĺ������ڣ���ΪĿǰ libmemcached û��
	* �ṩ�˽ӿڣ����Ըú���ʵ�ֵķ�ʽ���ȵ��� get ȡ����Ӧ����ֵ��Ȼ����
	* ���� set �������øü���ֵ������ʱ��
	* @param key {const char*} ��ֵ
	* @param klen {size_t} key ��ֵ����
	* @param timeout {time_t} ����ʱ��(��)
	* @return {bool} �Ƿ�ɹ�
	*/
	bool set(const char* key, size_t klen, time_t timeout = 0);

	/**
	* ���� memcached ���Ѿ����ڵļ��Ĺ������ڣ���ΪĿǰ libmemcached û��
	* �ṩ�˽ӿڣ����Ըú���ʵ�ֵķ�ʽ���ȵ��� get ȡ����Ӧ����ֵ��Ȼ����
	* ���� set �������øü���ֵ������ʱ��
	* @param key {const char*} �ַ�����ֵ
	* @param timeout {time_t} ����ʱ��(��)
	* @return {bool} �Ƿ�ɹ�
	*/
	bool set(const char* key, time_t timeout = 0);

	/**
	 * ����ʽ��ʽ�ϴ�������ʱ���ú�����������ͷ
	 * @param key {const char*} ��ֵ�ַ���
	 * @param dlen {size_t} ������������ܳ���
	 * @param timeout {time_t} ���ݵĹ���ʱ��(��)
	 * @param flags {unsigned short} �����ı�־λ
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool set_begin(const char* key, size_t dlen,
		time_t timeout = 0, unsigned short flags = 0);

	/**
	 * ѭ�����ñ������ϴ�����ֵ���ڲ����Զ������Ѿ��ϴ��������ܺ��Ƿ�ﵽ�� set_begin
	 * �����õ������ܳ��ȣ����ﵽ����Զ���һ�� "\r\n"�������߲�Ӧ�ٵ��ô˺����ϴ����ݣ�
	 * ������һ���µ��ϴ����̿�ʼ��
	 * @param data {const void*} ���ݵ�ַָ��
	 * @param len {data} data ���ݳ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool set_data(const void* data, size_t len);

	/**
	* �� memcached �л�ö�Ӧ��ֵ�Ļ�������
	* @param key {const char*} �ַ�����ֵ
	* @param klen {size_t} ��ֵ����
	* @param buf {string&} �洢����Ļ��������ڲ����Ȼ���ոû�����
	* @param flags {unsigned short*} �洢�����ı�־λ
	* @return {bool} ���� true ��ʾ��ȷ��ý��ֵ�������ʾ��ֵ��Ӧ��
	*  ���ݲ����ڻ����
	*/
	bool get(const char* key, size_t klen, string& buf,
		unsigned short* flags = NULL);

	/**
	* �� memcached �л�ö�Ӧ��ֵ�Ļ�������
	* @param key {const char*} �ַ�����ֵ
	* @param buf {string&} �洢����Ļ��������ڲ����Ȼ���ոû�����
	* @param flags {unsigned short*} �洢�����ı�־λ
	* @return {bool} ���� true ��ʾ��ȷ��ý��ֵ�������ʾ��ֵ��Ӧ��
	*  ���ݲ����ڻ����
	*/
	bool get(const char* key, string& buf, unsigned short* flags = NULL);

	/**
	 * ��ʽ��ʽ�ӷ���˻�ȡ���ݣ���������������Э��
	 * @param key {const void*} ��ֵ
	 * @param klen {size_t} key ��ֵ����
	 * @param flags {unsigned short*} �洢�����ı�־λ
	 * @return {int} ����������ĳ��ȣ��������������Σ�
	 *   0����ʾ������
	 *  -1����ʾ����
	 *  >0����ʾ������ĳ���
	 */
	int get_begin(const void* key, size_t klen, unsigned short* flags = NULL);

	/**
	 * ��ʽ��ʽ�ӷ���˻�ȡ���ݣ���������������Э��
	 * @param key {const char*} ��ֵ�ַ���
	 * @param flags {unsigned short*} �洢�����ı�־λ
	 * @return {int} ����������ĳ��ȣ��������������Σ�
	 *   0����ʾ������
	 *  -1����ʾ����
	 *  >0����ʾ������ĳ���
	 */
	int get_begin(const char* key, unsigned short* flags = NULL);

	/**
	 * ��ʽ��ʽ�ӷ���˻�ȡ���ݣ�ѭ�����ñ�������������
	 * @param buf {void*} ��������ַ
	 * @param size {size_t} ��������С
	 * @return {int} �Ѷ��������ݴ�С����Ϊ�����������Σ�
	 *  0����ʾ���ݶ���
	 *  > 0: ��ʾ���ζ��������ݳ���
	 *  -1����ʾ����
	 */
	int  get_data(void* buf, size_t size);

	/**
	* �� memcached ��ɾ������
	* @param key {const char*} ��ֵ
	* @param klen {size_t} ��ֵ����
	* @return {bool} ɾ���Ƿ�ɹ�
	*/
	bool del(const char* key, size_t klen);

	/**
	* �� memcached ��ɾ������
	* @param key {const char*} �ַ�����ֵ
	* @return {bool} ɾ���Ƿ�ɹ�
	*/
	bool del(const char* key);

	/**
	* ����ϴβ��� memcached ����������Ϣ
	* @return {const char*} ����������Ϣ������Ϊ��
	*/
	const char* last_serror() const;

	/**
	* ����ϴβ��� memcached �Ĵ����
	* @return {int} �����
	*/
	int  last_error() const;

	/**
	* ���� memcached ������, ��Ϊ set/get/del ���������Զ�����
	* memcached �����ӣ����Բ�����ʾ�ص��ô˺��������� memcached
	* ������
	* @return {bool} ���Ƿ�ɹ�
	*/
	virtual bool open();

	/**
	* �ر��� memcached �����ӣ�һ��ú�������Ҫ���ã���Ϊ�������
	* ����ʱ���Զ����ô˺���
	*/
	void close();

	/**
	* �г� memcached ���ӵ�һЩ���ԣ�������
	*/
	void property_list();

private:
	bool set(const string& key, const void* dat, size_t dlen,
		time_t timeout, unsigned short flags);
	bool get(const string& key, string& buf, unsigned short* flags);
	const string& build_key(const char* key, size_t klen);

	string* keypre_;         // �ǿ�ʱ�����ַ���������� KEY ֵǰ����µ� KEY
	rfc2047 coder_;          // ����Ҫ�� KEY ����ʱ�ı�����
	int   conn_timeout_;     // �������ӳ�ʱʱ��
	int   rw_timeout_;       // ���� IO ��ʱʱ��
	bool  encode_key_;       // �Ƿ���Ҫ�� KEY ���б���

	bool  opened_;           // �����Ƿ��
	bool  retry_;            // �Ƿ�֧�������ж�����
	char* addr_;             // �����ַ(ip:port)
	int   enum_;             // ����ţ���������������
	string ebuf_;            // �洢������Ϣ
	string kbuf_;            // �洢��ת���� KEY ֵ������

	size_t content_length_;  // ��������ʽ�ϴ�/���ش�����ʱ��ֵ��¼��������ܳ���
	size_t length_;          // �Ѿ��ϴ�/���ص������ܺ�

	socket_stream* conn_;    // ���˷�������Ӷ���
	string req_line_;        // �洢��������
	string res_line_;        // �洢��Ӧ����
	bool error_happen(const char* line);
};

} // namespace acl
