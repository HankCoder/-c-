#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

/**
 * ������ IO ע��ص��࣬������ʵ�ָ����е��鷽�����������ͨ�� setup_hook ע�ᣬ
 * Ȼ�����������е� IO ������Ϊ stream/aio_stream �����ĵײ� IO ���̱�ʹ�ã�
 * ��������� stream/aio_stream �� setup_hook ע����̣��� stream/aio_stream
 * �����ĵײ� IO ����ΪĬ�Ϲ���
 * XXX�� �������౻����Ϊ�Ѷ����࣬��������ҲӦ������Ϊ�Ѷ�����
 */
class ACL_CPP_API stream_hook
{
public:
	stream_hook() {}

	/**
	 * �����ݽӿ�
	 * @param buf {void*} ����������ַ�����������ݽ�����ڸû�������
	 * @param len {size_t} buf ��������С
	 * @return {int} �����ֽ�����������ֵ < 0 ʱ��ʾ����
	 */
	virtual int read(void* buf, size_t len) = 0;

	/**
	 * �������ݽӿ�
	 * @param buf {const void*} ���ͻ�������ַ
	 * @param len {size_t} buf �����������ݵĳ���(���� > 0)
	 * @return {int} д������ݳ��ȣ�����ֵ <����ʱ��ʾ����
	 */
	virtual int send(const void* buf, size_t len) = 0;

	/**
	 * �� stream/aio_stream �� setup_hook �ڲ�������� stream_hook::open
	 * ���̣��Ա����������������ʼ��һЩ���ݼ��Ự
	 * @param s {ACL_VSTREAM*} �� setup_hook �ڲ����ø÷�����������������
	 *  ��Ϊ��������
	 * @return {bool} �������ʵ������ false���� setup_hook ����ʧ���һ�ָ�ԭ��
	 */
	virtual bool open(ACL_VSTREAM* s) = 0;

	/**
	 * �� stream/aio_stream ������ر�ǰ����ص��ú����Ա�������ʵ����һЩ�ƺ���
	 * @param alive {bool} �������Ƿ���Ȼ����
	 * @return {bool}
	 */
	virtual bool on_close(bool alive) { (void) alive; return true; }

	/**
	 * �� stream/aio_stream ������Ҫ�ͷ� stream_hook �������ʱ���ô˷���
	 */
	virtual void destroy() {}

protected:
	virtual ~stream_hook() {}
};

} // namespace acl
