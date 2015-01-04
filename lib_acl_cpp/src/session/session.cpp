#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/snprintf.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/escape.hpp"
#include "acl_cpp/session/session.hpp"

namespace acl
{

VBUF* session::vbuf_new(const void* str, size_t len, todo_t todo)
{
	// �������Լ��ٷ����ڴ�Ĵ���
	VBUF* buf = (VBUF*) acl_mymalloc(sizeof(VBUF) + len + 1);
	buf->size = len + 1;
	buf->todo = todo;

	memcpy(buf->buf, str, len);
	// ���뱣֤β���� \0 �������������ַ�������ֵ
	buf->buf[len] = 0;
	buf->len = len;
	return buf;
}

VBUF* session::vbuf_set(VBUF* buf, const void* str, size_t len, todo_t todo)
{
	if (buf == NULL)
	{
		buf = (VBUF*) acl_mymalloc(sizeof(VBUF) + len + 1);
		buf->size = len + 1;
	}
	else if (buf->size <= len)
	{
		buf = (VBUF*) acl_myrealloc(buf, sizeof(VBUF) + len + 1);
		buf->size = len + 1;
	}

	buf->todo = todo;
	memcpy(buf->buf, str, len);
	buf->buf[len] = 0;
	buf->len = len;
	return buf;
}

void session::vbuf_free(VBUF* buf)
{
	acl_myfree(buf);
}

session::session(time_t ttl /* = 0 */, const char* sid /* = NULL */)
: ttl_(ttl)
, dirty_(false)
{
	struct timeval tv;

	(void) gettimeofday(&tv, NULL);
	if (sid == NULL || *sid == 0)
	{
		char buf[128];
		safe_snprintf(buf, sizeof(buf), "acl.%d.%d.%d",
			(int) tv.tv_sec, (int) tv.tv_usec, rand());
		sid_ = vbuf_new(buf, strlen(buf), TODO_NUL);
		sid_saved_ = false;
	}
	else
	{
		sid_ = vbuf_new(sid, strlen(sid), TODO_NUL);
		sid_saved_ = true;
	}
}

session::~session()
{
	reset();
	vbuf_free(sid_);
}

const char* session::get_sid() const
{
	return sid_->buf;
}

void session::set_sid(const char* sid)
{
	sid_ = vbuf_set(sid_, sid, strlen(sid), TODO_NUL);

	// �п����Ѿ��洢�ں�� cache �������
	if (!sid_saved_)
		sid_saved_ = true;

	// ��������ϴε��м���
	reset();
}

void session::reset()
{
	attrs_clear(attrs_);
	attrs_clear(attrs_cache_);
}

void session::attrs_clear(std::map<string, VBUF*>& attrs)
{
	if (attrs.empty())
		return;

	std::map<string, VBUF*>::iterator it = attrs.begin();
	for (; it != attrs.end(); ++it)
		vbuf_free(it->second);
	attrs.clear();
}

bool session::flush()
{
	if (!dirty_)
		return true;
	dirty_ = false;

	string buf(256);

	// ���ô���ӿڣ����ԭ���� sid ����
	if (get_data(sid_->buf, buf) == true)
	{
		if (!sid_saved_)
			sid_saved_ = true;
		deserialize(buf, attrs_);  // �����л�
	}

	std::map<string, VBUF*>::iterator it_cache = attrs_cache_.begin();
	for (; it_cache != attrs_cache_.end(); ++it_cache)
	{
		// ����������Ѵ��ڣ�����Ҫ���ͷ�ԭ��������ֵ���������ֵ

		std::map<string, VBUF*>::iterator it_attr =
			attrs_.find(it_cache->first);
		if (it_attr == attrs_.end())
		{
			if (it_cache->second->todo == TODO_SET)
				attrs_[it_cache->first] = it_cache->second;
			else
				vbuf_free(it_cache->second);
		}
		else if (it_cache->second->todo == TODO_SET)
		{
			// ����ɵ�����
			vbuf_free(it_attr->second);
			// �����µ�����
			attrs_[it_cache->first] = it_cache->second;
		}
		else if (it_cache->second->todo == TODO_DEL)
		{
			vbuf_free(it_attr->second);
			attrs_.erase(it_attr);
			vbuf_free(it_cache->second);
		}
		else
		{
			logger_warn("unknown todo(%d)",
				(int) it_cache->second->todo);
			vbuf_free(it_cache->second);
		}
	}

	// �����������ݣ���Ϊ�ڲ��������Ѿ�������� attrs_ �У�
	// ����ֻ��Ҫ�� attrs_cache_ �ռ��������
	attrs_cache_.clear();

	serialize(attrs_, buf);  // ���л�����
	attrs_clear(attrs_);  // ������Լ�������

	// ���ô���ӿڣ��� memcached �����ƻ������������
	if (set_data(sid_->buf, buf.c_str(), buf.length(), ttl_) == false)
	{
		logger_error("set cache error, sid(%s)", sid_->buf);
		return false;
	}

	if (!sid_saved_)
		sid_saved_ = true;
	return true;
}

bool session::set(const char* name, const char* value,
	bool delay /* = false */)
{
	return set(name, value, strlen(value), delay);
}

bool session::set(const char* name, const void* value, size_t len,
	bool delay /* = false */)
{
	if (delay)
	{
		std::map<string, VBUF*>::iterator it = attrs_cache_.find(name);
		if (it == attrs_cache_.end())
			attrs_cache_[name] = vbuf_new(value, len, TODO_SET);
		else
			attrs_cache_[name] = vbuf_set(it->second, value,
							len, TODO_SET);
		dirty_ = true;
		return true;
	}

	// ֱ�Ӳ������ cache ������������(���/�޸�) �����ֶ�

	string buf(256);

	// ���ô���ӿڣ����ԭ���� sid ����
	if (get_data(sid_->buf, buf) == false)
	{
		// ���û���򴴽��µ� sid ����
		serialize(name, value, len, buf);
	}

	// ������ڶ�Ӧ sid �����ݣ��������������ԭ��������
	else
	{
		if (!sid_saved_)
			sid_saved_ = true;

		// �����л�
		deserialize(buf, attrs_);

		// ����������Ѵ��ڣ�����Ҫ���ͷ�ԭ��������ֵ���������ֵ

		std::map<string, VBUF*>::iterator it = attrs_.find(name);
		if (it == attrs_.end())
			attrs_[name] = vbuf_new(value, len, TODO_SET);
		else
			attrs_[name] = vbuf_set(it->second, value,
						len, TODO_SET);
		serialize(attrs_, buf);  // ���л�����
		attrs_clear(attrs_);
	}

	// ���ô���ӿڣ��� memcached �����ƻ������������
	if (set_data(sid_->buf, buf.c_str(), buf.length(), ttl_) == false)
	{
		logger_error("set cache error, sid(%s)", sid_->buf);
		return false;
	}
	if (!sid_saved_)
		sid_saved_ = true;
	return true;
}

const char* session::get(const char* name, bool local_cached /* = false */)
{
	const VBUF* bf = get_vbuf(name, local_cached);
	if (bf == NULL)
		return "";
	return bf->buf;
}

const VBUF* session::get_vbuf(const char* name, bool local_cached /* = false */)
{
	string buf(256);
	if (local_cached == false || attrs_.empty())
	{
		if (get_data(sid_->buf, buf) == false)
			return NULL;
		deserialize(buf, attrs_);
	}
	std::map<string, VBUF*>::const_iterator cit = attrs_.find(name);
	if (cit == attrs_.end())
		return NULL;
	return cit->second;
}

bool session::set_ttl(time_t ttl, bool delay /* = true */)
{
	if (ttl == ttl_)
		return true;

	// ������ӳ��޸ģ����������س�Ա���������ͳһ flush
	else if (delay)
	{
		ttl_ = ttl;
		dirty_ = true;
		return true;
	}

	// ����� sid ��û���ں�� cache �ϴ洢��������ڶ����б�������һ��
	else if (!sid_saved_)
	{
		ttl_ = ttl;
		return true;
	}

	// �޸ĺ�� cache ����Ը� sid �� ttl
	else if (set_timeout(sid_->buf, ttl) == true)
	{
		ttl_ = ttl;
		return true;
	}
	else
		return false;
}

time_t session::get_ttl() const
{
	return ttl_;
}

bool session::del(const char* name, bool delay /* = false */)
{
	if (delay)
	{
		std::map<string, VBUF*>::iterator it = attrs_cache_.find(name);
		if (it == attrs_cache_.end())
			attrs_cache_[name] = vbuf_new("", 0, TODO_DEL);
		else
			it->second->todo = TODO_DEL;
		dirty_ = true;
		return true;
	}

	// ֱ�Ӳ������ cache ��������ɾ�������ֶ�

	string buf(256);
	if (get_data(sid_->buf, buf) == false)
		return true;

	deserialize(buf, attrs_);
	std::map<string, VBUF*>::iterator it = attrs_.find(name);
	if (it == attrs_.end())
		return false;

	// ��ɾ�����ͷŶ�Ӧ�Ķ���
	vbuf_free(it->second);
	attrs_.erase(it);

	// ��� sid ���Ѿ�û�������ݣ���Ӧ�ý� sid ����� memcached ��ɾ��
	if (attrs_.empty())
	{
		// �����麯����ɾ���� sid ��Ӧ�Ļ�������
		if (del_data(sid_->buf) == false)
		{
			logger_error("del sid(%s) error", sid_->buf);
			return false;
		}
		return true;
	}

	// �� memcached ���������ʣ�������

	serialize(attrs_, buf);
	attrs_clear(attrs_);

	if (set_data(sid_->buf, buf.c_str(), buf.length(), ttl_) == false)
	{
		logger_error("set cache error, sid(%s)", sid_->buf);
		return false;
	}
	return true;
}

bool session::remove()
{
	// �����麯����ɾ���������
	if (del_data(sid_->buf) == false)
	{
		logger_error("invalid sid(%s) error", sid_->buf);
		return false;
	}
	return true;
}

// ���� handlersocket �ı��뷽ʽ

void session::serialize(const std::map<string, VBUF*>& attrs, string& out)
{
	out.clear(); // �����������

	std::map<string, VBUF*>::const_iterator it = attrs.begin();
	if (it == attrs.end())
		return;

	// ��ӵ�һ������
	const char ch = 1;
	escape(it->first.c_str(), it->first.length(), out);
	escape(&ch, 1, out);
	escape(it->second->buf, it->second->len, out);
	++it;

	// ��Ӻ���������
	for (; it != attrs.end(); ++it)
	{
		// ����һ������������Ķ���Ҫ��ӷָ���
		out << '\t';
		escape(it->first.c_str(), it->first.length(), out);
		escape(&ch, 1, out);
		escape(it->second->buf, it->second->len, out);
	}
}

void session::serialize(const char* name, const void* value,
	size_t len, string& out)
{
	escape(name, strlen(name), out);
	const char ch = 1;
	escape(&ch, 1, out);
	escape((const char*) value, len, out);
}

// ���� handlersocket �Ľ��뷽ʽ

void session::deserialize(string& buf, std::map<string, VBUF*>& attrs)
{
	attrs_clear(attrs);  // ������ session ǰһ�β�ѯ״̬

	ACL_ARGV* tokens = acl_argv_split(buf.c_str(), "\t");
	ACL_ITER  iter;
	acl_foreach(iter, tokens)
	{
		char* ptr = (char*) iter.data;

		// �ظ�ʹ��ԭ�����ڴ�������Ϊ tokens ���Ѿ��洢���м�������
		buf.clear();
		if (unescape(ptr, strlen(ptr), buf) == false)
		{
			logger_error("unescape error");
			continue;
		}
		ptr = buf.c_str();
		// ��Ϊ acl::string �϶��ܱ�֤���������ݵ�β���� \0����������
		// strchr ʱ�����뵣��Խ�����⣬�� std::string ������֤����
		char* p1 = strchr(ptr, 1);
		if (p1 == NULL || *(p1 + 1) == 0)
			continue;
		*p1++ = 0;
		std::map<string, VBUF*>::iterator it = attrs.find(ptr);

		// xxx: �Է����ظ�������
		if (it != attrs.end())
			vbuf_free(it->second);
		// ���Ӻ��ȡ���������Զ���Ϊ TODO_SET
		attrs[ptr] = vbuf_new(p1, buf.length() - (p1 - buf.c_str()),
				TODO_SET);
	}

	acl_argv_free(tokens);
}

} // namespace acl
