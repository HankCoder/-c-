#pragma once
#include <time.h>
#include "acl_cpp/queue/queue_manager.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/locker.hpp"

#ifndef MAXPATH255
#define MAXPATH255 255
#endif

namespace acl {

class fstream;

class ACL_CPP_API queue_file
{
public:
	queue_file();

	/**
	 * ����ļ���ָ��
	 * @return {acl::fstream*} �ļ���ָ��, ���Ϊ NULL ��˵���ļ���δ��
	 */
	fstream* get_fstream(void) const;

	/**
	 * ����ļ�����ʱ��
	 * @return {time_t}, ������ 1970 ������������, �������ֵΪ (time_t) -1,
	 *  ���ʾ����
	 */
	time_t get_ctime(void) const;

	/**
	 * ���ļ���д����
	 * @param data {const void*} ���ݵ�ַ
	 * @param len {size} ���ݳ���
	 * @return {bool} д�����Ƿ�ɹ�
	 */
	bool write(const void* data, size_t len);
	int format(const char* fmt, ...) ACL_CPP_PRINTF(2, 3);
	int vformat(const char* fmt, va_list ap);

	/**
	 * ���ļ��ж�ȡ����
	 * @param buf {void*} �����ַ
	 * @param len {size_t} buf ��С
	 * @return {int} ��ȡ�����ݳ���, -1: ��ʾ���������ʧ�ܻ������������,
	 *  Ӧ�ùرո��ļ�����, > 0: ��ʾ�ɹ�
	 */
	int read(void* buf, size_t len);

	/**
	 * ȡ�ñ������ļ��ļ�ֵ, ��ֵ���Ƕ����ļ��Ĳ����ļ���(����·��,
	 * ��չ��)
	 * @return {const char*} �����ļ���ֵ
	 */
	const char* key(void) const
	{
		return m_partName;
	}

	/**
	 * ��ö����ļ��ķ���ȫ·��
	 * @return {const char*}
	 */
	const char* get_filePath(void) const
	{
		return m_filePath.c_str();
	}

	/**
	 * ��ö����ļ��ĸ�·������(��������Ŀ¼)
	 * @return {const char*}
	 */
	const char* get_home(void) const
	{
		return m_home;
	}

	/**
	 * ��øö����ļ��Ķ�����
	 * @return {const char*} ��������
	 */
	const char* get_queueName(void) const
	{
		return m_queueName;
	}

	/**
	 * ��ö�����Ŀ¼
	 * @return {const char*} ������Ŀ¼��
	 */
	const char* get_queueSub(void) const
	{
		return m_queueSub;
	}

	/**
	 * ��øö����ļ�����չ��
	 * @return {const char*} ��չ����
	 */
	const char* get_extName(void) const
	{
		return m_extName;
	}

	/**
	 * ����Ѿ�д������ݴ�С�ߴ�
	 * @return {size_t}
	 */
	size_t get_fileSize() const
	{
		return nwriten_;
	}

private:
	friend class queue_manager;

	~queue_file();

	/**
	 * �����µĶ����ļ�, ������Ϻ���Զ��������ļ��ļ�������
	 * ����ֱ�ӵ��ø��ļ��� lock()/unlock()
	 * @param home {const char*} �����ļ����ڸ�·��
	 * @param queueName {const char*} ������
	 * @param extName {const char*} �����ļ���չ��
	 * @param width {unsigned} ���ж���Ŀ¼�ĸ���
	 * @return {bool} �����¶����ļ��Ƿ�ɹ�, ������� false ��˵��
	 *  ����� path �� extName �Ƿ�
	 */
	bool create(const char* home, const char* queueName,
		const char* extName, unsigned width);

	/**
	 * ���Ѿ����ڵĶ����ļ�, �򿪺���Զ��������ļ��ļ�������,
	 * ����ֱ�ӵ��ø��ļ��� lock()/unlock()
	 * @param filePath {const char*} �����ļ�·��
	 * @return {bool} �򿪶����ļ��Ƿ�ɹ�
	 */
	bool open(const char* filePath);
	bool open(const char* home, const char* queueName, const char* queueSub,
		const char* partName, const char* extName);

	/**
	 * �رյ�ǰ�ļ����
	 */
	void close();

	/**
	 * �Ӵ�����ɾ���������ļ�
	 * @return {bool} ɾ���Ƿ�ɹ�
	 */
	bool remove();

	/**
	 * �������ļ��ӵ�ǰ����������Ŀ�������
	 * @param queueName {const char*} Ŀ���������
	 * @param extName {const char*} Ŀ����չ����
	 * @return {bool} �ƶ��ļ��Ƿ�ɹ�
	 */
	bool move_file(const char* queueName, const char* extName);

	/**
	 * ���ö�����
	 * @param queueName {const char*} ��������
	 */
	void set_queueName(const char* queueName);

	/**
	 * ���ö����ļ�����չ��
	 */
	void set_extName(const char* extName);

	/**
	 * �Ե�ǰ�����ļ��������(�������������ļ���)
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool lock(void);

	/**
	 * �Ե�ǰ�����ļ��������(�������������ļ���)
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool unlock(void);

private:
	// �ļ�������
	fstream* m_fp;

	// �����ļ�����ڶ��и�Ŀ¼��ȫ·����
	string m_filePath;

	// �����ļ��ĸ�·��
	char  m_home[MAXPATH255];

	// ��������
	char  m_queueName[32];

	// �����µ���Ŀ¼
	char  m_queueSub[32];

	// �����ļ���, ������·��, Ҳ�������ļ�����չ��
	char  m_partName[MAXPATH255];

	// �����ļ�����չ��
	char  m_extName[32];

	// ��������
	locker m_locker;

	// ��ǰ�ļ��Ƿ��Ѿ���������
	bool  m_bLocked;

	// �ļ����Ƿ��Ѿ�����
	bool  m_bLockerOpened;

	// �Ѿ�д����ļ��ߴ��С
	size_t nwriten_;
};

} // namespace acl
