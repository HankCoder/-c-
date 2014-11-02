#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

struct ACL_XINETD_CFG_PARSER;
struct ACL_CFG_INT_TABLE;
struct ACL_CFG_INT64_TABLE;
struct ACL_CFG_STR_TABLE;
struct ACL_CFG_BOOL_TABLE;

namespace acl {

typedef struct master_int_tbl
{
	const char *name;
	int  defval;
	int *target;
	int  min;
	int  max;
} master_int_tbl;

typedef struct master_str_tbl
{
	const char *name;
	const char *defval;
	char **target;
} master_str_tbl;

typedef struct master_bool_tbl
{
	const char *name;
	int   defval;
	int  *target;
} master_bool_tbl;

typedef struct master_int64_tbl
{
	const char *name;
#ifdef WIN32
	__int64  defval;
	__int64 *target;
	__int64  min;
	__int64  max;
#else
	long long int  defval;
	long long int *target;
	long long int  min;
	long long int  max;
#endif
} master_int64_tbl;

class master_base;

class ACL_CPP_API master_conf
{
public:
	master_conf();
	~master_conf();

	/**
	 * ���� bool ���͵�������
	 * @param table {master_bool_tbl*}
	 */
	void set_cfg_bool(master_bool_tbl* table);

	/**
	 * ���� int ���͵�������
	 * @param table {master_int_tbl*}
	 */
	void set_cfg_int(master_int_tbl* table);

	/**
	 * ���� int64 ���͵�������
	 * @param table {master_int64_tbl*}
	 */
	void set_cfg_int64(master_int64_tbl* table);

	/**
	 * ���� �ַ��� ���͵�������
	 * @param table {master_str_tbl*}
	 */
	void set_cfg_str(master_str_tbl* table);

	/**
	 * ���������ļ�
	 * @param path {const char*} �����ļ�ȫ·��
	 */
	void load(const char* path);

	/**
	 * �������ý�����״̬���ͷ�֮ǰ�������Դ�����ô˺�����
	 * ֮ǰ��õ��ַ�����������ڴ潫�ᱻ�ͷţ����Խ�ֹ���ã�
	 * ���øú�����������ý�������������ٴ�ʹ�ý�������
	 * �����ļ�
	 */
	void reset(void);

	ACL_CFG_INT_TABLE* get_int_cfg() const;
	ACL_CFG_INT64_TABLE* get_int64_cfg() const;
	ACL_CFG_STR_TABLE* get_str_cfg() const;
	ACL_CFG_BOOL_TABLE* get_bool_cfg() const;
protected:
private:
	bool cfg_loaded_;

	ACL_XINETD_CFG_PARSER* cfg_;
	ACL_CFG_INT_TABLE*  int_cfg_;
	ACL_CFG_INT64_TABLE* int64_cfg_;
	ACL_CFG_STR_TABLE*  str_cfg_;
	ACL_CFG_BOOL_TABLE* bool_cfg_;

	master_int_tbl* int_tbl_;
	master_str_tbl* str_tbl_;
	master_bool_tbl* bool_tbl_;
	master_int64_tbl* int64_tbl_;

	void load_str(void);
	void load_bool(void);
	void load_int(void);
	void load_int64(void);
};

} // namespace acl
