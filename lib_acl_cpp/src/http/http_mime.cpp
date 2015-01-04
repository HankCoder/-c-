#include "acl_stdafx.hpp"
#include "../mime/internal/mime_state.hpp"
#include "../mime/internal/header_opts.hpp"
#include "acl_cpp/stdlib/snprintf.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stdlib/charset_conv.hpp"
#include "acl_cpp/stream/ifstream.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/http/http_mime.hpp"

namespace acl
{

//////////////////////////////////////////////////////////////////////////

http_mime_node::http_mime_node(const char* path, const MIME_NODE* node,
	bool decodeIt /* = true */, const char* toCharset /* =  */,
	off_t off /* = 0 */)
: mime_attach(path, node, decodeIt, toCharset, off)
{
	param_value_ = NULL;

	if (get_filename() == NULL)
	{
		mime_type_ = HTTP_MIME_PARAM;
		const char* name = get_name();
		if (path && name)
			load_param(path);
	}
	else
		mime_type_ = HTTP_MIME_FILE;
}

http_mime_node::~http_mime_node()
{
	if (param_value_)
		acl_myfree(param_value_);
}

http_mime_t http_mime_node::get_mime_type() const
{
	return mime_type_;
}

void http_mime_node::load_param(const char* path)
{
	ifstream in;
	if (in.open_read(path) == false)
	{
		logger_error("open file %s error(%s)", path, last_serror());
		return;
	}
	off_t begin = get_bodyBegin();
	off_t end = get_bodyEnd();
	if (begin < 0 || end < 0 || begin >= end)
	{
		logger_error("invalid file offset, begin: %d, end: %d",
			(int) begin, (int) end);
		return;
	}

	if (in.fseek(begin, SEEK_SET) == -1)
	{
		logger_error("fseek file %s error(%s), begin: %d",
			path, last_serror(), (int) begin);
		return;
	}

	size_t len = end - begin;
	char* buf = (char*) acl_mymalloc(len + 1);
	if (in.read(buf, len) == -1)
	{
		acl_myfree(buf);
		logger_error("read file %s error(%s)", path, last_serror());
		return;
	}
	buf[len] = 0;
	char* ptr = buf + len - 1;
	while (ptr >= buf)
	{
		if (*ptr == '\r' || *ptr == '\n'
			|| *ptr == ' ' || *ptr == '\t')
		{
			*ptr-- = 0;
			continue;
		}
		break;
	}
	if (*buf == 0)
	{
		acl_myfree(buf);
		return;
	}

	char* value = acl_url_decode(buf);
	if (value == NULL)
		value = buf;
	else
		acl_myfree(buf);

	const char* fromCharset = get_charset();
	const char* toCharset = get_toCharset();
	if (fromCharset && *fromCharset && toCharset && *toCharset
		&& strcasecmp(fromCharset, toCharset))
	{
		charset_conv conv;
		string tmp;
		if (conv.convert(fromCharset, toCharset,
			value, strlen(value), &tmp) == true)
		{
			param_value_ = acl_mystrdup(tmp.c_str());
			acl_myfree(value);
		}
		else
			param_value_ = value;
	}
	else
		param_value_ = value;
}

const char* http_mime_node::get_value() const
{
	return param_value_;
}

//////////////////////////////////////////////////////////////////////////

http_mime::http_mime(const char* boundary,
	const char* local_charset /* = "gb2312" */)
{
	static const char ctype_pre[] =
		"Content-Type: multipart/form-data; boundary=";

	if (boundary == NULL || strlen(boundary) < 2)
	{
		logger_error("boundary invalid");
		boundary = NULL;
		mime_state_ = NULL;
		save_path_ = NULL;
		return;
	}
	// HTTP �� MIME ��ʽ�� �ʼ��� MIME �ķָ���������ͬ��
	// HTTP �ķָ���Ҫ���ʼ��ָ��������� '-'����Ŀǰ HTTP
	// �� MIME �������õ����ʼ��� MIME ��������Ŀǰ�ʼ���
	// MIME ���������Զ��������� '-' ǰ׺��������Ҫȥ��
	// ��ͷ������ '-'
	if (*boundary == '-')
		boundary++;
	if (*boundary == '-')
		boundary++;
	boundary_ = acl_mystrdup(boundary);

	if (local_charset && *local_charset)
		safe_snprintf(local_charset_, sizeof(local_charset_),
			"%s", local_charset);
	else
		local_charset_[0] = 0;

	decode_on_ = true;

	save_path_ = NULL;
	mime_state_ = mime_state_alloc();

	// Ϊ��ʹ���ʼ��� mime ����������Ҫģ���һ��ͷ���ֶ�
	mime_state_update(mime_state_, ctype_pre, sizeof(ctype_pre) - 1);
	size_t len = strlen(boundary);
	mime_state_update(mime_state_, boundary, len);
	mime_state_update(mime_state_, "\r\n\r\n", 4);

	// ��Ϊ��ͷ��Ϊ����������ͷ�Ƕ���ӽ�ȥ�ģ����Ի����ʵ�ʵ�ƫ������
	// ͨ�� off_ ������ƫ��������
	off_ = 0 - (sizeof(ctype_pre) - 1 + len + 4);

	parsed_ = false;
}

http_mime::~http_mime()
{
	if (boundary_)
		acl_myfree(boundary_);
	if (save_path_)
		acl_myfree(save_path_);
	if (mime_state_)
		mime_state_free(mime_state_);
	std::list<http_mime_node*>::iterator it = mime_nodes_.begin();
	for (; it != mime_nodes_.end(); ++it)
		delete *it;
}

void http_mime::set_saved_path(const char* path)
{
	if (path && *path)
		save_path_ = acl_mystrdup(path);
}

bool http_mime::update(const char* data, size_t len)
{
	return mime_state_update(mime_state_, data, len) == 1 ? true : false;
}

const std::list<http_mime_node*>& http_mime::get_nodes(void) const
{
	if (parsed_)
		return mime_nodes_;

	// �����û�з��������͵��ñ���������ֱ�ӷ��ؿյļ���
	if (mime_state_->curr_status != MIME_S_TERM)
		return mime_nodes_;

	const_cast<http_mime*>(this)->parsed_ = true;

	ACL_ITER iter;
	MIME_NODE* node;
	int  i = 0;
	acl_foreach(iter, mime_state_)
	{
		// ÿһ���ڵ�����ͷ��㣬��������
		if (i++ == 0)
			continue;
		node = (MIME_NODE*) iter.data;
		const_cast<http_mime*>(this)->mime_nodes_.push_back(
			NEW http_mime_node(save_path_, node,
			decode_on_, local_charset_, off_));
	}

	return mime_nodes_;
}

const http_mime_node* http_mime::get_node(const char* name) const
{
	get_nodes();

	const char* ptr;
	std::list<http_mime_node*>::const_iterator cit = mime_nodes_.begin();
	for (; cit != mime_nodes_.end(); ++cit)
	{
		ptr = (*cit)->get_name();
		if (ptr && strcmp(ptr, name) == 0)
			return *cit;
	}

	return NULL;
}

} // namespace acl
