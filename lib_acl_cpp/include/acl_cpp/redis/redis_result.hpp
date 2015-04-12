#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>

namespace acl
{

typedef enum
{
	REDIS_RESULT_UNKOWN,
	REDIS_RESULT_NIL,
	REDIS_RESULT_ERROR,
	REDIS_RESULT_STATUS,
	REDIS_RESULT_INTEGER,
	REDIS_RESULT_STRING,
	REDIS_RESULT_ARRAY,
} redis_result_t;

class string;
class dbuf_pool;
class redis_client;

/**
 * �� redis-server ���ؽ�������࣬�� redis-server ���ص����ݽ��з����󴴽�
 * redis_result �����
 * the redis result for redis-server's reply
 */
class ACL_CPP_API redis_result
{
public:
	redis_result(dbuf_pool* pool);

	/**
	 * ������ new/delete ���������� new �¶���ʱ��ʹ�ڴ�ķ�����
	 * �ڴ�ؽ��з���
	 * override new/delete operator, when the new object was created,
	 * memory was alloc in dbuf_pool, which is a memroy pool allocator
	 */
	void *operator new(size_t size, dbuf_pool* pool);
	void operator delete(void* ptr, dbuf_pool* pool);

	/**
	 * ��õ�ǰ���������������
	 * get the data type of the reply from redis-server
	 * @return {redis_result_t}
	 *  defined above REDIS_RESULT_
	 */
	redis_result_t get_type() const
	{
		return result_type_;
	}

	/**
	 * ��õ�ǰ������洢�Ķ���ĸ���
	 * get the number of objects from redis-server
	 * @return {size_t} ����ֵ��洢���͵Ķ�Ӧ��ϵ���£�
	 *  the relation between returned value and result type show below:
	 *  REDIS_RESULT_ERROR: 1
	 *  REDIS_RESULT_STATUS: 1
	 *  REDIS_RESULT_INTEGER: 1
	 *  REDIS_RESULT_STRING: > 0 ʱ��ʾ���ַ������ݱ��зֳɷ������ڴ��ĸ���
	 *  REDIS_RESULT_ARRAY: children_->size()
	 */
	size_t get_size() const;

	/**
	 * ������ֵΪ REDIS_RESULT_INTEGER ����ʱ�����������ض�Ӧ�� 32 λ����ֵ
	 * get the 32 bits integer for REDIS_RESULT_INTEGER result
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 *  when not NULL, storing the status of success
	 * @return {int}
	 */
	int get_integer(bool* success = NULL) const;

	/**
	 * ������ֵΪ REDIS_RESULT_INTEGER ����ʱ�����������ض�Ӧ�� 64 λ����ֵ
	 * get the 64 bits integer for REDIS_RESULT_INTEGER result
	 * @param success {bool*} ��ָ��� NULL ʱ��¼���������Ƿ�ɹ�
	 *  when not NULL, storing the status of success
	 * @return {long long int}
	 */
	long long int get_integer64(bool* success = NULL) const;

	/**
	 * ������ֵΪ REDIS_RESULT_STATUS ����ʱ������������״̬��Ϣ
	 * get operation status for REDIS_RESULT_STATUS result
	 * @return {const char*} ���� "" ��ʾ����
	 *  error if empty string returned
	 */
	const char* get_status() const;

	/**
	 * ������ʱ����ֵΪ REDIS_RESULT_ERROR ���ͣ����������س�����Ϣ
	 * when some error happened, this can get the error information
	 * @return {const char*} ���ؿմ� "" ��ʾû�г�����Ϣ
	 *  there was no error information if empty string returned
	 */
	const char* get_error() const;

	/**
	 * ���ض�Ӧ�±������(���������ͷ� REDIS_RESULT_ARRAY ʱ��
	 * get the string data of associated subscript(just for the type
	 * of no REDIS_RESULT_ARRAY)
	 * @param i {size_t} �����±�
	 *  the array's subscript
	 * @param len {size_t*} ��Ϊ�� NULL ָ��ʱ�洢���������ݵĳ���
	 *  when not NULL, the parameter will store the length of the result
	 * @return {const char*} ���� NULL ��ʾ�±�Խ��
	 *  NULL if nothing exists or the subscript is out of bounds
	 */
	const char* get(size_t i, size_t* len = NULL) const;

	/**
	 * �������е���������(���������ͷ� REDIS_RESULT_ARRAY ʱ����ַ
	 * return all data's array if the type isn't REDIS_RESULT_ARRAY
	 * @return {const char**}
	 */
	const char** gets_argv() const
	{
		return (const char**) argv_;
	}

	/**
	 * �������е����ݳ�������(���������ͷ� REDIS_RESULT_ARRAY ʱ����ַ
	 * return all length's array if the type isn't REDIS_RESULT_ARRAY
	 * @return {const size_t*}
	 */
	const size_t* get_lens() const
	{
		return lens_;
	}

	/**
	 * �����������ݵ��ܳ���(���������ͷ� REDIS_RESULT_ARRAY ʱ��
	 * return the total length of all data for no REDIS_RESULT_ARRAY
	 * @return {size_t}
	 */
	size_t get_length() const;

	/**
	 * ����������Ϊ REDIS_RESULT_STRING ����ʱ���ú��������ڴ���ŵ�����
	 * �洢�������ڴ��У�����Ҫע���ֹ�ڴ����
	 * compose a continus data for the slicing chunk data internal
	 * @param buf {string&} �洢������ݣ��ڲ����ȵ��� buf.clear()
	 *  store the result
	 * @return {int} ���ݵ��ܳ��ȣ�����ֵ -1 ��ʾ�ڲ�����Ϊ��
	 *  return the total length of data, -1 if data array has no elements
	 */
	int argv_to_string(string& buf) const;
	int argv_to_string(char* buf, size_t size) const;

	/**
	 * ����������Ϊ REDIS_RESULT_ARRAY ����ʱ���ú����������е��������
	 * return the objects array when result type is REDIS_RESULT_ARRAY
	 * @param size {size_t*} ����������ǿ�ʱ����õ�ַ������鳤��
	 *  store the array's length if size isn't NULL
	 * @return {const const redis_result*}
	 */
	const redis_result** get_children(size_t* size) const;

	/**
	 * ����������Ϊ REDIS_RESULT_ARRAY ����ʱ���ú������ض�Ӧ�±�Ľ������
	 * get one object of the given subscript from objects array
	 * @param i {size_t} �±�ֵ
	 *  the given subscript
	 * @return {const redis_result*} ���±�ֵԽ�����������ʱ���򷵻� NULL
	 *  NULL if subscript is out of bounds or object not exist
	 */
	const redis_result* get_child(size_t i) const;

	/**
	 * ���ع��캯��������ڴ�ض���
	 * get the memory pool object set in constructor
	 * @return {dbuf_pool*}
	 */
	dbuf_pool* get_pool()
	{
		return pool_;
	}

private:
	~redis_result();

	friend class redis_client;
	void clear();

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
