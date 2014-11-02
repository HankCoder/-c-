#include "acl_stdafx.hpp"
#include "internal/mime_state.hpp"
#include "acl_cpp/mime/rfc2047.hpp"
#include "acl_cpp/mime/mime_attach.hpp"

namespace acl {

mime_attach::mime_attach(const char* emailFile, const MIME_NODE* node,
	bool enableDecode /* = true */,
	const char* toCharset /* = "gdb2312" */,
	off_t off /* = 0 */)
: mime_node(emailFile, node, enableDecode, toCharset, off)
, m_filename(128)
{
	if (node->header_filename)
	{
		if (toCharset)
		{
			rfc2047 rfc;
			rfc.decode_update(node->header_filename,
					strlen(node->header_filename));
			rfc.decode_finish(toCharset, &m_filename);
		}
		else
			m_filename = node->header_filename;
	}
}

mime_attach::~mime_attach()
{
}

const char* mime_attach::get_filename() const
{
	if (m_filename.empty())
		return NULL;
	return (m_filename.c_str());
}

} // namespace acl
