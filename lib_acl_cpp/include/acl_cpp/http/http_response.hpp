#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/http/http_header.hpp"

namespace acl {

class http_client;
class http_pipe;
class socket_stream;
class xml;
class json;

class ACL_CPP_API http_response
{
public:
	/**
	 * ���캯����ͨ���ù��캯������� socket_stream ������
	 * ���ᱻ�رգ���Ҫ�������Լ��ر�
	 * @param client {socket_stream*} �������������ǿ�
	 * ע������ʵ���ڳ�����ʱ���Ա����ʹ�ã���һ����ע��ʹ��
	 * ˳��get_body->response
	 */
	http_response(socket_stream* client);
	virtual ~http_response(void);

	/////////////////////////////////////////////////////////////////////
	// ���ȡ����������صķ���

	/**
	 * ��ȡ HTTP ����ͻ��˵� HTTP ����ͷ���ڵ��ñ�������ſ��Ե���
	 * get_body/read_body ��ȡ HTTP ����������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool read_header();

	/**
	 * ��ȡ xml ��ʽ�� HTTP ���������壬������˺�����Ӧ�õ���
	 * response ���ͻ��˷�����Ӧ���ݣ��ú���ÿ�α�����ʱ���ڲ���
	 * ����ᱻ��ʼ�������Ըú��������ڶ���Ự�б���ε���
	 * @param out {xml&} �� HTTP ���������ݴ洢�ڸ� xml ������
	 * @param to_charset {const char*} ������ǿգ��ڲ��Զ�
	 *  ������ת�ɸ��ַ����洢�� xml ������
	 * @return {bool} �Ƿ�����
	 *  ע���������ȵ��� read_header ����ܵ��ñ�����
	 *      �������������ر��ʱ��Ӧ�ô˺����������ڴ�Ĺ�
	 */
	bool get_body(xml& out, const char* to_charset = NULL);

	/**
	 * ��ȡ json ��ʽ�� HTTP ���������壬������˺�����Ӧ�õ���
	 * response ���ͻ��˷�����Ӧ���ݣ��ú���ÿ�α�����ʱ���ڲ���
	 * ����ᱻ��ʼ�������Ըú��������ڶ���Ự�б���ε���
	 * @param out {json&} �� HTTP ���������ݴ洢�ڸ� json ������
	 * @param to_charset {const char*} ������ǿգ��ڲ��Զ�
	 *  ������ת�ɸ��ַ����洢�� json ������
	 * @return {bool} �Ƿ�����
	 *  ע���������ȵ��� read_header ����ܵ��ñ�����
	 *      �������������ر��ʱ��Ӧ�ô˺����������ڴ�Ĺ�
	 */
	bool get_body(json& out, const char* to_charset = NULL);

	/*
	 * ��ȡ HTTP ȫ�����������ݲ��洢������Ļ�������
	 * @param out {string&} �洢����������
	 * @param to_charset {const char*} ������ǿգ��ڲ��Զ�
	 *  ������ת�ɸ��ַ����洢�� out ������
	 * ע���������������ر��ʱ��Ӧ�ô˺����������ڴ�Ĺ�
	 *     �������ȵ��� read_header ����ܵ��ñ�����
	 */
	bool get_body(string& out, const char* to_charset = NULL);

	/*
	 * ��ȡ HTTP ���������ݲ��洢������Ļ������У�����ѭ��
	 * ���ñ�������ֱ�����ݶ����ˣ�
	 * @param buf {char*} �洢��������������
	 * @param size {size_t} buf ��������С
	 * @return {int} ����ֵ == 0 ��ʾ��������ϣ�< 0 ��ʾ�ͻ���
	 *  �ر����ӣ�> 0 ��ʾ�Ѿ����������ݣ��û�Ӧ��һֱ������ֱ��
	 *  ����ֵ <= 0 Ϊֹ
	 *  ע���ú�����������ԭʼ HTTP ���������ݣ�������ѹ���ַ���
	 *     ���룬�û��Լ�������Ҫ���д����������ȵ��� read_header
	 *     ����ܵ��ñ�����
	 */
	int read_body(char* buf, size_t size);

	/////////////////////////////////////////////////////////////////////
	// ��������Ӧ��صķ�������

	/**
	 * ��� HTTP ��Ӧͷ����Ȼ���ڷ��ص� HTTP ��Ӧͷ���������
	 * �Լ�����Ӧͷ�ֶλ� http_header::reset()������Ӧͷ״̬��
	 * �ο���http_header ��
	 * @return {http_header&}
	 */
	http_header& response_header(void);

	/**
	 * ��ͻ��˷��� HTTP ��Ӧ���ݣ�����ѭ�����ô˺�����
	 * <b>�ڵ��ñ�����ǰ��������ǰ��֤���²�����</b>
	 * 1�������ȵ��� read_header && get_body ��� HTTP �ͻ��˵��������ݣ�
	 * 2������ͨ�� response_header ȡ�� http_header ����ͬʱ������Ӧͷ��
	 *    �ֶΣ��磺set_status, set_keep_alive �ȣ�
	 * <b>�ڵ��ñ�����ʱ�����²������ᷢ����</b>
	 * 1���ڲ����Զ��ڵ�һ��дʱ���� HTTP ��Ӧͷ��
	 * 2����ͨ�� http_header::set_chunked ������ chunked ���䷽ʽ��
	 * �ڲ��Զ����� chunked ���䷽ʽ��
	 * 3����ʹ�� chunked ��ʽ��������ʱ��Ӧ������ٵ���һ�α�������
	 * �Ҳ�������Ϊ 0 ��ʾ���ݽ���
	 * @param data {const void*} ���ݵ�ַ
	 * @param len {size_t} data ���ݳ���
	 * @return {bool} �����Ƿ�ɹ���������� false ��ʾ�����ж�
	 */
	bool response(const void* data, size_t len);

	//////////////////////////////////////////////////////////////////////////

	/**
	 * ��� http_client HTTP ������������ͨ�����صĶ�����
	 * �ͻ�������ͷ�Ĳ������ݣ��ο���http_client ��
	 * @return {http_client*} �����ؿ�ʱ��ʾ��������
	 */
	http_client* get_client(void) const;

	/**
	 * �ر� HTTP ������
	 */
	void close(void);
protected:
private:
	bool debug_;
	bool header_ok_;
	http_client* client_;
	http_header  header_;
	bool head_sent_;
	http_pipe* get_pipe(const char* to_charset);
};

} // namespace acl
