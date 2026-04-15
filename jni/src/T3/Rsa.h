#pragma once
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <algorithm>

// ==================== Base64 编解码实现 ====================
static const char base64_chars[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

char* base64_encode_standalone(const unsigned char* bytes_to_encode, unsigned int in_len) {
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    
    std::string ret;
    
    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
            ret += '=';
    }

    char* result = (char*)malloc(ret.length() + 1);
    strcpy(result, ret.c_str());
    return result;
}

unsigned char* base64_decode_standalone(const char* encoded_string, int* out_len) {
    int in_len = strlen(encoded_string);
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::vector<unsigned char> ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = strchr(base64_chars, char_array_4[i]) - base64_chars;

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret.push_back(char_array_3[i]);
            i = 0;
        }
    }

    if (i) {
        for (j = 0; j < i; j++)
            char_array_4[j] = strchr(base64_chars, char_array_4[j]) - base64_chars;

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

        for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
    }

    *out_len = ret.size();
    unsigned char* result = (unsigned char*)malloc(*out_len);
    memcpy(result, ret.data(), *out_len);
    return result;
}

// ==================== 大数运算类 ====================
class BigInteger {
private:
    std::vector<unsigned int> digits; // 以32位为单位存储，小端序
    
public:
    BigInteger() : digits(1, 0) {}
    
    BigInteger(unsigned long long val) {
        if (val == 0) {
            digits.push_back(0);
        } else {
            while (val > 0) {
                digits.push_back(val & 0xFFFFFFFF);
                val >>= 32;
            }
        }
    }
    
    // 从字节数组创建大数（大端序）
    BigInteger(const unsigned char* bytes, int len) {
        if (len == 0) {
            digits.push_back(0);
            return;
        }
        
        digits.clear();
        int num_digits = (len + 3) / 4;
        digits.resize(num_digits, 0);
        
        for (int i = 0; i < len; i++) {
            int digit_idx = (len - 1 - i) / 4;
            int byte_idx = (len - 1 - i) % 4;
            digits[digit_idx] |= ((unsigned int)bytes[i]) << (byte_idx * 8);
        }
        
        trim();
    }
    
    void trim() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
    }
    
    bool isZero() const {
        return digits.size() == 1 && digits[0] == 0;
    }
    
    // 比较运算
    int compare(const BigInteger& other) const {
        if (digits.size() != other.digits.size()) {
            return digits.size() > other.digits.size() ? 1 : -1;
        }
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] != other.digits[i]) {
                return digits[i] > other.digits[i] ? 1 : -1;
            }
        }
        return 0;
    }
    
    bool operator<(const BigInteger& other) const { return compare(other) < 0; }
    bool operator>(const BigInteger& other) const { return compare(other) > 0; }
    bool operator==(const BigInteger& other) const { return compare(other) == 0; }
    bool operator>=(const BigInteger& other) const { return compare(other) >= 0; }
    
    // 加法
    BigInteger operator+(const BigInteger& other) const {
        BigInteger result;
        result.digits.clear();
        
        size_t max_size = std::max(digits.size(), other.digits.size());
        unsigned long long carry = 0;
        
        for (size_t i = 0; i < max_size || carry; i++) {
            unsigned long long sum = carry;
            if (i < digits.size()) sum += digits[i];
            if (i < other.digits.size()) sum += other.digits[i];
            
            result.digits.push_back(sum & 0xFFFFFFFF);
            carry = sum >> 32;
        }
        
        result.trim();
        return result;
    }
    
    // 减法（假设 this >= other）
    BigInteger operator-(const BigInteger& other) const {
        BigInteger result;
        result.digits.clear();
        result.digits.resize(digits.size(), 0);
        
        long long borrow = 0;
        for (size_t i = 0; i < digits.size(); i++) {
            long long diff = (long long)digits[i] - borrow;
            if (i < other.digits.size()) {
                diff -= other.digits[i];
            }
            
            if (diff < 0) {
                diff += 0x100000000LL;
                borrow = 1;
            } else {
                borrow = 0;
            }
            
            result.digits[i] = diff;
        }
        
        result.trim();
        return result;
    }
    
    // 左移一位
    BigInteger shiftLeft(int bits) const {
        if (isZero()) return *this;
        
        BigInteger result;
        int digit_shift = bits / 32;
        int bit_shift = bits % 32;
        
        result.digits.clear();
        result.digits.resize(digits.size() + digit_shift + 1, 0);
        
        if (bit_shift == 0) {
            for (size_t i = 0; i < digits.size(); i++) {
                result.digits[i + digit_shift] = digits[i];
            }
        } else {
            unsigned int carry = 0;
            for (size_t i = 0; i < digits.size(); i++) {
                unsigned long long val = ((unsigned long long)digits[i] << bit_shift) | carry;
                result.digits[i + digit_shift] = val & 0xFFFFFFFF;
                carry = val >> 32;
            }
            if (carry) {
                result.digits[digits.size() + digit_shift] = carry;
            }
        }
        
        result.trim();
        return result;
    }
    
    // 乘法
    BigInteger operator*(const BigInteger& other) const {
        BigInteger result;
        result.digits.clear();
        result.digits.resize(digits.size() + other.digits.size(), 0);
        
        for (size_t i = 0; i < digits.size(); i++) {
            unsigned long long carry = 0;
            for (size_t j = 0; j < other.digits.size() || carry; j++) {
                unsigned long long current = result.digits[i + j] + carry;
                if (j < other.digits.size()) {
                    current += (unsigned long long)digits[i] * other.digits[j];
                }
                result.digits[i + j] = current & 0xFFFFFFFF;
                carry = current >> 32;
            }
        }
        
        result.trim();
        return result;
    }
    
    // 除法和取模
    void divMod(const BigInteger& divisor, BigInteger& quotient, BigInteger& remainder) const {
        quotient = BigInteger(0);
        remainder = *this;
        
        if (divisor.isZero() || *this < divisor) {
            return;
        }
        
        // 找到最高位
        int shift = 0;
        BigInteger temp = divisor;
        while (temp < remainder) {
            temp = temp.shiftLeft(1);
            shift++;
        }
        
        if (temp > remainder) {
            temp = temp.shiftRight(1);
            shift--;
        }
        
        for (int i = shift; i >= 0; i--) {
            BigInteger shifted = divisor.shiftLeft(i);
            if (remainder >= shifted) {
                remainder = remainder - shifted;
                quotient = quotient + BigInteger(1).shiftLeft(i);
            }
        }
    }
    
    BigInteger operator%(const BigInteger& divisor) const {
        BigInteger quotient, remainder;
        divMod(divisor, quotient, remainder);
        return remainder;
    }
    
    // 右移
    BigInteger shiftRight(int bits) const {
        if (isZero() || bits == 0) return *this;
        
        int digit_shift = bits / 32;
        int bit_shift = bits % 32;
        
        if (digit_shift >= (int)digits.size()) {
            return BigInteger(0);
        }
        
        BigInteger result;
        result.digits.clear();
        result.digits.resize(digits.size() - digit_shift, 0);
        
        if (bit_shift == 0) {
            for (size_t i = 0; i < result.digits.size(); i++) {
                result.digits[i] = digits[i + digit_shift];
            }
        } else {
            for (size_t i = 0; i < result.digits.size(); i++) {
                result.digits[i] = digits[i + digit_shift] >> bit_shift;
                if (i + digit_shift + 1 < digits.size()) {
                    result.digits[i] |= digits[i + digit_shift + 1] << (32 - bit_shift);
                }
            }
        }
        
        result.trim();
        return result;
    }
    
    // 模幂运算：计算 (base^exp) % mod
    static BigInteger modPow(const BigInteger& base, const BigInteger& exp, const BigInteger& mod) {
        BigInteger result(1);
        BigInteger b = base % mod;
        BigInteger e = exp;
        
        while (!e.isZero()) {
            if (e.digits[0] & 1) {
                result = (result * b) % mod;
            }
            b = (b * b) % mod;
            e = e.shiftRight(1);
        }
        
        return result;
    }
    
    // 转换为字节数组（大端序）
    void toBytes(unsigned char* bytes, int len) const {
        memset(bytes, 0, len);
        
        for (size_t i = 0; i < digits.size() && i * 4 < (size_t)len; i++) {
            for (int j = 0; j < 4 && i * 4 + j < (size_t)len; j++) {
                bytes[len - 1 - (i * 4 + j)] = (digits[i] >> (j * 8)) & 0xFF;
            }
        }
    }
    
    int byteLength() const {
        if (isZero()) return 1;
        
        int len = (digits.size() - 1) * 4;
        unsigned int last = digits.back();
        
        while (last > 0) {
            len++;
            last >>= 8;
        }
        
        return len;
    }
};

// ==================== PEM公钥解析 ====================
struct RSAPublicKey {
    BigInteger n;  // 模数
    BigInteger e;  // 公钥指数
    int key_size;  // 密钥大小（字节）
};

// 解析ASN.1 DER编码的长度字段
int parseDERLength(const unsigned char* data, int* offset) {
    unsigned char first = data[*offset];
    (*offset)++;
    
    if (first < 0x80) {
        return first;
    }
    
    int num_bytes = first & 0x7F;
    int length = 0;
    
    for (int i = 0; i < num_bytes; i++) {
        length = (length << 8) | data[*offset];
        (*offset)++;
    }
    
    return length;
}

bool parseRSAPublicKey(const char* pem_key, RSAPublicKey& key) {
    // 提取Base64编码的部分
    std::string pem_str(pem_key);
    size_t start = pem_str.find("-----BEGIN PUBLIC KEY-----");
    size_t end = pem_str.find("-----END PUBLIC KEY-----");
    
    if (start == std::string::npos || end == std::string::npos) {
        return false;
    }
    
    start += strlen("-----BEGIN PUBLIC KEY-----");
    std::string base64_data;
    
    for (size_t i = start; i < end; i++) {
        char c = pem_str[i];
        if (c != '\n' && c != '\r' && c != ' ' && c != '\t') {
            base64_data += c;
        }
    }
    
    // Base64解码
    int decoded_len;
    unsigned char* decoded = base64_decode_standalone(base64_data.c_str(), &decoded_len);
    
    // 解析DER编码的公钥
    // 格式: SEQUENCE { SEQUENCE { OID, NULL }, BIT STRING { SEQUENCE { INTEGER n, INTEGER e } } }
    
    int offset = 0;
    
    // 跳过最外层SEQUENCE
    if (decoded[offset] != 0x30) { free(decoded); return false; }
    offset++;
    (void)parseDERLength(decoded, &offset); // outer_len未使用
    
    // 跳过算法标识SEQUENCE
    if (decoded[offset] != 0x30) { free(decoded); return false; }
    offset++;
    int algo_len = parseDERLength(decoded, &offset);
    offset += algo_len;
    
    // 读取BIT STRING
    if (decoded[offset] != 0x03) { free(decoded); return false; }
    offset++;
    (void)parseDERLength(decoded, &offset); // bitstring_len未使用
    offset++; // 跳过unused bits字段
    
    // 读取公钥SEQUENCE
    if (decoded[offset] != 0x30) { free(decoded); return false; }
    offset++;
    (void)parseDERLength(decoded, &offset); // pubkey_len未使用
    
    // 读取模数n（INTEGER）
    if (decoded[offset] != 0x02) { free(decoded); return false; }
    offset++;
    int n_len = parseDERLength(decoded, &offset);
    
    // 跳过可能的前导零
    if (decoded[offset] == 0x00) {
        offset++;
        n_len--;
    }
    
    key.n = BigInteger(decoded + offset, n_len);
    key.key_size = n_len;
    offset += n_len;
    
    // 读取公钥指数e（INTEGER）
    if (decoded[offset] != 0x02) { free(decoded); return false; }
    offset++;
    int e_len = parseDERLength(decoded, &offset);
    
    key.e = BigInteger(decoded + offset, e_len);
    
    free(decoded);
    return true;
}

// ==================== PKCS#1填充 ====================
bool pkcs1_padding(const unsigned char* data, int data_len, unsigned char* padded, int padded_len) {
    if (data_len > padded_len - 11) {
        return false;
    }
    
    padded[0] = 0x00;
    padded[1] = 0x02;
    
    // 随机填充
    int padding_len = padded_len - data_len - 3;
    for (int i = 0; i < padding_len; i++) {
        unsigned char val;
        do {
            val = rand() % 256;
        } while (val == 0);
        padded[2 + i] = val;
    }
    
    padded[2 + padding_len] = 0x00;
    memcpy(padded + 2 + padding_len + 1, data, data_len);
    
    return true;
}

bool pkcs1_unpadding(const unsigned char* padded, int padded_len, unsigned char* data, int* data_len) {
    if (padded_len < 11) {
        return false;
    }
    
    if (padded[0] != 0x00) {
        return false;
    }
    
    // 公钥解密时padding应该是0x02，私钥解密时是0x01
    if (padded[1] != 0x02 && padded[1] != 0x01) {
        return false;
    }
    
    int i = 2;
    while (i < padded_len && padded[i] != 0x00) {
        i++;
    }
    
    if (i >= padded_len) {
        return false;
    }
    
    i++; // 跳过0x00
    *data_len = padded_len - i;
    memcpy(data, padded + i, *data_len);
    
    return true;
}

// ==================== RSA加密解密 ====================
char* public_encrypt(const char* message, const char* public_key_pem) {
    RSAPublicKey key;
    if (!parseRSAPublicKey(public_key_pem, key)) {
        fprintf(stderr, "无法解析RSA公钥\n");
        return NULL;
    }
    
    int message_len = strlen(message);
    int block_size = key.key_size - 11;
    int num_blocks = message_len / block_size + 1;
    
    unsigned char* cipher_text = (unsigned char*)malloc(key.key_size * num_blocks);
    int encrypted_len = 0;
    
    for (int i = 0; i < num_blocks; i++) {
        int current_block_len = (i == num_blocks - 1) ? message_len % block_size : block_size;
        
        // PKCS#1填充
        unsigned char padded[512];
        if (!pkcs1_padding((unsigned char*)(message + i * block_size), 
                          current_block_len, padded, key.key_size)) {
            fprintf(stderr, "填充失败\n");
            free(cipher_text);
            return NULL;
        }
        
        // RSA加密：c = m^e mod n
        BigInteger m(padded, key.key_size);
        BigInteger c = BigInteger::modPow(m, key.e, key.n);
        
        // 转换为字节
        c.toBytes(cipher_text + i * key.key_size, key.key_size);
        encrypted_len += key.key_size;
    }
    
    // Base64编码
    char* base64_encoded = base64_encode_standalone(cipher_text, encrypted_len);
    
    free(cipher_text);
    return base64_encoded;
}

char* public_decrypt(const char* base64_cipher, const char* public_key_pem) {
    RSAPublicKey key;
    if (!parseRSAPublicKey(public_key_pem, key)) {
        fprintf(stderr, "无法解析RSA公钥\n");
        return NULL;
    }
    
    // Base64解码
    int decoded_len;
    unsigned char* decoded_cipher = base64_decode_standalone(base64_cipher, &decoded_len);
    
    int num_blocks = decoded_len / key.key_size;
    std::vector<unsigned char> result;
    
    for (int i = 0; i < num_blocks; i++) {
        // RSA解密：m = c^e mod n (公钥解密也是用e)
        BigInteger c(decoded_cipher + i * key.key_size, key.key_size);
        BigInteger m = BigInteger::modPow(c, key.e, key.n);
        
        // 转换为字节
        unsigned char decrypted_block[512];
        m.toBytes(decrypted_block, key.key_size);
        
        // 去除PKCS#1填充
        unsigned char unpadded[512];
        int unpadded_len;
        if (pkcs1_unpadding(decrypted_block, key.key_size, unpadded, &unpadded_len)) {
            for (int j = 0; j < unpadded_len; j++) {
                result.push_back(unpadded[j]);
            }
        }
    }
    
    free(decoded_cipher);
    
    // 转换为字符串
    char* decrypted_result = (char*)malloc(result.size() + 1);
    memcpy(decrypted_result, result.data(), result.size());
    decrypted_result[result.size()] = '\0';
    
    return decrypted_result;
}

// ==================== 辅助函数 ====================
char* hex2str(const unsigned char* hex, int length) {
    char* str = (char*)malloc(length * 2 + 1);
    for (int i = 0; i < length; i++) {
        sprintf(str + i * 2, "%02x", hex[i]);
    }
    str[length * 2] = 0;
    return str;
}

unsigned char* str2hex(const char* str, int length) {
    unsigned char* hex = (unsigned char*)malloc(length / 2);
    for (int i = 0; i < length / 2; i++) {
        unsigned int temp;
        sscanf(str + i * 2, "%02x", &temp);
        hex[i] = (unsigned char)temp;
    }
    return hex;
}

// 为了兼容性，提供与原来OpenSSL版本相同的函数名
char* base64_encode(const unsigned char* input, int length) {
    return base64_encode_standalone(input, length);
}

unsigned char* base64_decode(const char* input, int length) {
    int out_len;
    return base64_decode_standalone(input, &out_len);
}

