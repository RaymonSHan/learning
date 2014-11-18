
#pragma once

#include	<openssl/des.h>
#include	<openssl/md4.h>

int Base64encode_len(int len);
int Base64encode(char * coded_dst, const char *plain_src,int len_plain_src);
int Base64decode_len(const char * coded_src);
int Base64decode(char * plain_dst, const char *coded_src);

typedef struct  type_1_message{
	long				protocol1;
	long				protocol2;			// 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
	unsigned char		type;				// 0x01
	unsigned char		zero1[3];
	unsigned short		flags;				// 0xb203
	unsigned char		zero2[2];

	short				dom_len;			// domain string length
	short				dom_len1;			// domain string length
	short				dom_off;			// domain string offset
	unsigned char		zero3[2];

	short				host_len;			// host string length
	short				host_len1;			// host string length
	short				host_off;			// host string offset (always 0x20)
	unsigned char		zero4[2];

// 	unsigned char		*host;				// host string (ASCII)
// 	unsigned char		*dom;				// domain string (ASCII)
} type_1_message;

typedef struct type_2_message{
	long				protocol1;
	long				protocol2;			// 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
	unsigned char		type;				// 0x02
	unsigned char		zero1[7];
	short				msg_len;			// 0x28
	unsigned char		zero2[2];
	unsigned short		flags;				// 0x8201
	unsigned char		zero3[2];
	unsigned char		nonce[8];			// nonce
	unsigned char		zero4[8];
} type_2_message;

typedef struct type_3_message{
	long				protocol1;
	long				protocol2;			// 'N', 'T', 'L', 'M', 'S', 'S', 'P', '\0'
	unsigned char		type;				// 0x03
	unsigned char		zero1[3];

	short				lm_resp_len;		// LanManager response length (always 0x18)
	short				lm_resp_len1;		// LanManager response length (always 0x18)
	short				lm_resp_off;		// LanManager response offset
	unsigned char		zero2[2];

	short				nt_resp_len;		// NT response length (always 0x18)
	short				nt_resp_len1;		// NT response length (always 0x18)
	short				nt_resp_off;		// NT response offset
	unsigned char		zero3[2];

	short				dom_len;			// domain string length
	short				dom_len1;			// domain string length
	short				dom_off;			// domain string offset (always 0x40)
	unsigned char		zero4[2];

	short				user_len;			// username string length
	short				user_len1;			// username string length
	short				user_off;			// username string offset
	unsigned char		zero5[2];

	short				host_len;			// host string length
	short				host_len1;			// host string length
	short				host_off;			// host string offset
	unsigned char		zero6[6];

	short   msg_len;						// message length
	unsigned char    zero7[2];

	unsigned short   flags;					// 0x8201
	unsigned char    zero8[2];

	unsigned char    *dom;					// domain string (unicode UTF-16LE)
	unsigned char    *user;					// username string (unicode UTF-16LE)
	unsigned char    *host;					// host string (unicode UTF-16LE)
	unsigned char    *lm_resp;				// LanManager response
	unsigned char    *nt_resp;				// NT response
} type_3_message;

void createpasshash(char* passw, unsigned char* nosence, char* lm_resp, char* nt_resp);

int GenType1(char* domain, char* host, char* mess);
int GenType2(char* nosence, char* mess);
int GenType3(char* domain, char* user, char* host, char* pass, char* nosence, char* mess);