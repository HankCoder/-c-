#pragma once

namespace acl_min
{

class connect_client
{
public:
	connect_client() : when_(0) {}
	virtual ~connect_client() {}

	/**
	 * ��ø����Ӷ������һ�α�ʹ�õ�ʱ���
	 * @return {time_t}
	 */
	time_t get_when() const
	{
		return when_;
	}

	/**
	 * ���ø����Ӷ���ǰ��ʹ�õ�ʱ���
	 */
	void set_when(time_t when)
	{
		when_ = when;
	}

	/**
	 * ���麯�����������ʵ�ִ˺����������ӷ�����
	 * @return {bool} �Ƿ����ӳɹ�
	 */
	virtual bool open() = 0;
private:
	time_t when_;
};

} // namespace acl_min
