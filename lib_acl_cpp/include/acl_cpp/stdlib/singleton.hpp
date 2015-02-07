#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <assert.h>
#include "acl_cpp/stdlib/noncopyable.hpp"

//  singleton.hpp
//
// Copyright David Abrahams 2006. Original version
//
// Copyright Robert Ramey 2007.  Changes made to permit
// application throughout the serialization library.
//
// Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// The intention here is to define a template which will convert
// any class into a singleton with the following features:
//
// a) initialized before first use.
// b) thread-safe for const access to the class
// c) non-locking
//
// In order to do this,
// a) Initialize dynamically when used.
// b) Require that all singletons be initialized before main
// is called or any entry point into the shared library is invoked.
// This guarentees no race condition for initialization.
// In debug mode, we assert that no non-const functions are called
// after main is invoked.

namespace acl {

#ifdef WIN32
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

//////////////////////////////////////////////////////////////////////
// Provides a dynamically-initialized (singleton) instance of T in a
// way that avoids LNK1179 on vc6.  See http://tinyurl.com/ljdp8 or
// http://lists.boost.org/Archives/boost/2006/05/105286.php for
// details.
//

// singletons created by this code are guarenteed to be unique
// within the executable or shared library which creates them.
// This is sufficient and in fact ideal for the serialization library.
// The singleton is created when the module is loaded and destroyed
// when the module is unloaded.

// This base class has two functions.

// First it provides a module handle for each singleton indicating
// the executable or shared library in which it was created. This
// turns out to be necessary and sufficient to implement the tables
// used by serialization library.

// Second, it provides a mechanism to detect when a non-const function
// is called after initialization.

// make a singleton to lock/unlock all singletons for alteration.
// The intent is that all singletons created/used by this code
// are to be initialized before main is called. A test program
// can lock all the singletons when main is entereed.  This any
// attempt to retieve a mutable instances while locked will
// generate a assertion if compiled for debug.

class singleton_module : public noncopyable
{
public:
	static void lock()
	{
		get_lock() = true;
	}

	static void unlock()
	{
		get_lock() = false;		
	}

	static bool is_locked() {
		return get_lock();
	}
private:
	static bool& get_lock()
	{
		static bool lock_ = false;
		return lock_;
	}
};

template<class T>
class singleton_wrapper : public T
{
public:
	static bool destroyed_;
	~singleton_wrapper()
	{
		destroyed_ = true;
	}
};

template<class T>
bool singleton_wrapper< T >::destroyed_ = false;

/**
 * ����ģ���࣬��VC2010��GCC����ʱ������������ main ����֮ǰ��ִ�У�
 * ���������̰߳�ȫ�ģ����� VC2003 ����� release �汾ʱ�Ҵ����Ż�
 * ���أ����п������̲߳���ȫ�ģ���ʱ���ܱ�֤��������Ĺ��캯����
 * main ֮ǰִ��.
 * ʹ�þ������£�
 * class singleton_test : public acl::singleton<singlegon_test>
 * {
 * public:
 *   singleton_test() {}
 *   ~singleton_test() {}
 *   singleton_test& init() { return *this; }
 * };

 * int main()
 * {
 *   singleton_test& test = singleton_test::get_instance();
 *   test.init();
 *   ...
 *   return 0;
 * }
 */
template <class T>
class singleton : public singleton_module
{
public:
	static T& get_instance()
	{
		static singleton_wrapper< T > t;
		// refer to instance, causing it to be instantiated (and
		// initialized at startup on working compilers)
		assert(!singleton_wrapper< T >::destroyed_);
		use(instance_);
		return static_cast<T &>(t);
	}

	static bool is_destroyed()
	{
		return singleton_wrapper< T >::destroyed_;
	}

private:
	static T& instance_;
	// include this to provoke instantiation at pre-execution time
	static void use(T const &) {}
};

template<class T>
T& singleton< T >::instance_ = singleton< T >::get_instance();

//////////////////////////////////////////////////////////////////////////

/**
 * �����ʵ���� VC2003 �� release ����ʱ��������Ż����أ����ܱ�֤����
 * �Ĺ��캯������ main ִ�У�������� VC2003 �±��뵥���������ڶ���߳���
 * ���õ�������ʱ������ʹ�����µĵ���ģ���࣬ʾ�����£�
 * class singleton_test
 * {
 * public:
 *   singleton_test() {}
 *   ~singleton_test() {}
 *   singleton_test& init() { return *this; }
 * };

 * int main()
 * {
 *   singleton_test& test = acl::singleton2<singleton_test>::get_instance();
 *   test.init();
 *   ...
 *   return 0;
 * }
 * 
 */
template <typename T>
struct singleton2
{
private:
	struct object_creator
	{
		object_creator() { singleton2<T>::get_instance(); }
		inline void do_nothing() const {};
	};
	static object_creator create_object;

public:
	typedef T object_type;
	static object_type & get_instance()
	{
		static object_type obj;
		create_object.do_nothing();
		return obj;
	}
};

template <typename T>
typename singleton2<T>::object_creator singleton2<T>::create_object;

#ifdef WIN32
#pragma warning(pop)
#endif

} // namespace acl
