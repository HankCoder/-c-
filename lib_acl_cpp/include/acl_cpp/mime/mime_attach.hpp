#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/mime/mime_node.hpp"

namespace acl {

class ACL_CPP_API mime_attach : public mime_node
{
public:
	mime_attach(const char* emailFile, const MIME_NODE* node,
		bool enableDecode = true, const char* toCharset = "gb2312",
		off_t off = 0);

	virtual ~mime_attach();

	/**
	 * ��ø������ļ���
	 * @return {const char*} ����ֵΪ NULL ��˵��û���ҵ��ļ���
	 */
	const char* get_filename() const;

private:
	string m_filename;
};

} // namespace acl
