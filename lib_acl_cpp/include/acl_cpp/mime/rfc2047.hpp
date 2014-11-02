#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>

namespace acl {

class string;
class mime_code;

struct rfc2047_entry 
{
	string* pData;		// ��������
	string* pCharset;	// �ַ���
	char  coding;		// �����ʽ��B ��ʾ BASE64, Q ��ʾ QP
};

class ACL_CPP_API rfc2047
{
public:
	/**
	 * ���캯��
	 * @param strip_sp {bool} �ڽ���������Ƿ�ȥ���س����з���ÿ�п�ͷ�Ŀո�TAB
	 * @param addCrlf {bool} �ڱ�������е����ݱȽϳ�ʱ�Ƿ��Զ���� "\r\n"
	 */
	rfc2047(bool strip_sp = false, bool addCrlf = true);
	~rfc2047();

	/**
	 * ��ʽ��������, ����ѭ�����ô˺���, ÿ����Ӳ�������
	 * ֱ��������
	 * @param in {const char*} ����Դ�ַ���
	 * @param n {int} in ���봮�ĳ���
	 */
	void decode_update(const char* in, int n);

	/**
	 * �� rfc2047 �������ת����ָ�����ַ����ַ���, �������
	 * ��ȷת������Դ������
	 * @param to_charset {const char*} Ŀ���ַ���
	 * @param out {string*} �洢ת�����
	 * @param addInvalid {bool} ��Ϊ true ʱ����ת����������������
	 *  �Ƿ��ַ�������ֱ�ӿ�����������������Ĭ���������ֱ�ӿ���
	 * @return {bool} ת���Ƿ�ɹ�
	 */
	bool decode_finish(const char* to_charset, string* out,
		bool addInvalid = true);

	/**
	 * rfc2047 ����������������
	 * @param in {const char*} ��������
	 * @param n {int} in ���ݳ���
	 * @param out {string*} �洢������
	 * @param charset {const char*} �������ݵ��ַ�������
	 * @param coding {char} �������ͣ�֧�ֵı���������:
	 *   B: base64, Q: quoted_printable
	 * @return {bool} �����������Ƿ���ȷ�ұ����Ƿ�ɹ�
	 */
	bool encode_update(const char* in, int n, string* out,
		const char* charset = "gb2312", char coding = 'B');

	/**
	 * �� encode_update ��ӵ����ݽ��б����洢���û�ָ��������
	 * @param  out {string*} �洢���������û�������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool encode_finish(string* out);

	/**
	 * ��̬������
	 * @param in {const char*} �������ݵ�ַ
	 * @param n {int} ���ݳ���
	 * @param out {string*} �洢�������Ļ�����
	 * @param charset {const char*} �������ݵ��ַ���
	 * @param coding {char} �������ͣ�֧�ֵı���������:
	 *   B: base64, Q: quoted_printable
	 * @param addCrlf {bool} �ڱ�������е����ݱȽϳ�ʱ�Ƿ��Զ���� "\r\n"
	 * @return {bool} �����Ƿ�ɹ�
	 */
	static bool encode(const char* in, int n, string* out,
		const char* charset = "gb2312", char coding = 'B',
		bool addCrlf = true);

	/**
	 * ��̬������
	 * @param in {const char*} �������ݵ�ַ
	 * @param n {int} ���ݳ���
	 * @param out {string*} �洢�������Ļ�����
	 * @param to_charset {const char*} Ŀ���ַ���
	 * @param strip_sp {bool} �Ƿ�ȥ���س����з���ÿ�п�ͷ�Ŀո�TAB
	 * @param addInvalid {bool} ��Ϊ true ʱ����ת����������������
	 *  �Ƿ��ַ�������ֱ�ӿ�����������������Ĭ���������ֱ�ӿ���
	 * @return {bool} �����Ƿ�ɹ�
	 */
	static bool decode(const char* in, int n, string* out,
		const char* to_charset = "gb2312", bool strip_sp = false,
		bool addInvalid = true);

	/**
	 * ������������������ʽ����
	 * @return {const std::list<rfc2047_entry*>&}
	 */
	const std::list<rfc2047_entry*>& get_list(void) const;

	/**
	 * ���ý�����״̬��, �ý��������ٴ�ʹ��
	 * @param strip_sp {bool} �Ƿ�ȥ���س����з���ÿ�п�ͷ�Ŀո�TAB
	 */
	void reset(bool strip_sp = false);

	/**
	 * ��������������
	 */
	void debug_rfc2047(void) const;

protected:
private:
	std::list<rfc2047_entry*> m_List;
	rfc2047_entry* m_pCurrentEntry;
	mime_code* m_coder;
	int   m_status;
	bool  m_stripSp;
	bool  m_addCrlf;
	char  m_lastCh;

public:
	// ���º������ڲ�ʹ��

	int status_next(const char* s, int n);
	int status_data(const char* s, int n);
	int status_charset(const char* s, int n);
	int status_coding(const char* s, int n);
	int status_equal_question(const char* s, int n);
	int status_question_first(const char* s, int n);
	int status_question_second(const char* s, int n);
	int status_question_equal(const char* s, int n);
};

} // namespace acl
