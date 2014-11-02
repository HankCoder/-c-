#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/mime/mime_node.hpp"

namespace acl {

class ACL_CPP_API mime_image : public mime_node
{
public:
	mime_image(const char* emailFile, const MIME_NODE* node,
		bool enableDecode = true, const char* toCharset = "gb2312",
		off_t off = 0)
		: mime_node(emailFile, node, enableDecode, toCharset, off)
	{
	}

	~mime_image() {}

	const char* get_location() const;
};

} // namespace acl
