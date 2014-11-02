#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"

namespace acl {

/**
 * �� HTTP ͷ�е� Content-Type ��ص��ඨ�壬���Է����������ݣ�
 * Content-Type: application/x-www-form-urlencoded
 * Content-Type: multipart/form-data; boundary=xxx
 * Content-Type: application/octet-stream
 * Content-Type: text/html; charset=utf8
 * Content-Type: xxx/xxx; name=xxx
 * ...
 */
class ACL_CPP_API http_ctype
{
public:
	http_ctype(void);
	~http_ctype(void);

	/**
	 * ���� HTTP ͷ�� Content-Type �ֶ�ֵ
	 * @param cp {const char*} Content-Type �ֶ�ֵ���磺
	 * application/x-www-form-urlencoded
	 * multipart/form-data; boundary=xxx
	 * application/octet-stream
	 * @return {bool} ���������Ƿ�Ϸ�
	 */
	bool parse(const char* cp);

	/**
	 * ��� Content-Type �ֶ�ֵ text/html; charset=utf8 �е� text
	 * @return {const char*} ���� NULL ˵��û�и����ݣ�һ������Ϊ
	 *  parse ʧ�ܵ��µ�
	 */
	const char* get_ctype(void) const;

	/**
	 * ��� Content-Type �ֶ�ֵ text/html; charset=utf8 �е� text
	 * @return {const char*} ���� NULL ˵��û�и�����
	 */
	const char* get_stype(void) const;

	/**
	 * ��� Content-Type �ֶ�ֵ multipart/form-data; boundary=xxx
	 * �е� boundary ��ֵ xxx
	 * @return {const char*} ���� NULL ˵��û�и�����
	 */
	const char* get_bound(void) const;

	/**
	 * ��� Content-Type: xxx/xxx; name=name_xxx
	 * �е� name ��ֵ name_xxx
	 * @return {const char*} ���� NULL ˵��û�и�����
	 */
	const char* get_name(void) const;

	/**
	 * ��� Content-Type �ֶ�ֵ text/html; charset=utf8 �е� utf8
	 * @return {const char*} ���� NULL ˵��û�и�����
	 */
	const char* get_charset(void) const;
protected:
private:
	char* ctype_;
	char* stype_;
	char* name_;
	char* charset_;
	string* bound_;

	void reset(void);
};

} // namespace acl
