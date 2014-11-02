#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include <stdlib.h>
#include "acl_cpp/stream/stream.hpp"

namespace acl {

class string;

/**
 * �����������࣬�����ȷ��֪���������Ƿ�رջ�����������ļ�����
 * β����Ӧͨ������ stream->eof() �������ж�
 */

class ACL_CPP_API istream : virtual public stream
{
public:
	istream() {}
	virtual ~istream() {}

	int read();

	/**
	 * ���������ж�����
	 * @param buf {void*} �û�������
	 * @param size {size_t} �û�����������
	 * @param loop {bool} �Ƿ���� size ��ŷ���
	 * @return {int} ���������, -1 ��ʾ�رջ����, > 0 ��ʾ�ɹ�
	 */
	int read(void* buf, size_t size, bool loop = true);

	/**
	 * ��������������ֱ��������Ҫ����ַ��������ŷ���
	 * @param buf {void*} �û�������
	 * @param inout {size_t*} ��Ϊ����ʱ *inout ��ʾ���� buf
	 *  �Ŀռ��С���������غ��¼�洢�� buf �е����ݳ���
	 * @param tag {const char*} Ҫ��������ַ���
	 * @param len {size_t} tag �ַ����ĳ���
	 * @return {bool} �Ƿ������Ҫ����ַ�������
	 */

	bool readtags(void *buf, size_t* inout, const char *tag, size_t len);

	/**
	 * ���������ж���һ������
	 * @param buf {void*} �û�������
	 * @param size_inout {size_t*} ��Ϊ����ʱ *size_inout ��ʾ���� buf
	 *  �Ŀռ��С���������غ��¼�洢�� buf �е����ݳ���
	 * @param nonl {bool} ���Ϊ true ��Ὣ������һ������β���� "\r\n"
	 *  �� "\n" ȥ����*size_inout �洢�����ݳ�����ȥ�� "\r\n" �� "\n" ��
	 *  �ĳ��ȣ����򣬱����������е� "\r\n" �� "\n"��ͬʱ *size_inout ��
	 *  �����ǰ��� "\r\n" �� "\n" �����ݳ���
	 * @return {bool} �Ƿ������һ������, �����򷵻� false; ���ļ���������
	 *  �ԣ�������������������һ�����������ⲿ�����ݲ��� "\r\n" �� "\n"
	 *  ��Ҳ�᷵�� false, ��������Ҫ��� *size_inout ֵ�Ƿ���� 0
	 *  ��ȷ���Ƿ���������һ��������
	 */
	bool gets(void* buf, size_t* size_inout, bool nonl = true);

	/**
	 * ���������ж�һ�� 64 λ����
	 * @param n {acl_int64&} 64 λ����
	 * @param loop {bool} �Ƿ�����ʽ����8���ֽ�
	 * @return {bool} �Ƿ��ȡ�ɹ�
	 */
#ifdef WIN32
	bool read(__int64& n, bool loop = true);
#else
	bool read(long long int& n, bool loop = true);
#endif

	/**
	 * ���������ж�һ�� 32 λ����
	 * @param n {int&} 32 λ����
	 * @param loop {bool} �Ƿ�����ʽ����4���ֽ�
	 * @return {bool} �Ƿ��ȡ�ɹ�
	 */
	bool read(int& n, bool loop = true);

	/**
	 * ���������ж�һ�� 16 λ����
	 * @param n {short&} 16 λ����
	 * @param loop {bool} �Ƿ�����ʽ����2���ֽ�
	 * @return {bool} �Ƿ��ȡ�ɹ�
	 */
	bool read(short& n, bool loop = true);

	/**
	 * ���������ж�ȡһ���ֽ�
	 * @param ch {char&}
	 * @return {bool} ��ȡ�Ƿ�ɹ�
	 */
	bool read(char& ch);

	/**
	 * ���������ж���������������
	 * @param s {string*} ���������ڲ��������Զ���ոû�����
	 * @param loop {bool} �Ƿ�����ʽ�����������壬������
	 *  ������Ϊ s.capacity()
	 * @return {bool} �������Ƿ�ɹ�
	 */
	bool read(string& s, bool loop = true);
	bool read(string* s, bool loop = true);

	/**
	 * ���������ж���������������
	 * @param s {string*} ���������ڲ��������Զ���ոû�����
	 * @param max {size_t} ϣ�����������ݵ����ֵ
	 * @param loop {bool} �Ƿ����Ҫ��� max �ֽ���Ϊֹ
	 * @return {bool} �������Ƿ�ɹ�
	 */
	bool read(string& s, size_t max, bool loop = true);
	bool read(string* s, size_t max, bool loop = true);

	/**
	 * ���������ж�һ����������������
	 * @param s {string&} ���������ڲ��������Զ���ոû�����
	 * @param nonl {bool} �Ƿ��������������е� "\r\n" �� "\n"
	 * @param max {size_t} ����ֵ > 0 ʱ����ֵ�޶����������е����ֵ����
	 *  ���յ������г��ȴ��ڸ�ֵʱ��������ز������ݣ�ͬʱ�ڲ����¼���棻
	 *  ����ֵ = 0 ʱ�������������ݳ���
	 * @return {bool} �Ƿ������һ������
	 *  1��������� true ��˵������������һ�����ݣ��������������ֻ��
	 *     "\r\n" �� "\n"���� s ������Ϊ�գ�����s.empty() == true
	 *  2��������� false ��˵�����ر���δ����һ�����ݣ���ʱ s ���п���
	 *     �洢�Ų������ݣ���Ҫ�� if (s.empty() == true) �ж�һ��
	 */
	bool gets(string& s, bool nonl = true, size_t max = 0);
	bool gets(string* s, bool nonl = true, size_t max = 0);

	/**
	 * ���������ж�����ֱ������Ҫ����ַ���������Ϊ�ָ��������ݣ�
	 * ��ȡ�����ݵ���󲿷�Ӧ���Ǹ��ַ���
	 * @param s {string&} ���������ڲ��������Զ���ոû�����
	 * @param tag {const string&} Ҫ������ַ�������
	 * @return {bool} �Ƿ����Ҫ����ַ�������
	 */
	bool readtags(string& s, const string& tag);
	bool readtags(string* s, const string& tag);

	/**
	 * ���������ж�һ���ֽ�����
	 * @return {int} �����ֽڵ� ASCII ��ֵ
	 */
	int getch(void);

	/**
	 * ���������зż�һ���ֽڵ�����
	 * @param ch {int} һ���ַ��� ASCII ��ֵ
	 * @return {int} �������ֵ�� ch ֵ��ͬ���ʾ��ȷ�������ʾ����
	 */
	int ugetch(int ch);

	/**
	 * �����Դ��������ж�ȡһ������
	 * @param buf {string&} ������
	 * @param nonl {bool} �Ƿ��������������е� "\r\n" �� "\n"
	 * @param clear {bool} �Ƿ��ڲ��Զ���� buf ������
	 * @param max {int} ����ֵ > 0 ʱ���������������ݵ���󳤶��Ա��Ȿ��
	 *  ���������
	 * @return {bool} �Ƿ����һ������; ������� false ������ʾ����
	 *  ��������ֻ�Ǳ�ʾδ����һ�����������ݣ�Ӧ��ͨ������ stream->eof()
	 *  ������������Ƿ�رգ����⣬����������˲������ݣ��� buf ��洢
	 *  ��Щ��������
	 *  ע�⣺Ϊ�˷�ֹ buf ����������������ߵ��ø÷�����õ����ݼ�ʹ����
	 *  һ�����ݣ�Ӧ����ȡ�� buf �е�����Ȼ�� buf->clear()���Է�ֹ buf
	 *  �ڴ�����»��������
	 */
	bool gets_peek(string& buf, bool nonl = true,
		bool clear = false, int max = 0);
	bool gets_peek(string* buf, bool nonl = true,
		bool clear = false, int max = 0);

	/**
	 * �����Դ��������ж�ȡ����
	 * @param buf {string&} ������
	 * @param clear {bool} ������ʼʱ�Ƿ��ڲ��Զ���� buf ������
	 * @return {bool} �Ƿ��������, ������� false �� ��ʾδ������Ҫ��
	 *  �����ݳ��ȣ�Ӧ��ͨ������ stream->eof() ������������Ƿ�ر�
	 *  ע�⣺Ϊ�˷�ֹ buf ����������������ߵ��ø÷�����õ����ݼ�ʹ����
	 *  һ�����ݣ�Ӧ����ȡ�� buf �е�����Ȼ�� buf->clear()���Է�ֹ buf
	 *  �ڴ�����»��������
	 */
	bool read_peek(string& buf, bool clear = false);
	bool read_peek(string* buf, bool clear = false);

	/**
	 * �����Դ��������ж�ȡָ�����ȵ�����
	 * @param buf {string&} ������
	 * @param cnt {size_t} Ҫ����������ݳ���
	 * @param clear {bool} ������ʼʱ�Ƿ��ڲ��Զ���� buf ������
	 * @return {bool} �Ƿ������Ҫ�����ݳ��ȵ�����, ������� false ��
	 *  ��ʾδ������Ҫ������ݳ��ȣ�Ӧ��ͨ������ stream->eof() �����
	 *  �������Ƿ�ر�
	 *  ע�⣺Ϊ�˷�ֹ buf ����������������ߵ��ø÷�����õ����ݼ�ʹ����
	 *  һ�����ݣ�Ӧ����ȡ�� buf �е�����Ȼ�� buf->clear()���Է�ֹ buf
	 *  �ڴ�����»��������
	 */
	bool readn_peek(string& buf, size_t cnt, bool clear = false);
	bool readn_peek(string* buf, size_t cnt, bool clear = false);

	/* ���¼���������������������������Ƕ�������ʽ�������̣�����Ҫ
	 * ���� stream->eof() ���ж��������Ƿ�رջ��Ƿ�������ļ�β
	 */

	istream& operator>>(string& s);
#ifdef WIN32
	istream& operator>>(__int64& n);
#else
	istream& operator>>(long long int& n);
#endif
	istream& operator>>(int& n);
	istream& operator>>(short& n);
	istream& operator>>(char& ch);
};

} // namespace acl
