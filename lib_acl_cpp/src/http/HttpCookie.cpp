#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/snprintf.hpp"
#include "acl_cpp/http/http_header.hpp"
#include "acl_cpp/http/HttpCookie.hpp"

namespace acl
{

HttpCookie::HttpCookie(const char* name, const char* value)
{
	acl_assert(name && *name && value);
	name_ = acl_mystrdup(name);
	value_ = acl_mystrdup(value);
	dummy_[0] = 0;
}

HttpCookie::HttpCookie(void)
{
	name_ = NULL;
	value_ = NULL;
	dummy_[0] = 0;
}

HttpCookie::~HttpCookie(void)
{
	if (name_)
		acl_myfree(name_);
	if (value_)
		acl_myfree(value_);

	std::list<HTTP_PARAM*>::iterator it = params_.begin();
	for (; it != params_.end(); ++it)
	{
		acl_myfree((*it)->name);
		acl_myfree((*it)->value);
		acl_myfree(*it);
	}
}

void HttpCookie::destroy(void)
{
	delete this;
}

bool HttpCookie::splitNameValue(char* data, HTTP_PARAM* param)
{
#define SKIP_SPECIAL(x) { while (*(x) == ' ' || *(x) == '\t' || *(x) == '=') (x)++; }
#define SKIP_WHILE(cond, x) { while(*(x) && (cond)) (x)++; }

	// ��ʼ��������
	param->name = data;

	// ȥ����ͷ���õ������ַ�
	SKIP_SPECIAL(param->name);
	if (*(param->name) == 0)
		return false;

	// �ҵ� '='
	param->value = param->name;
	SKIP_WHILE(*(param->value) != '=', param->value);
	if (*(param->value) != '=')
		return false;

	// ȥ�� '=' ǰ��Ŀո�
	char* ptr = param->value - 1;
	*param->value++ = 0;
	while (ptr > param->name && (*ptr == ' ' || *ptr == '\t'))
		*ptr-- = 0;

	// ȥ�� value ��ʼ����Ч�ַ�
	SKIP_SPECIAL(param->value);

	// �ҵ� value ֵ�Ľ���λ��
	// ���� value = "\0"
	ptr = param->value + strlen(param->value) - 1;
	while (ptr >= param->value && (*ptr == ' ' || *ptr == '\t'))
		*ptr-- = 0;

	return true;
}

// value ��ʽ��xxx=xxx; domain=xxx; expires=xxx; path=xxx
bool HttpCookie::setCookie(const char* value)
{
	if (value == NULL || *value == 0)
		return false;

	HTTP_PARAM param;
	ACL_ARGV* tokens = acl_argv_split(value, ";");
	acl_assert(tokens->argc > 0);

	// �ӵ�һ�� name=value �ֶ���ȡ�� cookie ���� cookie ֵ
	if (splitNameValue(tokens->argv[0], &param) == false)
	{
		acl_argv_free(tokens);
		return false;
	}
	// name �϶��� "\0"���� value ����Ϊ "\0"
	name_ = acl_mystrdup(param.name);
	value_ = acl_mystrdup(param.value);

	for (int i = 1; i < tokens->argc; i++)
	{
		if (splitNameValue(tokens->argv[i], &param) == false)
			continue;
		if (*(param.value) == 0)
			continue;
		if (strcasecmp(param.name, "domain") == 0)
			setDomain(param.value);
		else if (strcasecmp(param.name, "expires") == 0)
			setExpires(param.value);
		else if (strcasecmp(param.name, "path") == 0)
			setPath(param.value);
		else
			add(param.name, param.value);
	}

	acl_argv_free(tokens);
	return true;
}

HttpCookie& HttpCookie::setDomain(const char* domain)
{
	add("Domain", domain);
	return *this;
}

HttpCookie& HttpCookie::setPath(const char* path)
{
	add("Path", path);
	return *this;
}

HttpCookie& HttpCookie::setExpires(time_t timeout)
{
	if (timeout > 0)
	{
		time_t n = time(NULL);
		n += timeout;

		char buf[64];
		http_header::date_format(buf, sizeof(buf), n);
		add("Expires", buf);
	}
	return *this;
}

HttpCookie& HttpCookie::setExpires(const char* expires)
{
	if (expires && *expires)
		add("Expires", expires);
	return *this;
}

HttpCookie& HttpCookie::setMaxAge(int max_age)
{
	char tmp[20];
	safe_snprintf(tmp, sizeof(tmp), "%d", max_age);
	add("Max-Age", tmp);
	return *this;
}

HttpCookie& HttpCookie::add(const char* name, const char* value)
{
	if (name == NULL || *name == 0 || value == NULL)
		return *this;

	HTTP_PARAM* param = (HTTP_PARAM*) acl_mymalloc(sizeof(HTTP_PARAM));
	param->name = acl_mystrdup(name);
	param->value = acl_mystrdup(value);
	params_.push_back(param);
	return *this;
}

const char* HttpCookie::getName(void) const
{
	if (name_ == NULL)
		return dummy_;
	return name_;
}

const char* HttpCookie::getValue(void) const
{
	if (value_ == NULL)
		return dummy_;
	return value_;
}

const char* HttpCookie::getExpires(void) const
{
	return getParam("Expires");
}

const char* HttpCookie::getDomain(void) const
{
	return getParam("Domain");
}

const char* HttpCookie::getPath(void) const
{
	return getParam("Path");
}

int HttpCookie::getMaxAge(void) const
{
	const char* ptr = getParam("Max-Age");
	if (ptr == NULL || *ptr == 0)
		return -1;
	return atoi(ptr);
}

const char* HttpCookie::getParam(const char* name,
	bool case_insensitive /* = true */) const
{
	std::list<HTTP_PARAM*>::const_iterator cit = params_.begin();
	for (; cit != params_.end(); ++cit)
	{
		if (case_insensitive)
		{
			if (strcasecmp((*cit)->name, name) == 0)
				return (*cit)->value;
		}
		else if (strcasecmp((*cit)->name, name) == 0)
			return (*cit)->value;
	}
	return dummy_;
}

const std::list<HTTP_PARAM*>& HttpCookie::getParams(void) const
{
	return params_;
}

} // namespace acl end
