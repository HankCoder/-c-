// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once


//#include <iostream>
//#include <tchar.h>

// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�

#include "acl_cpp/lib_acl.hpp"
#include "lib_acl.h"

#ifdef WIN32
 #include <io.h>
 #define	snprintf	_snprintf
#endif


#ifdef	DEBUG

// ���º궨����������������еĲ��������Ƿ�Ϸ�

#undef	logger
#define	logger		printf
#undef	logger_error
#define	logger_error	printf
#undef	logger_warn
#define	logger_warn	printf
#undef	logger_fatal
#define	logger_fatal	printf
#undef	logger_panic
#define	logger_panic	printf

extern void __attribute__((format(printf,3,4))) \
	dummy_debug(int, int, const char*, ...);
#undef	logger_debug
#define	logger_debug	dummy_debug
#endif
