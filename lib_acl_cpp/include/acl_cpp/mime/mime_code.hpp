#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/pipe_stream.hpp"

namespace acl {
	class string;

class ACL_CPP_API mime_code : public pipe_stream
{
public:
	mime_code();

	/**
	 * ���캯��
	 * @param addCrlf {bool} ����ʽ����ʱ�Ƿ���ĩβ��� "\r\n"
	 * @param addInvalid {bool} ��ʽ����ʱ�Ƿ������Ƿ��ַ��Ƿ�ԭ������
	 */
	mime_code(bool addCrlf, bool addInvalid);
	virtual ~mime_code() = 0;

	/* ��ʽ���뺯����ʹ�÷����� encode_update->encode_update->...->encode_finish */

	/**
	 * �������, ���Դ����, ������� out ��, ��������
	 * ���ݲ��������ʱ��������, ���ڲ�����ʱ�������������
	 * @param src {const char*} Դ���ݵ�ַ
	 * @param n {int} Դ���ݳ���
	 * @param out {string*} �洢������, ����ͨ���Ƚ�
	 *  ���ô˺���ǰ��� out->length() ���жϸú����ǽ�����
	 *  ������̻�����ʱ�������; ���ʹ���� out �еĽ�����ݣ�
	 *  �������Ӧ�õ��� out->clear() ����ù�������
	 */
	virtual void encode_update(const char *src, int n,
		string* out);

	/**
	 * �����������Ҫ���ô˺������Կ��ܴ�������ʱ�����е�
	 * Դ���ݽ������ı���
	 * @param out {string*} �洢������, ����ͨ���Ƚ�
	 *  ���ô˺���ǰ��� out->length() ���жϸú����ǽ�����
	 *  ������̻�����ʱ�������; ���ʹ���� out �еĽ�����ݣ�
	 *  �������Ӧ�õ��� out->clear() ����ù�������
	 */
	virtual void encode_finish(string* out);

	/* ��ʽ���뺯����ʹ�÷����� decode_update->decode_update->...->decode_finish */

	/**
	 * �������, ��Ӿ������������, ������������н���, ���
	 * ��������ݲ��������ʱ���ֽ�������, ���ڲ�������ʱ����
	 * ������, �������������ʱ�Ž��н���
	 * @param src {const char*} ��������������
	 * @param n {int} ���ݳ���
	 * @param out {string*} �洢������, ����ͨ���Ƚ�
	 *  ���ô˺���ǰ��� out->length() ���жϸú����ǽ�����
	 *  ������̻�����ʱ�������; ���ʹ���� out �еĽ�����ݣ�
	 *  �������Ӧ�õ��� out->clear() ����ù�������
	 */
	virtual void decode_update(const char *src, int n, string* out);

	/**
	 * �����������Ҫ���ô˺������Կ��ܴ�������ʱ�����е�
	 * Դ���ݽ������Ľ���
	 * @param out {string*} �洢������, ����ͨ���Ƚ�
	 *  ���ô˺���ǰ��� out->length() ���жϸú����ǽ�����
	 *  ������̻�����ʱ�������; ���ʹ���� out �еĽ�����ݣ�
	 *  �������Ӧ�õ��� out->clear() ����ù�������
	 */
	virtual void decode_finish(string* out);

	/**
	 * �����ڲ�������
	 */
	virtual void reset();

	/**
	 * �ڱ�������������Ƿ��Զ���ÿ���������� "\r\n"
	 * @param on {bool}
	 */
	virtual void add_crlf(bool on);

	/**
	 * �ڽ����������������Ƿ��ַ��Ƿ�������ڽ�������
	 * @param on {bool}
	 */
	virtual void add_invalid(bool on);

	/**
	 * ��������ı����������Ӧ�Ľ����
	 * @param toTab {const unsigned char*} ������ַ���
	 * @param out {string*} �洢���
	 */
	static void create_decode_tab(const unsigned char *toTab, string *out);

	/**
	 * �������δ���������麯�������ʹ�û��������Ĭ���麯��ʱ
	 * �����������ô˺��������Լ��ı����, ���������ַ�
	 * @param toTab {const unsigned char*} �����
	 * @param unTab {const unsigned char*} �����
	 * @param fillChar {unsigned char} ����ַ�
	 */
	void init(const unsigned char* toTab,
		const unsigned char* unTab, unsigned char fillChar);

	/**
	 * ����ת�����Ĺ���״̬����Ϊ��ת�����ɱ������ͽ�������ɣ�
	 * ������ʹ�� pipe_stream ��ʽ����ʱ����ָ����ת������״̬
	 * ��ָ���Ǵ��ڱ�����״̬���ǽ�����״̬
	 * @param encoding {bool} ���Ϊ true ��ʾΪ������״̬������
	 *  Ϊ������״̬
	 */
	void set_status(bool encoding = true);

	// pipe_stream �麯������

	virtual int push_pop(const char* in, size_t len,
		string* out, size_t max = 0);
	virtual int pop_end(string* out, size_t max = 0);
	virtual void clear();

	/**
	 * ��̬���������ݱ������� MIME_ENC_XXX (�μ���mime_define.hpp) ���
	 * ��Ӧ�ı������󣬵��е� encoding ����Ϊ MIME_ENC_QP,
	 * MIME_ENC_BASE64, MIME_ENC_UUCODE, MIME_ENC_XXCODE
	 * @param encoding {int} �������ͣ�ֻ��Ϊ MIME_ENC_QP, MIME_ENC_BASE64,
	 *  MIME_ENC_UUCODE, MIME_ENC_XXCODE
	 * @param warn_unsupport {bool} ��δ�ҵ�ƥ��ı������ʱ�Ƿ��¼������Ϣ
	 * @return {mime_code*} ������󣬵�δ�ҵ�ƥ��ı�������ʱ���� NULL
	 */
	static mime_code* create(int encoding, bool warn_unsupport = true);

private:
	void encode(string* out);
	void decode(string* out);

	char  m_encodeBuf[57];
	int   m_encodeCnt;
	char  m_decodeBuf[76];
	int   m_decodeCnt;
	bool  m_addCrLf;
	bool  m_addInvalid;
	bool  m_encoding;
	const unsigned char *m_toTab;
	const unsigned char *m_unTab;
	unsigned char m_fillChar;
	string* m_pBuf;
};

} // namespace acl
