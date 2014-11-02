#pragma once
#include "db.h"

class acl::db_handle;  // db_handle.hpp
class acl::locker;  // locker.hpp

class db_driver;
class database;
class idx_host;
class dat_host;

struct NAME_TYPE;
struct DB_HOST;
struct DB_TBL;
struct TBL_IDX;

typedef enum
{
	NAME_TYPE_DB = 0,
	NAME_TYPE_TBL = 1,
	NAME_TYPE_IDX = 2
} name_type_t;

enum
{
	DB_CTL_OK = 0,
	DB_CTL_ERR = 1,
};

class db_ctl
{
public:
	db_ctl(void);
	~db_ctl(void);

	/**
	 * �������ݿ���Ϣ
	 */
	void load();

	/**
	 * ��ָ�����ݿ��е����ݿ����������ݿⲻ���ڣ�����NULL
	 * @param dbname {const char*} ���ݿ�����
	 * @param dbuser {const char*} �û���
	 * @param dbpass {const char*} �û�����
	 * @return {database*} ���ݿ����������� NULL ���ʾʧ��
	 */
	database* db_open(const char* dbname, const char* dbuser = NULL,
		const char* dbpass = NULL);

	/**
	 * �������ݿ�
	 * @param dbname {const char*} ���ݿ�����
	 * @param dbuser {const char*} �û���
	 * @param dbpass {const char*} �û�����
	 * @return {database*} ���ݿ����������� NULL ���ʾʧ��
	 */
	database* db_create(const char* dbname, const char* dbuser = NULL,
		const char* dbpass = NULL);

	/**
	 * �ر����ݿ�
	 * @param {database*} db
	 */
	void db_close(database* db);

	/**
	 * �����ݿ���������ݱ�
	 * @param db {database*} ���ݿ���
	 * @param tbl_name {const char*} ���ݱ���
	 * @return {db_tbl*} �ǿ�ֵ��ʾ��ӳɹ�
	 */
	db_tbl* db_add_tbl(database* db, const char* tbl_name);

	/**
	 * �����ݱ������������
	 * @param tbl {db_tbl*} ���ݱ����
	 * @param tbl_idx {const char*} ���ݱ�������
	 * @param idx_type {idx_type_t} ���������ֶ�����
	 * @return {db_idx*} �ǿ�ֵ��ʾ��ӳɹ�
	 */
	db_idx* db_add_idx(db_tbl* tbl, const char* tbl_idx, idx_type_t idx_type);

	/**
	 * �����ݱ������������
	 * @param db {database*} ���ݿ����
	 * @param tbl_name {const char*} ���ݱ���
	 * @param tbl_idx {const char*} ���ݱ�������
	 * @param idx_type {idx_type_t} ���������ֶ�����
	 * @return {db_idx*} �ǿ�ֵ��ʾ��ӳɹ�
	 */
	db_idx* db_add_idx(database* db, const char* tbl_name,
		const char* tbl_idx, idx_type_t idx_type);

protected:
	/**
	 * �����������ݿ����������Ӧ�����ݿ�����У�����ӽ����ݿ���
	 * @param db {database*}
	 * @return {bool}
	 */
	bool db_host_set(database* db);
private:
	int  errnum_;
	db_driver* driver_;
	acl::locker* lock_;  // db_ctl ����Ļ�����
	acl::locker* ctl_conn_lock_; // ctl_conn_ ���ݿ����ӵĻ�����
	acl::db_handle* ctl_conn_;  // �������ݿ������
	std::map<std::string, database*> dbs_;  // ���ݿ��б�
	std::vector<idx_host*> idx_hosts_;  // �����������б�
	std::vector<dat_host*> dat_hosts_;  // ���ݷ������б�

	std::list<NAME_TYPE*> names_;
	std::list<DB_HOST*> db_hosts_;
	std::list<DB_TBL*> db_tbls_;
	std::list<TBL_IDX*> tbl_idxes_;

	// ���ر� tbl_name_type
	int load_names(void);

	// ���ر� tbl_idx_host
	int load_idx_hosts(void);

	// ���ر� tbl_dat_host
	int load_dat_hosts(void);

	// ���ر� tbl_db_host
	int load_db_hosts(void);

	// ���ر� tbl_db_tbl
	int load_db_tbls(void);

	// ���ر� tbl_tbl_idx
	int load_tbl_idxes(void);

	// �� NAME_TYPE �ļ����и���ID�ż����ͻ��ָ���� NAME_TYPE �ṹ����
	NAME_TYPE* get_name(unsigned int id, name_type_t type) const;

	// ��� NAME_TYPE������ names_ ������
	void add_name(const char* name, unsigned int id, name_type_t type);

	// ����µ����ּ�¼�����ݿ⣬��������ID��
	unsigned int db_add_name(const char* name, name_type_t type);

	// �������ݱ������Ϣ
	void build_db(void);

	// �����ݿ⼯���и���ID�Ż��ָ�������ݿ����
	database* get_db(unsigned int id) const;

	// ������������������ȡ�ö�ӦID�ŵ���������������
	idx_host* get_idx_host(unsigned int id) const;

	// ��ӱ�ṹ���������ݿ������
	void add_tbl(database* db, DB_TBL* tbl);

	// ����µı��¼������ db_tbls_ ������
	void add_tbl(unsigned int id_db, unsigned int id_tbl,
		long long int count);
};
