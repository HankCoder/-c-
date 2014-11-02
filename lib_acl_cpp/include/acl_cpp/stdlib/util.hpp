#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl
{

/**
 * ����ϴ�ϵͳ���ó���ʱ�Ĵ����
 * @return {int} �����
 */
ACL_CPP_API int last_error(void);

/**
 * �ֹ����ô����
 * @param errnum {int} �����
 */
ACL_CPP_API void set_error(int errnum);

/**
 * ����ϴ�ϵͳ���ó���ʱ�Ĵ���������Ϣ���ú����ڲ��������ֲ߳̾��������������߳�
 * ��ȫ�ģ���ʹ����������Щ
 * @return {const char *} ���ش�����ʾ��Ϣ 
 */
ACL_CPP_API const char* last_serror(void);

/**
 * ����ϴ�ϵͳ���ó���ʱ�Ĵ���������Ϣ
 * @param buf {char*} �洢����������Ϣ���ڴ滺����
 * @param size {int} buffer �Ŀռ��С
 * @return {const char*} ���صĵ�ַӦ�� buffer ��ͬ
 */
ACL_CPP_API const char* last_serror(char* buf, size_t size);

/**
 * �����ڱ�׼C�� strerror, ���ú����ǿ�ƽ̨�����̰߳�ȫ�ģ���ö�Ӧĳ������
 * �ŵĴ���������Ϣ
 * @param errnum {int} �����
 * @param buf {char*} �洢����������Ϣ���ڴ滺����
 * @param size {int} buffer �������Ĵ�С
 * @return {const char*} ���صĵ�ַӦ�� buffer ��ͬ
*/
ACL_CPP_API const char* string_error(int errnum, char* buf, size_t size);

ACL_CPP_API int strncasecmp_(const char *s1, const char *s2, size_t n);
ACL_CPP_API void assert_(bool n);
ACL_CPP_API void meter_time(const char *filename, int line, const char *info);

} // namespace acl
