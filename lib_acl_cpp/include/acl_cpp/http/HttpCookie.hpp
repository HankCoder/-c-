#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#include "acl_cpp/http/http_type.hpp"

namespace acl {

/**
 * http Э��ͷ�� cookie ������
 */
class ACL_CPP_API HttpCookie
{
public:
	/**
	 * ���캯�������������붯̬�������ұ������ destroy ���ͷ������
	 * @param name {const char*} cookie ����Ϊ�ǿ��ַ������ַ������� > 0
	 * @param value {const char*} cookie ֵ��ָ��ǿգ��ַ������ȿ���Ϊ 0
	 * ע�������������������������������ڲ������������
	 */
	HttpCookie(const char* name, const char* value);

	/**
	 * ��ʹ�øù��캯��ʱ������ʹ�� setCookie ����� cookie ��
	 */
	HttpCookie(void);

	/**
	 * ���� Set-Cookie: xxx=xxx; domain=xxx; expires=xxx; path=xxx; max-age=xxx; ...
	 * ������ݽ��з���
	 * @param value {const char*} ������ xxx=xxx; domain=xxx; ... ����
	 * @return {bool} ����������Ƿ�Ϸ�
	 */
	bool setCookie(const char* value);

	/**
	 * ��̬�����������ͨ���˺����ͷ�
	 */
	void destroy();

	/**
	 * ���� cookie ��������
	 * @param domain {const char*} cookie ������
	 * @return {HttpCookie&} ���ر���������ã������û���������
	 */
	HttpCookie& setDomain(const char* domain);

	/**
	 * ���� cookie �� path �ֶ�
	 * @param path {const char*} path �ֶ�ֵ
	 * @return {HttpCookie&} ���ر���������ã������û���������
	 */
	HttpCookie& setPath(const char* path);

	/**
	 * ���� cookie �Ĺ���ʱ��Σ����õ�ǰʱ��������ʱ�伴Ϊ cookie
	 * �Ĺ���ʱ��
	 * @param timeout {time_t} ����ʱ��ֵ(��λΪ��)����ǰʱ��Ӹ�ʱ��
	 * �� cookie �Ĺ���ʱ��
	 * @return {HttpCookie&} ���ر���������ã������û���������
	 */
	HttpCookie& setExpires(time_t timeout);

	/**
	 * ���� cookie �Ĺ���ʱ����ַ���
	 * @param expires {const char*} ����ʱ���
	 * @return {HttpCookie&} ���ر���������ã������û���������
	 */
	HttpCookie& setExpires(const char* expires);

	/**
	 * ���� cookie ����������
	 * @param max_age {int} ��������
	 * @return {HttpCookie&} ���ر���������ã������û���������
	 */
	HttpCookie& setMaxAge(int max_age);

	/**
	 * ������ cookie ������������ֵ
	 * @param name {const char*} ������
	 * @param value {const char*} ����ֵ
	 * @return {HttpCookie&} ���ر���������ã������û���������
	 */
	HttpCookie& add(const char* name, const char* value);

	/**
	 * ��� cookie ���ƣ�ȡ���ڹ�����������ֵ
	 * @return {const char*} Ϊ���ȴ��� 0 ���ַ�������Զ�ǿ�ָ��
	 * ע���û������ڵ��� HttpCookie(const char*, const char*) ����
	 *     ����� setCookie(const char*) �ɹ���ſ��Ե��øú�����
	 *     ���򷵻ص������� "\0"
	 */
	const char* getName(void) const;

	/**
	 * ��� cookie ֵ��ȡ���ڹ��캯������ֵ
	 * @return {const char*} �ǿ�ָ�룬�п����ǿ��ַ���("\0")
	 */
	const char* getValue(void) const;

	/**
	 * ����ַ�����ʽ�Ĺ���ʱ��
	 * @return {const char*} �ǿ�ָ�룬����ֵΪ "\0" ��ʾ������
	 */
	const char* getExpires(void) const;

	/**
	 * ��� cookie ������
	 * @return {const char*} �ǿ�ָ�룬����ֵΪ "\0" ��ʾ������
	 */
	const char* getDomain(void) const;

	/**
	 * ��� cookie �Ĵ洢·��
	 * @return {const char*} �ǿ�ָ�룬����ֵΪ "\0" ��ʾ������
	 */
	const char* getPath(void) const;

	/**
	 * ��� cookie ����������
	 * @return {int} ���� -1 ʱ��ʾû�и� Max-Age �ֶ�
	 */
	int  getMaxAge(void) const;

	/**
	 * ��ö�Ӧ�������Ĳ���ֵ
	 * @param name {const char*} ������
	 * @param case_insensitive {bool} �Ƿ����ִ�Сд��true ��ʾ
	 *  �����ִ�Сд
	 * @return {const char*} �ǿ�ָ�룬����ֵΪ "\0" ��ʾ������
	 */
	const char* getParam(const char* name,
		bool case_insensitive = true) const;

	/**
	 * ��ø� cookie ����ĳ� cookie ���� cookie ֵ֮���
	 * �������Լ�����ֵ
	 * @return {const std::list<HTTP_PARAM*>&}
	 */
	const std::list<HTTP_PARAM*>& getParams(void) const;
private:
	char  dummy_[1];
	char* name_;
	char* value_;
	std::list<HTTP_PARAM*> params_;

	~HttpCookie(void);
	bool splitNameValue(char* data, HTTP_PARAM* param);
};

} // namespace acl end
