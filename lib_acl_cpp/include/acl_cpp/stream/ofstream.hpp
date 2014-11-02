#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/fstream.hpp"

namespace acl {

class ACL_CPP_API ofstream : public fstream
{
public:
	ofstream();
	virtual ~ofstream();

	/**
	 * ��ֻд��ʽ���ļ�������ļ��������򴴽����ļ�������ļ�
	 * ���ڣ����ļ��������
	 * @param path {const char*} �ļ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool open_write(const char* path);

	/**
	 * ��β����ӷ�ʽ���ļ�������ļ��������򴴽����ļ�
	 * @param path {const char*} �ļ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool open_append(const char* path);
};

} // namespace acl
