#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>

namespace acl
{

typedef enum
{
	REDIS_RESULT_NIL,
	REDIS_RESULT_ERROR,
	REDIS_RESULT_STATUS,
	REDIS_RESULT_INTEGER,
	REDIS_RESULT_STRING,
	REDIS_RESULT_ARRAY,
} redis_result_t;

class dbuf_pool;
class redis_client;

class ACL_CPP_API redis_result
{
public:
	redis_result(dbuf_pool* pool);

	/**
	 * ������ new/delete ���������� new �¶���ʱ��ʹ�ڴ�ķ������ڴ�ؽ��з���
	 */
	void *operator new(size_t size, dbuf_pool* pool);
	void operator delete(void* ptr, dbuf_pool* pool);

	/**
	 * ��õ�ǰ���������������
	 * @return {redis_result_t}
	 */
	redis_result_t get_type() const
	{
		return result_type_;
	}

	/**
	 * ��õ�ǰ������洢�Ķ���ĸ���
	 * @return {size_t} ����ֵ��洢���͵Ķ�Ӧ��ϵ���£�
	 *  REDIS_RESULT_ERROR: 1
	 *  REDIS_RESULT_STATUS: 1
	 *  REDIS_RESULT_INTEGER: 1
	 *  REDIS_RESULT_STRING: > 0 ʱ��ʾ���ַ������ݱ��зֳɷ������ڴ��ĸ���
	 *  REDIS_RESULT_ARRAY: children_->size()
	 */
	size_t get_size() const;

	/**
	 * ������ֵΪ REDIS_RESULT_INTEGER ����ʱ�����������ض�Ӧ�� 32 λ����ֵ
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 * @return {int}
	 */
	int get_integer(bool* success = NULL) const;

	/**
	 * ������ֵΪ REDIS_RESULT_INTEGER ����ʱ�����������ض�Ӧ�� 64 λ����ֵ
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 * @return {long long int}
	 */
	long long int get_integer64(bool* success = NULL) const;

	/**
	 * ������ֵΪ REDIS_RESULT_STATUS ����ʱ������������״̬��Ϣ
	 * @return {const char*} ���� NULL ��ʾ����
	 */
	const char* get_status() const;

	/**
	 * ���ض�Ӧ�±������(���������ͷ� REDIS_RESULT_ARRAY ʱ��
	 * @param i {size_t} �����±�
	 * @param len {size_t*} ��Ϊ�� NULL ָ��ʱ�洢���������ݵĳ���
	 * @return {const char*}
	 */
	const char* get(size_t i, size_t* len = NULL) const;

	/**
	 * �������е���������(���������ͷ� REDIS_RESULT_ARRAY ʱ����ַ
	 * @return {const char**}
	 */
	const char** gets_argv() const
	{
		return (const char**) argv_;
	}

	/**
	 * �������е����ݳ�������(���������ͷ� REDIS_RESULT_ARRAY ʱ����ַ
	 * @return {const size_t*}
	 */
	const size_t* get_lens() const
	{
		return lens_;
	}

	/**
	 * �����������ݵ��ܳ���(���������ͷ� REDIS_RESULT_ARRAY ʱ��
	 * @return {size_t}
	 */
	size_t get_length() const;

	/**
	 * ����������Ϊ REDIS_RESULT_STRING ����ʱ���ú��������ڴ���ŵ����ݴ洢��
	 * �����ڴ��У�����Ҫע���ֹ�ڴ����
	 * @param buf {string&} �洢������ݣ��ڲ����ȵ��� buf.clear()
	 * @return {size_t} ���ݵ��ܳ���
	 */
	size_t argv_to_string(string& buf) const;
	size_t argv_to_string(char* buf, size_t size) const;

	/**
	 * ����������Ϊ REDIS_RESULT_ARRAY ����ʱ���ú����������е��������
	 * @param size {size_t*} ����������ǿ�ʱ����õ�ַ������鳤��
	 * @return {const const redis_result*}
	 */
	const redis_result** get_children(size_t* size) const;

	/**
	 * ����������Ϊ REDIS_RESULT_ARRAY ����ʱ���ú������ض�Ӧ�±�Ľ������
	 * @param i {size_t} �±�ֵ
	 * @return {const redis_result*} ���±�ֵԽ�����������ʱ���򷵻� NULL
	 */
	const redis_result* get_child(size_t i) const;

	/**
	 * ���ع��캯��������ڴ�ض���
	 * @return {dbuf_pool*}
	 */
	dbuf_pool* get_pool()
	{
		return pool_;
	}

private:
	~redis_result();

	friend class redis_client;
	void reset();

	redis_result& set_type(redis_result_t type);
	redis_result& set_size(size_t size);
	redis_result& put(const char* buf, size_t len);
	redis_result& put(const redis_result* rr, size_t idx);

private:
	redis_result_t result_type_;
	dbuf_pool* pool_;

	size_t  size_;
	size_t  idx_;
	const char** argv_;
	size_t* lens_;

	//std::vector<const redis_result*>* children_;
	const redis_result** children_;
	size_t  children_size_;
	size_t  children_idx_;
};

} // namespace acl
