#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

#ifdef WIN32
template<typename TDerive, typename TProvider>
class final_tpl_base
{
	friend TDerive;
	friend TProvider;
private:
	final_tpl_base() {}
	~final_tpl_base() {}
};
#else
template <typename T>
class identity
{
public:
	typedef T me;
};

template<typename TDerive, typename TProvider>
class final_tpl_base
{
	friend class identity<TDerive>::me;
	friend class identity<TProvider>::me;
private:
	final_tpl_base() {}
	~final_tpl_base() {}
};
#endif

/*
 * �ṩ��ֹ�����Ĺ���,��Ҫ�˹��ܵ�����Դ�final_tpl����,
 * ����������Ϊģ���������
 * @example:
 * class my_final_class : public acl::final_tpl <my_final_class>
 * {
 * public:
 *   my_final_class() {}
 *   ~my_final_class() {}
 * }
 * �����ͱ�֤�� my_final_class �ǲ��ܱ��̳е�
 */
template<typename TFinalClass>
class final_tpl : virtual public final_tpl_base<TFinalClass,
	final_tpl<TFinalClass> >
{
public:
	final_tpl() {}
	~final_tpl() {}
};

}
