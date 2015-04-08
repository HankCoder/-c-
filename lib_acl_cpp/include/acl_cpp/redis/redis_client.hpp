#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/connpool/connect_client.hpp"

namespace acl
{

class dbuf_pool;
class redis_result;
class redis_request;

/**
 * redis �ͻ��˶�������ͨ���࣬ͨ�����ཫ��֯�õ� redis ��������� redis ����ˣ�
 * ͬʱ���� redis �������Ӧ���������̳��� connect_client �࣬��ҪΪ��ʹ�����ӳ�
 * ���ܡ�
 * redis client network IO class. The redis request is sent to server
 * and the server's respond is handled in this class. The class inherits
 * connect_client, which can use the connection pool function.
 */
class ACL_CPP_API redis_client : public connect_client
{
public:
	/**
	 * ���캯��
	 * constructor
	 * @param addr {const char*} redis-server ������ַ
	 *  the redis-server listening addr
	 * @param conn_timeout {int} ���� redis-server �ĳ�ʱʱ��(��)
	 *  the timeout in seconds to connect the redis-server
	 * @param rw_timeout {int} �� redis-server ����ͨ�ŵ� IO ��ʱʱ��(��)
	 *  the network IO timeout in seconds with the redis-server
	 */
	redis_client(const char* addr, int conn_timeout = 60,
		int rw_timeout = 30, bool retry = true);
	~redis_client();

	/**
	 * �жϸ��������Ӷ����Ƿ��Ѿ��ر�
	 * check if the connection has been finish
	 * @return {bool}
	 */
	bool eof() const;

	/**
	 * �ر���������
	 * close the connection to the redis-server
	 */
	void close();

	/**
	 * �������������
	 * get acl::socket_stream from the connection
	 * @return {acl::socket_stream*} ��������Ѿ��ر��򷵻� NULL
	 *  NULL will be returned if the connectioin has been closed
	 */
	socket_stream* get_stream();

	/**
	 * �����������ݰ����˺�����������װ�������ݰ���ʱ��ϳ�һ�����ݰ�����
	 * just for request package, setting flag for sending data with
	 * multi data chunks; this is useful when the request data is large
	 * @param on {bool} ��Ϊ true ʱ�򲻻Ὣ�������ݺϳ�һ�����ݰ�����
	 *  if true the request data will not be combined one package
	 */
	void set_slice_request(bool on);

	/**
	 * ������Ӧ���ݰ����˺��������Ƿ� redis-server ��Ӧ�����ݷֲ�ɶ�����ݿ飬
	 * ����ڴ�����ݰ����ô������Բ���һ���Է���һ�������ԵĴ��ڴ�
	 * just for response package, settint flag for receiving data
	 * if split the large response data into multi little chunks
	 * @param on {bool} ��Ϊ true ʱ�����Ӧ���ݰ����в��
	 *  if true the response data will be splitted into multi little
	 *  data, which is useful for large reponse data for avoiding
	 *  malloc large continuously memory from system.
	 */
	void set_slice_respond(bool on);

	/**
	 * ���ڷǷ�Ƭ���ͷ�ʽ���� redis-server �����������ݣ�ͬʱ��ȡ�����������
	 * ���ص���Ӧ����
	 * send request to redis-server, and read/anlyse response from server,
	 * this function will be used for no-slice request mode.
	 * @param pool {dbuf_pool*} �ڴ�ع���������
	 *  memory pool manager
	 * @param req {const string&} �������ݰ�
	 *  the request package
	 * @param nchildren {size_t} ��Ӧ�����м������ݶ���
	 *  the data object number in the server's response data
	 * @return {const redis_result*} �����ķ�������Ӧ���󣬷��� NULL �����,
	 *  �ö��󲻱��ֹ��ͷţ���Ϊ������ pool �ڴ�ض����϶�̬����ģ����Ե��ͷ� pool
	 *  ʱ�ý������һͬ���ͷ�
	 *  the result object from server's response, NULL will be returned
	 *  when some error happens; the result object needn't be freed
	 *  manually, which was created in the pool object, and will be freed
	 *  when the pool were freed.
	 *  
	 */
	const redis_result* run(dbuf_pool* pool, const string& req,
		size_t nchildren);

	/**
	 * ���ڷ�Ƭ��������ʽ
	 * just for sending proccess in slice request mode
	 * @param req {const redis_request&} �������ݶ���
	 *  request object
	 */
	const redis_result* run(dbuf_pool* pool, const redis_request& req,
		size_t nchildren);

protected:
	// �����麯��
	virtual bool open();

private:
	socket_stream conn_;
	char* addr_;
	int   conn_timeout_;
	int   rw_timeout_;
	bool  retry_;
	string  buf_;
	bool slice_req_;
	bool slice_res_;

	redis_result* get_redis_objects(dbuf_pool* pool, size_t nobjs);
	redis_result* get_redis_object(dbuf_pool* pool);
	redis_result* get_redis_error(dbuf_pool* pool);
	redis_result* get_redis_status(dbuf_pool* pool);
	redis_result* get_redis_integer(dbuf_pool* pool);
	redis_result* get_redis_string(dbuf_pool* pool);
	redis_result* get_redis_array(dbuf_pool* pool);

	void put_data(dbuf_pool* pool, redis_result* rr,
		const char* data, size_t len);
};

} // end namespace acl
