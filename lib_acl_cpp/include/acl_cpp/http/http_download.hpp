#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

class http_client;
class http_request;
class http_header;

class ACL_CPP_API http_download
{
public:
	/**
	 * ���캯��
	 * @param url {const char*} �ļ��ڷ������ϵ� url ��ַ
	 * @param addr {const char*} �ǿ�ʱ�����÷�������ַ(��ʽΪ:
	 *  ip[|domain]:port�������������ַ�� url ����ȡ
	 */
	http_download(const char* url, const char* addr = NULL);
	virtual ~http_download();

	/**
	 * �ڵ��� run ֮ǰ����ͨ���������������ͷ���󣬱����û�����
	 * �Լ�������ͷ�ֶ�(�� set_method/set_range ���ڲ��Զ����õ�)
	 * @return {http_header*} ���� NULL ��ʾ����� URL �Ƿ�
	 */
	http_header* request_header() const;

	/**
	 * ���ô˺������Ի�� http_request ���󣬱������û��ѯ����ͷ
	 * �򷵻������еĲ���
	 * @return {http_request*} ���� NULL ��ʾ����� URL �Ƿ�
	 */
	http_request* request() const;

	/**
	 * �����ļ����� range_from >= 0 �� range_to >= range_from ʱ�Զ�
	 * ���÷ֶ����ط�ʽ���������ȫ�����ط�ʽ
	 * @param range_from {acl_int64} ������ʼƫ��λ�ã��±�� 0 ��ʼ��
	 *  ����ֵ >= 0 �� range_to >= ��ֵʱ�Ų��÷ֶ����ط�ʽ
	 * @param range_to {acl_int64} ���ؽ���ƫ��λ��
	 * @param req_body {const char*} �����������
	 * @param len {size_t} req_body �ǿ�ʱָ���䳤��
	 * @return {bool} �����Ƿ�ɹ���������� true ���ʾ���سɹ�������
	 *  ��������������Ƿ����� URL �����ڣ����������֧�ֶϵ㴫�䣬��
	 *  �����ع��������෵�� false ��ֹ��������
	 */
#ifdef WIN32
	bool get(__int64 range_from = -1, __int64 range_to = -1,
		const char* req_body = NULL, size_t len = 0);
#else
	bool get(long long int range_from = -1, long long int range_to = -1,
		const char* req_body = NULL, size_t len = 0);
#endif

	/**
	 * �����ڲ�����״̬
	 * @param url {const char*} �ǿ�ʱ���ô� URL ������캯��������� URL,
	 *  ������Ȼʹ�ù��캯����ʹ�õ� url
	 * @param addr {const char*} �ǿ�ʱ�����÷�������ַ(��ʽΪ:
	 *  ip[|domain]:port�������������ַ�� url ����ȡ
	 * @return {bool} ���� false ��ʾ url �Ƿ�
	 */
	bool reset(const char* url = NULL, const char* addr = NULL);

	/**
	 * ȡ���ɹ��캯���� reset ��������� url
	 * @return {const char*} ���� NULL ��ʾ����� url �Ƿ�
	 */
	const char* get_url() const;

	/**
	 * ȡ���ɹ��캯���� reset ��������� url ���õ��ķ�������ַ����ʽΪ��
	 * ip[|domain]:port
	 * @return {const char*} ���� NULL ��ʾ����� url �Ƿ�
	 */
	const char* get_addr() const;

protected:
	/**
	 * �������� HTTP �������ݺ󣬶��� HTTP ��������Ӧͷ��Ļص�����
	 * @param conn {http_client*}
	 * @return {bool} �����෵�� false ��ֹͣ��������
	 */
	virtual bool on_response(http_client* conn);

	/**
	 * ���õ����������������ļ����Ⱥ�Ļص�����
	 * @param n {__int64} �����ļ�����
	 * @return {bool} �����෵�� false ��ֹͣ��������
	 */
#ifdef WIN32
	virtual bool on_length(__int64 n);
#else
	virtual bool on_length(long long int n);
#endif

	/**
	 * ���ع����У������ر�֪ͨ�������ص����ݼ����ݳ���
	 * @param data {const void*} ���ص����ݵ�ַ
	 * @param len {size_t} ���ص����ݳ���
	 * @return {bool} �����෵�� false ��ֹͣ��������
	 */
	virtual bool on_save(const void* data, size_t len) = 0;

private:
	char* url_;
	char  addr_[128];
	http_request* req_;

	// ��ͷ��ʼ���������ļ�
	bool save_total(const char* body, size_t len);

	// �ϵ����ز����ļ�
#ifdef WIN32
	bool save_range(const char* body, size_t len,
		__int64 range_from, __int64 range_to);
#else
	bool save_range(const char* body, size_t len,
		long long int range_from, long long int range_to);
#endif

	// ��ʼ����
	bool save(http_request* req);
};

} // namespace acl
