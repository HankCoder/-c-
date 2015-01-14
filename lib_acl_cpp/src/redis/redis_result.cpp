#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/stdlib/dbuf_pool.hpp"
#include "acl_cpp/redis/redis_result.hpp"

namespace acl
{

redis_result::redis_result(dbuf_pool* pool)
: result_type_(REDIS_RESULT_NIL)
, pool_(pool)
, size_(0)
, idx_(0)
, argv_(NULL)
, lens_(NULL)
{
	acl_assert(pool_ != NULL);
}

redis_result::~redis_result()
{
}

void *redis_result::operator new(size_t size, dbuf_pool* pool)
{
	return pool->dbuf_alloc(size);
}

void redis_result::operator delete(void* ptr acl_unused,
	dbuf_pool* pool acl_unused)
{
	logger_error("DELETE NOW!");
}

redis_result& redis_result::set_size(size_t size)
{
	if (idx_ > 0)
	{
		logger_error("set size when putting, idx_: %d", (int) idx_);
		return *this;
	}
	size_ = size;
	return *this;
}

redis_result& redis_result::set_type(redis_result_t type)
{
	result_type_ = type;
	return *this;
}

redis_result& redis_result::put(const char* buf, size_t len)
{
	if (size_ == 0)
	{
		logger_error("size_ is 0, call set_size first!");
		return *this;
	}
	if (idx_ >= size_)
	{
		logger_error("overflow, idx_(%d) >= size_(%d)",
			(int) idx_, (int) size_);
		return *this;
	}
	if (argv_ == NULL)
	{
		argv_ = (const char**) pool_->dbuf_alloc(sizeof(char*) * size_);
		lens_ = (size_t*) pool_->dbuf_alloc(sizeof(size_t) * size_);
	}

	argv_[idx_] = buf;
	lens_[idx_] = len;
	idx_++;

	return *this;
}

size_t redis_result::get_size() const
{
	if (result_type_ == REDIS_RESULT_ARRAY)
		return children_.size();
	else if (result_type_ == REDIS_RESULT_STRING)
	{
		if (argv_ == NULL || lens_ == NULL)
			return 0;
		return size_;
	}
	else
		return size_;
}

int redis_result::get_integer(bool* success /* = NULL */) const
{
	if (success)
		*success = false;
	if (result_type_ != REDIS_RESULT_INTEGER)
		return -1;
	const char* ptr = get(0);
	if (ptr == NULL)
		return -1;
	if (success)
		*success = true;
	return atoi(ptr);
}

long long int redis_result::get_integer64(bool* success /* = NULL */) const
{
	if (success)
		*success = false;
	if (result_type_ != REDIS_RESULT_INTEGER)
		return -1;
	const char* ptr = get(0);
	if (ptr == NULL)
		return -1;
	if (success)
		*success = true;
	return acl_atoi64(ptr);
}

const char* redis_result::get_status() const
{
	if (result_type_ != REDIS_RESULT_STATUS)
		return NULL;
	return get(0);
}

const char* redis_result::get(size_t i, size_t* len /* = NULL */) const
{
	if (i >= idx_)
	{
		if (len)
			*len = 0;
		return NULL;
	}
	if (len)
		*len = lens_[i];
	return argv_[i];
}

size_t redis_result::get_length() const
{
	if (lens_ == NULL)
		return 0;

	size_t len = 0;
	for (size_t i = 0; i < idx_; i++)
		len += lens_[i];
	return len;
}

size_t redis_result::argv_to_string(string& buf) const
{
	if (idx_ == 0)
		return 0;

	size_t length = 0;
	for (size_t i = 0; i < idx_; i++)
	{
		buf.append(argv_[i], lens_[i]);
		length += lens_[i];
	}

	return length;
}

size_t redis_result::argv_to_string(char* buf, size_t size) const
{
	if (idx_ == 0 || size == 0)
		return 0;

	size--;
	if (size == 0)
		return 0;

	char* ptr = buf;
	size_t length = 0, n;
	for (size_t i = 0; i < idx_; i++)
	{
		n = size > lens_[i] ? lens_[i] : size;
		memcpy(ptr, argv_[i], n);
		ptr += n;
		size -= n;
		length += n;
		if (size == 0)
			break;
	}

	*ptr = 0;
	return length;
}

redis_result& redis_result::put(const redis_result* rr, size_t idx)
{
	if (idx == 0)
		children_.clear();
	children_.push_back(rr);
	return *this;
}

const redis_result* redis_result::get_child(size_t i) const
{
	if (i >= children_.size())
		return NULL;
	return children_[i];
}

} // namespace acl
