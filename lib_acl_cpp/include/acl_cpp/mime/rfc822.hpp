#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#include <time.h>

namespace acl {

/**
 * �ʼ���ַ
 */
struct rfc822_addr
{
	char* addr;	// �ʼ���ַ����ʽΪ�� xxx@xxx.xxx
	char* comment;	// �ʼ���ע
};

typedef enum
{
	tzone_gmt,
	tzone_cst
} tzone_t;

class ACL_CPP_API rfc822
{
public:
	rfc822();
	~rfc822();

	/**
	 * �������� RFC822 ��׼��ʱ���ʽ
	 * @param in {const char*} ʱ���ַ������磺
	 *  Wed, 11 May 2011 09:44:37 +0800 (CST)
	 *  Wed, 11 May 2011 16:17:39 GMT
	 */
	time_t parse_date(const char *in);

	/**
	 * ���ɷ��� RFC822 ��׼��ʱ���ʽ
	 * @param t {time_t}
	 * @param out {char*} �洢ת�����
	 * @param size {size_t} out �ռ��С
	 * @param zone {tzone_t} ����ʱ��
	 */
	void mkdate(time_t t, char* out, size_t size, tzone_t  zone = tzone_cst);

	/**
	 * ���ɶ�������ʱ���ʽ
	 * @param t {time_t}
	 * @param out {char*} �洢ת�����
	 * @param size {size_t} out �ռ��С
	 */
	void mkdate_cst(time_t t, char* out, size_t size);

	/**
	 * ���ɸ�������ʱ���ʱ���ʽ
	 * @param t {time_t}
	 * @param out {char*} �洢ת�����
	 * @param size {size_t} out �ռ��С
	 */
	void mkdate_gmt(time_t t, char* out, size_t size);

	/**
	 * �����ʼ���ַ�б������� RFC822 ��׼���ʼ���ַ�б������
	 * ���������������ʼ���ַ�б�ͬʱ���û���ע�Ͳ��ֽ���
	 * RFC2047����
	 * @param in {const char*} RFC822 ��ʽ���ʼ���ַ�б���:
	 *  "=?gb2312?B?1dSx+A==?= <zhaobing@51iker.com>;\r\n"
	 *  "\t\"=?GB2312?B?t+vBosn6?=\" <fenglisheng@51iker.com>;\r\n"
	 *  "\t\"zhengshuxin3\";\"zhengshuxin4\" <zhengshuxin2@51iker.com>;"
	 *  "<xuganghui@51iker.com>;<wangwenquan@51iker.com>;"
	 * @return {const std::list<rfc822_addr*>&} �������
	 */
	const std::list<rfc822_addr*>& parse_addrs(const char* in);

	/**
	 * ����һ������ RFC822 ��׼���ʼ���ַ��ͬʱ���û���ע�Ͳ��ְ�
	 * RFC2047 ��׼���н���
	 * @param in {const char*} RFC822 ��ʽ���ʼ���ַ
	 * @return {const rfc822_addr*} ���� NULL ����������ʼ���ַ������
	 *  RFC822 �淶
	 */
	const rfc822_addr* parse_addr(const char* in);

	/**
	 * ����ʼ���ַ�Ƿ�Ϸ�
	 * @param in {const char*} RFC822 ��ʽ���ʼ���ַ
	 * @return {bool}
	 */
	bool check_addr(const char* in);

private:
	std::list<rfc822_addr*> addrs_;

	void reset();
};

} // namespace acl
