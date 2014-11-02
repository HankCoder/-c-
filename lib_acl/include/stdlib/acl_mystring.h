#ifndef	ACL_MYSTRING_INCLUDE_H
#define	ACL_MYSTRING_INCLUDE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "acl_define.h"

#include <string.h>

/**
 * ����: ��ȫ���ַ��������꺯��, ���Ա�֤���һ���ֽ�Ϊ  "\0"
 * @param _obj {char*} Ŀ���ڴ���ָ��
 * @param _src {const char*} Դ�ַ���ָ��
 * @param _size {int} Ŀ���ڴ����Ŀռ��С
 */
#ifndef ACL_SAFE_STRNCPY
#ifdef	WIN32
#define ACL_SAFE_STRNCPY(_obj, _src, _size) do {            \
    size_t _n = strlen(_src);                               \
    _n = _n > (size_t ) _size - 1? (size_t) _size - 1 : _n; \
    memcpy(_obj, _src, _n);                                 \
    _obj[_n] = 0;                                           \
} while (0)
#else
#define ACL_SAFE_STRNCPY(_obj, _src, _size) do {            \
    if (_obj != NULL && _src != NULL && (int)_size >= 0) {  \
        strncpy(_obj, _src, _size);                         \
            if ((int)_size > 0)                             \
                _obj[_size - 1] = 0;                        \
            else                                            \
                _obj[_size] = 0;                            \
	}                                                       \
} while (0)
#endif
#endif

/**
 * ���ַ���ת��ΪСд��ֱ����ԭ�ڴ�ռ���в���
 * @param string {char *} �������ַ���
 * @return {char*} �ɹ������ַ�����ַ�����򷵻� NULL
 */
ACL_API char *acl_lowercase(char *string);

/**
 * �������ַ�����ǰ n ���ֽ�ת��ΪСд
 * @param string {char *} �������ַ���
 * @param n {int} ����ת�����ֽ���
 * @return {char*} �ɹ������ַ�����ַ�����򷵻� NULL
 */
ACL_API char *acl_lowercase2(char *string, size_t n);

/**
 * �������ַ���ת��ΪСд������洢����һ���ڴ�����
 * @param string {const char*} Դ�ַ���
 * @param buf {char*} �洢ת��������ڴ�ָ��
 * @param size {size_t} buf �Ŀռ��С
 * @return {char*} �ɹ������ַ�����ַ�����򷵻� NULL
 */
ACL_API char *acl_lowercase3(const char *string, char *buf, size_t size);

/**
 * ���ַ���ת��Ϊ��д��ֱ����ԭ�ڴ�ռ���в���
 * @param string {char *} �������ַ���
 * @return {char*} �ɹ������ַ�����ַ�����򷵻� NULL
 */
ACL_API char *acl_uppercase(char *string);

/**
 * ���ַ���ת��Ϊ��д��ֱ����ԭ�ڴ�ռ���в���, ���ת������������
 * @param string {char *} �������ַ���
 * @param n {int} ����ת�����ֽ���
 * @return {char*} �ɹ������ַ�����ַ�����򷵻� NULL
 */
ACL_API char *acl_uppercase2(char *string, size_t n);

/**
 * �������ַ�����ǰ n ���ֽ�ת��Ϊ��д
 * @param string {char *} �������ַ���
 * @param buf {char*} �洢ת��������ڴ���
 * @param size {size_t} buf �Ŀռ��С(�ֽ�)
 * @return {char*} �ɹ������ַ�����ַ�����򷵻� NULL
 */
ACL_API char *acl_uppercase3(const char *string, char *buf, size_t size);

/**
 * �������ַ�������һ���ָ����ַ������зָ�
 * @param src {char**} ��Ҫ���ָ���ַ����ĵ�ַ��ָ�룬�����Ƿǿ�ָ�룬
 *  �����ǿ��ַ�������ʱ�ú������� NULL
 * @param sep {const char*} �ָ������ǿ��ַ���
 * @return {char*} ��ǰ���ָ���ַ�����ָ��λ�ã�src ָ����һ����Ҫ����
 *  �ָ�����ʼλ�ã�
 *  1�������� NULL ʱ�����ʾ�ָ����̽�������ʱ src ��ָ��λ�ñ����� '\0'��
 *  2�������ط� NULL ʱ�����ʱ src ָ����ַ��������ǻ��ǿ��ַ�����
 *     ���ָ����ַ��������ٴηָ�ʱ�����϶��ܷ��� NULL�����򣬵��ٴηָ�ʱ
 *     �������ط� NULL ָ��
 *  ����: Դ�ַ�����"abcd=|efg=|hijk", �ָ��� "=|"�����һ�ηָ���
 *  src ��ָ�� "efg"�������صĵ�ַΪ "abcd"
 */
ACL_API char *acl_mystrtok(char **src, const char *sep);

/**
 * ���һ���߼���, ���ĳ�е�β�������ӷ� "\\" ���ӣ�����һ�кϲ�������,
 * ͬʱ��һ���ַ����е��Լ�ת���ַ��س�����("\r\n" or "\n")ȥ��
 * @param src {char**} Դ�ַ����ĵ�ַָ��
 * @return {char*} ����һ������, ������ؿ����ʾû�п��õ��߼���
 */
ACL_API char *acl_mystrline(char **src);

/**
 * ȥ�������ַ����е� " ", "\t"
 * @param str {char*} Դ�ַ���
 * @return {char*} ��Դ�ַ�����ͬ�ĵ�ַ
 */
ACL_API char *acl_mystr_trim(char *str);

/**
 * ��Դ�ַ�����ȥ�������ַ���
 * @param haystack {const char*} Դ�ַ���
 * @param needle {const char*} ��Ҫ��Դ�ַ����б�����ȥ�����ַ���
 * @param buf {char*} �洢������ڴ濪ʼλ��
 * @param bsize {int} buf �Ŀռ��С
 * @return {int} ������ buf �е��ַ�������
 */
ACL_API int acl_mystr_strip(const char *haystack, const char *needle, char *buf, int bsize);

/**
 * ��Դ�ַ������ҵ�һ�еĽ���λ�ò�ȥ�������س����з������Ժ���ַ���
 * @param str_src {char*} Դ�ַ���
 * @return {int} 0 ��ʾ�ɹ���-1��ʾʧ��, Ҳ��Ӧ�÷������ת������ĳ���!
 */
ACL_API int acl_mystr_truncate_byln(char *str_src);

/**
 * �Ӻ���ǰ�Ƚ����������ַ�������Сд���������޶����ȽϷ�Χ
 * @param s1 {const char*} �ַ�����ַ
 * @param s2 {const char*} �ַ�����ַ
 * @param n {size_t} ���ȽϷ�Χ
 * @return {int} �ȽϽ��. 0: ���, >0: ��һ���ַ������ڵڶ����ַ���,
 *  < 0: ��һ���ַ���С�ڶ����ַ���
 */
ACL_API int acl_strrncasecmp(const char *s1, const char *s2, size_t n);

/**
 * �Ӻ���ǰ�Ƚ����������ַ���, ��Сд�������޶����ȽϷ�Χ
 * @param s1 {const char*} �ַ�����ַ
 * @param s2 {const char*} �ַ�����ַ
 * @param n {size_t} ���ȽϷ�Χ
 * @return {int} �ȽϽ��. 0: ���, >0: ��һ���ַ������ڵڶ����ַ���,
 *  < 0: ��һ���ַ���С�ڶ����ַ���
 */
ACL_API int acl_strrncmp(const char *s1, const char *s2, size_t n);

/**
 * �Ӻ���ǰɨ������ַ�������Сд����
 * @param haystack {char *} Դ�ַ���
 * @param needle {const char *} ƥ����ҵ��ַ���
 * @return {char *} != NULL: Ok, NULL: δ����
 */
ACL_API char *acl_rstrstr(char *haystack, const char *needle);

/**
 * ��ǰ���ɨ������ַ�������Сд������
 * @param haystack {char *} Դ�ַ���
 * @param needle {const char *} ƥ����ҵ��ַ���
 * @return {char *} != NULL: Ok, NULL: δ����
 */
ACL_API char *acl_strcasestr(char *haystack, const char *needle);

/**
 * �Ӻ���ǰɨ������ַ�������Сд������
 * @param haystack {char *} Դ�ַ���
 * @param needle {const char *} ƥ����ҵ��ַ���
 * @return {char *} != NULL: Ok, NULL: δ����
 */
ACL_API char *acl_rstrcasestr(char *haystack, const char *needle);

/**
 * ��������ַ����ĳ��ȣ��������������㳤�ȣ��������Խ������Ҫ�� strlen
 * ��ȫ���磬��������ַ���û���� "\0" ��β����ú����Ͳ������Խ��
 * @param s {const char*} �ַ���
 * @param count {size_t} �����㳤��
 * @return {size_t} �ַ��� s ��ʵ�ʳ���
 */
ACL_API size_t acl_strnlen(const char * s, size_t count);

/**
 * �Ƚ������ַ����Ƿ���ͬ����Сд������
 * @param s1 {const char*}
 * @param s2 {cosnt char*}
 * @return {int} 0: ��ͬ; < 0: s1 < s2; > 0: s1 > s2
 */
ACL_API int acl_strcasecmp(const char *s1, const char *s2);

/**
 * �Ƚ������ַ����Ƿ���ͬ����Сд�����У�ͬʱ�޶����Ƚϳ���
 * @param s1 {const char*}
 * @param s2 {cosnt char*}
 * @param n {size_t} �޶��Ƚϵ���󳤶�
 * @return {int} 0: ��ͬ; < 0: s1 < s2; > 0: s1 > s2
 */
ACL_API int acl_strncasecmp(const char *s1, const char *s2, size_t n);
/**
 * WINDOWS�²�֧��һЩ�ַ����ȽϺ���
 */
#ifdef	WIN32
# ifndef strcasestr
#  define strcasestr	acl_strcasestr
# endif
# ifndef strcasecmp
#  define strcasecmp	acl_strcasecmp
# endif
# ifndef strncasecmp
#  define strncasecmp	acl_strncasecmp
# endif
#endif

#ifndef strrncasecmp
# define strrncasecmp	acl_strrncasecmp
#endif
#ifndef strrncmp
# define strrncmp	acl_strrncmp
#endif

/*----------------------------------------------------------------------------
 * ��֤���������������ʽ:
 * /home/avwall/test.txt
 * @param psrc_file_path {const char*} Դ�ַ���
 * @param pbuf {char*} �洢������ڴ���
 * @param sizeb {int} pbuf �Ŀռ��С
 * @return {int} 0 �ɹ���-1ʧ��
 */
ACL_API int acl_file_path_correct(const char *psrc_file_path, char *pbuf, int sizeb);

/*----------------------------------------------------------------------------
 * ��֤·���������˺�����Ϊ���¸�ʽ:
 * Դ:   /home/avwall/, /home//////avwall/, /home/avwall, /////home/avwall///
 *       /home/avwall////, /home///avwall///, ///home///avwall///
 * ���: /home/avwall/
 * @param psrc_dir {const char*} Դ�ַ���
 * @param pbuf {char*} �洢������ڴ���
 * @param sizeb {int} pbuf �Ŀռ��С
 * @return {int} 0 ��ʾ�ɹ���-1��ʾʧ��
 */
ACL_API int acl_dir_correct(const char *psrc_dir, char *pbuf, int sizeb);

/*----------------------------------------------------------------------------
 * ������: /home/avwall/log.txt ����ȡ /home/avwall/ ��Ϊ�������
 * @param pathname {const char*} Դ�ַ���
 * @param pbuf {char*} �洢������ڴ���
 * @param bsize {int} pbuf �Ŀռ��С
 * @return {int} 0 �ɹ���-1ʧ��
 */
ACL_API int acl_dir_getpath(const char *pathname, char *pbuf, int bsize);

/**
 * �������ַ���ת��Ϊ64λ�޷��ų�����
 * @param str {const char*} �ַ���ָ��
 * @return {acl_uint64} �޷��ų�����
 */
ACL_API acl_uint64 acl_atoui64(const char *str);

/**
* �������ַ���ת��Ϊ64λ���ų�����
* @param str {const char*} �ַ���ָ��
* @return {acl_int64} �޷��ų�����
*/
ACL_API acl_int64 acl_atoi64(const char *str);

/**
 * ��64λ�޷��ų�����ת��Ϊ�ַ���
 * @param value {acl_uint64} 64λ�޷��ų�����
 * @param buf {char*} ������ȡת��������ڴ�ռ�
 * @param size {sizt_t} buf �Ŀռ��С������Ҫ������СΪ21���ֽ�
 * @return {const char*} ת���Ľ�������ת���ɹ���Ϊ�գ�����Ϊ��
 */
ACL_API const char *acl_ui64toa(acl_uint64 value, char *buf, size_t size);

/**
 * ��64λ���ų�����ת��Ϊ�ַ���
 * @param value {acl_int64} 64λ���ų�����
 * @param buf {char*} ������ȡת��������ڴ�ռ�
 * @param size {sizt_t} buf �Ŀռ��С������Ҫ������СΪ21���ֽ�
 * @return {const char*} ת���Ľ�������ת���ɹ���Ϊ�գ�����Ϊ��
 */
ACL_API const char *acl_i64toa(acl_int64 value, char *buf, size_t size);

/**
 * ��64λ���ų�����ת��Ϊĳ���Ƶ��ַ���
 * @param value {acl_int64} 64λ���ų�����
 * @param buf {char*} ������ȡת��������ڴ�ռ�
 * @param size {sizt_t} buf �Ŀռ��С������Ҫ������СΪ21���ֽ�
 * @param radix {int} ����, ��: 8 ��ʾ�˽���, 10 ��ʾʮ����, 16 ��ʾʮ������
 * @return {const char*} ת���Ľ�������ת���ɹ���Ϊ�գ�����Ϊ��
 */
ACL_API const char *acl_i64toa_radix(acl_int64 value, char *buf, size_t size, int radix);

/**
 * ��64λ�޷��ų�����ת��Ϊĳ���Ƶ��ַ���
 * @param value {acl_int64} 64λ���ų�����
 * @param buf {char*} ������ȡת��������ڴ�ռ�
 * @param size {sizt_t} buf �Ŀռ��С������Ҫ������СΪ21���ֽ�
 * @param radix {int} ����, ��: 8 ��ʾ�˽���, 10 ��ʾʮ����, 16 ��ʾʮ������
 * @return {const char*} ת���Ľ�������ת���ɹ���Ϊ�գ�����Ϊ��
 */
ACL_API const char *acl_ui64toa_radix(acl_uint64 value, char *buf, size_t size, int radix);

#ifdef  __cplusplus
}
#endif

#endif
