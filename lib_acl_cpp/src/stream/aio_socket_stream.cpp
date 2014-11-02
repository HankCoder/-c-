#include "acl_stdafx.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"

namespace acl
{

aio_socket_stream::aio_socket_stream(aio_handle* handle,
	ACL_ASTREAM* stream, bool opened /* = false */)
: aio_stream(handle), aio_istream(handle), aio_ostream(handle)
, opened_(opened)
, open_hooked_(false)
{
	acl_assert(handle);
	acl_assert(stream);

	stream_ = stream;

	// ���û���� hook_error ���� handle �������첽������,
	// ͬʱ hook �رռ���ʱ�ص�����
	hook_error();

	// ֻ�е������ӳɹ���ſ� hook IO ��д״̬
	if (opened)
	{
		// hook ���ص�����
		hook_read();

		// hook д�ص�����
		hook_write();
	}
}

aio_socket_stream::aio_socket_stream(aio_handle* handle, ACL_SOCKET fd)
: aio_stream(handle), aio_istream(handle), aio_ostream(handle)
, opened_(true)
, open_hooked_(false)
{
	acl_assert(handle);

	ACL_VSTREAM* vstream = acl_vstream_fdopen(fd, O_RDWR, 8192, 0,
					ACL_VSTREAM_TYPE_SOCK);
	stream_ = acl_aio_open(handle->get_handle(), vstream);

	// ���û���� hook_error ���� handle �������첽������,
	// ͬʱ hook �رռ���ʱ�ص�����
	hook_error();

	// ֻ�е������ӳɹ���ſ� hook IO ��д״̬
	// hook ���ص�����
	hook_read();

	// hook д�ص�����
	hook_write();
}

aio_socket_stream::~aio_socket_stream()
{
	std::list<AIO_OPEN_CALLBACK*>::iterator it = open_callbacks_.begin();
	for (; it != open_callbacks_.end(); ++it)
		acl_myfree((*it));
	open_callbacks_.clear();
}

void aio_socket_stream::destroy()
{
	delete this;
}

void aio_socket_stream::add_open_callback(aio_open_callback* callback)
{
	// �Ȳ�ѯ�ûص������Ѿ�����
	std::list<AIO_OPEN_CALLBACK*>::iterator it = open_callbacks_.begin();
	for (; it != open_callbacks_.end(); ++it)
	{
		if ((*it)->callback == callback)
		{
			if ((*it)->enable == false)
				(*it)->enable = true;
			return;
		}
	}

	// ��һ����λ
	it = open_callbacks_.begin();
	for (; it != open_callbacks_.end(); ++it)
	{
		if ((*it)->callback == NULL)
		{
			(*it)->enable = true;
			(*it)->callback = callback;
			return;
		}
	}

	// ����һ���µ�λ��
	AIO_OPEN_CALLBACK* ac = (AIO_OPEN_CALLBACK*)
		acl_mycalloc(1, sizeof(AIO_OPEN_CALLBACK));
	ac->enable = true;
	ac->callback = callback;

	// ��ӽ��ص����������
	open_callbacks_.push_back(ac);
}

int aio_socket_stream::del_open_callback(aio_open_callback* callback /* = NULL */)
{
	std::list<AIO_OPEN_CALLBACK*>::iterator it = open_callbacks_.begin();
	int   n = 0;

	if (callback == NULL)
	{
		for (; it != open_callbacks_.end(); ++it)
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
		for (; it != open_callbacks_.end(); ++it)
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

int aio_socket_stream::disable_open_callback(aio_open_callback* callback /* = NULL */)
{
	std::list<AIO_OPEN_CALLBACK*>::iterator it = open_callbacks_.begin();
	int   n = 0;

	if (callback == NULL)
	{
		for (; it != open_callbacks_.end(); ++it)
		{
			if ((*it)->callback == NULL || !(*it)->enable)
				continue;
			(*it)->enable = false;
			n++;
		}
	}
	else
	{
		for (; it != open_callbacks_.end(); ++it)
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

int aio_socket_stream::enable_open_callback(aio_open_callback* callback /* = NULL */)
{
	std::list<AIO_OPEN_CALLBACK*>::iterator it = open_callbacks_.begin();
	int   n = 0;

	if (callback == NULL)
	{
		for (; it != open_callbacks_.end(); ++it)
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
		for (; it != open_callbacks_.end(); ++it)
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

aio_socket_stream* aio_socket_stream::open(aio_handle* handle,
	const char* addr, int timeout)
{
	acl_assert(handle);

	ACL_ASTREAM* astream =
		acl_aio_connect(handle->get_handle(), addr, timeout);
	if (astream == NULL)
		return NULL;

	aio_socket_stream* stream =
		NEW aio_socket_stream(handle, astream, false);

	// ���û���� hook_error ���� handle �������첽������,
	// ͬʱ hook �رռ���ʱ�ص�����
	stream->hook_error();
	// hook ���ӳɹ��Ļص�����
	stream->hook_open();

	return stream;
}

bool aio_socket_stream::is_opened() const
{
	return opened_;
}

void aio_socket_stream::hook_open()
{
	acl_assert(stream_);
	if (open_hooked_)
		return;
	open_hooked_ = true;

	acl_aio_ctl(stream_,
		ACL_AIO_CTL_CONNECT_HOOK_ADD, open_callback, this,
		ACL_AIO_CTL_END);
}

int aio_socket_stream::open_callback(ACL_ASTREAM* stream acl_unused, void* ctx)
{
	aio_socket_stream* ss = (aio_socket_stream*) ctx;

	// ����״̬�������Ѿ����ӳɹ�
	ss->opened_ = true;

	// hook ���ص�����
	ss->hook_read();

	// hook д�ص�����
	ss->hook_write();

	// �������еĴ򿪻ص����󣬲�����֮
	std::list<AIO_OPEN_CALLBACK*>::iterator it =
		ss->open_callbacks_.begin();
	for (; it != ss->open_callbacks_.end(); ++it)
	{
		if (!(*it)->enable || (*it)->callback == NULL)
			continue;

		if ((*it)->callback->open_callback() == false)
			return -1;
	}
	return 0;
}

}  // namespace acl
