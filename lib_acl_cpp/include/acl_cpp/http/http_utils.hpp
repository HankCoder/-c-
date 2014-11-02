#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

class ACL_CPP_API http_utils
{
public:
	http_utils() {}
	~http_utils() {}

	/**
	 * �������� url �л�� WEB ��������ַ����ʽ��domain:port
	 * @param url {const char*} HTTP url���ǿ�
	 * @param addr {char*} �洢������洢��ʽ��domain:port
	 * @param size {size_t} out ��������С
	 * @return {bool} �Ƿ�ɹ����
	 */
	static bool get_addr(const char* url, char* addr, size_t size);

	/**
	 * �������� url �л�� WEB ������ IP ��ַ���˿ں�
	 * @param url {const char*} HTTP url���ǿ�
	 * @param domain {char*} �洢����
	 * @param size {size_t} domain �ڴ��С
	 * @param port {unsigned short*} �洢�˿ںŴ�С
	 * @return {bool} �Ƿ�ɹ����
	 */
	static bool get_addr(const char* url, char* domain, size_t size,
		unsigned short* port);
};

} // namespace acl
