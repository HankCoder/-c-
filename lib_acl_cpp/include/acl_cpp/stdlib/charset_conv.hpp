#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/pipe_stream.hpp"

struct ACL_VSTRING;

namespace acl {

class ACL_CPP_API charset_conv : public pipe_stream
{
public:
	charset_conv();
	~charset_conv();

	/**
	 * �����Ƿ�������Ч���ַ���ֱ�ӿ���
	 * @param onoff {bool} ��Ϊ true ʱ����ת����������������
	 *  �Ƿ��ַ�������ֱ�ӿ�����������������Ĭ���������ֱ�ӿ���
	 */
	void set_add_invalid(bool onoff);

	/**
	 * ת������
	 * @param fromCharset {const char*} Դ�ַ���
	 * @param toCharset {const char*} Ŀ���ַ���
	 * @param in {const char*} �����Դ���ݵ�ַ(�ǿ�)
	 * @param n {size_t} ����Դ���ݵĳ���(>0)
	 * @param out {string*} �洢ת�����
	 * @return {bool} ת���Ƿ�ɹ�
	 */
	bool convert(const char* fromCharset, const char* toCharset,
		const char* in, size_t n, string* out);

	/**
	 * ���ת��ʧ��, �ú������س���ԭ��
	 * @return {const char*} ����ԭ��
	 */
	const char* serror(void) const;

	/**
	 * ����ת��״̬, �ý���������ظ�ʹ��, �����ٴ�ʹ��ǰ��Ҫ����
	 * set(from, to) ����Դ�ַ�����Ŀ���ַ���
	 */
	void reset(void);

	/* ��ʽ�������̣�update_begin->update->update ... ->update_finish */

	/**
	 * ��ʼ����ʽ��������ز���
	 * @param fromCharset {const char*} Դ�ַ���
	 * @param toCharset {const char*} Ŀ���ַ���
	 * @return {bool} ��ʼ���Ƿ�ɹ�
	 */
	bool update_begin(const char* fromCharset, const char* toCharset);

	/**
	 * ����ʽ��ʽ�����ַ���ת��
	 * @param in {const char*} Դ�ַ���
	 * @param len {size_t} in �ַ�������
	 * @param out {string*} �洢ת�����
	 * @return {bool} ��ǰת�������Ƿ�ɹ�
	 */
	bool update(const char* in, size_t len, string* out);

	/**
	 * ��ʽת����������Ҫ���ô˺�����ȡ����ת�����
	 * @param out {string*} �洢ת�����
	 */
	void update_finish(string* out);

	/**
	 * �����ַ���ת����
	 * @param fromCharset {const char*} Դ�ַ���
	 * @param toCharset {const char*} Ŀ���ַ���
	 * @return {charset_conv*} �����������Ƿ�����Դ�ַ���
	 *  ��Ŀ���ַ�����ͬ����֧�������ַ������ת���򷵻�NULL��
	 *  �������Ҫ���� delete ɾ��
	 */
	static charset_conv* create(const char* fromCharset,
	                const char* toCharset);

	// pipe_stream �麯������

	virtual int push_pop(const char* in, size_t len,
		string* out, size_t max = 0);
	virtual int pop_end(string* out, size_t max = 0);
	virtual void clear();

protected:
private:
	bool m_addInvalid;  // ���������Ч���ַ������Ƿ�ֱ�ӿ���
	string  m_errmsg;
	string* m_pBuf;
	char  m_fromCharset[32];
	char  m_toCharset[32];
	void* m_iconv;
	ACL_VSTRING* m_pInBuf;
	ACL_VSTRING* m_pOutBuf;
	const char* m_pUtf8Pre;
};

} // namespace acl
