#ifndef	ACL_TEST_GLOBAL_INCLUDE_H
#define	ACL_TEST_GLOBAL_INCLUDE_H

# ifdef	__plusplus
extern "C" {
# endif

#include "stdlib/acl_define.h"
#include "stdlib/acl_loadcfg.h"
#include "stdlib/acl_array.h"

#include "acl_test_struct.h"

/*-----------------------  �ڲ������ӿ�  -----------------------------------*/
/* in acl_test_cfg.c */
ACL_API ACL_ARRAY *aut_parse_args_list(const char *str_in);
ACL_API void aut_free_args_list(ACL_ARRAY *a);

/* in acl_test_cfg_general.c */
ACL_API int aut_cfg_add_general_line(const ACL_CFG_LINE *line);

/* in acl_test_inner.c*/
ACL_API AUT_LINE *aut_add_inner_cmd(const ACL_CFG_LINE *line);

/* in acl_test_outer.c */
ACL_API AUT_LINE *aut_add_outer_cmd(const ACL_CFG_LINE *line);

/* in acl_test_token.c */
ACL_API AUT_CMD_TOKEN *aut_line_peer_token(const AUT_LINE *test_line);
ACL_API AUT_LINE *aut_line_peer(const AUT_LINE *test_line);

/* in acl_test_misc.c */
ACL_API void aut_line_free(void *ctx);

/*------------------ �ⲿ���Ե��õ�һЩ�����ӿ� ----------------------------*/

/* in acl_test_cfg.c */

/**
 * ����:  ��ȡ�����ļ�
 * @param  pathname �����ļ���
 * @return  0 ��ʾ�ɹ�, -1 ��ʾʧ��
 */
ACL_API int aut_cfg_parse(const char *pathname);

/**
 * ����: ��ӡ������Ч������������
 * @return �ɹ�: 0, ʧ��: -1
 */
ACL_API int aut_cfg_print(void);

/* in acl_test_misc.c */

ACL_API AUT_LINE *aut_line_new(const ACL_CFG_LINE *cfg_line);

/**
 * ����: ��ö�Ӧ�������ֵ����ò������ϵ�����, �Զ�̬�����ʾ
 * @param cmd_name ������
 * @return �ɹ�: �ǿ�ָ��, ʧ��: ��ָ��
 */
ACL_API const ACL_ARRAY *aut_args_get(const char *cmd_name);

/**
 * ����: �����Ч�����������Ŀ
 * @return �ɹ�: >0, ʧ��: -1
 */
ACL_API int aut_size(void);

/**
 * ����: �����±�ֵȡ������Ӧ����Ч�����нṹָ��
 * @param idx �±�����ֵ
 * @return �ɹ�: �ǿսṹָ��, ʧ��: ��ָ��
 *
 */
ACL_API AUT_LINE *aut_index(int idx);

/**
 * ����: �Ƚ������������Ƿ���  test_line ������¼��������ͬ
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @param cmd_name: ����ѯ������
 * @return �������򷵻� 0, ��������򷵻ط� 0
 */
ACL_API int aut_line_cmdcmp(const AUT_LINE *test_line, const char *cmd_name);

/**
 * ����: �Ƚ�������ִ�н��ֵ�Ƿ��������ļ����������Ľ��ֵ���
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @param result: ����ĳ����������ִ�н��ֵ
 * @return ����򷵻� 0, ������򷵻ط� 0
 */
ACL_API int aut_line_resultcmp(const AUT_LINE *test_line, int result);

/**
 * ����: ȡ�ø���Ч�������������ļ��е��к�λ��
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @return �ɹ�: >=0, ��ֵ��Ϊ�к�, ʧ��: < 0
 */
ACL_API int aut_line_number(const AUT_LINE *test_line);

/**
 * ����: ȡ�����������е���Ч�к�
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @return �ɹ�: >=0, ��ֵ��Ϊ�к�, ʧ��: < 0
 */
ACL_API int aut_line_valid_linenum(const AUT_LINE *test_line);

/**
 * ����: ��ø������е�������
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @return ��ȷ��� 0, �����򷵻ط� 0
 */
ACL_API const char *aut_line_cmdname(const AUT_LINE *test_line);

/**
 * ����: ���ظ��������в����ĸ���
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @return �ɹ�: >= 0, ʧ��: -1
 */
ACL_API int aut_line_argc(const AUT_LINE *test_line);

/**
 * ����: ��ȡ�����ļ����еĲ���ֵ
 * @param  test_line AUT_LINE �ṹָ��
 * @param  name Ҫ���ҵĹؽ���
 * @return �ɹ�: �ǿ�ָ��, ʧ��: ��ָ��
 */
ACL_API const char *aut_line_getvalue(const AUT_LINE *test_line, const char *name);

/**
 * ����: ���ظ������е�����
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @return �ɹ�: �ǿ�ָ��, ʧ��: ��ָ��
 */
ACL_API const char *aut_line_argstr(const AUT_LINE *test_line);

/**
 * ����: ���ظ���������������ִ�н��ֵ
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @return ������ִ�н��ֵ
 * ˵��: û�г�������, �������Ĳ����Ƿ��򷵻� -1, ���޷����ָ� -1 ֵ��
 *       �Ƿ�ֵ����������ִ�н��ֵ
 */
ACL_API int aut_line_result(const AUT_LINE *test_line);

/**
 * ����: �Ƿ��������������еĽ����ֶ�
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @return ��: 1, ��: 0
 */
ACL_API int aut_line_stop(const AUT_LINE *test_line);

/**
 * ����: �ڲ������ֶ�, �������ڲ���������������Ҫ����
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @return ��: 1, ��: 0
 */
ACL_API int aut_line_reserved(AUT_LINE *test_line);

/**
 * ����: �����߿��԰��Լ��Ĳ�����ӵ� test_line ֮��
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @param arg: �û�Ҫ��ӵĲ���
 * @return �ɹ�: 0, ʧ��: -1
 */
ACL_API int aut_line_add_arg(AUT_LINE *test_line, void *arg);

/**
 * ����: �� test_line ɾ���û��Լ��Ĳ���
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @param free_fn: �û��Լ�����������
 */
ACL_API void aut_line_del_arg(AUT_LINE *test_line, void (*free_fn) (void *));

/**
 * ����: �� test_line ��ȡ���û��Լ��Ĳ���
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @return �ɹ�: �ǿ�ָ��, �������ָ��Ϊ�����п������ڲ�����������ǿ�
 */
ACL_API void *aut_line_get_arg(const AUT_LINE *test_line);

/**
 * ����: ȡ�����������еĽ�β��
 * @param start_linenum ���ʼִ�е������к�
 * @return >= 0 ok; < 0 δ�ҵ�
 */
ACL_API int aut_end_linenum(int start_linenum);

/**
 * ����: �ӵ�ǰ���ṩ�� test_line ��, һֱ�����ҵ�ĳ�������ṩ��־λ��ͬ��
 *       test_line.
 * @param test_line: ��ĳһ��Ч��������صĽṹָ��
 * @param flag: defined as AUT_FLAG_ in acl_test_struct.h
 * @return != NULL, ok find it; == NULL, not found.
 */
ACL_API const AUT_LINE *aut_lookup_from_line(const AUT_LINE *test_line, int flag);

/* in acl_test_runner.c */

/**
 * ���� ִ������ע��Ĳ��Ժ���, ������κ�һ������ִ�еĽ����Ԥ�ڽ����һ������
 *      ��ִ��
 * @return �ɹ�: 0, ʧ��: -1
 */
ACL_API int aut_start(void);

/**
 * ���Թ��̽�������Ҫ���ô˺������ͷ�һЩ�ڴ���Դ
 */
ACL_API void aut_stop(void);

/**
 * ����  ����Ҫ���е�Ԫ���Ե�������ע��
 * @param test_fn_tab ��Ԫ���Ժ����ṹ����
 */
ACL_API void aut_register(const AUT_FN_ITEM test_fn_tab[]);

/* in acl_test_loop.c */
ACL_API AUT_LINE *aut_loop_make_begin(const ACL_CFG_LINE *cfg_line);
ACL_API AUT_LINE *aut_loop_make_break(const ACL_CFG_LINE *cfg_line);
ACL_API AUT_LINE *aut_loop_make_end(const ACL_CFG_LINE *cfg_line);
ACL_API const AUT_LINE *aut_loop_end(const AUT_LINE *test_begin);
ACL_API int aut_loop_count(const AUT_LINE *test_line);
ACL_API int aut_loop_from(const AUT_LINE *test_line);
ACL_API int aut_loop_to(const AUT_LINE *test_line);

/* in acl_test_log.c */

ACL_API int  aut_log_open(const char *pathname);
ACL_API void aut_log_info(const char *format, ...);
ACL_API void aut_log_warn(const char *format, ...);
ACL_API void aut_log_error(const char *format, ...);
ACL_API void aut_log_fatal(const char *format, ...);
ACL_API void aut_log_panic(const char *format, ...);

# ifdef	__plusplus
}
# endif

#endif

