#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/charset_conv.hpp"
#include "acl_cpp/stream/ofstream.hpp"
#include "acl_cpp/stdlib/xml.hpp"
#include "acl_cpp/mime/mime_define.hpp"
#include "acl_cpp/mime/mime_body.hpp"

namespace acl {

bool mime_body::save_body(pipe_manager& out, const char* src /* = NULL */,
	int len /* = 0 */)
{
	// �Ƿ���Ҫ�� HTML ��������ȡ�����ı�����

	xml* pXml;
	if (m_htmlFirst == false && m_ctype == MIME_CTYPE_TEXT
		&& m_stype == MIME_STYPE_HTML)
	{
		pXml = NEW xml();
		out.push_front(pXml);
	}
	else
		pXml = NULL;

	// ����Ҫ�����ַ���ת��ʱ�����ַ���ת����

	charset_conv* conv;
	if (m_toCharset[0] != 0)
	{
		conv = charset_conv::create(m_charset, m_toCharset);
		if (conv)
			out.push_front(conv);
	}
	else
		conv = NULL;

	bool ret = save(out, src, len); // ���� mime_node::save

	if (pXml)
		delete pXml;
	if (conv)
		delete conv;

	return (ret);
}

bool mime_body::save_body(ostream& out, const char* src /* = NULL */,
	int len /* = 0 */)
{
	pipe_manager manager;
	manager.push_front(&out);

	return (save_body(manager, src, len));
}

bool mime_body::save_body(const char* file_path, const char* src /* = NULL */,
	int len /* = 0 */)
{
	ofstream out;

	if (out.open_trunc(file_path) == false)
	{
		logger_error("open file %s error(%s)",
			file_path, acl_last_serror());
		return (false);
	}

	bool ret = save_body(out, src, len);
	if (ret == false)
	{
#ifdef WIN32
		_unlink(file_path);
#else
		unlink(file_path);
#endif
		return (false);
	}
	return (true);
}

bool mime_body::save_body(pipe_string& out, const char* src /* = NULL */,
	int len /* = 0 */)
{
	pipe_manager manager;
	manager.push_front(&out);
	return (save_body(manager, src, len));
}

bool mime_body::save_body(string& out, const char* src /* = NULL */,
	int len /* = 0 */)
{
	pipe_string ps(out);
	return (save_body(ps, src, len));
}

bool mime_body::html_stype() const
{
	if (m_stype == MIME_STYPE_HTML)
		return (true);
	else
		return (false);
}

} // namespace acl
