#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <map>
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;
class redis_result;

class ACL_CPP_API redis_server : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	redis_server();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	redis_server(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_client_cluster*�� size_t)
	 */
	redis_server(redis_client_cluster* cluster, size_t max_conns);

	virtual ~redis_server();

	/////////////////////////////////////////////////////////////////////

	
	/**
	 * ִ��һ�� AOF�ļ� ��д��������д�ᴴ��һ����ǰ AOF �ļ�������Ż��汾��
	 * ��ʹ BGREWRITEAOF ִ��ʧ�ܣ�Ҳ�������κ����ݶ�ʧ����Ϊ�ɵ� AOF �ļ���
	 * BGREWRITEAOF �ɹ�֮ǰ���ᱻ�޸�
	 * @return {bool}
	 */
	bool bgrewriteaof();

	/**
	 * �ں�̨�첽(Asynchronously)���浱ǰ���ݿ�����ݵ����̣�BGSAVE ����ִ��֮��
	 * �������� OK ��Ȼ�� Redis fork ��һ�����ӽ��̣�ԭ���� Redis ����(������)
	 * ��������ͻ������󣬶��ӽ����������ݱ��浽���̣�Ȼ���˳����ͻ��˿���ͨ��
	 * LASTSAVE ����鿴�����Ϣ���ж� BGSAVE �����Ƿ�ִ�гɹ�
	 * @return {bool}
	 */
	bool bgsave();

	/**
	 * ���� CLIENT SETNAME ����Ϊ�������õ�����
	 * @param buf {string&} �洢��������û��������Ϊ��
	 * @return {bool} ���� false �����û�������������ֻ����
	 */
	bool client_getname(string& buf);

	/**
	 * �رյ�ַΪ ip:port �Ŀͻ���
	 * @param addr {const char*} �ͻ������ӵ�ַ����ʽ��ip:port
	 * @return {bool} �Ƿ�ɹ������� false �������Ӳ����ڻ����
	 */
	bool client_kill(const char* addr);

	/**
	 * �����������ӵ��������Ŀͻ�����Ϣ��ͳ������
	 * @param buf {string&} �洢���
	 * @return {int} ���ؽ�����ݳ��ȣ�-1 ��ʾ����
	 */
	int client_list(string& buf);

	/**
	 * Ϊ��ǰ���ӷ���һ�����֣������ֻ������ CLIENT LIST ����Ľ���У�
	 * �� Redis Ӧ�ó���������й©ʱ��Ϊ��������������һ�ֺܺõ� debug �ֶ�
	 * @param name {const char*} �������֣������ֲ���ҪΨһ��
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool client_setname(const char* name);

	/**
	 * ��������ȡ�������е� Redis �����������ò���
	 * @param parameter {const char*} ���ò�����
	 * @param out {std::map<string, string>&} �洢������� name-value ��ɣ�
	 *  ��Ϊ parameter ֧��ģ��ƥ�䣬�����п��ܷ��صĽ�����л��ж��������
	 * @return {int} ��� "����-ֵ" �ĸ�����-1 ��ʾ����
	 */
	int config_get(const char* parameter, std::map<string, string>& out);

	/**
	 * ���� INFO �����е�ĳЩͳ������
	 * @return {bool} �����Ƿ�ɹ�
	 */
	bool config_resetstat();

	/**
	 * ������ Redis ������ʱ��ָ���� redis.conf �ļ����и�д
	 * @return {bool} ��д�����Ƿ�ɹ�
	 */
	bool config_rewrite();

	/**
	 * ��̬�ص��� Redis �����������ö�������������
	 * @param name {const char*} ���ò�����
	 * @param value {const char*} ���ò���ֵ
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool config_set(const char* name, const char* value);

	/**
	 * ���ص�ǰ���ݿ�� key ������
	 * @return {int} ���� -1 ��ʾ����
	 */
	int dbsize();

	/**
	 * ������� Redis ������������(ɾ���������ݿ������ key )
	 * @return {bool}
	 *  ע��������Ҫ���ã�������������
	 */
	bool flushall();

	/**
	 * ��յ�ǰ���ݿ��е����� key
	 * @return {bool}
	 *  ע��������Ҫ���ã�������������
	 */
	bool flushdb();

	/**
	 * ���ع��� Redis �������ĸ�����Ϣ��ͳ����ֵ
	 * @param buf {string&} �洢���
	 * @return {int} �������洢�����ݳ���
	 */
	int info(string& buf);

	/**
	 * �������һ�� Redis �ɹ������ݱ��浽�����ϵ�ʱ�䣬�� UNIX ʱ�����ʽ��ʾ
	 * @return {time_t}
	 */
	time_t lastsave();

	/**
	 * ʵʱ��ӡ�� Redis ���������յ������������; ���ñ���������ѭ�����������
	 * get_command ������÷������յ�������
	 * @return {bool}
	 */
	bool monitor();

	/**
	 * ���� monitor ��������Ҫ���ñ�������÷������յ����������ѭ�����ñ�����
	 * �Ա��ڲ��ϵػ�÷������յ�������
	 * @param buf {string&} �洢���
	 * @return {bool}
	 */
	bool get_command(string& buf);

	/**
	 * ����ִ��һ��ͬ���������������ǰ Redis ʵ�����������ݿ���(snapshot)
	 * �� RDB �ļ�����ʽ���浽Ӳ��
	 * @return {bool}
	 */
	bool save();

	/**
	 * ֹͣ���пͻ������ӽ����ݱ��������̺�����������˳�
	 * @param save_data {bool} �Ƿ����˳�ǰ��������������
	 */
	void shutdown(bool save_data = true);

	/**
	 * ����ǰ������ת��Ϊָ���������Ĵ���������
	 * @param ip {const char*} ָ���������� IP
	 * @param port {int} ָ���������Ķ˿�
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool slaveof(const char* ip, int port);

	/**
	 * ��ѯ�����Ĳ�����־
	 * @param number {int} ���� 0 ʱ���޶���־�����������г�������־
	 * @return {const redis_result*}
	 */
	const redis_result* slowlog_get(int number = 0);

	/**
	 * ���Բ鿴��ǰ��־������
	 * @return {int}
	 */
	int slowlog_len();

	/**
	 * ������� slow log
	 * @return {bool}
	 */
	bool slowlog_reset();

	/**
	 * ���ص�ǰ������ʱ��
	 * @param stamp {time_t&} �洢ʱ���(�� UNIX ʱ�����ʽ��ʾ)
	 * @param escape {int*} �洢��ǰ��һ�����Ѿ���ȥ��΢����
	 */
	bool get_time(time_t& stamp, int& escape);
};

} // namespace acl
