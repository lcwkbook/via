#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <iomanip>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/asn1.h>
#include <openssl/x509.h>
#include <openssl/aes.h>

using namespace std;


// RC4 密钥调度算法 (KSA)
void rc4_ksa(const std::string& key, std::vector<unsigned char>& S) {
    int key_length = key.size();
    for (int i = 0; i < 256; ++i) {
        S[i] = i;
    }

    int j = 0;
    for (int i = 0; i < 256; ++i) {
        j = (j + S[i] + key[i % key_length]) % 256;
        std::swap(S[i], S[j]);
    }
}

// RC4 伪随机生成算法 (PRGA)
void rc4_prga(std::vector<unsigned char>& S, const std::string& data, std::string& output) {
    int i = 0, j = 0;
    for (size_t n = 0; n < data.size(); ++n) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        std::swap(S[i], S[j]);
        unsigned char K = S[(S[i] + S[j]) % 256];
        output.push_back(data[n] ^ K);
    }
}

// 将二进制数据转换为十六进制字符串
std::string to_hex_string(const std::string& data) {
    std::ostringstream oss;
    for (unsigned char c : data) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return oss.str();
}

// 将十六进制字符串转换为二进制数据
std::string from_hex_string(const std::string& hex) {
    std::string output;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = static_cast<char>(strtol(byteString.c_str(), nullptr, 16));
        output.push_back(byte);
    }
    return output;
}

// RC4 加密/解密
std::string rc4(const std::string& data, const std::string& key) {
    std::vector<unsigned char> S(256);
    rc4_ksa(key, S);

    std::string output;
    rc4_prga(S, data, output);

    return output;
} 


// Base64解码（兼容URL安全字符）
vector<unsigned char> base64_decode(const string &encoded)
{
    BIO *bio, *b64;
    vector<unsigned char> decoded(encoded.size());

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new_mem_buf(encoded.data(), encoded.length());
    bio = BIO_push(b64, bio);

    int decoded_len = BIO_read(bio, decoded.data(), encoded.size());
    decoded.resize(decoded_len);

    BIO_free_all(bio);
    return decoded;
}

// 从字符串加载公钥
RSA *load_public_key(const char *pub_key)
{
    BIO *bio = BIO_new_mem_buf(pub_key, -1);
    RSA *rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
    BIO_free(bio);

    if (!rsa)
    {
        cerr << "ERROR: 公钥格式错误！请确认包含-----BEGIN PUBLIC KEY-----标记" << endl;
    }
    return rsa;
}

// 分块解密核心逻辑
string rsa_decrypt(RSA *rsa, const string &ciphertext_base64)
{
    // Base64解码
    vector<unsigned char> ciphertext = base64_decode(ciphertext_base64);
    const int key_size = RSA_size(rsa); // 获取密钥字节长度（如256）

    if (ciphertext.size() % key_size != 0)
    {
        throw runtime_error("密文长度不匹配，应为" + to_string(key_size) + "字节的整数倍");
    }

    string plaintext;
    vector<unsigned char> buf(key_size);

    // 分块解密
    for (size_t i = 0; i < ciphertext.size(); i += key_size)
    {
        int decrypt_len = RSA_public_decrypt(
            key_size,
            &ciphertext[i],
            buf.data(),
            rsa,
            RSA_PKCS1_PADDING);

        if (decrypt_len == -1)
        {
            throw runtime_error("解密失败，错误代码: " + to_string(ERR_get_error()));
        }

        plaintext.append(reinterpret_cast<char *>(buf.data()), decrypt_len);
    }

    return plaintext;
}

// Base64编码（URL安全）
string base64_encode(const vector<unsigned char> &data)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, data.data(), data.size());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    string encoded(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    // 移除末尾换行符
    if (!encoded.empty() && encoded.back() == '\n')
    {
        encoded.pop_back();
    }
    return encoded;
}

// 分块加密核心逻辑
string rsa_encrypt(RSA *rsa, const string &plaintext)
{
    const int key_size = RSA_size(rsa);
    const int max_block_size = key_size - 11; // PKCS#1 v1.5填充最大块

    if (max_block_size <= 0)
    {
        throw runtime_error("无效的密钥长度");
    }

    vector<unsigned char> ciphertext;
    vector<unsigned char> buf(key_size);

    // 分块加密
    for (size_t i = 0; i < plaintext.size(); i += max_block_size)
    {
        int in_len = min(max_block_size, (int)(plaintext.size() - i));

        int encrypt_len = RSA_public_encrypt(
            in_len,
            reinterpret_cast<const unsigned char *>(plaintext.data() + i),
            buf.data(),
            rsa,
            RSA_PKCS1_PADDING);

        if (encrypt_len == -1)
        {
            throw runtime_error("加密失败，错误代码: " + to_string(ERR_get_error()));
        }

        ciphertext.insert(ciphertext.end(), buf.begin(), buf.begin() + encrypt_len);
    }

    return base64_encode(ciphertext);
}

// ==================== DH加密相关函数 ====================

// 从十六进制字符串创建BIGNUM
BIGNUM* hex_to_bn(const string& hex_str) {
    BIGNUM* bn = BN_new();
    if (!BN_hex2bn(&bn, hex_str.c_str())) {
        BN_free(bn);
        return nullptr;
    }
    return bn;
}

// 将BIGNUM转换为十六进制字符串
string bn_to_hex(const BIGNUM* bn) {
    if (!bn) return "";
    char* hex = BN_bn2hex(bn);
    if (!hex) return "";
    string result(hex);
    OPENSSL_free(hex);
    return result;
}

// 从十六进制字符串加载服务端DH公钥
// Java生成的公钥是SubjectPublicKeyInfo格式（DER编码）
// 使用d2i_PUBKEY加载，然后从中提取DH对象
DH* load_server_dh_public_key(const string& hex_public_key) {
    if (hex_public_key.empty()) {
        cerr << "ERROR: 服务端DH公钥为空" << endl;
        return nullptr;
    }
    
    // 将十六进制字符串转换为二进制
    string der_data = from_hex_string(hex_public_key);
    if (der_data.empty()) {
        cerr << "ERROR: 无法将十六进制字符串转换为二进制数据" << endl;
        return nullptr;
    }
    
    // 使用d2i_PUBKEY加载SubjectPublicKeyInfo格式的公钥
    const unsigned char* p = reinterpret_cast<const unsigned char*>(der_data.data());
    long len = der_data.size();
    EVP_PKEY* pkey = d2i_PUBKEY(nullptr, &p, len);
    
    if (!pkey) {
        unsigned long err = ERR_get_error();
        char err_buf[256];
        ERR_error_string_n(err, err_buf, sizeof(err_buf));
        cerr << "ERROR: 无法加载公钥，错误: " << err_buf << endl;
        return nullptr;
    }
    
    // 从EVP_PKEY中获取DH对象
    // 如果类型不是DH，EVP_PKEY_get1_DH会返回NULL
    // 注意：这里获取的DH对象可能不包含p和g参数，只有公钥
    DH* dh = EVP_PKEY_get1_DH(pkey);
    EVP_PKEY_free(pkey);
    
    if (!dh) {
        unsigned long err = ERR_get_error();
        char err_buf[256];
        ERR_error_string_n(err, err_buf, sizeof(err_buf));
        cerr << "ERROR: 无法从EVP_PKEY中提取DH对象，错误: " << err_buf << endl;
        cerr << "ERROR: 公钥类型可能不是DH" << endl;
        return nullptr;
    }
    
    // 检查DH对象是否包含p和g参数
    const BIGNUM* p_bn = nullptr;
    const BIGNUM* g_bn = nullptr;
    DH_get0_pqg(dh, &p_bn, nullptr, &g_bn);
    
    // 如果缺少p或g参数，使用标准的2048位DH参数组
    if (!p_bn || !g_bn) {
        cerr << "WARNING: DH对象缺少p或g参数，使用标准2048位DH参数组" << endl;
        
        // 使用RFC 5114定义的2048位DH参数组
        // 或者使用OpenSSL提供的标准参数组
        DH* standard_dh = DH_get_2048_256();
        if (!standard_dh) {
            DH_free(dh);
            cerr << "ERROR: 无法获取标准DH参数组" << endl;
            return nullptr;
        }
        
        // 获取标准参数组的p和g
        const BIGNUM* std_p = nullptr;
        const BIGNUM* std_g = nullptr;
        DH_get0_pqg(standard_dh, &std_p, nullptr, &std_g);
        
        // 获取服务端公钥
        const BIGNUM* server_pub_key = DH_get0_pub_key(dh);
        if (!server_pub_key) {
            DH_free(dh);
            DH_free(standard_dh);
            cerr << "ERROR: 无法获取服务端公钥" << endl;
            return nullptr;
        }
        
        // 创建新的DH对象，使用标准参数组和服务端公钥
        DH* new_dh = DH_new();
        BIGNUM* p_copy = BN_dup(std_p);
        BIGNUM* g_copy = BN_dup(std_g);
        BIGNUM* pub_key_copy = BN_dup(server_pub_key);
        
        if (!p_copy || !g_copy || !pub_key_copy) {
            BN_free(p_copy);
            BN_free(g_copy);
            BN_free(pub_key_copy);
            DH_free(dh);
            DH_free(standard_dh);
            DH_free(new_dh);
            cerr << "ERROR: 无法复制DH参数" << endl;
            return nullptr;
        }
        
        if (!DH_set0_pqg(new_dh, p_copy, nullptr, g_copy) || 
            !DH_set0_key(new_dh, pub_key_copy, nullptr)) {
            BN_free(p_copy);
            BN_free(g_copy);
            BN_free(pub_key_copy);
            DH_free(dh);
            DH_free(standard_dh);
            DH_free(new_dh);
            cerr << "ERROR: 无法设置DH参数和公钥" << endl;
            return nullptr;
        }
        
        DH_free(dh);
        DH_free(standard_dh);
        dh = new_dh;
    }
    
    return dh;
}

// 生成客户端DH密钥对
// 使用与服务端相同的DH参数（p, g）
DH* generate_client_dh_keypair(DH* server_dh) {
    if (!server_dh) {
        cerr << "ERROR: 服务端DH参数为空" << endl;
        return nullptr;
    }
    
    // 创建新的DH对象并复制参数
    DH* client_dh = DH_new();
    if (!client_dh) {
        cerr << "ERROR: 无法创建DH对象" << endl;
        return nullptr;
    }
    
    // 获取服务端的p和g参数
    const BIGNUM* p = nullptr;
    const BIGNUM* g = nullptr;
    DH_get0_pqg(server_dh, &p, nullptr, &g);
    
    if (!p || !g) {
        DH_free(client_dh);
        cerr << "ERROR: 无法获取DH参数" << endl;
        return nullptr;
    }
    
    // 设置客户端的p和g参数
    BIGNUM* p_copy = BN_dup(p);
    BIGNUM* g_copy = BN_dup(g);
    
    if (!p_copy || !g_copy) {
        BN_free(p_copy);
        BN_free(g_copy);
        DH_free(client_dh);
        cerr << "ERROR: 无法复制DH参数" << endl;
        return nullptr;
    }
    
    if (!DH_set0_pqg(client_dh, p_copy, nullptr, g_copy)) {
        BN_free(p_copy);
        BN_free(g_copy);
        DH_free(client_dh);
        cerr << "ERROR: 无法设置DH参数" << endl;
        return nullptr;
    }
    
    // 生成客户端密钥对
    if (!DH_generate_key(client_dh)) {
        DH_free(client_dh);
        cerr << "ERROR: 无法生成客户端DH密钥对" << endl;
        return nullptr;
    }
    
    return client_dh;
}

// 计算共享密钥
string compute_dh_shared_secret(DH* client_dh, DH* server_dh) {
    if (!client_dh || !server_dh) {
        throw runtime_error("DH对象为空");
    }
    
    // 获取服务端公钥
    const BIGNUM* server_pub_key = DH_get0_pub_key(server_dh);
    if (!server_pub_key) {
        throw runtime_error("无法获取服务端公钥");
    }
    
    // 获取共享密钥长度
    int key_len = DH_size(client_dh);
    if (key_len <= 0) {
        throw runtime_error("无效的DH密钥长度");
    }
    
    // 分配缓冲区
    vector<unsigned char> shared_secret(key_len);
    
    // 计算共享密钥
    int computed_len = DH_compute_key(shared_secret.data(), server_pub_key, client_dh);
    if (computed_len <= 0) {
        unsigned long err = ERR_get_error();
        throw runtime_error("计算共享密钥失败，错误代码: " + to_string(err));
    }
    
    // 调整大小以匹配实际计算的密钥长度
    shared_secret.resize(computed_len);
    
    // 将共享密钥转换为字符串（用于RC4加密）
    return string(reinterpret_cast<const char*>(shared_secret.data()), computed_len);
}

// 从十六进制字符串加载客户端DH私钥和公钥
// 如果密钥为空，则生成新的密钥对
DH* load_or_generate_client_dh(DH* server_dh, const string& hex_private_key, const string& hex_public_key) {
    // 如果私钥和公钥都为空，生成新的密钥对
    if (hex_private_key.empty() && hex_public_key.empty()) {
        return generate_client_dh_keypair(server_dh);
    }
    
    // 如果提供了密钥，加载它们
    if (!hex_private_key.empty() && !hex_public_key.empty()) {
        DH* client_dh = DH_new();
        if (!client_dh) {
            cerr << "ERROR: 无法创建DH对象" << endl;
            return nullptr;
        }
        
        // 获取服务端的p和g参数
        const BIGNUM* p = nullptr;
        const BIGNUM* g = nullptr;
        DH_get0_pqg(server_dh, &p, nullptr, &g);
        
        if (!p || !g) {
            DH_free(client_dh);
            cerr << "ERROR: 无法获取DH参数" << endl;
            return nullptr;
        }
        
        // 设置客户端的p和g参数
        BIGNUM* p_copy = BN_dup(p);
        BIGNUM* g_copy = BN_dup(g);
        
        if (!p_copy || !g_copy) {
            BN_free(p_copy);
            BN_free(g_copy);
            DH_free(client_dh);
            cerr << "ERROR: 无法复制DH参数" << endl;
            return nullptr;
        }
        
        if (!DH_set0_pqg(client_dh, p_copy, nullptr, g_copy)) {
            BN_free(p_copy);
            BN_free(g_copy);
            DH_free(client_dh);
            cerr << "ERROR: 无法设置DH参数" << endl;
            return nullptr;
        }
        
        // 加载私钥和公钥
        BIGNUM* priv_key = hex_to_bn(hex_private_key);
        BIGNUM* pub_key = hex_to_bn(hex_public_key);
        
        if (!priv_key || !pub_key) {
            BN_free(priv_key);
            BN_free(pub_key);
            DH_free(client_dh);
            cerr << "ERROR: 无法加载客户端密钥" << endl;
            return nullptr;
        }
        
        if (!DH_set0_key(client_dh, pub_key, priv_key)) {
            BN_free(priv_key);
            BN_free(pub_key);
            DH_free(client_dh);
            cerr << "ERROR: 无法设置客户端密钥" << endl;
            return nullptr;
        }
        
        return client_dh;
    }
    
    // 如果只提供了部分密钥，生成新的密钥对
    return generate_client_dh_keypair(server_dh);
}

// 获取客户端DH公钥的十六进制字符串（X.509 SubjectPublicKeyInfo格式）
string get_client_dh_public_key_hex(DH* client_dh) {
    if (!client_dh) {
        return "";
    }
    
    // 创建EVP_PKEY对象并设置DH公钥
    EVP_PKEY* pkey = EVP_PKEY_new();
    if (!pkey) {
        cerr << "ERROR: 无法创建EVP_PKEY对象" << endl;
        return "";
    }
    
    // 将DH对象设置到EVP_PKEY中
    // 注意：这里需要复制DH对象，因为EVP_PKEY会获取所有权
    DH* dh_copy = nullptr;
    const BIGNUM* p = nullptr;
    const BIGNUM* g = nullptr;
    const BIGNUM* pub_key = nullptr;
    
    DH_get0_pqg(client_dh, &p, nullptr, &g);
    pub_key = DH_get0_pub_key(client_dh);
    
    if (!p || !g || !pub_key) {
        EVP_PKEY_free(pkey);
        cerr << "ERROR: DH对象缺少必要的参数" << endl;
        return "";
    }
    
    // 创建DH对象的副本
    dh_copy = DH_new();
    if (!dh_copy) {
        EVP_PKEY_free(pkey);
        cerr << "ERROR: 无法创建DH对象副本" << endl;
        return "";
    }
    
    // 复制p、g和公钥
    BIGNUM* p_copy = BN_dup(p);
    BIGNUM* g_copy = BN_dup(g);
    BIGNUM* pub_key_copy = BN_dup(pub_key);
    
    if (!p_copy || !g_copy || !pub_key_copy) {
        BN_free(p_copy);
        BN_free(g_copy);
        BN_free(pub_key_copy);
        DH_free(dh_copy);
        EVP_PKEY_free(pkey);
        cerr << "ERROR: 无法复制DH参数" << endl;
        return "";
    }
    
    if (!DH_set0_pqg(dh_copy, p_copy, nullptr, g_copy) || 
        !DH_set0_key(dh_copy, pub_key_copy, nullptr)) {
        BN_free(p_copy);
        BN_free(g_copy);
        BN_free(pub_key_copy);
        DH_free(dh_copy);
        EVP_PKEY_free(pkey);
        cerr << "ERROR: 无法设置DH参数" << endl;
        return "";
    }
    
    // 将DH对象赋值给EVP_PKEY
    if (EVP_PKEY_set1_DH(pkey, dh_copy) != 1) {
        DH_free(dh_copy);
        EVP_PKEY_free(pkey);
        unsigned long err = ERR_get_error();
        char err_buf[256];
        ERR_error_string_n(err, err_buf, sizeof(err_buf));
        cerr << "ERROR: 无法设置DH到EVP_PKEY，错误: " << err_buf << endl;
        return "";
    }
    
    // 释放DH副本（EVP_PKEY已经获取了引用）
    DH_free(dh_copy);
    
    // 将EVP_PKEY编码为DER格式（X.509 SubjectPublicKeyInfo）
    unsigned char* der_data = nullptr;
    int der_len = i2d_PUBKEY(pkey, &der_data);
    
    EVP_PKEY_free(pkey);
    
    if (der_len <= 0 || !der_data) {
        unsigned long err = ERR_get_error();
        char err_buf[256];
        ERR_error_string_n(err, err_buf, sizeof(err_buf));
        cerr << "ERROR: 无法编码DH公钥为DER格式，错误: " << err_buf << endl;
        return "";
    }
    
    // 将DER二进制数据转换为十六进制字符串
    string hex_result = to_hex_string(string(reinterpret_cast<const char*>(der_data), der_len));
    
    // 释放DER数据
    OPENSSL_free(der_data);
    
    return hex_result;
}

// 获取客户端DH私钥的十六进制字符串
string get_client_dh_private_key_hex(DH* client_dh) {
    if (!client_dh) {
        return "";
    }
    
    const BIGNUM* priv_key = DH_get0_priv_key(client_dh);
    if (!priv_key) {
        return "";
    }
    
    return bn_to_hex(priv_key);
}

// AES/ECB/PKCS5Padding 加密（固定使用AES-128，16字节密钥）
string aes_ecb_encrypt(const string& plaintext, const string& key) {
    // 固定使用AES-128（16字节密钥）
    const EVP_CIPHER* cipher = EVP_aes_128_ecb();
    const int AES128_KEY_LEN = 16;
    
    // 准备16字节密钥
    // 如果密钥长度大于16字节，使用前16字节；如果小于16字节，用0填充
    vector<unsigned char> key_data(AES128_KEY_LEN, 0);
    int copy_len = min(AES128_KEY_LEN, (int)key.length());
    memcpy(key_data.data(), key.data(), copy_len);
    
    // 创建加密上下文
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw runtime_error("无法创建加密上下文");
    }
    
    // 初始化加密
    if (EVP_EncryptInit_ex(ctx, cipher, nullptr, key_data.data(), nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("无法初始化AES加密");
    }
    
    // 启用PKCS5填充（OpenSSL中PKCS5和PKCS7在AES中是相同的）
    EVP_CIPHER_CTX_set_padding(ctx, 1);
    
    // 计算输出缓冲区大小（明文长度 + 一个块大小用于填充）
    int out_len = plaintext.length() + 16;
    vector<unsigned char> ciphertext(out_len);
    
    // 执行加密
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &out_len, 
                         reinterpret_cast<const unsigned char*>(plaintext.data()), 
                         plaintext.length()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("AES加密失败");
    }
    
    // 完成加密（处理PKCS5填充）
    int final_len = 0;
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + out_len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("AES加密完成失败");
    }
    
    out_len += final_len;
    EVP_CIPHER_CTX_free(ctx);
    
    // 返回加密结果
    return string(reinterpret_cast<const char*>(ciphertext.data()), out_len);
}

// AES/ECB/PKCS5Padding 解密（固定使用AES-128，16字节密钥）
string aes_ecb_decrypt(const string& ciphertext, const string& key) {
    // 固定使用AES-128（16字节密钥）
    const EVP_CIPHER* cipher = EVP_aes_128_ecb();
    const int AES128_KEY_LEN = 16;
    
    // 准备16字节密钥
    // 如果密钥长度大于16字节，使用前16字节；如果小于16字节，用0填充
    vector<unsigned char> key_data(AES128_KEY_LEN, 0);
    int copy_len = min(AES128_KEY_LEN, (int)key.length());
    memcpy(key_data.data(), key.data(), copy_len);
    
    // 检查密文长度是否为16字节的倍数（AES块大小）
    if (ciphertext.length() % 16 != 0) {
        throw runtime_error("密文长度必须是16字节的倍数");
    }
    
    // 创建解密上下文
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw runtime_error("无法创建解密上下文");
    }
    
    // 初始化解密
    if (EVP_DecryptInit_ex(ctx, cipher, nullptr, key_data.data(), nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("无法初始化AES解密");
    }
    
    // 启用PKCS5填充（OpenSSL中PKCS5和PKCS7在AES中是相同的）
    EVP_CIPHER_CTX_set_padding(ctx, 1);
    
    // 计算输出缓冲区大小（密文长度）
    int out_len = ciphertext.length();
    vector<unsigned char> plaintext(out_len);
    
    // 执行解密
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &out_len,
                         reinterpret_cast<const unsigned char*>(ciphertext.data()),
                         ciphertext.length()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("AES解密失败");
    }
    
    // 完成解密（移除PKCS5填充）
    int final_len = 0;
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + out_len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw runtime_error("AES解密完成失败，可能是填充错误");
    }
    
    out_len += final_len;
    EVP_CIPHER_CTX_free(ctx);
    
    // 返回解密结果（已自动移除PKCS5填充）
    return string(reinterpret_cast<const char*>(plaintext.data()), out_len);
}

// DH加密（使用共享密钥进行AES/ECB/PKCS5Padding加密）
string dh_encrypt(const string& plaintext, const string& shared_secret) {
    // 使用共享密钥进行AES/ECB/PKCS5Padding加密
    string ciphertext = aes_ecb_encrypt(plaintext, shared_secret);
    // Base64编码
    vector<unsigned char> ciphertext_vec(ciphertext.begin(), ciphertext.end());
    return base64_encode(ciphertext_vec);
}

// DH解密（使用共享密钥进行AES/ECB/PKCS5Padding解密）
string dh_decrypt(const string& ciphertext_base64, const string& shared_secret) {
    // Base64解码
    vector<unsigned char> ciphertext = base64_decode(ciphertext_base64);
    string ciphertext_str(ciphertext.begin(), ciphertext.end());
    // 使用共享密钥进行AES/ECB/PKCS5Padding解密
    return aes_ecb_decrypt(ciphertext_str, shared_secret);
}

// 完整的DH加密流程
string dh_encrypt_with_server_key(const string& plaintext, const string& server_public_key_hex, 
                                   DH* client_dh) {
    // 加载服务端DH公钥
    DH* server_dh = load_server_dh_public_key(server_public_key_hex);
    if (!server_dh) {
        throw runtime_error("无法加载服务端DH公钥");
    }
    
    // 计算共享密钥
    string shared_secret = compute_dh_shared_secret(client_dh, server_dh);
    
    // 使用共享密钥加密
    string encrypted = dh_encrypt(plaintext, shared_secret);
    
    // 清理资源
    DH_free(server_dh);
    
    return encrypted;
}

// 完整的DH解密流程
string dh_decrypt_with_server_key(const string& ciphertext_base64, const string& server_public_key_hex,
                                   DH* client_dh) {
    // 加载服务端DH公钥
    DH* server_dh = load_server_dh_public_key(server_public_key_hex);
    if (!server_dh) {
        throw runtime_error("无法加载服务端DH公钥");
    }
    
    // 计算共享密钥
    string shared_secret = compute_dh_shared_secret(client_dh, server_dh);
    
    // 使用共享密钥解密
    string decrypted = dh_decrypt(ciphertext_base64, shared_secret);
    
    // 清理资源
    DH_free(server_dh);
    
    return decrypted;
}

