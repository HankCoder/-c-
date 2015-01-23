#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/snprintf.hpp"
#include "acl_cpp/redis/redis_client.hpp"
#include "acl_cpp/redis/redis_result.hpp"
#include "acl_cpp/redis/redis_hash.hpp"

namespace acl
{

#define INT64_LEN	21
#define FLOAT_LEN	32

redis_hash::redis_hash(redis_client* conn /* = NULL */)
: redis_command(conn)
{

}

redis_hash::~redis_hash()
{
}

/////////////////////////////////////////////////////////////////////////////

bool redis_hash::hmset(const char* key, const std::map<string, string>& attrs)
{
	const string& req = conn_->build("HMSET", key, attrs);
	return conn_->get_status(req);
}

bool redis_hash::hmset(const char* key, const std::map<string, char*>& attrs)
{
	const string& req = conn_->build("HMSET", key, attrs);
	return conn_->get_status(req);
}

bool redis_hash::hmset(const char* key, const std::map<string, const char*>& attrs)
{
	const string& req = conn_->build("HMSET", key, attrs);
	return conn_->get_status(req);
}

bool redis_hash::hmset(const char* key, const std::map<int, string>& attrs)
{
	const string& req = conn_->build("HMSET", key, attrs);
	return conn_->get_status(req);
}

bool redis_hash::hmset(const char* key, const std::map<int, char*>& attrs)
{
	const string& req = conn_->build("HMSET", key, attrs);
	return conn_->get_status(req);
}

bool redis_hash::hmset(const char* key, const std::map<int, const char*>& attrs)
{
	const string& req = conn_->build("HMSET", key, attrs);
	return conn_->get_status(req);
}

/////////////////////////////////////////////////////////////////////////////

bool redis_hash::hmget(const char* key, const std::vector<string>& names,
	std::vector<string>* result /* = NULL */)
{
	const string& req = conn_->build("HMGET", key, names);
	return conn_->get_strings(req, result) >= 0 ? true : false;
}

bool redis_hash::hmget(const char* key, const std::vector<char*>& names,
	std::vector<string>* result /* = NULL */)
{
	const string& req = conn_->build("HMGET", key, names);
	return conn_->get_strings(req, result) >= 0 ? true : false;
}

bool redis_hash::hmget(const char* key, const std::vector<const char*>& names,
	std::vector<string>* result /* = NULL */)
{
	const string& req = conn_->build("HMGET", key, names);
	return conn_->get_strings(req, result) >= 0 ? true : false;
}

bool redis_hash::hmget(const char* key, const std::vector<int>& names,
	std::vector<string>* result /* = NULL */)
{
	const string& req = conn_->build("HMGET", key, names);
	return conn_->get_strings(req, result) >= 0 ? true : false;
}

bool redis_hash::hmget(const char* key, const char* names[], size_t argc,
	std::vector<string>* result /* = NULL */)
{
	const string& req = conn_->build("HMGET", key, names, argc);
	return conn_->get_strings(req, result) >= 0 ? true : false;
}

bool redis_hash::hmget(const char* key, const int names[], size_t argc,
	std::vector<string>* result /* = NULL */)
{
	const string& req = conn_->build("HMGET", key, names, argc);
	return conn_->get_strings(req, result) >= 0 ? true : false;
}

bool redis_hash::hmget(const char* key, const char* names[],
	const size_t lens[], size_t argc, std::vector<string>* result /* = NULL */)
{
	const string& req = conn_->build("HMGET", key, names, lens, argc);
	return conn_->get_strings(req, result) >= 0 ? true : false;
}

const redis_result* redis_hash::hmget_child(size_t i) const
{
	return conn_->get_child(i);
}

const char* redis_hash::hmget_value(size_t i, size_t* len /* = NULL */) const
{
	return conn_->get_value(i, len);
}

size_t redis_hash::hmget_size() const
{
	return conn_->get_size();
}

/////////////////////////////////////////////////////////////////////////////

int redis_hash::hset(const char* key, const char* name, const char* value)
{
	return hset(key, name, value, strlen(value));
}

int redis_hash::hset(const char* key, const char* name,
	const char* value, size_t value_len)
{
	return hset(key, name, strlen(name), value, value_len);
}

int redis_hash::hset(const char* key, const char* name, size_t name_len,
	const char* value, size_t value_len)
{
	const char* argv[4];
	size_t lens[4];

	argv[0] = "HSET";
	lens[0] = sizeof("HSET") - 1;
	argv[1] = key;
	lens[1] = strlen(key);
	argv[2] = name;
	lens[2] = name_len;
	argv[3] = value;
	lens[3] = value_len;

	const string& req = conn_->build_request(4, argv, lens);
	return conn_->get_number(req);
}

int redis_hash::hsetnx(const char* key, const char* name, const char* value)
{
	return hsetnx(key, name, value, strlen(value));
}

int redis_hash::hsetnx(const char* key, const char* name,
		      const char* value, size_t value_len)
{
	return hsetnx(key, name, strlen(name), value, value_len);
}

int redis_hash::hsetnx(const char* key, const char* name, size_t name_len,
	const char* value, size_t value_len)
{
	const char* argv[4];
	size_t lens[4];

	argv[0] = "HSETNX";
	lens[0] = sizeof("HSETNX") - 1;
	argv[1] = key;
	lens[1] = strlen(key);
	argv[2] = name;
	lens[2] = name_len;
	argv[3] = value;
	lens[3] = value_len;

	const string& req = conn_->build_request(4, argv, lens);
	return conn_->get_number(req);
}

bool redis_hash::hget(const char* key, const char* name, string& result)
{
	return hget(key, name, strlen(name), result);
}

bool redis_hash::hget(const char* key, const char* name,
	size_t name_len, string& result)
{
	const char* argv[3];
	size_t lens[3];

	argv[0] = "HGET";
	lens[0] = sizeof("HGET") - 1;
	argv[1] = key;
	lens[1] = strlen(key);
	argv[2] = name;
	lens[2] = name_len;

	const string& req = conn_->build_request(3, argv, lens);
	return conn_->get_string(req, result) >= 0 ? true : false;
}

bool redis_hash::hgetall(const char* key, std::map<string, string>& result)
{
	const char* keys[1];
	keys[0] = key;
	const string& req = conn_->build("HGETALL", NULL, keys, 1);
	return conn_->get_strings(req, result) < 0 ? false : true;
}

bool redis_hash::hgetall(const char* key, std::vector<string>& names,
	std::vector<string>& values)
{
	const char* keys[1];
	keys[0] = key;
	const string& req = conn_->build("HGETALL", NULL, keys, 1);
	return conn_->get_strings(req, names, values) < 0 ? false : true;
}

bool redis_hash::hgetall(const char* key, std::vector<const char*>& names,
	std::vector<const char*>& values)
{
	const char* keys[1];
	keys[0] = key;
	const string& req = conn_->build("HGETALL", NULL, keys, 1);
	return conn_->get_strings(req, names, values) < 0 ? false : true;
}

int redis_hash::hdel(const char* key, const char* first_name, ...)
{
	const char* name;
	std::vector<const char*> names;
	names.push_back(first_name);
	va_list ap;
	va_start(ap, first_name);
	while((name = va_arg(ap, const char*)) != NULL)
		names.push_back(name);
	return hdel(key, names);
}

int redis_hash::hdel(const char* key, const char* names[], size_t argc)
{
	const string& req = conn_->build("HDEL", key, names, argc);
	return conn_->get_number(req);
}

int redis_hash::hdel(const char* key, const char* names[],
	const size_t names_len[], size_t argc)
{
	const string& req = conn_->build("HDEL", key, names, names_len, argc);
	return conn_->get_number(req);;
}

int redis_hash::hdel(const char* key, const std::vector<string>& names)
{
	const string& req = conn_->build("HDEL", key, names);
	return conn_->get_number(req);
}

int redis_hash::hdel(const char* key, const std::vector<char*>& names)
{
	const string& req = conn_->build("HDEL", key, names);
	return conn_->get_number(req);
}

int redis_hash::hdel(const char* key, const std::vector<const char*>& names)
{
	const string& req = conn_->build("HDEL", key, names);
	return conn_->get_number(req);
}

bool redis_hash::hincrby(const char* key, const char* name,
	long long int inc, long long int* result /* = NULL */)
{
	const char* names[1];

	names[0] = name;
	char buf[INT64_LEN];
	(void) acl_i64toa(inc, buf, sizeof(buf));
	const char* values[1];
	values[0] = buf;

	const string& req = conn_->build("HINCRBY", key, names, values, 1);

	bool success;
	if (result != NULL)
		*result = conn_->get_number64(req, &success);
	else
		(void) conn_->get_number64(req, &success);
	return success;
}

bool redis_hash::hincrbyfloat(const char* key, const char* name,
	double inc, double* result /* = NULL */)
{
	const char* names[1];
	const char* values[1];

	names[0] = name;
	char buf[FLOAT_LEN];
	(void) safe_snprintf(buf, sizeof(buf), "%f", inc);
	values[0] = buf;

	const string& req = conn_->build("HINCRBYFLOAT", key, names,
		values, 1);
	if (conn_->get_string(req, buf, sizeof(buf)) == false)
		return false;

	if (result != NULL)
		*result = atof(buf);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool redis_hash::hkeys(const char* key, std::vector<string>& names)
{
	const char* keys[1];
	keys[0] = key;

	const string& req = conn_->build("HKEYS", NULL, keys, 1);
	return conn_->get_strings(req, names) < 0 ? false : true;
}

bool redis_hash::hexists(const char* key, const char* name)
{
	return hexists(key, name, strlen(name));
}

bool redis_hash::hexists(const char* key, const char* name, size_t name_len)
{
	const char* names[1];
	names[0] = name;
	size_t names_len[1];
	names_len[0] = name_len;

	const string& req = conn_->build("HEXISTS", key, names, names_len, 1);
	return conn_->get_number(req) < 0 ? false : true;
}

int redis_hash::hlen(const char* key)
{
	const char* keys[1];
	keys[0] = key;

	const string& req = conn_->build("HLEN", NULL, keys, 1);
	return conn_->get_number(req);
}

int redis_hash::hscan(const char* key, int cursor,
	  std::map<string, string>& out,
	  const char* pattern /* = NULL */, const size_t* count /* = NULL */)
{
	if (key == NULL || *key == 0 || cursor < 0)
		return -1;

	size_t size;
	const redis_result** children = scan_keys("HSCAN", key, cursor,
		size, pattern, count);
	if (children == NULL)
		return cursor;

	if (size % 2 != 0)
		return -1;

	const redis_result* rr;
	string name(128), value(128);

	for (size_t i = 0; i < size;)
	{
		rr = children[i];
		rr->argv_to_string(name);
		i++;

		rr = children[i];
		rr->argv_to_string(value);
		i++;

		out[name] = value;
	}

	return cursor;
}

/////////////////////////////////////////////////////////////////////////////

} // namespace acl
