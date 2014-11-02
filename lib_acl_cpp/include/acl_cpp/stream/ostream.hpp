#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/stream.hpp"
#include "acl_cpp/stdlib/pipe_stream.hpp"

struct iovec;

namespace acl {

class string;

/**
 * ��������������࣬��������ȷ��֪��������Ƿ������Ƿ�رգ�
 * Ӧ�õ��� stream->eof() �������ж�
 */

class ACL_CPP_API ostream
	: virtual public stream
	, public pipe_stream
{
public:
	ostream() {}
	virtual ~ostream() {}

	/**
	 * д�������������
	 * @param v {const struct iovec*}
	 * @param count {int} ���� v ��Ԫ�ظ���
	 * @param loop {bool} �Ƿ�֤����ȫ������ŷ��أ����Ϊ true��
	 *  ��ú���ֱ������ȫ����������Ż᷵�أ������дһ�α㷵�أ�
	 *  ��������֤����ȫ��д��
	 * @return {int} ��ʵд���������, ���� -1 ��ʾ����
	 */
	int writev(const struct iovec *v, int count, bool loop = true);

	/**
	 * д�������������
	 * @param data {const void*} ����ָ���ַ
	 * @param size {size_t} data ���ݳ���(�ֽ�)
	 * @param loop {bool} �Ƿ�֤����ȫ������ŷ��أ����Ϊ true,
	 *  ��ú���ֱ������ȫ����������Ż᷵�أ������дһ�α㷵�أ�
	 *  ��������֤����ȫ��д��
	 * @return {int} ��ʵд���������, ���� -1 ��ʾ����
	 */
	int write(const void* data, size_t size, bool loop = true);

	/**
	 * ����ʽ��ʽд���ݣ������� vfprintf����֤����ȫ��д��
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ap {va_list} ����б�
	 * @return {int} ��ʵд������ݳ��ȣ����� -1 ��ʾ����
	 */
	int vformat(const char* fmt, va_list ap);

	/**
	 * ����ʽ��ʽд���ݣ������� fprintf����֤����ȫ��д��
	 * @param fmt {const char*} ��θ�ʽ�ַ���
	 * @return {int} ��ʵд������ݳ��ȣ����� -1 ��ʾ����
	 */
	int format(const char* fmt, ...) ACL_CPP_PRINTF(2, 3);

	/**
	 * д��һ�� 64 λ����
	 * @param n {acl_int64} 64 λ����
	 * @return {int} д������ݳ��ȣ����� -1 ��ʾ����
	 */
#ifdef WIN32
	int write(__int64 n);
#else
	int write(long long int n);
#endif

	/**
	 * д��һ�� 32 λ����
	 * @param n {int} 32 λ����
	 * @return {int} д������ݳ��ȣ����� -1 ��ʾ����
	 */
	int write(int n);

	/**
	 * д��һ�� 16 λ������
	 * @param n {int} 16 λ����
	 * @return {int} д������ݳ��ȣ����� -1 ��ʾ����
	 */
	int write(short n);

	/**
	 * дһ���ֽ�
	 * @param ch {char}
	 * @return {int} д������ݳ��ȣ����� -1 ��ʾ����
	 */
	int write(char ch);

	/**
	 * ����������е�����
	 * @param s {const string&}
	 * @param loop {bool} �Ƿ�Ҫ��ȫ�������ŷ���
	 * @return {int} ������ݵĳ��ȣ����� -1 ��ʾ����
	 */
	int write(const string& s, bool loop = true);

	/**
	 * ���һ���ַ������ݣ��������ַ�������� "\r\n"
	 * @param s {const char*} �ַ���ָ�룬������ '\0' ��β
	 * @return {int} ������ݵĳ��ȣ����� -1 ��ʾ����
	 */
	int puts(const char* s);

	/**
	 * ���¼�������Ϊ������������غ������Ҷ�������������̣�
	 * ������ж�������Ƿ�����ر�Ӧ�õ��� stream->eof()
	 * �������ж�
	 */

	ostream& operator<<(const string& s);
	ostream& operator<<(const char* s);
#ifdef WIN32
	ostream& operator<<(__int64 n);
#else
	ostream& operator<<(long long int n);
#endif
	ostream& operator<<(int n);
	ostream& operator<<(short n);
	ostream& operator<<(char ch);

	// pipe_stream �����麯��
	// ��Ϊ������������Խ�ʵ��һ��
	virtual int push_pop(const char* in, size_t len,
		string* out = NULL, size_t max = 0);
	virtual int pop_end(string* out, size_t max = 0)
	{
		(void) out;
		(void) max;
		return (0);
	}

protected:
private:
};

} // namespace acl
