#pragma once

class acl::locker;
class db_tbl;
class idx_host;
class database;

// �����ֶε�����
typedef enum
{
	IDX_TYPE_STR = 0,	// �ַ�������
	IDX_TYPE_BOOL,		// ��������
	IDX_TYPE_INT16,		// 16λ����
	IDX_TYPE_INT32,		// 32λ����
	IDX_TYPE_INT64		// 64λ����
} idx_type_t;

/**
 * ���ݿ������е���������
 */
class db_idx
{
public:
	/**
	 * �����������캯��
	 * @param tbl {db_tbl*} �����
	 * @param name {const char*} ��������
	 * @param id {unsigned int} ������Ӧ��ID��
	 * @param type {idx_type_t} �����ֶε���������
	 */
	db_idx(db_tbl* tbl, const char* name, unsigned int id, idx_type_t type);
	~db_idx();

	const char* get_name() const
	{
		return name_;
	}

	unsigned int get_id() const
	{
		return id_;
	}

	db_tbl* get_tbl() const
	{
		return tbl_;
	}

	idx_type_t get_type() const
	{
		return type_;
	}
protected:
private:
	db_tbl* tbl_; // ���������ݱ����
	char* name_;  // ������
	unsigned int id_;  // ����ID��
	idx_type_t type_;  // �����ֶε���������
};

class db_tbl
{
public:
	/**
	 * ������캯��
	 * @param db {database*} ���ݿ����
	 * @param name {const char*} ���ݱ���
	 * @param id {unsigned int} ���ݱ��Ӧ��ID��
	 */
	db_tbl(database* db, const char* name, unsigned int id);
	~db_tbl();

	const char* get_name() const
	{
		return name_;
	}

	unsigned int get_id() const
	{
		return id_;
	}

	database* get_db() const
	{
		return db_;
	}

	void add_idx(db_idx* idx);
private:
	database* db_; // ���������ݿ����
	char* name_;  // ���ݱ���
	unsigned int id_;  // ���ݱ��ID��
	std::list<db_idx*> idxes_;  // ����������
};

class database
{
public:
	/**
	 * ���ݿ�����캯��
	 * @param name {const char*} ���ݿ�����
	 * @param id {unsigned int} ���ݿ��Ӧ��ID��
	 */
	database(const char* name, unsigned int id);
	~database();

	const char* get_name() const
	{
		return name_;
	}

	unsigned int get_id() const
	{
		return id_;
	}

	void add_tbl(db_tbl*);
	void add_idx_host(idx_host* host);
protected:
private:
	char* name_;  // ���ݿ���
	unsigned int id_;  // ���ݿ��ID��
	std::map<std::string, db_tbl*> tables_;  // ���ݱ���
	std::vector<idx_host*> idx_hosts_;  // ����Ӧ�������������ļ���

	acl::locker* lock_;  // ���ݿ���
};
