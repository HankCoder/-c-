#include "acl_stdafx.hpp"
#include "acl_cpp/stream/fstream.hpp"

namespace acl {

fstream::fstream()
{

}

fstream::~fstream()
{
	close();
}

void fstream::open(ACL_FILE_HANDLE fh, unsigned int oflags)
{
	open_stream(true);  // ���û��෽���ȴ�����������

	acl_assert(ACL_VSTREAM_FILE(stream_) == ACL_FILE_INVALID);

	stream_->fread_fn  = acl_file_read;
	stream_->fwrite_fn = acl_file_write;
	stream_->fwritev_fn = acl_file_writev;
	stream_->fclose_fn = acl_file_close;

	stream_->fd.h_file = fh;
	stream_->type = ACL_VSTREAM_TYPE_FILE;
	stream_->oflags = oflags;
	opened_ = true;
	eof_ = false;
}

bool fstream::open(const char* path, unsigned int oflags, int mode)
{
	if (path == NULL)
		return false;

	ACL_FILE_HANDLE fh;

	fh = acl_file_open(path, oflags, mode);
	if (fh == ACL_FILE_INVALID)
		return false;

	open_stream(true);  // ���û��෽���ȴ�����������

	stream_->fread_fn  = acl_file_read;
	stream_->fwrite_fn = acl_file_write;
	stream_->fwritev_fn = acl_file_writev;
	stream_->fclose_fn = acl_file_close;

	stream_->fd.h_file = fh;
	stream_->type = ACL_VSTREAM_TYPE_FILE;
	stream_->oflags = oflags;
	acl_vstream_set_path(stream_, path);
	opened_ = true;
	eof_ = false;
	return true;
}

const char* fstream::file_path() const
{
	return stream_ ? stream_->path : NULL;
}

bool fstream::open_trunc(const char* path)
{
	return open(path, O_RDWR | O_CREAT | O_TRUNC, 0600);
}

bool fstream::create(const char* path)
{
	return open(path, O_RDWR | O_CREAT, 0600);
}

acl_off_t fstream::fseek(acl_off_t offset, int whence)
{
	acl_off_t ret = acl_vstream_fseek(stream_, offset, whence);
	eof_ = ret >= 0 ? false : true;
	return ret;
}

acl_off_t fstream::ftell()
{
	acl_off_t ret = acl_vstream_ftell(stream_);
	eof_ = ret >= 0 ? false : true;
	return ret;
}

bool fstream::ftruncate(acl_off_t length)
{
	fseek(0, SEEK_SET); // ��Ҫ�Ƚ��ļ�ָ���Ƶ���ʼλ��
	return acl_file_ftruncate(stream_, length) == 0 ? true : false;
}

acl_int64 fstream::fsize() const
{
	return acl_vstream_fsize(stream_);
}

ACL_FILE_HANDLE fstream::file_handle() const
{
	return ACL_VSTREAM_FILE(stream_);
}

} // namespace acl
