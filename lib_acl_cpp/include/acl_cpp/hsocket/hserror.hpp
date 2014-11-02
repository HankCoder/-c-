#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

#define	HS_ERR_INVALID_REPLY	-7		// �������������ݴ���
#define	HS_ERR_EMPTY		-6		// ��������������Ϊ��
#define	HS_ERR_PARAMS		-5		// �����������
#define	HS_ERR_NOT_OPEN		-4		// ��δ��
#define	HS_ERR_READ		-3		// ������ʧ��
#define	HS_ERR_WRITE		-2		// д����ʧ��
#define	HS_ERR_CONN		-1		// ����ʧ��
#define	HS_ERR_OK		0		// ��ȷ

namespace acl {

class ACL_CPP_API hserror
{
public:
	hserror();
	~hserror();

	static const char* get_serror(int errnum);
};

}  // namespace acl
