#pragma once

class res_callback;

class req_callback : public acl::aio_callback
{
public:
	req_callback(acl::aio_socket_stream* conn,
		acl::ofstream* req_fp, acl::ofstream* res_fp);
	~req_callback();

	void start(const char* server_addr);

	void disconnect();
	void on_connected();

	acl::aio_socket_stream& get_conn();

protected:
	/** 
	 * ʵ�ָ����е��麯�����ͻ������Ķ��ɹ��ص����� 
	 * @param data {char*} ���������ݵ�ַ 
	 * @param len {int} ���������ݳ��� 
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽�� 
	 */
	virtual bool read_callback(char* data, int len);

	/** 
	 * ʵ�ָ����е��麯�����ͻ�������д�ɹ��ص����� 
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽�� 
	 */  
	virtual bool write_callback()
	{
		return true;  
	}

	/** 
	 * ʵ�ָ����е��麯�����ͻ������Ĺرջص����� 
	 */  
	virtual void close_callback();

	/** 
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص����� 
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽�� 
	 */  
	virtual bool timeout_callback()
	{
		// ���� false ֪ͨ�첽��ܹرո��첽��
		return false;
	}

private:
	acl::aio_socket_stream* conn_;
	res_callback* res_;
	acl::ofstream* req_fp_;
	acl::ofstream* res_fp_;
};
