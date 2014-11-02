#pragma once
#include "db_driver.h"

struct ACL_DB_POOL;
class db_result;

class driver_mysql : public db_driver
{
public:
	driver_mysql(const char* dbaddr, const char* dbname,
		const char* dbuser, const char* dbpass,
		int dbpool_limit = 50, int dbpool_ping = 30,
		int dbpool_timeout = 60);
	virtual ~driver_mysql(void);

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
	virtual  bool create(const char* dbname, const char* tbl,
			const char* idx, bool idx_unique = false,
			const char* user = NULL, const char* pass = NULL);

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
			const char* user = NULL, const char* pass = NULL);

	/**
	 * ��ӻ��޸����ݣ�������ֵ��Ӧ���ݲ�����ʱ��������ݣ������޸�
	 * Ϊ�µ�����
	 * @param idx_value {const char*} ����ֵ�������Ӧ open �����е�������
	 * @param data {cost void*} ���ݵ�ַ
	 * @param dlen {size_t} data ���ݳ���
	 * @return {bool} ���� false ��ʾʧ�ܣ�ԭ������� last_error() ���
	 */
	virtual bool set(const char* idx_value, const void* data, size_t dlen);

	/**
	 * �������������ֵ�������
	 * @param idx_value {const char*} ����ֵ����Ӧ open �򿪵������ֶ�
	 * @return {db_result*} �洢��ѯ������������ֵ�ǿ����ʾ��ѯ�ɹ���
	 *  ��Ҫ��һ������ db_result �����еĺ�������ò�ѯ������������
	 *  NULL ���ʾ������Ҫ���� last_error() ��ó���ԭ��
	 */
	virtual db_result* get(const char* idx_value);

	/**
	 * �������������ֵɾ�����ݣ�ͬʱɾ�����������ص��������������ֵ
	 * @param idx_value {const char*} ��Ӧ open �е������ֶε�����ֵ
	 * @return {bool} ���� false ��ʾʧ�ܣ�ԭ������� last_error() ��ã�
	 *  ɾ������������� affect_count() ���
	 */
	virtual bool del(const char* idx_value);

	/**
	 * �ڵ��� set/del ����ʱ���ô˺������Ի����Ӱ��ļ�¼������������ get
	 * ����û�б�Ҫ���ô˺���
	 * @return {int} ����ֵ >= 0 ��ʾӰ���¼������
	 */
	virtual size_t affect_count() const;

	/**
	 * ���ô˺�������ϴβ����Ĵ����
	 * @return {db_error_t} ���������
	 */
	virtual db_error_t last_error() const;

private:
	ACL_DB_POOL* dbpool_;
};
