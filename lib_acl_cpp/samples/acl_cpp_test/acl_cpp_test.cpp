// acl_cpp_test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "acl_cpp/acl_cpp_test.hpp"
#include "acl_cpp/stdlib/noncopyable.hpp"

using namespace acl;

class A : private noncopyable
{
public:
	A() {}
	virtual ~A() {}
protected:
private:
};

class B : public A
{
public:
	B() {}
	virtual ~B() {}
protected:
private:
};

class C : public B
{
public:
	C() {}
	~C() {}
protected:
private:
};

int main(int argc, char* argv[])
{
	test_snprintf();
#ifdef WIN32
	getchar();
#endif

	//A a1, a2;
	//a2 = a1;
	//B b1, b2(b1);
	//b2 = b2;
	//C c1, c2;
	//c2 = c1;

	return 0;
}
