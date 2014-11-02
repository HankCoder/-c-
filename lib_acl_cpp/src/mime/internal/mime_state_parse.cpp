#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/string.hpp"
#include "header_opts.hpp"
#include "tok822.hpp"
#include "rec_type.hpp"
#include "lex_822.hpp"
#include "rec_type.hpp"
#include "mime_state.hpp"

#define SCP(x, s)	acl_vstring_strcpy((x), (s))
#define SCAT(x, s)	acl_vstring_strcat((x), (s))
#define STR(x)		acl_vstring_str((x))
#define LEN(x)		ACL_VSTRING_LEN((x))
#define END(x)		acl_vstring_end((x))
#define TERM(x)		ACL_VSTRING_TERMINATE((x))
#define ADDCH(x, ch)	{ ACL_VSTRING_ADDCH((x), (ch)); TERM((x)); }
#define APPEND(x, s, n)	{ acl_vstring_memcat((x), (s), (n)); TERM((x)); }
#define EQUAL(x, y)	!strcasecmp((x), (y))

/*
* MIME encodings and domains. We intentionally use the same codes for
* encodings and domains, so that we can easily find out whether a content
* transfer encoding header specifies a domain or whether it specifies
* domain+encoding, which is illegal for multipart/any and message/any.
*/
typedef struct MIME_ENCODING {
	const char *name;       /* external representation */
	int     encoding;       /* internal representation */
	int     domain;         /* subset of encoding */
} MIME_ENCODING;

static const MIME_ENCODING mime_encoding_map[] = {  /* RFC 2045 */
	{ "7bit", MIME_ENC_7BIT, MIME_ENC_7BIT },       /* domain */
	{ "8bit", MIME_ENC_8BIT, MIME_ENC_8BIT },       /* domain */
	{ "binary", MIME_ENC_BINARY, MIME_ENC_BINARY }, /* domain */
	{ "base64", MIME_ENC_BASE64, MIME_ENC_7BIT },   /* encoding */
	{ "uucode", MIME_ENC_UUCODE, MIME_ENC_7BIT },   /* encoding */
	{ "xxcode", MIME_ENC_XXCODE, MIME_ENC_7BIT },   /* encoding */
	{ "quoted-printable", MIME_ENC_QP, MIME_ENC_7BIT }, /* encoding */
	{ 0, 0, 0 },
};

/*-------------------------------------------------------------------------*/

#define RFC2045_TSPECIALS	"()<>@,;:\\\"/[]?="
//#define RFC2045_TSPECIALS	"()<>@,;:\"/[]?="
#define TOKEN_MATCH(tok, text) \
	((tok).type == HEADER_TOK_TOKEN && EQUAL((tok).u.value, (text)))

static void mime_content_type(MIME_NODE *node, const HEADER_OPTS *header_info)
{
	const char *cp;
	ssize_t tok_count;
	MIME_STATE *state = node->state;

#define PARSE_CONTENT_TYPE_HEADER(state, ptr) \
	header_token(state->token, MIME_MAX_TOKEN, \
		state->token_buffer, ptr, RFC2045_TSPECIALS, ';')

	cp = STR(node->buffer) + strlen(header_info->name) + 1;
	if ((tok_count = PARSE_CONTENT_TYPE_HEADER(state, &cp)) <= 0) {

		/*
		* other/whatever.
		*/
		node->ctype = MIME_CTYPE_OTHER;
		return;
	}

	/* tok_count > 0 */

	/*
	* message/whatever body parts start with another block of message
	* headers that we may want to look at. The partial and external-body
	* subtypes cannot be subjected to 8-bit -> 7-bit conversion, so we
	* must properly recognize them.
	*/
	if (TOKEN_MATCH(state->token[0], "message")) {
		node->ctype = MIME_CTYPE_MESSAGE;
		node->stype = MIME_STYPE_OTHER;
		if (tok_count >= 3 && state->token[1].type == '/') {
			if (TOKEN_MATCH(state->token[2], "rfc822"))
				node->stype = MIME_STYPE_RFC822;
			else if (TOKEN_MATCH(state->token[2], "partial"))
				node->stype = MIME_STYPE_PARTIAL;
			else if (TOKEN_MATCH(state->token[2], "external-body"))
				node->stype = MIME_STYPE_EXTERN_BODY;
		}
	}

	/*
	* multipart/digest has default content type message/rfc822,
	* multipart/whatever has default content type text/plain.
	*/
	else if (TOKEN_MATCH(state->token[0], "multipart")) {
		node->ctype = MIME_CTYPE_MULTIPART;
		if (tok_count >= 3 && state->token[1].type == '/') {
			if (TOKEN_MATCH(state->token[2], "digest")) {
				node->ctype = MIME_CTYPE_MESSAGE;
				node->stype = MIME_STYPE_RFC822;
			} else if (TOKEN_MATCH(state->token[2], "alternative")) {
				node->stype = MIME_STYPE_ALTERNATIVE;
			} else if (TOKEN_MATCH(state->token[2], "related")) {
				node->stype = MIME_STYPE_RELATED;
			} else if (TOKEN_MATCH(state->token[2], "mixed")) {
				node->stype = MIME_STYPE_MIXED;
			} else {
				node->stype = MIME_STYPE_OTHER;
			}
		} else {
			node->ctype = MIME_CTYPE_TEXT;
			node->stype = MIME_STYPE_PLAIN;
		}

		/*
		* Yes, this is supposed to capture multiple boundary strings,
		* which are illegal and which could be used to hide content in
		* an implementation dependent manner. The code below allows us
		* to find embedded message headers as long as the sender uses
		* only one of these same-level boundary strings.
		* 
		* Yes, this is supposed to ignore the boundary value type.
		*/
		while ((tok_count = PARSE_CONTENT_TYPE_HEADER(state, &cp)) >= 0) {
			if (tok_count < 3 || state->token[1].type != '=')
				continue;
			if (TOKEN_MATCH(state->token[0], "boundary")) {
				if (node->boundary == NULL)
					node->boundary = acl_vstring_alloc(256);
				/* ��Ҫ��� "--" ��Ϊ�ָ�����ǰ���� */
				SCP(node->boundary, "--");
				SCAT(node->boundary, state->token[2].u.value);
				break;
			}
		}
	}

	/*
	* text/whatever. Right now we don't really care if it is plain or
	* not, but we may want to recognize subtypes later, and then this
	* code can serve as an example.
	*/
	else if (TOKEN_MATCH(state->token[0], "text")) {
		node->ctype = MIME_CTYPE_TEXT;
		if (tok_count >= 3 && state->token[1].type == '/') {
			if (TOKEN_MATCH(state->token[2], "plain"))
				node->stype = MIME_STYPE_PLAIN;
			else if (TOKEN_MATCH(state->token[2], "html"))
				node->stype = MIME_STYPE_HTML;
			else
				node->stype = MIME_STYPE_OTHER;
		} else
			node->stype = MIME_STYPE_OTHER;

		while ((tok_count = PARSE_CONTENT_TYPE_HEADER(state, &cp)) >= 0) {
			if (tok_count < 3 || state->token[1].type != '=')
				continue;
			if (TOKEN_MATCH(state->token[0], "charset")
				&& node->charset == NULL)
			{
				node->charset = acl_mystrdup(state->token[2].u.value);
				break;
			}
		}

		/* ���û���ַ���, ��ȱʡ���� gb2312 */
		if (node->charset == NULL)
			node->charset = acl_mystrdup("gb2312");
	}
	else if (TOKEN_MATCH(state->token[0], "image")) {
		node->ctype = MIME_CTYPE_IMAGE;
		if (tok_count >= 3 && state->token[1].type == '/') {
			if (TOKEN_MATCH(state->token[2], "jpeg"))
				node->stype = MIME_STYPE_JPEG;
			else if (TOKEN_MATCH(state->token[2], "gif"))
				node->stype = MIME_STYPE_GIF;
			else if (TOKEN_MATCH(state->token[2], "bmp"))
				node->stype = MIME_STYPE_BMP;
			else if (TOKEN_MATCH(state->token[2], "png"))
				node->stype = MIME_STYPE_PNG;
			else
				node->stype = MIME_STYPE_OTHER;
		} else
			node->stype = MIME_STYPE_OTHER;

		while ((tok_count = PARSE_CONTENT_TYPE_HEADER(state, &cp)) >= 0) {
			if (tok_count < 3 || state->token[1].type != '=')
				continue;
			if (TOKEN_MATCH(state->token[0], "name")
				&& node->header_name == NULL)
			{
				node->header_name = acl_mystrdup(state->token[2].u.value);
				break;
			}
		}
	}
	else if (TOKEN_MATCH(state->token[0], "application")) {
		node->ctype = MIME_CTYPE_APPLICATION;
		if (tok_count >= 3 && state->token[1].type == '/') {
			if (TOKEN_MATCH(state->token[2], "octet-stream"))
				node->stype = MIME_STYPE_OCTET_STREAM;
			else
				node->stype = MIME_STYPE_OTHER;
		}
		while ((tok_count = PARSE_CONTENT_TYPE_HEADER(state, &cp)) >= 0) {
			if (tok_count < 3 || state->token[1].type != '=')
				continue;
			if (TOKEN_MATCH(state->token[0], "name")
				&& node->header_name == NULL)
			{
				node->header_name = acl_mystrdup(state->token[2].u.value);
				break;
			}
		}
	}
}

/* mime_state_content_encoding - process content-transfer-encoding header */

static void mime_content_encoding(MIME_NODE *node,
	const HEADER_OPTS *header_info)
{
	MIME_STATE *state = node->state;
	const char *cp;
	const MIME_ENCODING *cmp;

#define PARSE_CONTENT_ENCODING_HEADER(state, ptr) \
	header_token(state->token, 1, state->token_buffer, ptr, (char *) 0, 0)

	/*
	* Do content-transfer-encoding header. Never set the encoding domain to
	* something other than 7bit, 8bit or binary, even if we don't recognize
	* the input.
	*/
	cp = STR(node->buffer) + strlen(header_info->name) + 1;
	if (PARSE_CONTENT_ENCODING_HEADER(state, &cp) > 0
		&& state->token[0].type == HEADER_TOK_TOKEN)
	{
		for (cmp = mime_encoding_map; cmp->name != 0; cmp++) {
			if (EQUAL(state->token[0].u.value, cmp->name)) {
				node->encoding = cmp->encoding;
				node->domain = cmp->domain;
				break;
			}
		}
	}
}

static void mime_content_disposition(MIME_NODE *node,
	const HEADER_OPTS *header_info)
{
	const char *cp = STR(node->buffer) + strlen(header_info->name) + 1;
	ssize_t tok_count;

#define PARSE_CONTENT_DISPOSITION(state, ptr) \
	header_token(state->token, MIME_MAX_TOKEN, \
		state->token_buffer, ptr, RFC2045_TSPECIALS, ';')

	while (1) {
		tok_count = PARSE_CONTENT_DISPOSITION(node->state, &cp);
		if (tok_count <= 0)
			break;

		if (tok_count < 3 || node->state->token[1].type != '=')
			continue;

		if (TOKEN_MATCH(node->state->token[0], "filename")
			&& node->header_filename == NULL)
		{
			node->header_filename =
				acl_mystrdup(node->state->token[2].u.value);
		} else if (TOKEN_MATCH(node->state->token[0], "name")
			&& node->header_name == NULL)
		{
			node->header_name =
				acl_mystrdup(node->state->token[2].u.value);
		}
	}
}

/* mime_state_downgrade - convert 8-bit data to quoted-printable */

void mime_state_downgrade(MIME_STATE *state, int rec_type,
	const char *text, ssize_t len)
{
	static char hexchars[] = "0123456789ABCDEF";
	const unsigned char *cp;
	MIME_NODE *node = state->curr_node;
	int     ch;

#define CU_CHAR_PTR(x)	((const unsigned char *) (x))
#define QP_ENCODE(buffer, ch) { \
	buffer += '='; \
	buffer += (char) hexchars[(ch >> 4) & 0xff]; \
	buffer += (char) hexchars[ch & 0xf]; \
}

	/*
	* Insert a soft line break when the output reaches a critical length
	* before we reach a hard line break.
	*/
	for (cp = CU_CHAR_PTR(text); cp < CU_CHAR_PTR(text + len); cp++) {
		/* Critical length before hard line break. */
		if (LEN(node->buffer) > 72) {
			node->buffer += '=';
		}
		/* Append the next character. */
		ch = *cp;
		if ((ch < 32 && ch != '\t') || ch == '=' || ch > 126) {
			QP_ENCODE(node->buffer, ch);
		} else {
			ADDCH(node->buffer, ch);
		}
	}

	/*
	* Flush output after a hard line break (i.e. the end of a REC_TYPE_NORM
	* record). Fix trailing whitespace as per the RFC: in the worst case,
	* the output length will grow from 73 characters to 75 characters.
	*/
	if (rec_type == REC_TYPE_NORM) {
		if (LEN(node->buffer) > 0
			&& ((ch = END(node->buffer)[-1]) == ' ' || ch == '\t'))
		{
			acl_vstring_truncate(node->buffer, LEN(node->buffer) - 1);
			QP_ENCODE(node->buffer, ch);
		}
		ACL_VSTRING_TERMINATE(node->buffer);
	}
}

static ACL_FIFO *mail_addr_add(ACL_FIFO *addr_list, const char *addr)
{
	MAIL_ADDR *mail_addr;

	if (addr_list == NULL)
		addr_list = acl_fifo_new();
	mail_addr = (MAIL_ADDR*) acl_mycalloc(1, sizeof(MAIL_ADDR));
	mail_addr->addr = acl_mystrdup(addr);
	acl_fifo_push(addr_list, mail_addr);
	return (addr_list);
}

static void mail_rcpt(MIME_NODE *node, const HEADER_OPTS *header_info)
{
	TOK822 *tree;
	TOK822 **addr_list;
	TOK822 **tpp;
	ACL_VSTRING *temp;
	const char *cp;

	temp = acl_vstring_alloc(10);
	cp = STR(node->buffer) + strlen(header_info->name) + 1;
	tree = tok822_parse(cp);
	addr_list = tok822_grep(tree, TOK822_ADDR);
	for (tpp = addr_list; *tpp; tpp++) {
		tok822_internalize(temp, tpp[0]->head, TOK822_STR_DEFL);
		if (header_info->type == HDR_TO) {
			node->header_to_list =
				mail_addr_add(node->header_to_list, STR(temp));
		} else if (header_info->type == HDR_CC) {
			node->header_cc_list =
				mail_addr_add(node->header_cc_list, STR(temp));
		} else if (header_info->type == HDR_BCC) {
			node->header_bcc_list =
				mail_addr_add(node->header_bcc_list, STR(temp));
		}
	}

	acl_myfree(addr_list);
	tok822_free_tree(tree);
	acl_vstring_free(temp);
}

static void mail_from(MIME_NODE *node, const HEADER_OPTS *header_info)
{
	//MIME_STATE *state = node->state;
	TOK822 *tree;
	TOK822 **addr_list;
	TOK822 **tpp;
	ACL_VSTRING *temp;
	const char *cp;

	temp = acl_vstring_alloc(10);
	cp = STR(node->buffer) + strlen(header_info->name) + 1;
	tree = tok822_parse(cp);
	addr_list = tok822_grep(tree, TOK822_ADDR);
	for (tpp = addr_list; *tpp; tpp++) {
		tok822_internalize(temp, tpp[0]->head, TOK822_STR_DEFL);
		if (header_info->type == HDR_SENDER) {
			if (node->header_sender)
				acl_myfree(node->header_sender);
			node->header_sender = acl_mystrdup(STR(temp));
			break;
		} else if (header_info->type == HDR_FROM) {
			if (node->header_from)
				acl_myfree(node->header_from);
			node->header_from = acl_mystrdup(STR(temp));
			break;
		} else if (header_info->type == HDR_REPLY_TO) {
			if (node->header_replyto)
				acl_myfree(node->header_replyto);
			node->header_replyto = acl_mystrdup(STR(temp));
			break;
		} else if (header_info->type == HDR_RETURN_PATH) {
			if (node->header_returnpath)
				acl_myfree(node->header_returnpath);
			node->header_returnpath = acl_mystrdup(STR(temp));
		}
	}

	acl_myfree(addr_list);
	
	tok822_free_tree(tree);
	acl_vstring_free(temp);
}

static void mail_subject(MIME_NODE *node, const HEADER_OPTS *header_info)
{
	const char *cp;

	if (header_info->type != HDR_SUBJECT)
		return;

	cp = STR(node->buffer) + strlen(header_info->name) + 1;
	if (strlen(cp) == 0)
		return;
	if (node->header_subject)
		acl_myfree(node->header_subject);
	node->header_subject = acl_mystrdup(cp);
}

static void mime_header_line(MIME_NODE *node)
{
	const HEADER_OPTS *header_info;
	size_t len = LEN(node->buffer);
	char *ptr = strrchr(STR(node->buffer), '\n');

	if (ptr) {
		*ptr-- = 0;
		len--;
		if (ptr > STR(node->buffer)) {
			if (*ptr == '\r') {
				*ptr = 0;
				len--;
			}
		}
		if (len == 0)
			return;

		acl_vstring_truncate(node->buffer, len);
	}

	header_info = header_opts_find(STR(node->buffer), node->state->key_buffer);
	if (header_info) {
		if (header_info->type == HDR_CONTENT_TYPE)
			mime_content_type(node, header_info);
		else if (header_info->type == HDR_CONTENT_TRANSFER_ENCODING)
			mime_content_encoding(node, header_info);
		else if (node == node->state->root) {
			/* ˵���ʼ�ͷ */

			if ((header_info->flags & HDR_OPT_RECIP)
				&& (header_info->flags & HDR_OPT_EXTRACT))
			{
				/* �����ռ��˵�ַ: To, Cc, Bcc */
				mail_rcpt(node, header_info);
			} else if ((header_info->flags & HDR_OPT_SENDER)) {
				/* ���������˵�ַ: From, Sender,
				*  Replyto, Returnpath
				*/
				mail_from(node, header_info);
			} else if ((header_info->flags & HDR_OPT_SUBJECT)) {
				mail_subject(node, header_info);
			}
		} else if (header_info->type == HDR_CONTENT_DISPOSITION) {
			mime_content_disposition(node, header_info);
		}
	}

	if (node->header_list) {
		HEADER_NV *header = header_split(STR(node->buffer));
		if (header)
			node->header_list->push_back(node->header_list, header);
	}

	ACL_VSTRING_RESET(node->buffer); /* ��ջ����� */
	node->last_ch = 0;
	node->last_lf = 0;
}

/* ״̬�����ݽṹ���� */

struct MIME_STATUS_MACHINE {
	/**< ״̬�� */
	int   status;

	/**< ״̬�������� */
	int (*callback) (MIME_STATE*, const char*, int);
};

// �����ʼ�ͷ�� multipart �����ֵ�ͷ
static int mime_state_head(MIME_STATE *state, const char *s, int n)
{
	MIME_NODE *node = state->curr_node;

	if (n <= 0)
		return (n);

	/* �����δ�ҵ����з�������� */

	if (node->last_lf != '\n') {
		while (n > 0) {
			node->last_ch = *s;
			ADDCH(node->buffer, node->last_ch);
			n--;
			state->curr_off++;

			if (node->last_ch == '\n') {
				node->last_lf = '\n';
				break;
			}
			s++;
		}

		return (n);
	}

	/* ��������Ի��п�ʼ�� ˵����ǰ���ʼ�ͷ���� */

	if (*s == '\n') {
		/* �ϴ�����Ϊ: \n\r �� \n */

		state->curr_off++;
		node->header_end = state->curr_off;

		if (LEN(node->buffer) > 0) {
			/* ����ͷ�������һ������ */
			mime_header_line(node);
			node->valid_line++;
		}

		/* �Թ���ͷ���õĿ��� */
		if (node->valid_line == 0)
			return (0);

		/* �����ǰ���Ϊ multipart ��ʽ, ������ state->curr_bound */
		if (node->boundary != NULL)
			state->curr_bound = STR(node->boundary);
		state->curr_status = MIME_S_BODY;
		node->body_begin = state->curr_off;
		return (n - 1);
	}
	if (*s == '\r') {
		state->curr_off++;
		if (node->last_ch == '\r') {
			/* XXX: ������ \n\r\r ���� */
			node->last_ch = '\r';
			node->last_lf = 0;
			return (n - 1);
		}

		node->last_ch = '\r';
		/* ����, ���ڴ���һ���ַ�Ϊ '\n' */
		return (n - 1);
	}

	/* ��� '\n' */
	node->last_lf = 0;

	/* ��������Կո��TAB��ʼ�� ˵�����ݸ�������һ�� */

	if (IS_SPACE_TAB(*s)) {
		/* ˵���������ݸ�������һ������ */
		while (n > 0) {
			node->last_ch = *s;
			ADDCH(node->buffer, node->last_ch);
			n--;
			state->curr_off++;

			if (node->last_ch == '\n') {
				/* �����걾���������� */
				node->last_lf = '\n';
				break;
			}
			s++;
		}
		return (n);
	}

	/* ����ͷ������һ������ */

	if (LEN(node->buffer) > 0) {
		mime_header_line(node);
		node->valid_line++;
	}
	return (n);
}

// ���� multipart ������, ��ƥ�䵽һ�������ķָ�����������ò���������������
static int mime_bound_body(const char *boundary, MIME_NODE *node,
	const char *s, int n, int *finish)
{
	const unsigned char *cp;
	const unsigned char *startn = NULL;

	for (cp = (const unsigned char *) s;
		cp < (const unsigned char *) s + n; cp++)
	{
		// ��¼�� \r\n ��λ��
		if (*cp == '\r')
			node->last_cr_pos = node->state->curr_off;
		else if (*cp == '\n')
			node->last_lf_pos = node->state->curr_off;

		node->state->curr_off++;
		if (node->bound_ptr) {
			if (*cp == *node->bound_ptr) {
				node->bound_ptr++;
				if (*node->bound_ptr == 0) {

					/* ˵����ȫƥ�� */
					*finish = 1;

					node->body_end = node->state->curr_off
						- strlen(node->state->curr_bound);
					node->body_data_end = node->body_end;

					// ��Ϊ body_end ��¼����ĳ���������λ�ã����л����
					// ����Э�鸽�ӵ� \r\n��������ʵ���ݵĽ���λ�� body_data_end
					// ����ȥ����Щ���ݺ��λ��
					if (node->last_lf_pos + (off_t) strlen(boundary)
						== node->state->curr_off - 1)
					{
						node->body_data_end--;
						if (node->last_cr_pos + 1 == node->last_lf_pos)
							node->body_data_end--;
					}

					if (startn > (const unsigned char *) s) {

						/* ��ƥ��֮ǰ�����ݿ��� */
						APPEND(node->body, (const char*) s,
							(const char*) startn - s);
					}
					node->bound_ptr = NULL;
					cp++;
					break;
				}
			} else {
				/* ˵��֮ǰ��ƥ��ʧЧ����Ҫ����ƥ�䣬
				 * �����뽫֮ǰƥ����ַ�����
				 */

				if (node->bound_ptr > boundary) {
					APPEND(node->body, boundary,
						node->bound_ptr - boundary);
				}
				node->bound_ptr = NULL;
			}
		}
		if (!node->bound_ptr) {
			if (*cp == *boundary) {
				node->bound_ptr = boundary + 1;

				/* ˵����ȫƥ�� */
				if (*node->bound_ptr == 0) {
					node->body_end = node->state->curr_off
						- strlen(node->state->curr_bound);
					node->body_data_end = node->body_end;

					// ��Ϊ body_end ��¼����ĳ���������λ�ã����л����
					// ����Э�鸽�ӵ� \r\n��������ʵ���ݵĽ���λ�� body_data_end
					// ����ȥ����Щ���ݺ��λ��
					if (node->last_lf_pos + (off_t) strlen(boundary)
						== node->state->curr_off - 1)
					{
						node->body_data_end--;
						if (node->last_cr_pos + 1 == node->last_lf_pos)
							node->body_data_end--;
					}
					*finish = 1;
					node->bound_ptr = NULL;
					cp++;
					break;
				}
				startn = cp;
			} else {
				ADDCH(node->body, *cp);
			}
		}
	}

	return (n - ((const char*) cp - s));
}

// �����ʼ���� multipart ������
static int mime_state_body(MIME_STATE *state, const char *s, int n)
{
	int   finish = 0;

	if (state->curr_node->body == NULL)
		state->curr_node->body = acl_vstring_alloc(1024);

	if (state->curr_bound == NULL) {

		/* ���û�зָ�������˵�����ı����ͣ���ֻ���������� */

		APPEND(state->curr_node->body, s, n);
		state->curr_off += n;

		/* ��Ϊ curr_off ָ�������һ��ƫ��λ�ã����� body_end = curr_off - 1 */
		state->curr_node->body_end = state->curr_off - 1;
		state->curr_node->body_data_end = state->curr_node->body_end;  // add by zsx, 2012.5.28
		return (0);
	}

	n  = mime_bound_body(state->curr_bound, state->curr_node, s, n, &finish);
	if (finish)
		state->curr_status = MIME_S_BODY_BOUND_CRLF;

	return (n);
}

// ���ҷָ������ "\r\n"
static int mime_state_body_bound_crlf(MIME_STATE *state, const char *s, int n)
{
	if (n <= 0)
		return (n);

	/* ������Ƿָ������������ "--" ��˵��������������ɱ��ָ����ָ� */

	if (*s == '\n') {
		state->curr_node->last_lf_pos = state->curr_off;
		state->curr_node->last_ch = '\n';
		
		state->curr_off++;
		state->curr_node->bound_end = state->curr_node->state->curr_off;

		/*
		state->curr_node->body_end = state->curr_node->bound_end
			- strlen(state->curr_bound) - 1;
		if (state->curr_node->last_ch == '\r')
			state->curr_node->body_end--;
		*/

		/* ���������Ϊ "--\r\n", ˵������������,
		 * ͬʱ�����ĸ����Ҳ����
		 */
		if (state->curr_node->bound_term[0] == '-'
			&& state->curr_node->bound_term[1] == '-')
		{
			/* ˵�����ָ�����Լ���Ľ����� */

			if (state->curr_node == state->root) {
				/* xxx: ����㲻ӦΪ����� */
				state->curr_status = MIME_S_TERM;
			} else if (state->curr_node->parent == state->root) {
				/* ˵��������һ���ӽ�㶼����, �������ʼ�������� */
				state->curr_status = MIME_S_TERM;
			} else {
				/* ˵�������Ϊ�����Ķ��������Ͻ��, ͬʱ˵��
				 * �����ĸ�����Ѿ�����, ��һ����Ҫ�ҳ�������
				 * үү���ķָ���
				 */

				/* ֻ�и���� root �ĸ����Ϊ NULL */
				acl_assert(state->curr_node->parent);
				acl_assert(state->curr_node->parent->boundary);

				state->curr_node = state->curr_node->parent;
				state->curr_bound = STR(state->curr_node->parent->boundary);
				
				state->curr_status = MIME_S_MULTI_BOUND;
				state->curr_node->bound_ptr = NULL;
			}
		} else {
			acl_assert(state->curr_node != NULL);

			MIME_NODE *node = mime_node_new(state);

			node->header_begin = state->curr_off;
			if (state->curr_node->boundary != NULL) {
				acl_assert(state->curr_bound == STR(state->curr_node->boundary));
				mime_node_add_child(state->curr_node, node);
			} else {
				acl_assert(state->curr_node->parent->boundary != NULL);
				acl_assert(state->curr_bound == STR(state->curr_node->parent->boundary));
				mime_node_add_child(state->curr_node->parent, node);
			}
			state->curr_node = node;
			state->curr_status = MIME_S_HEAD;
			state->curr_node->last_ch = 0;
			state->curr_node->last_lf = 0;
		}
		return (n - 1);
	} else if (*s == '\r') {
		state->curr_node->last_cr_pos = state->curr_off;
		state->curr_node->last_ch = '\r';
		state->curr_off++;
		state->use_crlf = 1;
		/* �ڴ���һ���ַ�Ϊ '\n' */
		return (n - 1);
	} else if (*s == '-') {
		state->curr_off++;
		if (state->curr_node->bound_term[0] == '-') {
			state->curr_node->bound_term[1] = '-';
			/* �ڴ���һ���ַ�Ϊ '\r' �� '\n' */
			return (n - 1);
		} else {
			/* �ڴ���һ���ַ�Ϊ '-' */
			state->curr_node->bound_term[0] = '-';
			return (n - 1);
		}
	} else {
		/* XXX: �ָ�����Ƿ��ַ� ? */

		/* ���������, ����ʼ�������ڸø�������һ����� */

		state->curr_node = state->curr_node->parent;
		state->curr_off++;

		/* xxx */
		if (state->curr_node == NULL)
			state->curr_node = state->root;
		state->curr_status = MIME_S_HEAD;
		state->curr_node->last_ch = 0;
		state->curr_node->last_lf = 0;
		return (n - 1);
	}
}

static int mime_state_multi_bound(MIME_STATE *state, const char *s, int n)
{
	MIME_NODE *node = state->curr_node;
	const unsigned char *cp;
	//const unsigned char *startn = NULL;

	acl_assert(state->curr_bound != NULL);
	acl_assert(node->parent != NULL);
	acl_assert(node->parent->boundary != NULL);

	const char *boundary = state->curr_bound;

	for (cp = (const unsigned char *) s;
		cp < (const unsigned char *) s + n; cp++)
	{
		// ��¼�� \r\n ��λ��
		if (*cp == '\r')
			node->last_cr_pos = state->curr_off;
		else if (*cp == '\n')
			node->last_lf_pos = state->curr_off;

		state->curr_off++;
		if (node->bound_ptr) {
			if (*cp == *node->bound_ptr) {
				node->bound_ptr++;
				if (*node->bound_ptr == 0) {

					/* ˵����ȫƥ�� */

					state->curr_status = MIME_S_MULTI_BOUND_CRLF;
					node->bound_ptr = NULL;
					cp++;
					break;
				}
			} else {

				/* ˵��֮ǰ��ƥ��ʧЧ����Ҫ����ƥ�䣬
				 * �����뽫֮ǰƥ����ַ�����
				 */

				node->bound_ptr = NULL;
			}
		}
		if (!node->bound_ptr && *cp == *boundary) {
			node->bound_ptr = boundary + 1;

			/* ˵����ȫƥ�� */
			if (*node->bound_ptr == 0) {
				state->curr_status = MIME_S_MULTI_BOUND_CRLF;
				node->bound_ptr = NULL;
				cp++;
				break;
			}
			//startn = cp;
		}
	}

	return (n - ((const char*) cp - s));
}

static int mime_state_multi_bound_crlf(MIME_STATE *state, const char *s, int n)
{
	if (*s == '\n') {
		MIME_NODE *node = mime_node_new(state);

		node->last_lf_pos = state->curr_off;
		state->curr_off++;

		/* ���� multipart �����λ�ã���λ�ð������������ӽ������ */
		state->curr_node->bound_end = state->curr_off;

		/*
		state->curr_node->body_end = state->curr_node->bound_end
			- strlen(state->curr_bound) - 1;
		if (state->curr_node->last_ch == '\r')
			state->curr_node->body_end--;
		*/

		node->header_begin = state->curr_off;
		mime_node_add_child(state->curr_node->parent, node);
		state->curr_node = node;
		state->curr_status = MIME_S_HEAD;
		node->bound_ptr = NULL;
		node->last_ch = 0;
		node->last_lf = 0;
		return (n - 1);
	} else if (*s == '\r') {
		state->curr_node->last_cr_pos = state->curr_off;
		state->curr_node->last_ch = '\r';
		state->curr_off++;
		state->use_crlf = 1;
		/* �ڴ���һ���ַ�Ϊ '\n' */
		return (n - 1);
	} else {
		/* xxx */
		state->curr_off += n;
		return (0);
	}
}

static int mime_state_term(MIME_STATE *state, const char *s, int n)
{
	(void) s;
	state->curr_off += n;
	return (0);
}

static struct MIME_STATUS_MACHINE status_tab[] = {
	{ MIME_S_HEAD, mime_state_head },
	{ MIME_S_BODY, mime_state_body },
	{ MIME_S_BODY_BOUND_CRLF, mime_state_body_bound_crlf },
	{ MIME_S_MULTI_BOUND, mime_state_multi_bound },
	{ MIME_S_MULTI_BOUND_CRLF, mime_state_multi_bound_crlf },
	{ MIME_S_TERM, mime_state_term },
};

int mime_state_update(MIME_STATE *state, const char *ptr, int n)
{
	const char *s = ptr;
	while (n > 0) {
		int ret = status_tab[state->curr_status].callback(state, s, n);
		if (state->curr_status == MIME_S_TERM)
			return (1);
		acl_assert(ret >= 0);
		if (ret == 0)
			break;
		s += n - ret;
		n = ret;
	}

	return (0);
}
