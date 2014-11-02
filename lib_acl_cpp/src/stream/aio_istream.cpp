#include "acl_stdafx.hpp"
#include "acl_cpp/stream/aio_istream.hpp"

namespace acl
{

void aio_timer_reader::timer_callback(unsigned int id acl_unused)
{
	in_->timer_reader_ = NULL;
	if (delay_gets_)
	{
		int  timeout = delay_timeout_;
		bool nonl = delay_nonl_;

		in_->gets(timeout, nonl, 0);
	}
	else
	{
		int  timeout = delay_timeout_;
		int  count = delay_count_;

		in_->read(count, timeout, 0);
	}
}

//////////////////////////////////////////////////////////////////////

aio_istream::aio_istream(aio_handle* handle)
: aio_stream(handle)
, timer_reader_(NULL)
, read_hooked_(false)
{

}

aio_istream::~aio_istream()
{
	if (timer_reader_ != NULL)
	{
		handle_->del_timer(timer_reader_);
		timer_reader_->destroy();
	}

	std::list<AIO_CALLBACK*>::iterator it = read_callbacks_.begin();
	for (; it != read_callbacks_.end(); ++it)
		acl_myfree((*it));
	read_callbacks_.clear();
}

void aio_istream::destroy()
{
	delete this;
}

void aio_istream::add_read_callback(aio_callback* callback)
{
	acl_assert(callback);

	// �Ȳ�ѯ�ûص������Ѿ�����
	std::list<AIO_CALLBACK*>::iterator it = read_callbacks_.begin();
	for (; it != read_callbacks_.end(); ++it)
	{
		if ((*it)->callback == callback)
		{
			if ((*it)->enable == false)
				(*it)->enable = true;
			return;
		}
	}

	// ��һ����λ
	it = read_callbacks_.begin();
	for (; it != read_callbacks_.end(); ++it)
	{
		if ((*it)->callback == NULL)
		{
			(*it)->enable = true;
			(*it)->callback = callback;
			return;
		}
	}

	// ����һ���µ�λ��
	AIO_CALLBACK* ac = (AIO_CALLBACK*)
		acl_mycalloc(1, sizeof(AIO_CALLBACK));
	ac->enable = true;
	ac->callback = callback;

	// ��ӽ��ص����������
	read_callbacks_.push_back(ac);
}

int aio_istream::del_read_callback(aio_callback* callback /* = NULL */)
{
	std::list<AIO_CALLBACK*>::iterator it = read_callbacks_.begin();
	int   n = 0;

	if (callback == NULL)
	{
		for (; it != read_callbacks_.end(); ++it)
		{
			if ((*it)->callback == NULL)
				continue;
			(*it)->enable = false;
			(*it)->callback = NULL;
			n++;
		}
	}
	else
	{
		for (; it != read_callbacks_.end(); ++it)
		{
			if ((*it)->callback != callback)
				continue;
			(*it)->enable = false;
			(*it)->callback = NULL;
			n++;
			break;
		}
	}

	return n;
}

int aio_istream::disable_read_callback(aio_callback* callback)
{
	std::list<AIO_CALLBACK*>::iterator it = read_callbacks_.begin();
	int   n = 0;

	if (callback == NULL)
	{
		for (; it != read_callbacks_.end(); ++it)
		{
			if ((*it)->callback == NULL || !(*it)->enable)
				continue;
			(*it)->enable = false;
			n++;
		}
	}
	else
	{
		for (; it != read_callbacks_.end(); ++it)
		{
			if ((*it)->callback != callback || !(*it)->enable)
				continue;
			(*it)->enable = false;
			n++;
			break;
		}
	}

	return n;
}

int aio_istream::enable_read_callback(aio_callback* callback /* = NULL */)
{
	std::list<AIO_CALLBACK*>::iterator it = read_callbacks_.begin();
	int   n = 0;

	if (callback == NULL)
	{
		for (; it != read_callbacks_.end(); ++it)
		{
			if (!(*it)->enable && (*it)->callback != NULL)
			{
				(*it)->enable = true;
				n++;
			}
		}
	}
	else
	{
		for (; it != read_callbacks_.end(); ++it)
		{
			if (!(*it)->enable && (*it)->callback == callback)
			{
				(*it)->enable = true;
				n++;
			}
		}
	}

	return n;
}

void aio_istream::hook_read()
{
	acl_assert(stream_);
	if (read_hooked_)
		return;
	read_hooked_ = true;

	/*
	acl_aio_ctl(stream_,
		ACL_AIO_CTL_READ_HOOK_ADD, read_callback, this,
		ACL_AIO_CTL_CTX, this,
		ACL_AIO_CTL_END);
	*/
	acl_aio_add_read_hook(stream_, read_callback, this);
}

void aio_istream::disable_read()
{
	acl_assert(stream_);
	acl_aio_disable_read(stream_);
}

void aio_istream::keep_read(bool onoff)
{
	acl_assert(stream_);
	acl_aio_stream_set_keep_read(stream_, onoff ? 1 : 0);
}

bool aio_istream::keep_read() const
{
	acl_assert(stream_);
	return acl_aio_stream_get_keep_read(stream_) == 0 ? false : true;
}

aio_istream& aio_istream::set_buf_max(int max)
{
	acl_assert(stream_);
	acl_aio_stream_set_line_length(stream_, max);
	return *this;
}

int aio_istream::get_buf_max(void) const
{
	acl_assert(stream_);
	return acl_aio_stream_get_line_length(stream_);
}

void aio_istream::gets(int timeout /* = 0 */, bool nonl /* = true */,
	acl_int64 delay /* = 0 */, aio_timer_reader* callback /* = NULL */)
{
	if (delay > 0)
	{
		// �����µĻ����ö��ӳٶ�ʱ��

		disable_read();

		if (callback != NULL)
		{
			if (timer_reader_ != NULL)
			{
				handle_->del_timer(timer_reader_);
				timer_reader_->destroy();
			}
			timer_reader_= callback;
		}

		if (timer_reader_ == NULL)
			timer_reader_ = NEW aio_timer_reader();
		// ���� timer_reader_ ����ĳ�Ա����
		timer_reader_->in_ = this;
		timer_reader_->delay_gets_ = true;
		timer_reader_->delay_timeout_ = timeout;
		timer_reader_->delay_nonl_ = nonl;

		// �����첽����ʱ��
		handle_->set_timer(timer_reader_, delay);
		return;
	}
	else if (timer_reader_ != NULL)
	{
		// ����ȡ��֮ǰ���õ��첽����ʱ��
		handle_->del_timer(timer_reader_);
		timer_reader_->destroy();
		timer_reader_ = NULL;
	}

	// ���������첽����ʱʱ��
	if (timeout >= 0)
		if (timeout >= 0)
			ACL_AIO_SET_TIMEOUT(stream_, timeout);
	if (nonl)
		acl_aio_gets_nonl(stream_);
	else
		acl_aio_gets(stream_);
}

void aio_istream::read(int count /* = 0 */, int timeout /* = 0 */,
	acl_int64 delay /* = 0 */, aio_timer_reader* callback /* = NULL */)
{
	if (delay > 0)
	{
		// �����µĻ����ö��ӳٶ�ʱ��

		disable_read();

		if (callback != NULL)
		{
			if (timer_reader_ != NULL)
			{
				handle_->del_timer(timer_reader_);
				timer_reader_->destroy();
			}
			timer_reader_= callback;
		}

		if (timer_reader_ == NULL)
			timer_reader_ = NEW aio_timer_reader();
		// ���� timer_reader_ ����ĳ�Ա����
		timer_reader_->in_ = this;
		timer_reader_->delay_gets_ = false;
		timer_reader_->delay_timeout_ = timeout;
		timer_reader_->delay_count_ = count;

		// �����첽����ʱ��
		handle_->set_timer(timer_reader_, delay);
		return;
	}
	else if (timer_reader_ != NULL)
	{
		// ����ȡ��֮ǰ���õ��첽����ʱ��
		handle_->del_timer(timer_reader_);
		timer_reader_->destroy();
		timer_reader_ = NULL;
	}

	// ���������첽����ʱʱ��
	if (timeout >= 0)
		ACL_AIO_SET_TIMEOUT(stream_, timeout);
	if (count > 0)
		acl_aio_readn(stream_, count);
	else
		acl_aio_read(stream_);
}

void aio_istream::read_wait(int timeout /* = 0 */)
{
	// ���������첽����ʱʱ��
	if (timeout >= 0)
		ACL_AIO_SET_TIMEOUT(stream_, timeout);
	acl_aio_enable_read(stream_, read_wakeup, this);
}

int aio_istream::read_callback(ACL_ASTREAM* stream acl_unused, void* ctx,
	char* data, int len)
{
	aio_istream* in = (aio_istream*) ctx;
	std::list<AIO_CALLBACK*>::iterator it = in->read_callbacks_.begin();
	for (; it != in->read_callbacks_.end(); ++it)
	{
		if ((*it)->enable == false || (*it)->callback == NULL)
			continue;

		if ((*it)->callback->read_callback(data, len) == false)
			return -1;
	}
	return 0;
}

int aio_istream::read_wakeup(ACL_ASTREAM* stream acl_unused, void* ctx)
{
	aio_istream* in = (aio_istream*) ctx;
	std::list<AIO_CALLBACK*>::iterator it = in->read_callbacks_.begin();
	for (; it != in->read_callbacks_.end(); ++it)
	{
		if ((*it)->enable == false || (*it)->callback == NULL)
			continue;

		if ((*it)->callback->read_wakeup() == false)
			return -1;
	}
	return 0;
}

}  // namespace acl
