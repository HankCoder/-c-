#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>

namespace acl {

class string;

class ACL_CPP_API hsrow
{
public:
	/**
	 * ���캯��
	 * @param ncolum {int} ���ݿ��ѯʱÿ����¼������
	 */
	hsrow(int ncolum);
	~hsrow();

	/**
	 * �������ݲ�ѯʱ������¼������
	 * @param ncolum {int} ���ݿ��ѯʱÿ����¼������
	 */
	void reset(int ncolum);

	/**
	 * ��ò�ѯ��¼���������ֵ
	 * @param value {const char*} ��ֵ
	 * @param dlen {size_t} ��ֵ����
	 */
	void push_back(const char* value, size_t dlen);

	/**
	 * ȡ�ò�ѯ��¼
	 * @return {const std::vector<const char*>&}
	 */
	const std::vector<const char*>& get_row() const;
private:
	std::vector<const char*> row_;
	int     ncolum_;
	int     icolum_;
	string* colums_;
};

}  // namespace acl
