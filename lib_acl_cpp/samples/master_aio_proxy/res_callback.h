#pragma once

class req_callback;

class res_callback : public acl::aio_open_callback
{
public:
	res_callback(req_callback* req, acl::ofstream* res_fp);
	~res_callback();

	bool start(acl::aio_handle& handle, const char* server_addr);

	acl::aio_socket_stream& get_conn();
	void disconnect();
	void on_closed();

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

	/**
	 * �����麯��, ���첽���ӳɹ�����ô˺���
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	virtual bool open_callback();

private:
	acl::aio_socket_stream* conn_;
	req_callback* req_;
	acl::ofstream* res_fp_;
};
