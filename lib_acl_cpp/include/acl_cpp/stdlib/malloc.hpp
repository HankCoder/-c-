#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

#ifdef HOOK_NEW

#ifdef WIN32
# ifdef NDEBUG
void* operator new(size_t n);
void  operator delete(void *p);
# endif
#else
#include <new>
void* operator new(size_t n)  throw (std::bad_alloc);
void  operator delete(void *p) throw();
#endif

#endif

namespace acl {

ACL_CPP_API void  acl_slice_init(void);

/**
 * �ڴ���亯��
 * @param size {size_t} ��Ҫ����ĳߴ��С
 * @param filename {const char*} Դ��������
 * @param funcname {const char*} ������
 * @param lineno {int} Դ�����к�
 * @return {void*} ������ڴ��ַ����������ڴ�ʧ�ܣ���ֱ��abort
 */
ACL_CPP_API void* acl_new(size_t size, const char* filename,
	const char* funcname, int lineno);


/**
 * �ͷ��ڴ溯��
 * @param ptr {void*} �ڴ��ַ
 * @param filename {const char*} Դ��������
 * @param funcname {const char*} ������
 * @param lineno {int} Դ�����к�
 */
ACL_CPP_API void  acl_delete(void *ptr, const char* filename,
	const char* funcname, int lineno);

}
