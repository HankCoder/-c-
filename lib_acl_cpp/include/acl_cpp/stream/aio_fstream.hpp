#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/aio_istream.hpp"
#include "acl_cpp/stream/aio_ostream.hpp"

namespace acl {

class fstream;

/**
 * �첽�ļ���д�����������ֻ������ UNIX ϵͳ��
 */
class ACL_CPP_API aio_fstream
	: public aio_istream
	, public aio_ostream
{
public:
	/**
	 * ���캯��
	 * @param handle {aio_handle*} �첽�¼����
	 */
	aio_fstream(aio_handle* handle);

#ifdef	WIN32
	aio_fstream(aio_handle* handle, HANDLE fd, unsigned int oflags = 0600);
#else
	aio_fstream(aio_handle* handle, int fd, unsigned int oflags = 0600);
#endif

	/**
	 * �����ļ�·�����ļ���, ����������Ĵ��ļ��ķ�ʽ
	 * @param path {const char*} �ļ���
	 * @param oflags {unsigned int} ��־λ, We're assuming that O_RDONLY: 0x0000,
	 *  O_WRONLY: 0x0001, O_RDWR: 0x0002, O_APPEND: 0x0008, O_CREAT: 0x0100,
	 *  O_TRUNC: 0x0200, O_EXCL: 0x0400; just for win32, O_TEXT: 0x4000,
	 *  O_BINARY: 0x8000, O_RAW: O_BINARY, O_SEQUENTIAL: 0x0020, O_RANDOM: 0x0010.
	 * @param mode {int} ���ļ����ʱ��ģʽ(��: 0600)
	 * @return {bool} ���ļ��Ƿ�ɹ�
	 */
	bool open(const char* path, unsigned int oflags, unsigned int mode);

	/**
	 * �Զ�/д��ʽ���ļ��������ļ�������ʱ�򴴽����ļ������ļ�����ʱ��
	 * ���ļ����, �ļ�����Ϊ 0700
	 * @param path {const char*} �ļ���
	 * @param mode {int} ���ļ����ʱ��ģʽ(��: 0600)
	 * @return {bool} ���ļ��Ƿ�ɹ�
	 */
	bool open_trunc(const char* path, unsigned int mode = 0600);

	/**
	 * �Զ�/д��ʽ�����ļ����ļ�����Ϊ 0600, ���ļ��������򴴽����ļ�����������
	 * �򿪾��ļ�
	 * @param path {const char*} �ļ�ȫ·��
	 * @param mode {int} ���ļ����ʱ��ģʽ(��: 0600)
	 * @return {bool} �ļ������Ƿ�ɹ�
	 */
	bool create(const char* path, unsigned int mode = 0600);

	/**
	 * ��ֻ����ʽ���Ѿ����ڵ��ļ�
	 * @param path {const char*} �ļ���
	 * @return {bool} ���ļ��Ƿ�ɹ�
	 */
	bool open_read(const char* path);

	/**
	 * ��ֻд��ʽ���ļ�������ļ��������򴴽����ļ�������ļ�
	 * ���ڣ����ļ��������
	 * @param path {const char*} �ļ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool open_write(const char* path);

	/**
	 * ��β����ӷ�ʽ���ļ�������ļ��������򴴽����ļ�
	 * @param path {const char*} �ļ���
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool open_append(const char* path);

protected:
	~aio_fstream();
	/**
	 * ͨ���˺�������̬�ͷ�ֻ���ڶ��Ϸ�����첽�������
	 */
	virtual void destroy();

private:
};

}  // namespace acl
