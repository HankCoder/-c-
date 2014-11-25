#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <vector>
#include <list>
#include <utility>

struct ACL_VSTRING;
struct ACL_LINE_STATE;

namespace acl {

/**
 * ����Ϊ�ַ��������֧࣬�ִ󲿷� std::string �еĹ��ܣ�ͬʱ֧���䲻֧�ֵ�һЩ
 * ���ܣ������ڲ��Զ���֤���һ���ַ�Ϊ \0
 */
class ACL_CPP_API string
{
public:
	/**
	 * ���캯��
	 * @param n {size_t} ��ʼʱ������ڴ��С
	 * @param bin {bool} �Ƿ��Զ����Ʒ�ʽ�������������󣬸�ֵΪ true ʱ��
	 *  �򵱵��� += int|int64|short|char ����� << int|int64|short|char
	 *  ʱ���򰴶����Ʒ�ʽ���������ı���ʽ����
	 */
	string(size_t n = 64, bool bin = false);

	/**
	 * ���캯��
	 * @param s {const string&} Դ�ַ������󣬳�ʼ�����������ڲ��Զ�����
	 *  ���ַ���
	 */
	string(const string& s);

	/**
	 * ���캯��
	 * @param s {const char*} �ڲ��Զ��ø��ַ�����ʼ�������s ������
	 *  �� \0 ��β
	 */
	string(const char* s);

	/**
	 * ���캯��
	 * @param s {const char*} Դ��������
	 * @param n {size_t} s ���������ݳ���
	 */
	string(const void* s, size_t n);
	~string(void);

	/**
	 * �����ַ��������Ϊ�����ƴ���ģʽ
	 * @param bin {bool} ����ֵΪ true ʱ���������ַ��������Ϊ�����ƴ���
	 *  ��ʽ������Ϊ�ı���ʽ��Ϊ true ʱ���򵱵��� += int|int64|short|char
	 *  ����� << int|int64|short|char ʱ���򰴶����Ʒ�ʽ���������ı�
	 *  ��ʽ����
	 * @return {string&}
	 */
	string& set_bin(bool bin);

	/**
	 * ���û���������󳤶ȣ��Ա��⻺�������
	 * @param max {int}
	 * @return {string&}
	 */
	string& set_max(int  max);

	/**
	 * �жϵ�ǰ�ַ���������Ƿ�Ϊ�����ƴ���ʽ 
	 * @return {bool} ����ֵΪ true ʱ���ʾΪ�����Ʒ�ʽ
	 */
	bool get_bin() const;

	/**
	 * ���ص�ǰ����������󳤶����ƣ�������ֵ <= 0 ���ʾû������
	 * @return {int}
	 */
	int get_max(void) const;

	/**
	 * �����ַ������±���ָ��λ�õ��ַ��������������Ϊ�Ϸ�ֵ��������
	 * �ڲ���������
	 * @param n {size_t} ָ����λ�ã���ֵ >= 0 �� < �ַ�������)�����Խ��
	 *  ���������
	 * @return {char} ����ָ��λ�õ��ַ�
	 */
	char operator[](size_t n) const;

	/**
	 * �����ַ������±���ָ��λ�õ��ַ��������������Ϊ�Ϸ�ֵ��������
	 * �ڲ���������
	 * @param n {int} ָ����λ�ã���ֵ >= 0 �� < �ַ�������)�����Խ�磬
	 *  ���������
	 * @return {char} ����ָ��λ�õ��ַ�
	 */
	char operator[](int n) const;

	/**
	 * ��ֵ��ֵ���أ��û�����ֱ��ʹ�ö���������±���и�ֵ������±���ֵ
	 * Խ�磬���ڲ����Զ����仺�����ռ�
	 * @param n {size_t} �����±�λ��
	 * @return {char&}
	 */
	char& operator[](size_t n);

	/**
	 * ��ֵ��ֵ���أ��û�����ֱ��ʹ�ö���������±���и�ֵ������±���ֵ
	 * Խ�磬���ڲ����Զ����仺�����ռ�
	 * @param n {int} �����±�λ�ã���ֵ���� >= 0
	 * @return {char&}
	 */
	char& operator[](int n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param s {const char*} Դ�ַ���
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(const char* s);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param s {const string&} Դ�ַ�������
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(const string& s);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param s {const string*} Դ�ַ�������
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(const string* s);

#ifdef WIN32
	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {long long int} Դ 64 λ���ų�����������ǰ����ĵ�ǰ״̬Ϊ
	 *  ������ģʽ����ú�������Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���
	 *  ʽ��ֵ���ַ������󣻹��ڶ�����ģʽ�����ı���ʽ���京��μ�
	 *  set_bin(bool)
	 * @return {string&} ���ص�ǰ�ֶ�������ã����ڶԸ�������������в���
	 */
	string& operator=(__int64 n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {unsinged long long int} Դ 64 λ�޷��ų����������ַ�������
	 *  �ĵ�ǰ״̬Ϊ������ģʽ����ú�������Զ����Ʒ�ʽ��ֵ���ַ�������
	 *  �������ı���ʽ��ֵ���ַ������󣻹��ڶ�����ģʽ�����ı���ʽ���京��
	 *  �μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(unsigned __int64);
#else
	string& operator=(long long int);
	string& operator=(unsigned long long int);
#endif

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�з����ַ������ַ�������ĵ�ǰ״̬Ϊ������ģʽ��
	 *  ��ú�������Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ����
	 *  �������󣻹��ڶ�����ģʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(char n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�޷����ַ���������ĵ�ǰ״̬Ϊ������ģʽ����ú���
	 *  ����Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ�������
	 *  ���ڶ�����ģʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(unsigned char n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�з��ų����ͣ�������ǰ״̬Ϊ������ģʽ����ú���
	 *  ����Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ�������
	 *  ���ڶ�����ģʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(long n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�޷��ų����ͣ����ֶ���ĵ�ǰ״̬Ϊ������ģʽ��
	 *  ��ú�������Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ��
	 *  �ַ������󣻹��ڶ�����ģʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(unsigned long n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�з������ͣ����ַ�������ĵ�ǰ״̬Ϊ������ģʽ��
	 *  ��ú�������Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ�
	 *  �����󣻹��ڶ�����ģʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(int n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�޷������ͣ����ַ�������ĵ�ǰ״̬Ϊ������ģʽ��
	 *  ��ú�������Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ�
	 *  �����󣻹��ڶ�����ģʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(unsigned int n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�з��Ŷ����ͣ����ַ�������ĵ�ǰ״̬Ϊ������ģʽ,
	 *  ��ú�������Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ�
	 *  �����󣻹��ڶ�����ģʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(short n);

	/**
	 * ��Ŀ���ַ��������ֵ
	 * @param n {char} Դ�޷��Ŷ����ͣ�������ĵ�ǰ״̬Ϊ������ģʽ�����
	 *  ��������Զ����Ʒ�ʽ��ֵ���ַ������󣬷������ı���ʽ��ֵ���ַ���
	 *  ���󣻹��ڶ�����ģʽ�����ı���ʽ���京��μ� set_bin(bool)
	 * @return {string&} ���ص�ǰ��������ã����ڶԸ�������������в���
	 */
	string& operator=(unsigned short n);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const char*} Դ�ַ���ָ��
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(const char* s);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const string&} Դ�ַ�����������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(const string& s);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const string*} Դ�ַ�������ָ��
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(const string* s);
#ifdef WIN32
	/**
	 * ��Ŀ���ַ�������β������з��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long long int} Դ 64 λ�з�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(__int64 n);

	/**
	 * ��Ŀ���ַ�������β������޷��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long long int} Դ 64 λ�޷�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(unsigned __int64 n);
#else
	string& operator+=(long long int n);
	string& operator+=(unsigned long long int n);
#endif

	/**
	 * ��Ŀ���ַ�������β������з��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з��ų�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(long n);

	/**
	 * ��Ŀ���ַ�������β������޷��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷��ų�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(unsigned long n);

	/**
	 * ��Ŀ���ַ�������β������з����������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(int n);

	/**
	 * ��Ŀ���ַ�������β������޷����������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(unsigned int n);

	/**
	 * ��Ŀ���ַ�������β������з��Ŷ��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з��Ŷ�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(short n);

	/**
	 * ��Ŀ���ַ�������β������޷��Ŷ��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷��Ŷ�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(unsigned short n);

	/**
	 * ��Ŀ���ַ�������β������з����ַ�����ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з����ַ�
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(char n);

	/**
	 * ��Ŀ���ַ�������β������޷����ַ�����ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷����ַ�
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator+=(unsigned char n);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const string&} Դ�ַ�����������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(const string& s);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const string*} Դ�ַ�������ָ��
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(const string* s);

	/**
	 * ��Ŀ���ַ�������β������ַ���
	 * @param s {const char*} Դ�ַ���ָ��
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(const char* s);
#ifdef WIN32
	/**
	 * ��Ŀ���ַ�������β������з��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long long int} Դ 64 λ�з�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(__int64 n);

	/**
	 * ��Ŀ���ַ�������β������޷��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long long int} Դ 64 λ�޷�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(unsigned __int64 n);
#else
	string& operator<<(long long int n);
	string& operator<<(unsigned long long int n);
#endif

	/**
	 * ��Ŀ���ַ�������β������з��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з��ų�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(long n);

	/**
	 * ��Ŀ���ַ�������β������޷��ų��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷��ų�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(unsigned long n);

	/**
	 * ��Ŀ���ַ�������β������з����������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(int n);

	/**
	 * ��Ŀ���ַ�������β������޷����������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷�������
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(unsigned int n);

	/**
	 * ��Ŀ���ַ�������β������з��Ŷ��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з��Ŷ�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(short n);

	/**
	 * ��Ŀ���ַ�������β������޷��Ŷ��������֣���ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷��Ŷ�����
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(unsigned short n);

	/**
	 * ��Ŀ���ַ�������β������з����ַ�����ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�з����ַ�
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(char n);

	/**
	 * ��Ŀ���ַ�������β������޷����ַ�����ΪĿ���ַ�������Ϊ
	 * �����Ʒ�ʽʱ���򰴶��������ַ�ʽ��ӣ������ı���ʽ���
	 * @param n {long} Դ�޷����ַ�
	 * @return {string&} Ŀ���ַ������������
	 */
	string& operator<<(unsigned char n);

	/**
	 * ���ַ��������е����ݸ���Ŀ���ַ�������
	 * @param s {string*} Ŀ���ַ�������
	 * @return {size_t} ���ؿ�����ʵ���ֽ�����empty() == true ʱ���򷵻� 0
	 */
	size_t operator>>(string* s);
#ifdef WIN32
	/**
	 * ���ַ��������е����ݸ���Ŀ�� 64 λ�з�������
	 * @param n {string*} Ŀ�� 64 λ�з�������
	 * @return {size_t} ���ؿ�����ʵ���ֽ�����empty() == true ʱ���򷵻� 0
	 */
	size_t operator>>(__int64& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 64 λ�޷�������
	 * @param n {string*} Ŀ�� 64 λ�޷�������
	 * @return {size_t} ���ؿ�����ʵ���ֽ�����empty() == true ʱ���򷵻� 0
	 */
	size_t operator>>(unsigned __int64& n);
#else
	size_t operator>>(long long int&);
	size_t operator>>(unsigned long long int&);
#endif

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 32 λ�з�������
	 * @param n {string*} Ŀ�� 32 λ�з�������
	 * @return {size_t} ���ؿ�����ʵ���ֽ�����empty() == true ʱ���򷵻� 0
	 */
	size_t operator>>(int& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 32 λ�޷�������
	 * @param n {string*} Ŀ�� 32 λ�޷�������
	 * @return {size_t} ���ؿ�����ʵ���ֽ�����empty() == true ʱ���򷵻� 0
	 */
	size_t operator>>(unsigned int& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 16 λ�з�������
	 * @param n {string*} Ŀ�� 16 λ�з�������
	 * @return {size_t} ���ؿ�����ʵ���ֽ�����empty() == true ʱ���򷵻� 0
	 */
	size_t operator>>(short& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 16 λ�޷�������
	 * @param n {string*} Ŀ�� 16 λ�޷�������
	 * @return {size_t} ���ؿ�����ʵ���ֽ�����empty() == true ʱ���򷵻� 0
	 */
	size_t operator>>(unsigned short& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 8 λ�з����ַ�
	 * @param n {string*} Ŀ�� 16 λ�з����ַ�
	 * @return {size_t} ���ؿ�����ʵ���ֽ�����empty() == true ʱ���򷵻� 0
	 */
	size_t operator>>(char& n);

	/**
	 * ���ַ��������е����ݸ���Ŀ�� 8 λ�޷����ַ�
	 * @param n {string*} Ŀ�� 16 λ�޷����ַ�
	 * @return {size_t} ���ؿ�����ʵ���ֽ�����empty() == true ʱ���򷵻� 0
	 */
	size_t operator>>(unsigned char& n);

	/**
	 * �жϵ�ǰ������������������ַ������������Ƿ���ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�����������
	 * @return {bool} ���� true ��ʾ�ַ���������ͬ
	 */
	bool operator==(const string& s) const;

	/**
	 * �жϵ�ǰ������������������ַ������������Ƿ���ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�������ָ��
	 * @return {bool} ���� true ��ʾ�ַ���������ͬ
	 */
	bool operator==(const string* s) const;

	/**
	 * �жϵ�ǰ������������������ַ��������Ƿ���ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�������ָ��
	 * @return {bool} ���� true ��ʾ�ַ���������ͬ
	 */
	bool operator==(const char* s) const;

	/**
	 * �жϵ�ǰ������������������ַ������������Ƿ񲻵ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�����������
	 * @return {bool} ���� true ��ʾ�ַ������ݲ�ͬ
	 */
	bool operator!=(const string& s) const;

	/**
	 * �жϵ�ǰ������������������ַ������������Ƿ񲻵ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�������ָ��
	 * @return {bool} ���� true ��ʾ�ַ������ݲ�ͬ
	 */
	bool operator!=(const string* s) const;

	/**
	 * �жϵ�ǰ������������������ַ��������Ƿ񲻵ȣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�������ָ��
	 * @return {bool} ���� true ��ʾ�ַ������ݲ�ͬ
	 */
	bool operator!=(const char* s) const;

	/**
	 * �жϵ�ǰ����������Ƿ�С���������ַ����������ݣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�����������
	 * @return {bool} ���� true ��ʾ��ǰ�ַ������������С��������ַ���
	 *  ��������
	 */
	bool operator<(const string& s) const;

	/**
	 * �жϵ�ǰ����������Ƿ�����������ַ����������ݣ��ڲ����ִ�Сд��
	 * @param s {const string&} ������ַ�����������
	 * @return {bool} ���� true ��ʾ��ǰ�ַ�����������ݴ���������ַ���
	 *  ��������
	 */
	bool operator>(const string& s) const;

	/**
	 * ����ǰ����ֱ��תΪ�ַ���ָ�루�����ڲ�������ֱ�ӵ�����
	 * @return {const char*} ����ֵ��ԶΪ�ǿ�ָ�룬�п���Ϊ�մ�
	 */
	operator const char*() const;

	/**
	 * ����ǰ�ַ�������ֱ��תΪͨ��ָ�루�����ڲ�������ֱ�ӵ�����
	 * @return {const char*} ����ֵ��ԶΪ�ǿ�ָ��
	 */
	operator const void*() const;

	/**
	 * ��һ���з����ַ���ӽ���ǰ�ַ��������β��
	 * @param ch {char} �з����ַ�
	 * @return {string&} ��ǰ�ַ������������
	 */
	string& push_back(char ch);

	/**
	 * �Ƚ������ַ�������������Ƿ���ͬ�����ִ�Сд��
	 * @param s {const string&} ������ַ������������
	 * @return {int} 0����ʾ������ͬ�� > 0����ǰ�ַ������ݴ�����������ݣ�
	 *  < 0 ����ǰ�ַ�������С�����������
	 */
	int compare(const string& s) const;

	/**
	 * �Ƚ������ַ�������������Ƿ���ͬ�����ִ�Сд��
	 * @param s {const string&} ������ַ��������ָ��
	 * @return {int} 0����ʾ������ͬ�� > 0����ǰ�ַ������ݴ�����������ݣ�
	 *  < 0 ����ǰ�ַ�������С�����������
	 */
	int compare(const string* s) const;

	/**
	 * �Ƚ������ַ����������Ƿ���ͬ
	 * @param s {const string&} ������ַ������������
	 * @param case_sensitive {bool} Ϊ true ��ʾ���ִ�Сд
	 * @return {int} 0����ʾ������ͬ�� > 0����ǰ�ַ������ݴ�����������ݣ�
	 *  < 0 ����ǰ�ַ�������С�����������
	 */
	int compare(const char* s, bool case_sensitive = true) const;

	/**
	 * �Ƚϵ�ǰ����Ļ����������Ƿ��������Ļ�������������ͬ
	 * @param ptr {const void*} ����Ļ�������ַ
	 * @param len {size_t} ptr �Ļ����������ݳ���
	 * @return {int} ���ؽ����������:
	 *  0����ʾ������ͬ��
	 *  > 0����ǰ���󻺳������ݴ�����������ݣ�
	 *  < 0 ����ǰ���󻺳�����С�����������
	 */
	int compare(const void* ptr, size_t len) const;

	/**
	 * �Ƚϵ�ǰ���󻺳��������Ƿ��������Ļ�������������ͬ���޶��Ƚ����ݳ���
	 * @param s {const void*} ����Ļ�������ַ
	 * @param len {size_t} ptr �Ļ����������ݳ���
	 * @param case_sensitive {bool} Ϊ true ��ʾ���ִ�Сд
	 * @return {int} 0����ʾ������ͬ�� > 0����ǰ���󻺳������ݴ�����������ݣ�
	 *  < 0 ����ǰ���󻺳�����С�����������
	 */
	int ncompare(const char* s, size_t len, bool case_sensitive = true) const;

	/**
	 * ��β����ǰ�Ƚϵ�ǰ����Ļ����������Ƿ��������Ļ�������������ͬ��
	 * �޶��Ƚ����ݳ���
	 * @param s {const void*} ����Ļ�������ַ
	 * @param len {size_t} ptr �Ļ����������ݳ���
	 * @param case_sensitive {bool} Ϊ true ��ʾ���ִ�Сд
	 * @return {int} 0����ʾ������ͬ��
	 *  > 0����ǰ���󻺳������ݴ�����������ݣ�
	 *  < 0 ����ǰ���󻺳�����С�����������
	 */
	int rncompare(const char* s, size_t len, bool case_sensitive = true) const;

	/**
	 * �ڵ�ǰ�ַ����������в��ҿ��е�λ�ã�����ѭ�����ñ������Ի�����е�
	 * ��������������
	 * @param left_count {int*} ����ָ��ǿ�ʱ�洢��ǰ�ַ���ʣ������ݳ���
	 * @param buf {string*} ���ҵ�����ʱ������һ�ο���(�����ÿ���)��
	 *  ���ο���(�����ÿ���)֮������ݴ���ڸû������ڣ�ע���ڲ���������
	 *  ��ոû���������Ϊ��������׷�ӷ�ʽ
	 * @return {int} ���� 0 ��ʾδ�ҵ����У�����ֵ > 0 ��ʾ���е���һ��
	 *  λ��(��Ϊ��Ҫ�ҵ�һ�����з��ص��Ǹÿ��е���һ��λ�ã��������ҵ�
	 *  �����򷵻�ֵһ������ 0)������ֵ < 0 ��ʾ�ڲ�����
	 */
	int find_blank_line(int* left_count = NULL, string* buf = NULL);

	/**
	 * �����ڲ���ѯ״̬������Ҫ���¿�ʼ���� find_blank_line ʱ��Ҫ���ñ�
	 * �����������ڲ���ѯ״̬
	 * @return {string&}
	 */
	string& find_reset(void);

	/**
	 * ����ָ���ַ��ڵ�ǰ���󻺳�����λ�ã��±�� 0 ��ʼ��
	 * @param n {char} Ҫ���ҵ��з����ַ�
	 * @return {int} �ַ��ڻ������е�λ�ã�������ֵ < 0 ���ʾ������
	 */
	int find(char n) const;

	/**
	 * ����ָ���ַ����ڵ�ǰ���󻺳�������ʼλ�ã��±�� 0 ��ʼ��
	 * @param needle {const char*} Ҫ���ҵ��з����ַ���
	 * @param case_sensitive {bool} Ϊ true ��ʾ���ִ�Сд
	 * @return {char*} �ַ����ڻ������е���ʼλ�ã����ؿ�ָ�����ʾ������
	 */
	char* find(const char* needle, bool case_sensitive=true) const;

	/**
	 * ��β����ǰ����ָ���ַ����ڵ�ǰ���󻺳�������ʼλ�ã��±�� 0 ��ʼ��
	 * @param needle {const char*} Ҫ���ҵ��з����ַ���
	 * @param case_sensitive {bool} Ϊ true ��ʾ���ִ�Сд
	 * @return {char*} �ַ����ڻ������е���ʼλ�ã�������ֵΪ��ָ�����ʾ������
	 */
	char* rfind(const char* needle, bool case_sensitive=true) const;

	/**
	 * ���شӵ�ǰ�ַ��������л�����ָ��λ�����������
	 * @param npos {size_t} �±�λ�ã�����ֵ���ڵ��ڵ�ǰ�ַ��������ݳ���ʱ��
	 *  �򷵻������ַ������󣻷���ֵ��������ֵָ��λ�õ��ַ�����
	 * @return {string} ����ֵΪһ�����Ķ��󣬲���Ҫ�����ͷţ��ú�����Ч��
	 *  ���ܲ���̫��
	 */
	string left(size_t npos);

	/**
	 * ���شӵ�ǰ�ַ��������л�����ָ��λ�����ҵ�����
	 * @param npos {size_t} �±�λ�ã�����ֵ���ڵ��ڵ�ǰ�ַ��������ݳ���ʱ��
	 *  �򷵻ص��ַ�����������Ϊ�գ�����ֵ��������ֵָ��λ�õ��ַ�����
	 * @return {const string} ����ֵΪһ�����Ķ��󣬲���Ҫ�����ͷţ���
	 *  ������Ч�ʿ��ܲ���̫��
	 */
	string right(size_t npos);

	/**
	 * ����ǰ����Ļ������ݿ���һ����������Ŀ�껺����
	 * @param buf {void*} Ŀ�껺������ַ
	 * @param size {size_t} buf ����������
	 * @param move {bool} �ڿ��������ݺ��Ƿ���Ҫ�������������ǰ�ƶ���
	 *  ����ǰ����ѿ���������
	 * @return {size_t} ���ؿ�����ʵ���ֽ������� empty() == true ʱ���򷵻� 0
	 */
	size_t scan_buf(void* buf, size_t size, bool move = false);

	/**
	 * �ӵ�ǰ����Ļ������п���һ������(����"\r\n")��Ŀ�껺�����ڣ�������
	 * ��������Ŀ�껺��������Դ��������δ�����������ݻᷢ���ƶ������Ǳ�
	 * ��������������
	 * @param out {string&} Ŀ�껺�����������ڲ��������Զ���ոû�����
	 * @param nonl {bool} ���ص�һ�������Ƿ�ȥ��β���� "\r\n" �� "\n"
	 * @param n {size_t*} �ò���Ϊ�ǿ�ָ��ʱ����洢�����������ݳ��ȣ�����
	 *  ��һ�������� nonl Ϊ true ʱ����õ�ַ�洢 0
	 * @param move {bool} �ڿ��������ݺ��Ƿ���Ҫ�������������ǰ�ƶ���
	 *  ����ǰ����ѿ���������
	 * @return {bool} �Ƿ񿽱���һ�����������ݣ�������� false ����Ҫ����
	 *  empty() == true ���жϵ�ǰ���������Ƿ�������
	 */
	bool scan_line(string& out, bool nonl = true, size_t* n = NULL,
		bool move = false);

	/**
	 * ��ʹ�� scan_xxx �෽���Ի��������в���ʱδָ�� move ����������ñ�
	 * ��������ʹ��������ʣ���������ǰ�ƶ����������ײ�
	 * @return {size_t} �ƶ����ֽ���
	 */
	size_t scan_move();

	/**
	 * ���ص�ǰ���󻺳����е�һ���������ݵ�β����ַ
	 * @return {char*} ����ֵΪ NULL ��˵���ڲ�����Ϊ�գ��� empty() == true
	 */
	char* buf_end(void);

	/**
	 * ���ص�ǰ���󻺳�������ʼ��ַ
	 * @return {void*} ���ص�ַ��Զ�ǿ�
	 */
	void* buf() const;

	/**
	 * ���ַ�����ʽ���ص�ǰ���󻺳�������ʼ��ַ
	 * @return {char*} ���ص�ַ��Զ�ǿ�
	 */
	char* c_str() const;

	/**
	 * ���ص�ǰ�����ַ����ĳ��ȣ�����\0��
	 * @return {size_t} ����ֵ >= 0
	 */
	size_t length() const;

	/**
	 * ���ص�ǰ�����ַ����ĳ��ȣ�����\0���������� length ��ͬ
	 * @return {size_t} ����ֵ >= 0
	 */
	size_t size() const;

	/**
	 * ���ص�ǰ����Ļ������Ŀռ䳤�ȣ���ֵ >= �����������ݳ���
	 * @return {size_t} ����ֵ > 0
	 */
	size_t capacity() const;

	/**
	 * �жϵ�ǰ����Ļ����������ݳ����Ƿ�Ϊ 0
	 * @return {bool} ���� true ��ʾ����Ϊ��
	 */
	bool empty() const;

	/**
	 * ���ص�ǰ�����ڲ����õ� acl C ���е� ACL_VSTRING �����ַ
	 * @return {ACL_VSTRING*} ����ֵ��Զ�ǿ�
	 */
	ACL_VSTRING* vstring(void) const;

	/**
	 * ����ǰ����Ļ��������±�λ������ָ��λ��
	 * @param n {size_t} Ŀ���±�λ�ã�����ֵ >= capacity ʱ���ڲ���
	 *  ���·������Щ���ڴ�
	 * @return {string&} ��ǰ���������
	 */
	string& set_offset(size_t n);

	/**
	 * ���øú�������Ԥ�ȱ�֤����Ҫ�Ļ�������С
	 * @param n {size_t} ϣ���Ļ������ռ��Сֵ
	 * @return {string&} ��ǰ���������
	 */
	string& space(size_t n);

	/**
	 * ����ǰ����洢���ַ������зָ�
	 * @param sep {const char*} ���зָ�ʱ�ķָ���
	 * @return {std::list<string>&} ���� list ��ʽ�ķָ��������صĽ��
	 *  ����Ҫ�ͷţ��������˵�ǰ�����һ���ڲ�ָ��
	 */
	std::list<string>& split(const char* sep);

	/**
	 * ����ǰ����洢���ַ������зָ�
	 * @param sep {const char*} ���зָ�ʱ�ķָ���
	 * @return {std::vector<string>&} ���� vector ��ʽ�ķָ��������ص�
	 *  �������Ҫ�ͷţ��������˵�ǰ�����һ���ڲ�ָ��
	 */
	std::vector<string>& split2(const char* sep);

	/**
	 * �� '=' Ϊ�ָ�������ǰ����洢���ַ����ָ�� name/value �ԣ��ָ�ʱ��
	 * �Զ�ȥ��Դ�ַ�������ʼ������β���Լ��ָ��� '=' ���ߵĿո� TAB
	 * @return {std::pair<string, string>&} �����ǰ����洢���ַ���
	 *  �����Ϸָ��������������ϸ�� name=value��ʽ�����򷵻صĽ�����ַ�
	 *  ������Ϊ�մ�,���صĽ������Ҫ�ͷţ��������˵�ǰ�����һ���ڲ���ַ
	 */
	std::pair<string, string>& split_nameval(void);

	/**
	 * ���ַ�����������ǰ����Ļ�������
	 * @param ptr {const char*} Դ�ַ�����ַ������ '\0' ����
	 * @return {string&} ��ǰ���������
	 */
	string& copy(const char* ptr);

	/**
	 * ��Դ���ݵĶ������ݿ�������ǰ����Ļ�������
	 * @param ptr {const void*} Դ���ݵ�ַ
	 * @param len {size_t} ptr Դ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& copy(const void* ptr, size_t len);

	/**
	 * ��Դ�ַ����������ƶ�����ǰ����Ļ������У��ڲ����Զ��ж�Դ����
	 * ��ַ�Ƿ���ڵ�ǰ����Ļ�������
	 * @param src {const char*} Դ���ݵ�ַ
	 * @return {string&} ��ǰ���������
	 */
	string& memmove(const char* src);

	/**
	 * ��Դ�ַ����������ƶ�����ǰ����Ļ������У��ڲ����Զ��ж�Դ����
	 * ��ַ�Ƿ���ڵ�ǰ����Ļ�������
	 * @param src {const char*} Դ���ݵ�ַ
	 * @param len {size_t} �ƶ����ݵĳ���
	 * @return {string&} ��ǰ���������
	 */
	string& memmove(const char* src, size_t len);

	/**
	 * ��ָ���ַ�������ڵ�ǰ�������ݻ��������ݵ�β��
	 * @param s {const string&} Դ���ݶ�������
	 * @return {string&} ��ǰ���������
	 */
	string& append(const string& s);

	/**
	 * ��ָ���ַ�������ڵ�ǰ�������ݻ��������ݵ�β��
	 * @param s {const string&} Դ���ݶ���ָ��
	 * @return {string&} ��ǰ���������
	 */
	string& append(const string* s);

	/**
	 * ��ָ���ַ�������ڵ�ǰ�������ݻ��������ݵ�β��
	 * @param s {const string&} Դ���ݶ���ָ��
	 * @return {string&} ��ǰ���������
	 */
	string& append(const char* s);

	/**
	 * ��ָ���������е���������ڵ�ǰ�������ݻ��������ݵ�β��
	 * @param ptr {const void*} Դ���ݶ���ָ��
	 * @param len {size_t} ptr ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& append(const void* ptr, size_t len);

	/**
	 * ��ָ���ַ�����������ڵ�ǰ�������ݻ��������ݵ��ײ�
	 * @param s {const char*} Դ���ݵ�ַ
	 * @return {string&} ��ǰ���������
	 */
	string& prepend(const char* s);

	/**
	 * ��ָ���ڴ���������ڵ�ǰ�������ݻ��������ݵ��ײ�
	 * @param ptr {const void*} Դ���ݵ�ַ
	 * @param len {size_t} ptr ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& prepend(const void* ptr, size_t len);

	/**
	 * ���ڴ����ݲ���ָ���±�λ�ÿ�ʼ�ĵ�ǰ���󻺳�����
	 * @param start {size_t} ��ǰ���󻺳����Ŀ�ʼ�����±�ֵ
	 * @param ptr {const void*} �ڴ����ݵĵ�ַ
	 * @param len {size_t} �ڴ����ݵĳ���
	 * @return {string&} ��ǰ���������
	 */
	string& insert(size_t start, const void* ptr, size_t len);

	/**
	 * ����ʽ��ʽ��������ݣ������� sprintf �ӿڷ�ʽ��
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ... �������
	 * @return {string&} ��ǰ���������
	 */
	string& format(const char* fmt, ...) ACL_CPP_PRINTF(2, 3);

	/**
	 * ����ʽ��ʽ��������ݣ������� vsprintf �ӿڷ�ʽ��
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ap {va_list} �������
	 * @return {string&} ��ǰ���������
	 */
	string& vformat(const char* fmt, va_list ap);

	/**
	 * ����ʽ��ʽ�ڵ�ǰ�����β���������
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ... �������
	 * @return {string&} ��ǰ���������
	 */
	string& format_append(const char* fmt, ...)  ACL_CPP_PRINTF(2, 3);

	/**
	 * ����ʽ��ʽ�ڵ�ǰ�����β���������
	 * @param fmt {const char*} ��ʽ�ַ���
	 * @param ap {va_list} �������
	 * @return {string&} ��ǰ���������
	 */
	string& vformat_append(const char* fmt, va_list ap);

	/**
	 * ����ǰ�����е����ݵ��ַ������滻
	 * @param from {char} Դ�ַ�
	 * @param to {char} Ŀ���ַ�
	 * @return {string&} ��ǰ���������
	 */
	string& replace(char from, char to);

	/**
	 * ����ǰ��������ݽض̣��������ڲ��ƶ��±�ָ���ַ
	 * @param n {size_t} ���ݽض̺�����ݳ��ȣ������ֵ >= ��ǰ������
	 *  ���ݳ��ȣ����ڲ������κα仯
	 * @return {string&} ��ǰ���������
	 */
	string& truncate(size_t n);

	/**
	 * �ڵ�ǰ����Ļ�����������ȥ��ָ�����ַ������ݣ��ڴ�������лᷢ��
	 * �����ƶ����
	 * @param needle {const char*} ָ����Ҫȥ�����ַ�������
	 * @param each {bool} ��Ϊ true ʱ����ÿһ�������� needle �е��ַ���
	 *  ���ڵ�ǰ����Ļ�������ȥ�������򣬽��ڵ�ǰ���󻺳�����ȥ��������
	 *  needle �ַ���
	 * @return {string&} ��ǰ���������
	 *  �� acl::string s("hello world!");
	 *  �� s.strip("hel", true), ����Ϊ�� s == "o word!"
	 *  �� s.strip("hel", false), ����Ϊ: s = "lo world!"
	 */
	string& strip(const char* needle, bool each = false);

	/**
	 * ����ǰ���󻺳�����ߵĿհף������ո�TAB��ȥ��
	 * @return {string&} ��ǰ���������
	 */
	string& trim_left_space();

	/**
	 * ����ǰ���󻺳����ұߵĿհף������ո�TAB��ȥ��
	 * @return {string&} ��ǰ���������
	 */
	string& trim_right_space();

	/**
	 * ����ǰ���󻺳��������еĿհף������ո�TAB��ȥ��
	 * @return {string&} ��ǰ���������
	 */
	string& trim_space();

	/**
	 * ����ǰ���󻺳�����ߵĻس����з�ȥ��
	 * @return {string&} ��ǰ���������
	 */
	string& trim_left_line();

	/**
	 * ����ǰ���󻺳����ұߵĻس����з�ȥ��
	 * @return {string&} ��ǰ���������
	 */
	string& trim_right_line();

	/**
	 * ����ǰ���󻺳��������еĻس����з�ȥ��
	 * @return {string&} ��ǰ���������
	 */
	string& trim_line();

	/**
	 * ��յ�ǰ��������ݻ�����
	 * @return {string&} ��ǰ���������
	 */
	string& clear();

	/**
	 * ����ǰ��������ݻ������е����ݾ�תΪСд
	 * @return {string&} ��ǰ���������
	 */
	string& lower(void);

	/**
	 * ����ǰ��������ݻ������е����ݾ�תΪ��д
	 * @return {string&} ��ǰ���������
	 */
	string& upper(void);

	/**
	 * �ӵ�ǰ�������н�ָ��ƫ����ָ�����ȵ����ݿ�����Ŀ�껺������
	 * @param out {string&} Ŀ�껺�������ڲ�����׷�ӷ�ʽ��������ոö���
	 * @param pos {size_t} ��ǰ����������ʼλ��
	 * @param len {size_t} �� pos ��ʼλ�ÿ�ʼ������������������ֵΪ 0 ʱ
	 *  �򿽱�ָ�� pos λ�ú����е����ݣ����򿽱�ָ�����ȵ����ݣ���ָ����
	 *  ���ݳ��ȴ���ʵ��Ҫ�����ĳ��ȣ��������ʵ�ʴ��ڵ�����
	 * @return {size_t} ���ؿ�����ʵ�����ݳ��ȣ�pos Խ��ʱ��÷���ֵΪ 0
	 */
	size_t substr(string& out, size_t pos = 0, size_t len = 0);

	/**
	 * ����ǰ��������ݻ������е����ݽ��� base64 ת��
	 * @return {string&} ��ǰ���������
	 */
	string& base64_encode(void);

	/**
	 * �������Դ���ݽ��� base64 ת�벢���뵱ǰ����Ļ�������
	 * @param ptr {const void*} Դ���ݵĵ�ַ
	 * @param len {size_t} Դ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& base64_encode(const void* ptr, size_t len);

	/**
	 * �����ǰ����Ļ������е������Ǿ� base64 ����ģ���˺�������Щ
	 * ���ݽ��н���
	 * @return {string&} ��ǰ���������
	 */
	string& base64_decode(void);

	/**
	 * ������� base64 ��������ݽ��н��벢���뵱ǰ����Ļ�������
	 * @param s {const char*} �� base64 �����Դ����
	 * @return {string&} ��ǰ���������
	 */
	string& base64_decode(const char* s);

	/**
	 * ������� base64 ��������ݽ��н��벢���뵱ǰ����Ļ�������
	 * @param ptr {const void*} �� base64 �����Դ����
	 * @param len {size_t} ptr ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& base64_decode(const void* ptr, size_t len);

	/**
	 * �������Դ���ݽ��� url ���벢���뵱ǰ����Ļ�������
	 * @param s {const char*} Դ����
	 * @return {string&} ��ǰ���������
	 */
	string& url_encode(const char* s);

	/**
	 * ��������� url �����Դ���ݽ��벢���뵱ǰ����Ļ�������
	 * @param s {const char*} �� url �����Դ����
	 * @return {string&} ��ǰ���������
	 */
	string& url_decode(const char* s);

	/**
	 * ��Դ���ݽ��� H2B ���벢���뵱ǰ����Ļ�������
	 * @param s {const void*} Դ���ݵ�ַ
	 * @param len {size_t} Դ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& hex_encode(const void* s, size_t len);

	/**
	 * ��Դ���ݽ��� H2B ���벢���뵱ǰ����Ļ�������
	 * @param s {const char*} Դ���ݵ�ַ
	 * @param len {size_t} Դ���ݳ���
	 * @return {string&} ��ǰ���������
	 */
	string& hex_decode(const char* s, size_t len);

	/**
	 * �� 32 λ�з�������תΪ�ַ����棨�ڲ�ʹ�����ֲ߳̾�������
	 * @param n {int} 32 λ�з�������
	 * @return {string&} ת�������������ã����������ڲ���һ���ֲ߳̾�����
	 */
	static string& parse_int(int n);

	/**
	 * �� 32 λ�޷�������תΪ�ַ����棨�ڲ�ʹ�����ֲ߳̾�������
	 * @param n {int} 32 λ�޷�������
	 * @return {string&} ת�������������ã����������ڲ���һ���ֲ߳̾�����
	 */
	static string& parse_int(unsigned int n);
#ifdef WIN32
	static string& parse_int64(__int64 n);
	static string& parse_int64(unsigned __int64 n);
#else
	/**
	 * �� 64 λ�з�������תΪ�ַ����棨�ڲ�ʹ�����ֲ߳̾�������
	 * @param n {long long int} 64 λ�з�������
	 * @return {string&} ת�������������ã����������ڲ���һ���ֲ߳̾�����
	 */
	static string& parse_int64(long long int n);

	/**
	 * �� 64 λ�޷�������תΪ�ַ����棨�ڲ�ʹ�����ֲ߳̾�������
	 * @param n {unsigned long long int} 64 λ�޷�������
	 * @return {string&} ת�������������ã����������ڲ���һ���ֲ߳̾�����
	 */
	static string& parse_int64(unsigned long long int n);
#endif

private:
	bool use_bin_;
	ACL_VSTRING* vbf_;
	char* scan_ptr_;
	std::list<string>* list_tmp_;
	std::vector<string>* vector_tmp_;
	std::pair<string, string>* pair_tmp_;
	ACL_LINE_STATE* line_state_;
	int   line_state_offset_;

	void init(size_t len);
};

} // namespce acl
