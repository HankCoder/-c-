#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/mime/mime_code.hpp"

namespace acl {

class string;

class ACL_CPP_API mime_quoted_printable : public mime_code
{
public:
	mime_quoted_printable();

	/**
	 * ���캯��
	 * @param addCrlf {bool} ����ʽ����ʱ�Ƿ���ĩβ��� "\r\n"
	 * @param addInvalid {bool} ��ʽ����ʱ�Ƿ������Ƿ��ַ��Ƿ�ԭ������
	 */
	mime_quoted_printable(bool addCrlf, bool addInvalid);
	~mime_quoted_printable();

	// ������麯������

	/* ��ʽ���뺯����ʹ�÷����� encode_update->encode_update->...->encode_finish */

	/**
	 * ��ʽ���뺯��
	 * @param src {const char*} ԭʼ����
	 * @param n {int} src ���ݳ���
	 * @param out {string*} �洢������������ͨ�� out->empty()
	 *  ������ out ���Ƿ��н�����ݣ��� out->length() ��� out �н��
	 *  ���ݵĳ��ȣ�ע�⵱���� out �еĽ�����ݺ�һ��Ҫ���� out->clear()
	 *  ������ù��Ľ�����ݣ�Ҳ���Զ�ε��øú�������������
	 *  encode_finish ��һ����ȡ����������
	 */
	void encode_update(const char *src, int n, string* out);

	/**
	 * ��ʽ����������������øú�����ȡ�����Ľ������
	 * @param out {string*} �洢������������ͨ�� out->empty()
	 *  ������ out ���Ƿ��н�����ݣ��� out->length() ��� out �н��
	 *  ���ݵĳ��ȣ�ע�⵱���� out �еĽ�����ݺ�һ��Ҫ���� out->clear()
	 *  ������ù��Ľ������
	 */
	void encode_finish(string* out);

	/* ��ʽ���뺯����ʹ�÷����� decode_update->decode_update->...->decode_finish */

	/**
	 * ��ʽ���뺯��
	 * @param src {const char*} ԭʼ����
	 * @param n {int} src ���ݳ���
	 * @param out {string*} �洢������������ͨ�� out->empty()
	 *  ������ out ���Ƿ��н�����ݣ��� out->length() ��� out �н��
	 *  ���ݵĳ��ȣ�ע�⵱���� out �еĽ�����ݺ�һ��Ҫ���� out->clear()
	 *  ������ù��Ľ�����ݣ�Ҳ���Զ�ε��øú�������������
	 *  decode_finish ��һ����ȡ����������
	 */
	void decode_update(const char *src, int n, string* out);

	/**
	 * ��ʽ����������������øú�����ȡ�����Ľ������
	 * @param out {string*} �洢������������ͨ�� out->empty()
	 *  ������ out ���Ƿ��н�����ݣ��� out->length() ��� out �н��
	 *  ���ݵĳ��ȣ�ע�⵱���� out �еĽ�����ݺ�һ��Ҫ���� out->clear()
	 *  ������ù��Ľ������
	 */
	void decode_finish(string* out);

	/**
	 * ��̬���뺯����ֱ�ӽ��������ݽ��б���ͬʱ�����û�������
	 * �û�������
	 * @param in {const char*} �������ݵ�ַ
	 * @param n {int} �������ݳ���
	 * @param out {string*} �洢����Ļ�����
	 */
	static void encode(const char* in, int n, string* out);

	/**
	 * ��̬���뺯����ֱ�ӽ��������ݽ��н����������û�������
	 * @param in {const char*} �������ݵ�ַ
	 * @param n {int} ���ݳ���
	 * @param out {string*} �洢�������
	 */
	static void decode(const char* in, int n, string* out);

	/**
	 * ���ñࡢ������״̬
	 */
	void reset();

	/**
	 * �����ڱ������ʱ�Ƿ���� "\r\n"
	 * @param on {bool}
	 */
	void add_crlf(bool on);

	/**
	 * �����ڽ���������Ƿ񿽱��Ƿ��ַ�
	 * @param on {bool}
	 */
	void add_invalid(bool on);

protected:
private:
	void encode(string* out);
	void decode(string* out);

	bool hex_decode(unsigned char first, unsigned char second,
		unsigned int *result);

	char  m_encodeBuf[72];
	int   m_encodeCnt;
	char  m_decodeBuf[144];
	int   m_decodeCnt;
	bool  m_addCrLf;
	bool  m_addInvalid;
};

}  // namespace acl

