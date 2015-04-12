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

class ACL_CPP_API redis_pubsub : virtual public redis_command
{
public:
	/**
	 * see redis_command::redis_command()
	 */
	redis_pubsub();

	/**
	 * see redis_command::redis_command(redis_client*)
	 */
	redis_pubsub(redis_client* conn);

	/**
	 * see redis_command::redis_command(redis_client_cluster*�� size_t)
	 */
	redis_pubsub(redis_client_cluster* cluster, size_t max_conns);

	virtual ~redis_pubsub();

	/////////////////////////////////////////////////////////////////////

	/**
	 * ����Ϣ���͵�ָ����Ƶ�� channel
	 * post a message to a channel
	 * @param channel {const char*} ��������Ϣ��Ŀ��Ƶ��
	 *  the specified channel
	 * @param msg {const char*} ��Ϣ����
	 *  the message to be sent
	 * @param len {size_t} ��Ϣ����
	 *  the message's length
	 * @return {int} �ɹ����������ĸ�Ƶ���Ķ���������
	 *  the number of clients that received the message
	 *  -1����ʾ����
	 *      error happened
	 *   0��û�ж�����
	 *      no client subscribe the channel
	 *  >0�����ĸ�Ƶ���Ķ���������
	 *      the number of clients that received the message
	 */
	int publish(const char* channel, const char* msg, size_t len);

	/**
	 * ���ĸ�����һ������Ƶ������Ϣ���ڵ��ñ�������Ĳ���ֻ�ܷ��͵������У�
	 * subscribe��unsubscribe��psubscribe��punsubscribe��get_message��ֻ��
	 * ȡ������������Ƶ�����������ؽ�����Ű��Ѹ�����
	 * subscribe one or more channel(s). Once the client enters the
	 * subscribed state it is not supposed to issue any other commands,
	 * except for additional SUBSCRIBE, PSUBSCRIBE, UNSUBSCRIBE
	 * and PUNSUBSCRIBE commands
	 * @param first_channel {const char*} �����ĵ�Ƶ���б�ĵ�һ���ǿ��ַ���
	 *  ��Ƶ�������ڱ���б��е����һ�������� NULL
	 *  the first non-NULL channel in the channel list, and the last
	 *  parameter must be NULL indicating the end of the channel list
	 * @return {int} ���ص�ǰ�Ѿ��ɹ����ĵ�Ƶ���������������ĵ�����Ƶ��������
	 *  the number of channels subscribed by the current client
	 */
	int subscribe(const char* first_channel, ...);
	int subscribe(const std::vector<const char*>& channels);
	int subscribe(const std::vector<string>& channels);

	/**
	 * ȡ�����ĸ�����һ������Ƶ������Ϣ
	 * stop listening for messages posted to the given channels
	 * @param first_channel {const char*} ��ȡ����������Ƶ���б�ĵ�һ��Ƶ��
	 *  the fist channel in channel list, and the last parameter must be
	 *  NULL indicating the end of the channel list
	 * @return {int} ����ʣ��������ĵ�Ƶ���ĸ���
	 *  the rest channels listened by the current client
	 */
	int unsubscribe(const char* first_channel, ...);
	int unsubscribe(const std::vector<const char*>& channels);
	int unsubscribe(const std::vector<string>& channels);

	/**
	* ����һ���������ϸ���ģʽ��Ƶ����ÿ��ģʽ�� * ��Ϊƥ������ڵ��ñ�������Ĳ���
	* ֻ�ܷ��͵������У�subscribe��unsubscribe��psubscribe��punsubscribe��
	* get_message��ֻ��ȡ������������Ƶ�����������ؽ�����Ű��Ѹ�����
	* listen for messages published to channels matching the give patterns
	 * @param first_pattern {const char*} ��һ��ƥ��ģʽ��
	 *  the first pattern in pattern list, the last parameter must be NULL
	 *  int the variable args
	 * @return {int} ���ص�ǰ�Ѿ��ɹ����ĵ�Ƶ���������������ĵ�����Ƶ��������
	 *  the number of channels listened by the current client
	 */
	int psubscribe(const char* first_pattern, ...);
	int psubscribe(const std::vector<const char*>& patterns);
	int psubscribe(const std::vector<string>& patterns);

	/**
	 * ����ģʽƥ�䴮ȡ�����ĸ�����һ������Ƶ������Ϣ
	 * stop listening for messaged posted to channels matching
	 * the given patterns
	 * @param first_pattern {const char*} ��һ��ƥ��ģʽ��
	 *  the first parttern in a variable args ending with NULL
	 * @return {int} ����ʣ��������ĵ�Ƶ���ĸ���
	 *  the rest number of channels be listened by the client
	 */
	int punsubscribe(const char* first_pattern, ...);
	int punsubscribe(const std::vector<const char*>& patterns);
	int punsubscribe(const std::vector<string>& patterns);

	/**
	 * �ڶ���Ƶ�������ѭ�����ñ������������ĵ�Ƶ���л�ȡ������Ϣ��
	 * �ڵ��� subscribe �� psubscribe ��ſɵ��ñ���������ȡ�����ĵ�Ƶ������Ϣ
	 * get messages posted to channels after SUBSCRIBE or PSUBSCRIBE
	 * @param channel {string&} ��ŵ�ǰ����Ϣ��Ƶ����
	 *  buffer for storing the channel associate with the msg
	 * @param msg {string&} ��ŵ�ǰ��õ���Ϣ����
	 *  store the message posted to the channel
	 * @return {bool} �Ƿ�ɹ���������� false ���ʾ����
	 *  true on success, false on error
	 */
	bool get_message(string& channel, string& msg);

	/**
	 * �г���ǰ�Ļ�ԾƵ������ԾƵ��ָ������Щ������һ�������ߵ�Ƶ���� ����ģʽ��
	 * �ͻ��˲���������
	 * Lists the currently active channels.
	 * @param channels {std::vector<string>*} �ǿ�ʱ���Ƶ�������
	 *  store the active channels
	 * @param first_pattern {const char*} ��Ϊ���ӵ�ƥ��ģʽ��һ��ƥ���ַ�����
	 *  ��ָ�����Ϊ NULL����ʱ��ȡָ���еĻ�ԾƵ�������ڱ�ζ������һ��������Ϊ NULL
	 *  the first pattern in a variable args ending with NULL arg, and
	 *  the first arg can be NULL.
	 * @return {int} ���ػ�ԾƵ������ -1 ��ʾ����
	 *  the number of active channels. -1 if error
	 *
	 *  �����ɹ������ͨ��������һ��ʽ�������
	 *  1�����෽�� get_value ���ָ���±��Ԫ������
	 *  2�����෽�� get_child ���ָ���±��Ԫ�ض���(redis_result����Ȼ����ͨ��
	 *     redis_result::argv_to_string �������Ԫ������
	 *  3�����෽�� get_result ����ȡ���ܽ�������� redis_result��Ȼ����ͨ��
	 *     redis_result::get_child ���һ��Ԫ�ض���Ȼ����ͨ����ʽ 2 ��ָ��
	 *     �ķ�����ø�Ԫ�ص�����
	 *  4�����෽�� get_children ��ý��Ԫ�����������ͨ�� redis_result ��
	 *     �ķ��� argv_to_string ��ÿһ��Ԫ�ض����л��Ԫ������
	 *  5���ڵ��÷����д���ǿյĴ洢�������ĵ�ַ
	 *
	 */
	int pubsub_channels(std::vector<string>* channels,
		const char* first_pattern, ...);
	int pubsub_channels(const std::vector<const char*>& patterns,
		std::vector<string>* channels);
	int pubsub_channels(const std::vector<string>& patterns,
		std::vector<string>* channels);

	/**
	 * ���ظ���Ƶ���Ķ����������� ����ģʽ�Ŀͻ��˲���������
	 * Returns the number of subscribers (not counting clients
	 * subscribed to patterns) for the specified channels.
	 * @param out {std::map<string, int>&} �洢��ѯ��������� out->first ���
	 *  Ƶ������out->second ������Ƶ���Ķ���������
	 *  store the results
	 * @param first_pattern {const char*} ��Ϊ���ӵ�ƥ��ģʽ��һ��ƥ���ַ�����
	 *  ��ָ�����Ϊ NULL����ʱ��ȡָ���еĻ�ԾƵ�������ڱ�ζ������һ��������Ϊ NULL
	 *  the first pattern in a variable args ending with NULL arg, and
	 *  the first arg can be NULL.
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
	 * Returns the number of subscriptions to patterns.
	 * @return {int} �ͻ������ж���ģʽ���ܺͣ�-1 ��ʾ����
	 *  the number of patterns all the clients are subscribed to,
	 *  -1 if error.
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
