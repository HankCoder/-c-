#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <stdarg.h>

namespace acl {

/**
 * ��׼ C snprintf API ��װ�����Ա�֤����������е����һ���ֽ�Ϊ '\0'
 * @param buf {char*} �洢����Ļ�����
 * @param size {size_t} buf ����������
 * @param fmt {const char*} ��θ�ʽ�ַ���
 * @return {int} �����������ȹ���ʱ����ʵ�ʿ��������ݳ��ȣ�����:
 *  1) UNIX/LINUX ƽ̨�·���ʵ����Ҫ�Ļ��������ȣ��������������Ȳ���ʱ����ֵ
 *     >= size����Ҫע��÷���ֵ�ĺ����� WIN32 �µĲ�ͬ
 *  2) WIN32 ƽ̨�·��� -1
 */
ACL_CPP_API int ACL_CPP_PRINTF(3, 4) safe_snprintf(char *buf, size_t size,
	const char *fmt, ...);

/**
 * ��׼ C snprintf API ��װ�����Ա�֤����������е����һ���ֽ�Ϊ '\0'
 * @param buf {char*} �洢����Ļ�����
 * @param size {size_t} buf ����������
 * @param fmt {const char*} ��θ�ʽ�ַ���
 * @param ap {va_list} ��α���
 * @return {int} �����������ȹ���ʱ����ʵ�ʿ��������ݳ��ȣ�����:
 *  1) UNIX/LINUX ƽ̨�·���ʵ����Ҫ�Ļ��������ȣ��������������Ȳ���ʱ����ֵ
 *     >= size����Ҫע��÷���ֵ�ĺ����� WIN32 �µĲ�ͬ
 *  2) WIN32 ƽ̨�·��� -1
 */
ACL_CPP_API int safe_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);

}  // namespace acl
