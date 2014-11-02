#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <list>

struct ACL_HTABLE;

namespace acl {

class hsclient;
class locker;

class ACL_CPP_API hspool
{
public:
	/**
	 * ���캯��
	 * @param addr_rw {const char*} handlersocket ����� Mysql �ϵļ�����ַ��
	 * ��ʽΪ��ip:port��ע���õ�ַ�� handlersocket �Ķ�д��ַ
	 * @param addr_rd {const char*} handlersocket ����� Mysql �ϵļ�����ַ��
	 * ��ʽΪ��ip:port��ע���õ�ַ�� handlersocket ��ֻ����ַ
	 * @param cache_enable {bool} �Ƿ��ڲ��Զ������ж��󻺴����
	 * @param retry_enable {bool} ����Ϊ����ԭ�������ʱ�Ƿ���Ҫ��������
	 */
	hspool(const char* addr_rw, const char* addr_rd = NULL,
		bool cache_enable = true, bool retry_enable = true);

	~hspool();

	/**
	 * �����ӳ��л�����Ӷ���
	 * @param dbn {const char*} ���ݿ�����
	 * @param tbl {const char*} ���ݿ����
	 * @param idx {const char*} �����ֶ���
	 * @param flds {const char*} Ҫ�򿪵������ֶ������ϣ���ʽΪ
	 *  �ɷָ��� ",; \t" �ָ����ֶ����ƣ��磺user_id,user_name,user_mail
	 * @param readonly {bool} �Ƿ����ֻ����ʽ��
	 */
	hsclient* peek(const char* dbn, const char* tbl,
		const char* idx, const char* flds, bool readonly = false);

	/**
	 * �黹���Ӷ���
	 * @param client {hsclient*}
	 */
	void put(hsclient* client);
private:
	char* addr_rw_;
	char* addr_rd_;
	bool cache_enable_;
	bool retry_enable_;
	std::list<hsclient*> pool_;
	locker* locker_;
};

}  // namespace acl
