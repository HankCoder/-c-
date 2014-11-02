#pragma once
#include <map>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stdlib/locker.hpp"
#include "acl_cpp/queue/queue_file.hpp"

typedef struct ACL_SCAN_DIR ACL_SCAN_DIR;

namespace acl {

class queue_file;

class ACL_CPP_API queue_manager
{
public:
	/**
	 * ���ж���Ĺ��캯��
	 * @param home {const char*} ���еĸ�Ŀ¼
	 * @param queueName {const char*} �ö��ж���Ķ�������
	 */
	queue_manager(const char* home, const char* queueName,
		unsigned sub_width = 2);
	~queue_manager();

	/**
	 * ��ö�����
	 * @return {const char*}
	 */
	const char* get_queueName() const;

	/**
	 * ��ö��и�Ŀ¼
	 * @return {const char*}
	 */
	const char* get_home() const;

	/**
	 * ���������ļ�
	 * @param extName {const char*} �����ļ���չ��
	 * @return {queue_file*} �����ļ�����, ��Զ��NULL, �÷���ֵ
	 *  Ϊ��̬������, �����������Ҫ delete ���ͷ�����ռ�ڴ�
	 */
	queue_file* create_file(const char* extName);

	/**
	 * �򿪴����ϴ��ڵĶ����ļ����ڶ�/д
	 * @param path {const char*} �����ļ���
	 * @param no_cache {bool} Ϊ true ʱ��Ҫ���ڻ����и��ļ���Ӧ�� KEY
	 *  ���벻���ڣ���������򷵻� NULL ��ʾ���ļ���������; ���ò���Ϊ
	 *  false ʱ�������ֱ��ʹ�û����еĶ���
	 * @return {queue_file*} �����ļ�����, ����򲻴����򷵻� NULL
	 */
	queue_file* open_file(const char* path, bool no_cache = true);

	/**
	 * �رն����ļ����, ���ͷŸ��ļ����󣬲���ɾ���ļ�
	 * @param fp {queue_file*} �����ļ�����
	 * @return {bool} �ر��Ƿ�ɹ�
	 */
	bool close_file(queue_file* fp);

	/**
	 * �Ӵ�����ɾ�������ļ�, ���ͷŸ��ļ�����
	 * �Լ�ɾ���ö���
	 * @param fp {queue_file*} �����ļ�����
	 * @return {bool} ɾ���ļ��Ƿ�ɹ�
	 */
	bool delete_file(queue_file* fp);

	/**
	 * �޸��ļ�����չ��
	 * @param fp {queue_file*} �����ļ�����
	 * @param extName {const char*} �µ���չ��
	 * @return {bool} �޸��ļ���չ���Ƿ�ɹ�
	 */
	bool rename_extname(queue_file* fp, const char* extName);

	/**
	 * �������ļ�����Ŀ�������, �ƶ��ɹ���, �ļ������ڲ����ݽ��ᷢ���ı�
	 * @param fp {queue_file*} �����ļ�����
	 * @param queueName {const char*} Ŀ�������
	 * @param extName {const char*} �ļ���չ��
	 * @return {bool} �ƶ������ļ��Ƿ�ɹ�, ����ƶ�ʧ��, �������Ӧ�õ���
	 *  close_file �رոö����ļ�����, ���ļ����ᱻ��ʱɨ����������
	 */
	bool move_file(queue_file* fp, const char* queueName, const char* extName);

	/**
	 * ��һ�������ļ���������Ŀ����ж�����
	 * @param fp {queue_file*} �����ļ�����
	 * @param toQueue {queue_manager*} Ŀ����ж���
	 * @param extName {const char*} �ļ���չ��
	 * @return {bool} �ƶ������ļ��Ƿ�ɹ�, ����ƶ�ʧ��, �������Ӧ�õ���
	 *  close_file �رոö����ļ�����, ���ļ����ᱻ��ʱɨ����������
	 */
	bool move_file(queue_file* fp, queue_manager* toQueue, const char* extName);

	/**
	 * �Ӵ�����ɾ���������ļ�, ɾ���ɹ���ö����ļ�����Ѿ���ɾ��, ��������,
	 * ��ʹɾ���ļ�ʧ��, �ö����ļ�����Ҳ���ͷ�, ֻ�ǴӴ�����ɾ�����ļ�ʧ��,
	 * ���Ե��ô˺����� fp �����ٴ�ʹ��
	 * @param fp {queue_file*}
	 * @return {bool} ɾ���Ƿ�ɹ�
	 */
	bool remove(queue_file* fp);

	/**
	* ��������ļ����Ƿ����ڱ�ʹ��
	* @param fileName {const char*} �ļ���
	* @return {bool} �Ƿ�ʹ��
	*/
	bool busy(const char* fileName);

	/**
	* �ڶ��ж���Ļ����в���ĳ�������ļ�����
	* @param key {const char*} �����ļ��Ĳ����ļ���(����·������չ��)
	* @return {queue_file*} ���� NULL ���ʾδ�鵽
	*/
	queue_file* cache_find(const char* key);

	/**
	* ����ж���Ļ��������ĳ�������ļ�����
	* @param fp {queue_file*} �����ļ�����
	* @return {bool} ����Ƿ�ɹ�, ��ʧ����˵���ö�������Ӧ�ļ�ֵ
	*  �Ѿ������ڻ�����
	*/
	bool cache_add(queue_file* fp);

	/**
	* �Ӷ��ж���Ļ�����ɾ��ĳ�������ļ�����
	* @param key {const char*} �����ļ�����ļ�ֵ
	* @return {bool} ɾ���Ƿ�ɹ�, ��ʧ����˵���ö����ļ����󲻴���
	*/
	bool cache_del(const char* key);

	/*-------------------- �����ɨ����صĺ��� ------------------------*/

	/**
	* �򿪴���ɨ�����
	* @param scanSub {bool} �Ƿ�ݹ�ɨ����Ŀ¼
	* @return {bool} �򿪶����Ƿ�ɹ�
	*/
	bool scan_open(bool scanSub = true);

	/**
	* �ر�ɨ�����
	*/
 	void scan_close();

	/**
	* ��ô��̶����е���һ�������ļ�, ��ɨ������򷵻ؿ�
	* @return {queue_file*} ɨ��Ķ����ļ�����, ���ؿ����ʾɨ�����
	*  ������ǿն���һ��Ҫ������� delete ���ͷ��ڲ��������Դ
	*/
	queue_file* scan_next(void);

	/**
	* �����ļ�·��������������, �ļ���(����·������չ������), �ļ���չ��
	* @param filePath {const char*} �ļ�ȫ·����
	* @param home {acl::string*} �洢�ļ����ڵĸ�Ŀ¼
	* @param queueName {acl::string*} �洢�ļ����ڵĶ�����
	* @param queueSub {acl::string*} �洢�ļ��Ķ�����Ŀ¼
	* @param partName {acl::string*} �洢�ļ����ļ�������(����·������չ��)
	* @param extName {acl::string*} �洢�ļ�����չ������
	*/
	static bool parse_filePath(const char* filePath, acl::string* home,
		string* queueName, string* queueSub,
		string* partName, string* extName);

	/**
	* �����ļ�����(����չ��������·��), �������ļ���(����·������չ��),
	* ���ļ���չ����
	*/
	static bool parse_fileName(const char* fileName, acl::string* partName,
		string* extName);

	/**
	* ����·��, ������ȡ����������
	*/
	static bool parse_path(const char* path, acl::string* home,
		string* queueName, acl::string* queueSub);

	/**
	* ���ݲ����ļ���(����Ŀ¼����չ��)������������Ŀ¼·��(�����ֱ�ʾ)
	* @param partName {const char*} �����ļ���
	* @param width {unsigned} ���ж���Ŀ¼�ĸ���
	* @return {unsigned int} ������Ŀ¼·��(�����ֱ�ʾ)
	*/
	static unsigned int hash_queueSub(const char* partName, unsigned width);

protected:
private:
	bool cache_check(queue_file* fp);

	//typedef struct ACL_SCAN_DIR ACL_SCAN_DIR;

	// ɨ��Ŀ¼�ľ��
	ACL_SCAN_DIR* m_scanDir;
	string m_home;
	string m_queueName;
	unsigned sub_width_;

	std::map<string, queue_file*> m_queueList;
	locker m_queueLocker;
};

} // namespace acl
