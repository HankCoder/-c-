#include "lib_acl.h"

int main(void)
{
        ACL_IFCONF *ifconf;	/* ������ѯ������� */
        ACL_IFADDR *ifaddr;	/* ÿ��������Ϣ���� */
        ACL_ITER iter;		/* �������� */

	/* ��ѯ��������������Ϣ */
	ifconf = acl_get_ifaddrs();

	if (ifconf == NULL) {
		printf("acl_get_ifaddrs error: %s\r\n", acl_last_serror());
		return 1;
	}

	/* ����������������Ϣ */
	acl_foreach(iter, ifconf) {
		ifaddr = (ACL_IFADDR*) iter.data;
		printf(">>>ip: %s, name: %s\r\n", ifaddr->ip, ifaddr->name);
	}

	/* �ͷŲ�ѯ��� */
	acl_free_ifaddrs(ifconf);

	return 0;
}
