#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/connpool/connect_client.hpp"

namespace acl {

class http_client;
class http_pipe;
class socket_stream;
class charset_conv;
class xml;
class json;

/**
 * HTTP �ͻ��������࣬�������֧�ֳ����ӣ�ͬʱ�����Ӷ�ʱ���Զ�����
 */
class ACL_CPP_API http_request : public connect_client
{
public:
	/**
	 * ���캯����ͨ���ù��캯������� socket_stream ������
	 * ���ᱻ�رգ���Ҫ�������Լ��ر�
	 * @param client {socket_stream*} �������������ǿգ�
	 *  �ڱ����������ʱ�������󲢲��ᱻ���٣������û��������ͷ�
	 * @param conn_timeout {int} �����������رգ����ڲ���
	 *  �Զ����ԣ���ʱ��Ҫ��ֵ��ʾ���ӷ������ĳ�ʱʱ��(��)��
	 *  ������������ IO ��д��ʱʱ���Ǵ� ��������м̳е�
	 * @param unzip {bool} �Ƿ�Է�������Ӧ�������Զ����н�ѹ
	 * ע��������ʵ�������ʹ��ʱ���û�Ӧ����ÿ�ε���ǰ����
	 * request_header::http_header::reset()
	 */
	http_request(socket_stream* client, int conn_timeout = 60,
		bool unzip = true);

	/**
	 * ���캯�����ù��캯���ڲ������� socket_stream �������йر�
	 * @param addr {const char*} WEB ��������ַ
	 * @param conn_timeout {int} Զ�����ӷ�������ʱʱ��(��)
	 * @param rw_timeout {int} IO ��д��ʱʱ��(��)
	 * @param unzip {bool} �Ƿ�Է�������Ӧ�������Զ����н�ѹ
	 */
	http_request(const char* addr, int conn_timeout = 60,
		int rw_timeout = 60, bool unzip = true);

	virtual ~http_request(void);

	/**
	 * �����ڶ�ȡ������Ӧ����ʱ�Ƿ����ѹ�����ݽ��н�ѹ
	 * @param on {bool}
	 * @return {http_request&}
	 */
	http_request& set_unzip(bool on);

	/**
	 * ��� HTTP ����ͷ����Ȼ���ڷ��ص� HTTP ����ͷ���������
	 * �Լ�������ͷ�ֶλ� http_header::reset()��������ͷ״̬��
	 * �ο���http_header ��
	 * @return {http_header&}
	 */
	http_header& request_header(void);

	/**
	 * ���ñ����ַ������������ַ����ǿ�ʱ����߽������ݱ߽����ַ���ת��
	 * @param local_charset {const char*} �����ַ���
	 * @return {http_header&}
	 */
	http_request& set_local_charset(const char* local_charset);

	/**
	 * �� HTTP ���������� HTTP ����ͷ�� HTTP �����壬ͬʱ��
	 * HTTP ��������ȡ HTTP ��Ӧͷ�����ڳ����ӣ��������ж�ʱ
	 * ��������һ�Σ��ڵ�������ļ��� get_body ����ǰ������
	 * ���ñ�����(����� write_head/write_body)��
	 * ��������£��ú����ڷ������������ݺ��� HTTP ��Ӧͷ��
	 * �����û��ڱ��������� true ����Ե��ã�get_body() ��
	 * http_request::get_clinet()->read_body(char*, size_t)
	 * ������ HTTP ��Ӧ��������
	 * @param data {const void*} ���͵��������ַ���ǿ�ʱ�Զ���
	 *  POST �������ͣ����� GET ��������
	 * @param len {size_} data �ǿ�ʱָ�� data ���ݳ���
	 * @return {bool} �����������ݼ��� HTTP ��Ӧͷ�����Ƿ�ɹ�
	 */
	bool request(const void* data, size_t len);

	/**
	 * ��������ʽд����ʱ����Ҫ���ȵ��ñ��������� HTTP ����ͷ
	 * @return {bool} �Ƿ�ɹ�������ɹ��ſ��Լ������� write_body
	 */
	bool write_head();

	/**
	 * ��������ʽд����ʱ���ڵ��� write_head �󣬿���ѭ�����ñ�����
	 * ���� HTTP ���������ݣ����������������Ϊ��ֵʱ���ʾ����д�ꣻ
	 * �����������ݺ󣬸ú����ڲ����Զ���ȡ HTTP ��Ӧͷ���ݣ��û���
	 * �������� get_body/read_body ��ȡ HTTP ��Ӧ������
	 * @param data {const void*} ���ݵ�ַָ�룬����ֵΪ��ָ��ʱ��ʾ
	 *  ���ݷ������
	 * @param len {size_t} data �ǿ�ָ��ʱ��ʾ���ݳ���
	 * @return {bool} �����������Ƿ�ɹ�
	 *  ע����Ӧ�÷��������ݺ󣬱����ٵ���һ�α�������ͬʱ����������������
	 */
	bool write_body(const void* data, size_t len);

	/////////////////////////////////////////////////////////////////////

	/**
	 * ���������������ݺ��ڲ����Զ����ö� HTTP ��Ӧͷ���̣�����ͨ���˺�����÷����
	 * ��Ӧ�� HTTP ״̬��(2xx, 3xx, 4xx, 5xx)��
	 * ��ʵ�ú����ڲ�ֻ�ǵ����� http_client::response_status ����
	 * @return {int}
	 */
	int http_status() const;

	/**
	 * ��� HTTP ��Ӧ�������峤��
	 * @return {int64) ����ֵ��Ϊ -1 ����� HTTP ͷ�����ڻ�û�г����ֶ�
	 */
#ifdef WIN32
	__int64 body_length(void) const;
#else
	long long int body_length(void) const;
#endif
	/**
	 * HTTP ������(��Ӧ���Ƿ������ֳ�����)
	 * @return {bool}
	 */
	bool keep_alive(void) const;

	/**
	 * ��� HTTP ��Ӧͷ��ĳ���ֶ������ֶ�ֵ
	 * @param name {const char*} �ֶ���
	 * @return {const char*} �ֶ�ֵ��Ϊ��ʱ��ʾ������
	 */
	const char* header_value(const char* name) const;

	/**
	 * �Ƿ������������
	 * @return {bool}
	 */
	bool body_finish() const;

	/**
	 * ������ request �ɹ�����ñ���������ȡ��������Ӧ������
	 * ��������洢�ڹ涨�� xml ������
	 * @param out {xml&} HTTP ��Ӧ�����ݴ洢�ڸ� xml ������
	 * @param to_charset {const char*} ������ǿգ��ڲ��Զ�
	 *  ������ת�ɸ��ַ����洢�� xml ������
	 * @return {bool} �������Ƿ�ɹ�
	 * ע������Ӧ�������ر��ʱ��Ӧ�ô˺����������ڴ�Ĺ�
	 */
	bool get_body(xml& out, const char* to_charset = NULL);

	/**
	 * ������ request �ɹ�����ñ���������ȡ��������Ӧ������
	 * ��������洢�ڹ涨�� json ������
	 * @param out {json&} HTTP ��Ӧ�����ݴ洢�ڸ� json ������
	 * @param to_charset {const char*} ������ǿգ��ڲ��Զ�
	 *  ������ת�ɸ��ַ����洢�� json ������
	 * @return {bool} �������Ƿ�ɹ�
	 * ע������Ӧ�������ر��ʱ��Ӧ�ô˺����������ڴ�Ĺ�
	 */
	bool get_body(json& out, const char* to_charset = NULL);

	/*
	 * ������ request �ɹ�����ñ���������ȡ������ȫ����Ӧ����
	 * �洢������Ļ�������
	 * @param out {string&} �洢��Ӧ������
	 * @param to_charset {const char*} ������ǿգ��ڲ��Զ�
	 *  ������ת�ɸ��ַ����洢�� out ������
	 * ע������Ӧ�������ر��ʱ��Ӧ�ô˺����������ڴ�Ĺ�
	 */
	bool get_body(string& out, const char* to_charset = NULL);

	/*
	 * ������ request �ɹ�����ñ���������ȡ��������Ӧ���ݲ�
	 * �洢������Ļ������У�����ѭ�����ñ�������ֱ�����ݶ����ˣ�
	 * @param buf {char*} �洢������Ӧ������
	 * @param size {size_t} buf ��������С
	 * @return {int} ����ֵ == 0 ��ʾ��������ϣ�< 0 ��ʾ������
	 *  �ر����ӣ�> 0 ��ʾ�Ѿ����������ݣ��û�Ӧ��һֱ������ֱ��
	 *  ����ֵ <= 0 Ϊֹ
	 *  ע���ú�����������ԭʼ HTTP ���������ݣ�������ѹ���ַ���
	 *  ���룬�û��Լ�������Ҫ���д���
	 */
	int read_body(char* buf, size_t size);

	/**
	 * ������ request �ɹ�����ñ������� HTTP ��Ӧ�����壬����ѭ������
	 * ���������������ڲ��Զ���ѹ�����ݽ��н�ѹ������ڵ��ñ�����֮ǰ����
	 * set_charset �����˱����ַ�������ͬʱ�����ݽ����ַ���ת�����
	 * @param out {string&} �洢�������
	 * @param clean {bool} ÿ�ε��ñ�����ʱ���Ƿ�Ҫ�����Զ��������� out
	 *  ���������
	 * @param real_size {int*} ����ָ��ǿ�ʱ���洢��ѹǰ��������������
	 *  ���ȣ�����ڹ��캯����ָ���˷��Զ���ѹģʽ�Ҷ��������� > 0�����
	 *  ֵ�洢�ĳ���ֵӦ���뱾��������ֵ��ͬ�����������δ�����κ�����ʱ��
	 *  ��ֵ�洢�ĳ���ֵΪ 0
	 * @return {int} == 0 ��ʾ����ϣ��������Ӳ�δ�رգ�>0 ��ʾ���ζ�����
	 *  ���������ݳ���(��Ϊ��ѹ�������ʱ�����ʾΪ��ѹ֮������ݳ��ȣ�
	 *  ����ʵ���������ݲ�ͬ����ʵ���������ݳ���Ӧ��ͨ������ real_size ��
	 *  ���); < 0 ��ʾ�������رգ���ʱ�� real_size �ǿգ��� real_size ��
	 *  ����ֵӦ��Ϊ 0
	 */
	int read_body(string& out, bool clean = false, int* real_size = NULL);

	/**
	 * ������ request �ɹ�����ñ��������� HTTP ����˶�һ�����ݣ�����ѭ������
	 * ��������ֱ������ false �� body_finish() ���� true Ϊֹ��
	 * �������ڲ��Զ���ѹ�����ݽ��н�ѹ������ڵ��ñ�����֮ǰ���� set_charset ������
	 * �����ַ�������ͬʱ�����ݽ����ַ���ת�����
	 * @param out {string&} �洢�������
	 * @param nonl {bool} ������һ�������Ƿ��Զ�ȥ��β���� "\r\n" �� "\n"
	 * @param size {size_t*} ��ָ��ǿ�ʱ��Ŷ��������ݳ���
	 * @return {bool} �Ƿ������һ�����ݣ������� true ʱ��ʾ������һ�����ݣ�����
	 *  ͨ�� body_finish() �Ƿ�Ϊ true ���ж��Ƿ���������Ѿ�������������һ������
	 *  �� nonl = true ʱ���� *size = 0�������� false ʱ��ʾδ���������Ҷ���ϣ�
	 *  *size �д���Ŷ��������ݳ���
	 */
	bool body_gets(string& out, bool nonl = true, size_t* size = NULL);

	/**
	 * ��ͨ�� http_request::request_header().set_range() ������
	 * range ������ʱ���˺��������������ص������Ƿ�֧�� range
	 * @return {bool}
	 */
	bool support_range(void) const;

#ifdef WIN32
	/**
	 * �������� http_request::request_header().set_range() �Ҷ�ȡ������
	 * ���ص�����ͷ�󣬴˺����������֧�ֶַι��ܵ���ʼƫ��λ��
	 * @return {acl_int64} ����������֧�� range ��ʽ���򷵻�ֵ < 0
	 */
	__int64 get_range_from(void) const;

	/**
	 * �������� http_request::request_header().set_range() �Ҷ�ȡ������
	 * ���ص�����ͷ�󣬴˺����������֧�ֶַι��ܽ���ƫ��λ��
	 * @return {acl_int64} ����������֧�� range ��ʽ���򷵻�ֵ < 0
	 */
	__int64 get_range_to(void) const;

	/**
	 * �������� http_request::request_header().set_range() �Ҷ�ȡ������
	 * ���ص�����ͷ�󣬴˺����������֧�ֶַι��ܵ������������С����ֵ
	 * ������ HTTP ��Ӧ�������С
	 * @return {acl_int64} ����������֧�� range ��ʽ���򷵻�ֵ < 0
	 */
	__int64 get_range_max(void) const;
#else
	long long int get_range_from(void) const;
	long long int get_range_to(void) const;
	long long int get_range_max(void) const;
#endif

	/**
	 * ��÷��������ص� Set-Cookie �ļ���
	 * @return {const std::vector<HttpCookie*>*} ���ؿձ�ʾ
	 *  û�� cookie �����������Ϊ��
	 */
	const std::vector<HttpCookie*>* get_cookies(void) const;

	/**
	 * ��÷��������ص� Set-Cookie ���õ�ĳ�� cookie ����
	 * @param name {const char*} cookie ��
	 * @param case_insensitive {bool} �Ƿ����ִ�Сд��true ��ʾ
	 *  �����ִ�Сд
	 * @return {const HttpCookie*} ���� NULL ��ʾ������
	 */
	const HttpCookie* get_cookie(const char* name,
		bool case_insensitive = true) const;

	/////////////////////////////////////////////////////////////////////

	/**
	 * ��� http_client HTTP ������������ͨ�����صĶ�����
	 * ��������Ӧ��ͷ�������ݣ��ο���http_client ��
	 * @return {http_client*} �����ؿ�ʱ��ʾ��������
	 */
	http_client* get_client(void) const;

	/**
	 * ��������״̬����ͬһ�����ӵĶ������ʱ����ô˺���
	 */
	void reset(void);

	/**
	 * �����������ӳ�ʱ�������д��ʱʱ��(��)
	 * @param conn_timeout {int} �������ӳ�ʱ
	 * @param rw_timeout {int} �����д��ʱ
	 */
	void set_timeout(int conn_timeout, int rw_timeout);

protected:
	/**
	 * ���� connect_client �Ĵ��麯������ʽ�ص��ñ����������������˵�����
	 * @return {bool} �����Ƿ�ɹ�
	 */
	virtual bool open();

private:
	char addr_[64];
	bool keep_alive_;
	int  conn_timeout_;
	int  rw_timeout_;
	bool unzip_;
	char local_charset_[64];
	charset_conv* conv_;
        http_client* client_;
	http_header  header_;
	bool cookie_inited_;
	std::vector<HttpCookie*>* cookies_;
#ifdef WIN32
	__int64 range_from_;
	__int64 range_to_;
	__int64 range_max_;
#else
	long long int range_from_;
	long long int range_to_;
	long long int range_max_;
#endif
	// ��д HTTP ����������ʱ���ñ�־λ��ʶ�Ƿ��������Թ�
	bool need_retry_;

	bool send_request(const void* data, size_t len);
	bool try_open(bool* reuse_conn);
	void close(void);
	void create_cookies(void);
	http_pipe* get_pipe(const char* to_charset);
	void set_charset_conv();
	void check_range(void);
};

} // namespace acl
