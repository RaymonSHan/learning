
#include "stdafx.h"

#ifdef	NTLM_FUNCTION

/* aaaack but it's fast and const should make it shared text page. */
static const unsigned char pr2six[256] =
{
	/* ASCII table */
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

int Base64decode_len(const char *bufcoded)
{
	int nbytesdecoded;
	register const unsigned char *bufin;
	register int nprbytes;

	bufin = (const unsigned char *) bufcoded;
	while (pr2six[*(bufin++)] <= 63);

	nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
	nbytesdecoded = ((nprbytes + 3) / 4) * 3;

	return nbytesdecoded + 1;
}

int Base64decode(char *bufplain, const char *bufcoded)
{
	int nbytesdecoded;
	register const unsigned char *bufin;
	register unsigned char *bufout;
	register int nprbytes;

	bufin = (const unsigned char *) bufcoded;
	while (pr2six[*(bufin++)] <= 63);
	nprbytes = (bufin - (const unsigned char *) bufcoded) - 1;
	nbytesdecoded = ((nprbytes + 3) / 4) * 3;

	bufout = (unsigned char *) bufplain;
	bufin = (const unsigned char *) bufcoded;

	while (nprbytes > 4) {
		*(bufout++) =
			(unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
		*(bufout++) =
			(unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
		*(bufout++) =
			(unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
		bufin += 4;
		nprbytes -= 4;
	}

	/* Note: (nprbytes == 1) would be an error, so just ingore that case */
	if (nprbytes > 1) {
		*(bufout++) =
			(unsigned char) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
	}
	if (nprbytes > 2) {
		*(bufout++) =
			(unsigned char) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
	}
	if (nprbytes > 3) {
		*(bufout++) =
			(unsigned char) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
	}

	*(bufout++) = '\0';
	nbytesdecoded -= (4 - nprbytes) & 3;
	return nbytesdecoded;
}

static const char basis_64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64encode_len(int len)
{
	return ((len + 2) / 3 * 4) + 1;
}

int Base64encode(char *encoded, const char *string, int len)
{
	int i;
	char *p;

	p = encoded;
	for (i = 0; i < len - 2; i += 3) {
		*p++ = basis_64[(string[i] >> 2) & 0x3F];
		*p++ = basis_64[((string[i] & 0x3) << 4) |
			((int) (string[i + 1] & 0xF0) >> 4)];
		*p++ = basis_64[((string[i + 1] & 0xF) << 2) |
			((int) (string[i + 2] & 0xC0) >> 6)];
		*p++ = basis_64[string[i + 2] & 0x3F];
	}
	if (i < len) {
		*p++ = basis_64[(string[i] >> 2) & 0x3F];
		if (i == (len - 1)) {
			*p++ = basis_64[((string[i] & 0x3) << 4)];
			*p++ = '=';
		}
		else {
			*p++ = basis_64[((string[i] & 0x3) << 4) |
				((int) (string[i + 1] & 0xF0) >> 4)];
			*p++ = basis_64[((string[i + 1] & 0xF) << 2)];
		}
		*p++ = '=';
	}

	*p++ = '\0';
	return p - encoded;
}

/*
 * turns a 56 bit key into the 64 bit, odd parity key and sets the key.
 * The key schedule ks is also set.
 */
void setup_des_key(unsigned char key_56[14], des_key_schedule* ks)
{
    des_cblock key;

    key[0] = key_56[0];
    key[1] = ((key_56[0] << 7) & 0xFF) | (key_56[1] >> 1);
    key[2] = ((key_56[1] << 6) & 0xFF) | (key_56[2] >> 2);
    key[3] = ((key_56[2] << 5) & 0xFF) | (key_56[3] >> 3);
    key[4] = ((key_56[3] << 4) & 0xFF) | (key_56[4] >> 4);
    key[5] = ((key_56[4] << 3) & 0xFF) | (key_56[5] >> 5);
    key[6] = ((key_56[5] << 2) & 0xFF) | (key_56[6] >> 6);
    key[7] =  (key_56[6] << 1) & 0xFF;

    des_set_odd_parity(&key);
    DES_set_key(&key, ks);
}
/*
 * takes a 21 byte array and treats it as 3 56-bit DES keys. The
 * 8 byte plaintext is encrypted with each key and the resulting 24
 * bytes are stored in the results array.
 */
void calc_resp(unsigned char *keys, unsigned char *plaintext, char *results)
{
    des_key_schedule ks;

    setup_des_key(keys, &ks);
    des_ecb_encrypt((des_cblock*) plaintext, (des_cblock*) results, ks, DES_ENCRYPT);

    setup_des_key(keys+7, &ks);
    des_ecb_encrypt((des_cblock*) plaintext, (des_cblock*) (results+8), ks, DES_ENCRYPT);

    setup_des_key(keys+14, &ks);
    des_ecb_encrypt((des_cblock*) plaintext, (des_cblock*) (results+16), ks, DES_ENCRYPT);
}


void createpasshash(char* passw, unsigned char* nosence, char* lm_resp, char* nt_resp)
{
	/* setup LanManager password */
	int idx;
	unsigned char  lm_pw[14];
	int   len = strlen(passw);
	if (len > 14)  len = 14;

	for (idx=0; idx<len; idx++)	lm_pw[idx] = toupper(passw[idx]);
	for (; idx<14; idx++) lm_pw[idx] = 0;

	/* create LanManager hashed password */
	unsigned char magic[] = { 0x4B, 0x47, 0x53, 0x21, 0x40, 0x23, 0x24, 0x25 };
	unsigned char lm_hpw[21];
	des_key_schedule ks;

	setup_des_key(lm_pw, &ks);
	des_ecb_encrypt((des_cblock*)&magic[0], (des_cblock*)&lm_hpw[0], ks, DES_ENCRYPT);

	setup_des_key(lm_pw+7, &ks);
	des_ecb_encrypt((des_cblock*)&magic[0], (des_cblock*)&lm_hpw[8], ks, DES_ENCRYPT);

	memset(lm_hpw+16, 0, 5);

	/* create NT hashed password */
	len = strlen(passw);
	char  nt_pw[100];
	for (idx=0; idx<len; idx++)
	{
		nt_pw[2*idx]   = passw[idx];
		nt_pw[2*idx+1] = 0;
	}

	unsigned char nt_hpw[21];
	MD4_CTX context;
	MD4_Init(&context);
	MD4_Update(&context, nt_pw, 2*len);
	MD4_Final(nt_hpw, &context);
	memset(nt_hpw+16, 0, 5);

	/* create responses */
	//	YOU DIG SO DEEP
	calc_resp(lm_hpw, nosence, lm_resp);
	calc_resp(nt_hpw, nosence, nt_resp);
};

int GenType1(char* domain, char* host, char* mess)
{
	type_1_message* type1 = (type_1_message*)mess;
	memset(mess, 0, sizeof(type_1_message));
	type1->protocol1 = 'N'+('T'<<8)+('L'<<16)+('M'<<24);
	type1->protocol2 = 'S'+('S'<<8)+('P'<<16);
	type1->type = 0x01;
	type1->flags = 0xb203;
	type1->dom_len = type1->dom_len1 = strlen(domain);
	type1->host_len = type1->host_len1 = strlen(host);
	type1->dom_off = 0x20+type1->host_len;
	type1->host_off = 0x20;
	memcpy_up(mess+type1->host_off, host, type1->host_len);
	memcpy_up(mess+type1->dom_off, domain, type1->dom_len);
	return type1->dom_off+type1->dom_len;
}

int GenType2(char* nosence, char* mess)
{
	type_2_message* type2 = (type_2_message*)mess;
	memset(mess, 0, sizeof(type_2_message));
	type2->protocol1 = 'N'+('T'<<8)+('L'<<16)+('M'<<24);
	type2->protocol2 = 'S'+('S'<<8)+('P'<<16);
	type2->type = 0x02;
	type2->msg_len = 0x28;
	type2->flags = 0x8201;
	memcpy(type2->nonce, nosence, 8);
	return 0x28;
}

int GenType3(char* domain, char* user, char* host, char* pass, char* nosence, char* mess)
{
	type_3_message* type3 = (type_3_message*)mess;
	memset(mess, 0, sizeof(type_3_message));
	type3->protocol1 = 'N'+('T'<<8)+('L'<<16)+('M'<<24);
	type3->protocol2 = 'S'+('S'<<8)+('P'<<16);
	type3->type = 0x03;
	type3->lm_resp_len = type3->lm_resp_len1 = 0x18;
	type3->nt_resp_len = type3->nt_resp_len1 = 0x18;
	type3->dom_len = type3->dom_len1 = strlen(domain)*2;
	type3->user_len = type3->user_len1 = strlen(user)*2;
	type3->host_len = type3->host_len1 = strlen(host)*2;

	type3->dom_off = 0x40;
	type3->user_off = 0x40+type3->dom_len;
	type3->host_off = type3->user_off+type3->user_len;
	type3->lm_resp_off = type3->host_off+type3->host_len;
	type3->nt_resp_off = type3->lm_resp_off+type3->lm_resp_len;
	type3->msg_len = type3->nt_resp_off+type3->nt_resp_len;
	type3->flags = 0x8201;
	memcpy_u_up(mess+type3->dom_off, domain, type3->dom_len);
	memcpy_u(mess+type3->user_off, user, type3->user_len);
	memcpy_u_up(mess+type3->host_off, host, type3->host_len);

	createpasshash(pass, (unsigned char*)nosence, mess+type3->lm_resp_off, mess+type3->nt_resp_off);
	return type3->msg_len;
}

#endif	NTLM_FUNCTION