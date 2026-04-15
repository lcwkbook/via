#ifndef _ENCRYPT_RC4_OR_MD5
#define _ENCRYPT_RC4_OR_MD5

#include <string.h>
#define BOX_LEN 256
 
#ifdef __cplusplus
extern "C" {
#endif

int GetKey(const unsigned char* pass, int pass_len, unsigned char *out);
int MyRC4(const unsigned char* data, int data_len, const unsigned char* key, int key_len, unsigned char* out, int* out_len);
static void swap_byte(unsigned char* a, unsigned char* b);
 
char* Encrypt(const char* szSource, const char* szPassWord); // 加密，返回加密结果
char* Decrypt(const char* szSource, const char* szPassWord); // 解密，返回解密结果
 
char* ByteToHex(const unsigned char* vByte, const int vLen); // 把字节码pbBuffer转为十六进制字符串，方便传输
unsigned char* HexToByte(const char* szHex); // 把十六进制字符串转为字节码pbBuffer，解码
 


// md5
typedef struct {
	unsigned int count[2];
	unsigned int state[4];
	unsigned char buffer[64];
} MD5_CTX;


void MD5Init(MD5_CTX * context);
void MD5Update(MD5_CTX * context, unsigned char *input, unsigned int inputlen);
void MD5Final(MD5_CTX * context, unsigned char digest[16]);
void MD5Transform(unsigned int state[4], unsigned char block[64]);
void MD5Encode(unsigned char *output, unsigned int *input, unsigned int len);
void MD5Decode(unsigned int *output, unsigned char *input, unsigned int len);
	
#ifdef __cplusplus
}
#endif
// md5

// md5代码区




#endif // _ENCRYPT_RC4_OR_MD5
