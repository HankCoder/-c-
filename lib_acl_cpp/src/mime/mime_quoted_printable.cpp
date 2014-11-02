#include "acl_stdafx.hpp"
#include "acl_cpp/mime/mime_quoted_printable.hpp"

namespace acl {

#define CU_CHAR_PTR(x)	((const unsigned char *) (x))

mime_quoted_printable::mime_quoted_printable()
{
	m_addCrLf = true;
	m_addInvalid = false;
	reset();
}

mime_quoted_printable::mime_quoted_printable(bool addCrlf, bool addInvalid)
{
	m_addCrLf = addCrlf;
	m_addInvalid = addInvalid;
	reset();
}

mime_quoted_printable::~mime_quoted_printable()
{

}

void mime_quoted_printable::reset()
{
	m_encodeCnt = 0;
	m_decodeCnt = 0;
}

void mime_quoted_printable::add_crlf(bool on)
{
	m_addCrLf = on;
}

void mime_quoted_printable::add_invalid(bool on)
{
	m_addInvalid = on;
}

void mime_quoted_printable::encode_update(const char *src, int n, acl::string* out)
{
	int  i = 0;

	while (n > 0) {
		if (m_encodeCnt == (int) sizeof(m_encodeBuf))
			encode(out);
		i = n;
		if (i > (int) sizeof(m_encodeBuf) - m_encodeCnt)
			i = (int) sizeof(m_encodeBuf) - m_encodeCnt;
		memcpy(m_encodeBuf + m_encodeCnt, src, i);
		m_encodeCnt += i;
		src += i;
		n -= i;
	}
}

static char hexchars[] = "0123456789ABCDEF";

#define QP_ENCODE(buffer, ch) { \
	(buffer)->push_back('='); \
	(buffer)->push_back((char) hexchars[((ch) >> 4) & 0xff]); \
	(buffer)->push_back((char) hexchars[(ch) & 0xf]); \
}
#define QP_SOFT_LINE(buffer) { \
	(buffer)->push_back('='); \
	(buffer)->push_back('\r'); \
	(buffer)->push_back('\n'); \
}

void mime_quoted_printable::encode_finish(acl::string* out)
{
	encode(out);
	if (m_encodeCnt == 1)
		QP_ENCODE(out, m_encodeBuf[0]);
	m_encodeCnt = 0;
}

void mime_quoted_printable::encode(const char* in, int n, acl::string* out)
{
	mime_quoted_printable coder(false, false);
	coder.encode_update(in, n, out);
	coder.encode_finish(out);
}

void mime_quoted_printable::decode(const char* in, int n, acl::string* out)
{
	mime_quoted_printable decoder(false, false);
	decoder.decode_update(in, n, out);
	decoder.decode_finish(out);
}

void mime_quoted_printable::encode(acl::string* out)
{
	const unsigned char *cp;
	const unsigned char *end = CU_CHAR_PTR(m_encodeBuf + m_encodeCnt);
	int     n;

	/*
	* Insert a soft line break when the output reaches
	* a critical length before we reach a hard line break.
	*/
	for (cp = CU_CHAR_PTR(m_encodeBuf), n = 0; cp < end; cp++) {
		if (*cp == ' ' || *cp == '\t') {
			if (cp + 1 == end) {
				/* �޷�ȷ�������ֽ��Ƿ��ǻس�����,
				* ������ʱ���������
				*/
				m_encodeBuf[0] = *cp;
				m_encodeCnt = 1;
				break;
			}
			if (*(cp + 1) == '\r' || *(cp + 1) == '\n') {
				QP_ENCODE(out, *cp);
				n += 3;
			} else {
				out->push_back((char) (*cp));
				n++;
			}
		} else if (*cp == '\r' || *cp == '\n') {
			out->push_back((char) (*cp));
			n = 0;
		} else if (*cp < 32 || *cp == '=' || *cp > 126) {
			QP_ENCODE(out, *cp);
			n += 3;
		} else {
			out->push_back((char) (*cp));
			n++;
		}

		m_encodeCnt--;

		/* Critical length before hard line break. */
		if (n >= 72 && m_addCrLf) {
			QP_SOFT_LINE(out);
			n = 0;
		}
	}
}

#if 0	
static int Index_hex[128] = {
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
	-1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
	-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
}; 

#define hexval(c) Index_hex[(unsigned char)(c)]
#define INVALID	-1
#endif

void mime_quoted_printable::decode_update(const char *src,
	int n, acl::string* out)
{
	int  i = 0;

	while (n > 0) {
		if (m_decodeCnt == (int) sizeof(m_decodeBuf)) {
			decode(out);
		}
		i = n;
		if (i > (int) sizeof(m_decodeBuf) - m_decodeCnt)
			i = (int) sizeof(m_decodeBuf) - m_decodeCnt;
		memcpy(m_decodeBuf + m_decodeCnt, src, i);
		src += i;
		n -= i;
		m_decodeCnt += i;
	}
}

void mime_quoted_printable::decode_finish(acl::string* out)
{
	decode(out);
	if (m_addInvalid)
	{
		if (m_decodeCnt == 1)
			out->push_back(m_decodeBuf[0]);
		else if (m_decodeCnt == 2) {
			out->push_back(m_decodeBuf[0]);
			out->push_back(m_decodeBuf[1]);
		}
	}
	m_decodeCnt = 0;
}

bool mime_quoted_printable::hex_decode(unsigned char first,
	unsigned char second, unsigned int *result)
{
	unsigned int hex;
	unsigned int bin;

	hex = first;
	if (hex >= '0' && hex <= '9')
		bin = (hex - '0') << 4;
	else if (hex >= 'A' && hex <= 'F')
		bin = (hex - 'A' + 10) << 4;
	else if (hex >= 'a' && hex <= 'f')
		bin = (hex - 'a' + 10) << 4;
	else
		return (false);
	hex = second;
	if (hex >= '0' && hex <= '9')
		bin |= (hex - '0') ;
	else if (hex >= 'A' && hex <= 'F')
		bin |= (hex - 'A' + 10) ;
	else if (hex >= 'a' && hex <= 'f')
		bin |= (hex - 'a' + 10) ;
	else
		return (false);
	*result = bin;
	return (true);
}

void mime_quoted_printable::decode(acl::string* out)
{
	const unsigned char *cp;
	const unsigned char *end = CU_CHAR_PTR(m_decodeBuf + m_decodeCnt);
	unsigned char first, second;
	unsigned int  result;

//#define IS_CRLF(x) ((x) == '\r' || (x) == '\n')

	for (cp = CU_CHAR_PTR(m_decodeBuf); cp < end;) {
		if (*cp != '=') {
			out->push_back((char) (*cp));
			m_decodeCnt--;
			cp++;
			continue;
		}

		/* '=' ������Ҫ�������ֽڲſɲ��� */
		if (m_decodeCnt == 1) {
			m_decodeBuf[0] = *cp;
			m_decodeCnt = 1;
			break;
		} else if (m_decodeCnt == 2) {
			m_decodeBuf[0] = *cp;
			m_decodeBuf[1] = *(cp + 1);
			m_decodeCnt = 2;
			break;
		}

		if (*(cp + 1) == '\r') {
			if (*(cp + 2) == '\n') {
				/* ȥ����س����� */
				cp += 3;
				m_decodeCnt -= 3;
			} else {
				if (m_addInvalid) {
					/* xxx: ���Ƿ�����ԭ���հ� */
					out->push_back((char) (*(cp + 1)));
					out->push_back((char) (*(cp + 2)));
				}
				cp += 3;
				m_decodeCnt -= 3;
			}
		} else if (*(cp + 1) == '\n') {
			/* xxx: unix ��ʽ������ ? */
			cp += 2;
			m_decodeCnt -= 2;
		} else {
			first = *(cp + 1);
			second = *(cp + 2);
			m_decodeCnt -= 3;
			cp += 3;
			if (hex_decode(first, second, &result)) {
				out->push_back((char) result);
			} else {
				if (m_addInvalid) {
					/* ���Ƿ�����ԭ���հ� */
					out->push_back((char) first);
					out->push_back((char) second);
				}
			}
		}
	}
}

} // namespace acl
