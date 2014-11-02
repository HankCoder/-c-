#pragma once

typedef enum
{
	DB_OK,                  // ����
	DB_ERR_OPEN_DB,         // �����ݿ�ʧ��
	DB_ERR_CREATE_DB,       // �������ݿ�ʧ��
	DB_ERR_CREATE_TBL,      // �������ݱ�ʧ��
	DB_ERR_DB_NO_EXIST,     // ���ݿⲻ����
	DB_ERR_TBL_NO_EXIST,    // ���ݱ�����
	DB_ERR_IDX_NO_EXIST,    // ����������
} db_error_t;

class db_result
{
public:
	db_result(void) {}
	~db_result(void) {}
};

/**
 * �����࣬���ඨ�������˴洢����صĽӿ�
 */
class db_driver
{
public:
	db_driver(void) {}
	virtual ~db_driver(void) {};

	/**
	 * �������ݿ⼰���ݱ������ݿ�δ�����򴴽��µ����ݿ⣬����
	 * �ڸ����ݿ��ϴ����µ����ݱ������ݱ�Ҳ�������ڸ����ݱ��ϴ���
	 * �µ�������������Ҳ���ڣ���ֱ�ӷ�����ȷ
	 * @param dbname {const char*} ���ݿ�����
	 * @param tbl {const char* tbl} ���ݱ���
	 * @param idx {const char* idx} ���ݱ������������������ֵ�����ݱ�
	 *  �������򴴽��µĻ��ڸ���������������������Ѿ����ڵ�������
	 * @param idx_unique {bool} �����ֶ��Ƿ�Ҫ��Ψһ��
	 * @param user {const char*} �򿪸����ݿ���˺���
	 * @param pass {const char*} �򿪸����ݿ���˺�����
	 * @return {bool} �����Ƿ�ɹ�
	 */
	virtual bool create(const char* dbname, const char* tbl,
			const char* idx, bool idx_unique = false,
			const char* user = NULL, const char* pass = NULL) = 0;

	/**
	 * �����ݿ⼰���ݱ�������ݿ⡢���ݱ�������������򷵻�ʧ��
	 * @param dbname {const char*} ���ݿ�����
	 * @param tbl {const char* tbl} ���ݱ���
	 * @param idx {const char*} �����ֶ���
	 * @param user {const char*} �򿪸����ݿ���˺���
	 * @param pass {const char*} �򿪸����ݿ���˺�����
	 * @return {bool} �����Ƿ�ɹ�
	 */
	virtual bool open(const char* dbname, const char*tbl, const char* idx,
			const char* user = NULL, const char* pass = NULL) = 0;

	/**
	 * ��ӻ��޸����ݣ�������ֵ��Ӧ���ݲ�����ʱ��������ݣ������޸�
	 * Ϊ�µ�����
	 * @param idx_value {const char*} ����ֵ�������Ӧ open �����е�������
	 * @param data {cost void*} ���ݵ�ַ
	 * @param dlen {size_t} data ���ݳ���
	 * @return {bool} ���� false ��ʾʧ�ܣ�ԭ������� last_error() ���
	 */
	virtual bool set(const char* idx_value, const void* data, size_t dlen) = 0;

	/**
	 * �������������ֵ�������
	 * @param idx_value {const char*} ����ֵ����Ӧ open �򿪵������ֶ�
	 * @return {db_result*} �洢��ѯ������������ֵ�ǿ����ʾ��ѯ�ɹ���
	 *  ��Ҫ��һ������ db_result �����еĺ�������ò�ѯ������������
	 *  NULL ���ʾ������Ҫ���� last_error() ��ó���ԭ��
	 */
	virtual db_result* get(const char* idx_value) = 0;

	/**
	 * �������������ֵɾ�����ݣ�ͬʱɾ�����������ص��������������ֵ
	 * @param idx_value {const char*} ��Ӧ open �е������ֶε�����ֵ
	 * @return {bool} ���� false ��ʾʧ�ܣ�ԭ������� last_error() ��ã�
	 *  ɾ������������� affect_count() ���
	 */
	virtual bool del(const char* idx_value) = 0;

	/**
	 * �ڵ��� set/del ����ʱ���ô˺������Ի����Ӱ��ļ�¼������������ get
	 * ����û�б�Ҫ���ô˺���
	 * @return {int} ����ֵ >= 0 ��ʾӰ���¼������
	 */
	virtual size_t affect_count() const = 0;

	/**
	 * ���ô˺�������ϴβ����Ĵ����
	 * @return {db_error_t} ���������
	 */
	virtual db_error_t last_error() const = 0;

private:
};
