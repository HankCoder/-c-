#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <map>
#include "acl_cpp/redis/redis_command.hpp"

namespace acl
{

class redis_client;
class redis_result;
class string;

class ACL_CPP_API redis_pubsub : public redis_command
{
public:
	redis_pubsub(redis_client* conn = NULL);
	~redis_pubsub();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ����Ϣ���͵�ָ����Ƶ�� channel
	 * @param channel {const char*} ��������Ϣ��Ŀ��Ƶ��
	 * @param msg {const char*} ��Ϣ����
	 * @param len {size_t} ��Ϣ����
	 * @return {int} �ɹ����������ĸ�Ƶ���Ķ���������
	 *  -1����ʾ����
	 *   0��û�ж�����
	 *  >0�����ĸ�Ƶ���Ķ���������
	 */
	int publish(const char* channel, const char* msg, size_t len);

	/**
	 * ���ĸ�����һ������Ƶ������Ϣ���ڵ��ñ�������Ĳ���ֻ�ܷ��͵������У�
	 * subscribe��unsubscribe��psubscribe��punsubscribe��get_message��ֻ��
	 * ȡ������������Ƶ�����������ؽ�����Ű��Ѹ�����
	 * @param first_channel {const char*} �����ĵ�Ƶ���б�ĵ�һ���ǿ��ַ���
	 *  ��Ƶ�������ڱ���б��е����һ�������� NULL
	 * @return {int} ���ص�ǰ�Ѿ��ɹ����ĵ�Ƶ���������������ĵ�����Ƶ��������
	 */
	int subscribe(const char* first_channel, ...);
	int subscribe(const std::vector<const char*>& channels);
	int subscribe(const std::vector<string>& channels);

	/**
	 * ȡ�����ĸ�����һ������Ƶ������Ϣ
	 * @param first_channel {const char*} ��ȡ����������Ƶ���б�ĵ�һ��Ƶ��
	 * @return {int} ����ʣ��������ĵ�Ƶ���ĸ���
	 */
	int unsubscribe(const char* first_channel, ...);
	int unsubscribe(const std::vector<const char*>& channels);
	int unsubscribe(const std::vector<string>& channels);

	/**
	* ����һ���������ϸ���ģʽ��Ƶ����ÿ��ģʽ�� * ��Ϊƥ������ڵ��ñ�������Ĳ���
	* ֻ�ܷ��͵������У�
	* subscribe��unsubscribe��psubscribe��punsubscribe��get_message��ֻ��
	* ȡ������������Ƶ�����������ؽ�����Ű��Ѹ�����
	 * @param first_pattern {const char*} ��һ��ƥ��ģʽ��
	 * @return {int} ���ص�ǰ�Ѿ��ɹ����ĵ�Ƶ���������������ĵ�����Ƶ��������
	 */
	int psubscribe(const char* first_pattern, ...);
	int psubscribe(const std::vector<const char*>& patterns);
	int psubscribe(const std::vector<string>& patterns);

	/**
	 * ����ģʽƥ�䴮ȡ�����ĸ�����һ������Ƶ������Ϣ
	 * @param first_pattern {const char*} ��һ��ƥ��ģʽ��
	 * @return {int} ����ʣ��������ĵ�Ƶ���ĸ���
	 */
	int punsubscribe(const char* first_pattern, ...);
	int punsubscribe(const std::vector<const char*>& patterns);
	int punsubscribe(const std::vector<string>& patterns);

	/**
	 * �ڶ���Ƶ�������ѭ�����ñ������������ĵ�Ƶ���л�ȡ������Ϣ���ڵ��� subscribe
	 * �� psubscribe ��ſɵ��ñ���������ȡ�����ĵ�Ƶ������Ϣ
	 * @param channel {string&} ��ŵ�ǰ����Ϣ��Ƶ����
	 * @param msg {string&} ��ŵ�ǰ��õ���Ϣ����
	 * @return {bool} �Ƿ�ɹ���������� false ���ʾ����
	 */
	bool get_message(string& channel, string& msg);

	/**
	 * �г���ǰ�Ļ�ԾƵ������ԾƵ��ָ������Щ������һ�������ߵ�Ƶ���� ����ģʽ��
	 * �ͻ��˲���������
	 * @param channels {std::vector<string>&} ���Ƶ�������
	 * @param first_pattern {const char*} ��Ϊ���ӵ�ƥ��ģʽ��һ��ƥ���ַ�����
	 *  ��ָ�����Ϊ NULL����ʱ��ȡָ���еĻ�ԾƵ�������ڱ�ζ������һ��������Ϊ NULL
	 * @return {int} ���ػ�ԾƵ������ -1 ��ʾ����
	 *
	 */
	int pubsub_channels(std::vector<string>& channels,
		const char* first_pattern, ...);
	int pubsub_channels(const std::vector<const char*>& patterns,
		std::vector<string>& channels);
	int pubsub_channels(const std::vector<string>& patterns,
		std::vector<string>& channels);

	/**
	 * ���ظ���Ƶ���Ķ����������� ����ģʽ�Ŀͻ��˲���������
	 * @param out {std::map<string, int>&} �洢��ѯ��������� out->first ���
	 *  Ƶ������out->second ������Ƶ���Ķ���������
	 * @param first_pattern {const char*} ��Ϊ���ӵ�ƥ��ģʽ��һ��ƥ���ַ�����
	 *  ��ָ�����Ϊ NULL����ʱ��ȡָ���еĻ�ԾƵ�������ڱ�ζ������һ��������Ϊ NULL
	 * @return {int} Ƶ����������-1 ��ʾ����
	 */
	int pubsub_numsub(std::map<string, int>& out,
		const char* first_channel, ...);
	int pubsub_numsub(const std::vector<const char*>& channels,
		std::map<string, int>& out);
	int pubsub_numsub(const std::vector<string>& channels,
		std::map<string, int>& out);

	/**
	 * ���ض���ģʽ���������������صĲ��Ƕ���ģʽ�Ŀͻ��˵������� ���ǿͻ��˶��ĵ�
	 * ����ģʽ�������ܺ�
	 * @return {int} �ͻ������ж���ģʽ���ܺͣ�-1 ��ʾ����
	 */
	int pubsub_numpat();

private:
	int subop(const char* cmd, const std::vector<const char*>& channels);
	int subop(const char* cmd, const std::vector<string>& channels);
	int check_channel(const redis_result* obj, const char* cmd,
		const string& channel);
	int pubsub_numsub(std::map<string, int>& out);
};

} // namespace acl
