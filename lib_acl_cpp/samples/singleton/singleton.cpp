// singleton.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "acl_cpp/stdlib/singleton.hpp"

// ���ٵ��ù��̵ļ�����
static int __nstep = 0;

class singleton_test : public acl::singleton <singleton_test>
{
public:
	singleton_test()
	{
		// ����þ仰��ӡ���� main �����еĴ�ӡ�������
		// ˵���õ������� main ����ִ���ȱ���ʼ����
		printf("step %d: singleton_test construct called\r\n", ++__nstep);
		fflush(stdout);
	}

	virtual ~singleton_test()
	{
		printf("step %d: singleton_test destruct called\r\n", ++__nstep);
		fflush(stdout);
		getchar();
	}

	const singleton_test& init() const
	{
		printf("step %d: singleton_test init called\r\n", ++__nstep);
		fflush(stdout);
		return *this;
	}

	const singleton_test& set(const char* name) const
	{
		printf("step %d: singleton_test set(%s) called\r\n", ++__nstep, name);
		fflush(stdout);
		return *this;
	}
};

//////////////////////////////////////////////////////////////////////////

class singleton_test2
{
public:
	singleton_test2()
	{
		// ����þ仰��ӡ���� main �����еĴ�ӡ�������
		// ˵���õ������� main ����ִ���ȱ���ʼ����
		printf("step %d: singleton_test2 construct called\r\n", ++__nstep);
		fflush(stdout);
	}

	virtual ~singleton_test2()
	{
		printf("step %d: singleton_test2 destruct called\r\n", ++__nstep);
		fflush(stdout);
		getchar();
	}

	const singleton_test2& init() const
	{
		printf("step %d: singleton_test2 init called\r\n", ++__nstep);
		fflush(stdout);
		return *this;
	}

	const singleton_test2& set(const char* name) const
	{
		printf("step %d: singleton_test2 set(%s) called\r\n", ++__nstep, name);
		fflush(stdout);
		return *this;
	}
};

//singleton_test::get_const_instance();
int main()
{
	printf("step %d: first line in main\r\n", ++__nstep);
	fflush(stdout);

	// ����һ
	const singleton_test& test1 = singleton_test::get_instance().init();
	const singleton_test& test2 = singleton_test::get_instance();
	test1.set("test1");
	test2.set("test2");

	// ���������� VC2003 ����� release �汾ʱ��
	// �÷�ʽ���Ա�֤����ʵ���� main ֮ǰ������
	acl::singleton2<singleton_test2>::get_instance().init();
	acl::singleton2<singleton_test2>::get_instance().set("test1");
	acl::singleton2<singleton_test2>::get_instance().set("test2");
	return 0;
}
