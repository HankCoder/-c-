#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

struct ACL_DBUF_POOL;

namespace acl
{

/**
 * �ڴ��������࣬������ṩ�ڴ���亯�������������������ʱ���ڴ����ᱻһ���Ե��ͷţ�
 * �����ʺ�����ҪƵ������һЩ��С���ȵ�С�ڴ��Ӧ�ã�
 * ����ʵ�����Ƿ�װ�� lib_acl �е� ACL_DBUF_POOL �ṹ������
 */
class ACL_CPP_API dbuf_pool
{
public:
	dbuf_pool(size_t block_size = 8192);
	~dbuf_pool();

	/**
	 * ����ָ�����ȵ��ڴ�
	 * @param len {size_t} ��Ҫ������ڴ泤�ȣ����ڴ�Ƚ�Сʱ(С�ڹ��캯���е�
	 *  block_size)ʱ����������ڴ����� dbuf_pool ��������ڴ����ϣ����ڴ�ϴ�ʱ
	 *  ��ֱ��ʹ�� malloc ���з���
	 * @return {void*} �·�����ڴ��ַ
	 */
	void* dbuf_alloc(size_t len);

	/**
	 * ����ָ�����ȵ��ڴ沢���ڴ���������
	 * @param len {size_t} ��Ҫ������ڴ泤��
	 * @return {void*} �·�����ڴ��ַ
	 */
	void* dbuf_calloc(size_t len);

	/**
	 * ����������ַ�����̬�����µ��ڴ沢���ַ����ڴ���и��ƣ������� strdup
	 * @param s {const char*} Դ�ַ���
	 * @return {char*} �¸��Ƶ��ַ�����ַ
	 */
	char* dbuf_strdup(const char* s);

	/**
	 * ����������ڴ����ݶ�̬�����ڴ沢�����ݽ��и���
	 * @param s {const void*} Դ�����ڴ��ַ
	 * @param len {size_t} Դ���ݳ���
	 * @return {void*} �¸��Ƶ����ݵ�ַ
	 */
	void* dbuf_memdup(const void* s, size_t len);

private:
	ACL_DBUF_POOL* pool_;
};

} // namespace acl
