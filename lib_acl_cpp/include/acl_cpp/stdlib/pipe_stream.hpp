#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>
#include "acl_cpp/stdlib/string.hpp"

namespace acl {

/**
 * ˫�������������ܵ���, ���������ɽ����������ݣ�ͬʱ���������
 * ��������ݣ�������࣬������Ҫʵ�������ӿں���
 */
class ACL_CPP_API pipe_stream
{
public:
	pipe_stream() {}
	virtual ~pipe_stream() {}

	/**
	 * ������������ӿ�
	 * @param in {const char*} �������ݵĵ�ַ
	 * @param len {size_t} �������ݳ���
	 * @param out {string*} �洢������������������Ϊ��
	 * @param max {size_t} ϣ�����յ��������ĳ������ƣ����Ϊ0��
	 *  ��ʾû�����ƣ����������洢�� out ��������
	 * @return {int} ������ݵĳ��ȣ���� < 0 ���ʾ����
	 */
	virtual int push_pop(const char* in, size_t len,
		string* out, size_t max = 0) = 0;

	/**
	 * ������������ݽӿ�
	 * @param out {string*} �洢������������������Ϊ��
	 * @param max {size_t} ϣ�����յ��������ĳ������ƣ����Ϊ0��
	 *  ��ʾû�����ƣ����������洢�� out ��������
	 * @return {int} ������ݵĳ��ȣ���� < 0 ���ʾ����
	 */
	virtual int pop_end(string* out, size_t max = 0) = 0;

	/**
	 * ����ڲ�������
	 */
	virtual void clear() {}
};

/**
 * �ַ�������˫�������
 */
class ACL_CPP_API pipe_string : public pipe_stream
{
public:
	pipe_string();
	pipe_string(string& s);
	virtual ~pipe_string();

	// pipe_stream �����е��ĸ��麯��
	virtual int push_pop(const char* in, size_t len,
		string* out, size_t max = 0);
	virtual int pop_end(string* out, size_t max = 0);
	virtual void clear()
	{
		m_pBuf->clear();
		m_pos = 0;
	}

	string& get_buf() const
	{
		return (*m_pBuf);
	}

	char* c_str() const
	{
		return (m_pBuf->c_str());
	}

	size_t length() const
	{
		return (m_pBuf->length());
	}

	bool empty() const
	{
		return (m_pBuf->empty());
	}

private:
	string* m_pBuf;
	string* m_pSavedBufPtr;
	size_t  m_pos;
};

/**
 * �ܵ���������������������еĹ����������������δ��ݸ����й�������
 * ����ӿڣ�ͬʱ�����йܵ���������ӿ��л������Ȼ���ٽ����ݴ��ݸ�
 * ��һ���ܵ���������ӿڣ��Դ����ƣ�ֱ�����һ���ܵ���
 */
class ACL_CPP_API pipe_manager
{
public:
	pipe_manager();
	~pipe_manager();

	/**
	 * ��β����ӵķ�ʽע���µĹܵ���������
	 * @param stream {pipe_stream*} �ܵ�������������
	 * @return {bool} ����ùܵ��������������Ѿ������򷵻� false
	 */
	bool push_back(pipe_stream* stream);

	/**
	 * ��ͷ����ӵķ�ʽע���µĹܵ���������
	 * @param stream {pipe_stream*} �ܵ�������������
	 * @return {bool} ����ùܵ��������������Ѿ������򷵻� false
	 */
	bool push_front(pipe_stream* stream);

	/**
	 * Ӧ����ܵ�����������������ݣ��ɸù��������δ��ݸ�������ע��ܵ���
	 * ��������ͬʱ����ע��ܵ������������մ����������δ��ݸ���һ��
	 * @param src {const char*} ����������ݵ�ַ
	 * @param len {size_t} src ���ݳ���
	 * @param out {pipe_stream*} ����ǿգ���ùܵ��������������һ��ֻ����
	 *  ���������������Ĺܵ�������
	 * @return {bool} �Ƿ��д�����
	 */
	bool update(const char* src, size_t len, pipe_stream* out = NULL);

	/**
	 * ���������һ�θú�������ʹ��Щ�ܵ��Ļ�����������ݿ���һ���Ե�
	 * ˢ�������Ĺܵ���
	 * @param out {pipe_stream*} ����ǿգ���ùܵ��������������һ��ֻ����
	 *  ���������������Ĺܵ�������
	 * @return {bool} �Ƿ��д�����
	 */
	bool update_end(pipe_stream* out = NULL);

	pipe_manager& operator<<(const string&);
	pipe_manager& operator<<(const string*);
	pipe_manager& operator<<(const char*);
#ifdef WIN32
	pipe_manager& operator<<(__int64);
	pipe_manager& operator<<(unsigned __int64);
#else
	pipe_manager& operator<<(long long int);
	pipe_manager& operator<<(unsigned long long int);
#endif
	pipe_manager& operator<<(long);
	pipe_manager& operator<<(unsigned long);
	pipe_manager& operator<<(int);
	pipe_manager& operator<<(unsigned int);
	pipe_manager& operator<<(short);
	pipe_manager& operator<<(unsigned short);
	pipe_manager& operator<<(char);
	pipe_manager& operator<<(unsigned char);

	char* c_str() const;
	size_t length() const;
	void clear();

private:
	std::list<pipe_stream*> m_streams;
	string* m_pBuf1, *m_pBuf2;
	pipe_string* m_pPipeStream;
};

} // namespace acl
