#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <map>
#include <cstdlib>
#include <thread>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include "utils/http_utils.cpp"
#include "utils/sign_utils.cpp"
#include "utils/crypt_utils.cpp"
#include "utils/common_utils.cpp"
#include "utils/cJSON.h"
#include "utils/http_check.h"
#include "Updater.h"
// #########################系统配置开始###################################

// 平台地址
std::string host;
const std::string domains[] = {"vip.jsyz.asia", "vip.jszun.com", "vip.jsjst.top"};
// std::string host = "api.jsjst.top";
//   APP编号
std::string app_id = "3575";
// APP密钥
std::string app_secret = "1snamcsfh76mmpi21jmpy55utk0bhy3f";

// 服务端公钥
const char *server_public_key = R"(308202283082011b06092a864886f70d0103013082010c0282010100ffffffffffffffffc90fdaa22168c234c4c6628b80dc1cd129024e088a67cc74020bbea63b139b22514a08798e3404ddef9519b3cd3a431b302b0a6df25f14374fe1356d6d51c245e485b576625e7ec6f44c42e9a637ed6b0bff5cb6f406b7edee386bfb5a899fa5ae9f24117c4b1fe649286651ece45b3dc2007cb8a163bf0598da48361c55d39a69163fa8fd24cf5f83655d23dca3ad961c62f356208552bb9ed529077096966d670c354e4abc9804f1746c08ca18217c32905e462e36ce3be39e772c180e86039b2783a2ec07a28fb5c55df06f4c52c9de2bcbf6955817183995497cea956ae515d2261898fa051015728e5a8aacaa68ffffffffffffffff020102020200e0038201050002820100246135d4410b8094247a96e66a15c19e8ee6cb503c4b8138044edabf8c4e3aa105f757963c3039ab147321731a115fcde2a366a869d2c94938c5a679649ea774e8af1d293b35a71fe99d92799c9ccbee35c440774aeac955a683786603dbe729d9179aa8f37255b4e5267f74e3a920a64790b68f358005a1583969a8b4503f5dc34a9f80802af751f58f40ae6e163b3c8b0375078b5276119d753d6f16f6407f094cf17bcf4ca6c3743481dc2a59dbd94f7f2aaf3f2e8c79f495c20482b89cce281655d0bdf3e5c783567c122dc45d713aac9ddd2143ccce1ed6925464dda6c7be7f02daa089b411a61a421125bd22d9d467bff3d926ab7446e8e6c6d7a261d5)";

// 当前程序版本号 - 如果为空则不检查版本
std::string app_version = "1.37.2.08";
// 公告变量编号 - 如果为空则不输出公告
std::string notice_id = "1971";

// 卡密存储路径
const static char *storage_dir = "/sdcard/AuraKernel";
const static char *card_path = "/sdcard/AuraKernel/Aura.km";
// 机器码存储路径
const static char *imei_path = "/sdcard/AuraKernel/Aura.imei";

static bool ensureStorageDir()
{
    if (mkdir(storage_dir, 0777) != 0 && errno != EEXIST)
    {
        printf("  \033[1;31m  Failed to create %s: %s\033[0m\n", storage_dir, strerror(errno));
        return false;
    }

    chmod(storage_dir, 0777);
    return true;
}

static bool readTokenFile(const char *path, char *out, size_t outSize)
{
    if (out == nullptr || outSize == 0)
        return false;

    out[0] = '\0';
    FILE *fp = fopen(path, "r");
    if (fp == nullptr)
        return false;

    char fmt[32];
    snprintf(fmt, sizeof(fmt), "%%%zus", outSize - 1);
    int ret = fscanf(fp, fmt, out);
    fclose(fp);

    return ret == 1 && out[0] != '\0';
}

static bool writeTokenFile(const char *path, const char *value)
{
    FILE *fp = fopen(path, "w");
    if (fp == nullptr)
    {
        printf("  \033[1;31m  Failed to open %s: %s\033[0m\n", path, strerror(errno));
        return false;
    }

    fprintf(fp, "%s", value);
    fclose(fp);
    return true;
}

// 心跳容错次数 - 连续失败指定的次数就会停止运行
const static int canError = 5;
// 心跳失败次数 - 这里别动
int errorCount = 0;
// 心跳验证频率 - 如果为0则不进行心跳验证 单码应用→用户安全配置→心跳时间间隔
// 注意: 1.单位:秒 2.如果后台填写的频率为300 这里的heartRate建议你填写为55 说明:后台心跳间隔[300秒]=源码心跳请求间隔[55秒]*canError[5次]+手机心跳请求延迟预留[25秒]
const static int heartRate = 0;
// 客户端dh私钥（运行时赋值）
std::string client_dh_private_key = "";
// 客户端dh公钥（运行时赋值）
std::string client_dh_public_key = "";
// #########################系统配置结束###################################

std::string login_token = "";

std::string reqCommonParams()
{
    std::string params = "";
    std::string timestamp = getCurrentTimestamp();
    params += "timestamp=" + timestamp + "&";
    params += "safeCode=" + generate_random_string();
    return params;
}

DH *get_client_dh_keypair()
{
    static DH *client_dh = nullptr;
    static bool initialized = false;

    if (!initialized)
    {
        DH *server_dh = load_server_dh_public_key(std::string(server_public_key));
        if (!server_dh)
        {
            std::cout << "加载服务端DH公钥失败" << std::endl;
            exit(0);
        }

        client_dh = load_or_generate_client_dh(server_dh, client_dh_private_key, client_dh_public_key);

        if (!client_dh)
        {
            printf("  \033[1;31m  ✘ 获取客户端DH密钥对失败\033[0m\n");
            exit(0);
        }

        if (client_dh_private_key.empty() || client_dh_public_key.empty())
        {
            client_dh_public_key = get_client_dh_public_key_hex(client_dh);
            client_dh_private_key = get_client_dh_private_key_hex(client_dh);
        }

        DH_free(server_dh);
        initialized = true;
    }

    return client_dh;
}

std::string reqCommonInit(std::string params)
{
    DH *client_dh = get_client_dh_keypair();
    if (!client_dh)
    {
        std::cout << "获取客户端DH密钥对失败" << std::endl;
        exit(0);
    }

    if (client_dh_public_key.empty())
    {
        client_dh_public_key = get_client_dh_public_key_hex(client_dh);
        client_dh_private_key = get_client_dh_private_key_hex(client_dh);
    }

    std::string sorted_params = sort_dict_req(params + "&bob=" + client_dh_public_key);
    std::string md5_str = md5(sorted_params + app_secret);

    std::string encrypted;
    try
    {
        encrypted = dh_encrypt_with_server_key(params + "&signature=" + md5_str,
                                               std::string(server_public_key),
                                               client_dh);
    }
    catch (const std::exception &e)
    {
        printf("  \033[1;31m  ✘ DH加密失败: %s\033[0m\n", e.what());
        exit(0);
    }

    return "appId=" + app_id + "&params=" + encrypted + "&bob=" + client_dh_public_key;
}

void resCommonInit(std::string reqParams, std::string resParams)
{
    // 验证安全码是否正确
    std::string req_safe_code = extract_safe_code(reqParams);
    std::string res_safe_code = extract_safe_code_from_json(resParams);
    if (req_safe_code != res_safe_code)
    {
        printf("  \033[1;31m┌──────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;31m│  🚨 安全码不匹配，请求可能被劫持！\033[0m           │\n");
        printf("  \033[1;31m└──────────────────────────────────────────┘\033[0m\n");
        exit(0);
    }

    // 验证签名是否正确
    std::string res_signature = extract_signature_from_json(resParams);
    std::string sorted_params = sort_json_by_ascii(resParams);
    std::string md5_str = md5(sorted_params + app_secret);

    if (res_signature != md5_str)
    {
        printf("  \033[1;31m┌──────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;31m│  🚨 签名验证失败，请求可能被劫持！\033[0m             │\n");
        printf("  \033[1;31m└──────────────────────────────────────────┘\033[0m\n");
        exit(0);
    }

    // 验证时间戳是否正确
    std::string req_timestamp = extract_timestamp(reqParams);
    std::string res_timestamp = extract_timestamp_from_json(resParams);

    // 时间戳相差10秒以内
    if (std::abs(std::stoll(res_timestamp) - std::stoll(req_timestamp)) > 10 * 1000)
    {
        printf("  \033[1;31m┌──────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;31m│  🚨 时间戳不匹配，请求可能被劫持！\033[0m             │\n");
        printf("  \033[1;31m└──────────────────────────────────────────┘\033[0m\n");
        exit(0);
    }
}

std::string decrypt_response(std::string response)
{
    // 获取客户端DH密钥对
    DH *client_dh = get_client_dh_keypair();
    if (!client_dh)
    {
        std::cout << "获取客户端DH密钥对失败" << std::endl;
        exit(0);
    }

    try
    {
        // 使用DH解密
        std::string decrypted = dh_decrypt_with_server_key(response,
                                                           std::string(server_public_key),
                                                           client_dh);
        return decrypted;
    }
    catch (const std::exception &e)
    {
        printf("  \033[1;31m  ✘ DH解密失败: %s\033[0m\n", e.what());
        return "error";
    }
}

/**
 * 检测域名API
 */
void check_host()
{
    int domain_count = sizeof(domains) / sizeof(domains[0]);
    printf("  \033[1;36m┌──────────────────────────────────────────┐\033[0m\n");
    printf("  \033[1;36m│  🌐 正在检测服务器节点...\033[0m                    │\n");
    printf("  \033[1;36m└──────────────────────────────────────────┘\033[0m\n");
    for (int i = 0; i < domain_count; i++)
    {
        printf("  \033[1;34m  ⏳ 正在尝试节点 [%d/%d]...\033[0m ", i + 1, domain_count);
        fflush(stdout);
        int status_code = checkHttpStatusCode(domains[i]);
        if (status_code == -1)
        {
            printf("\033[1;31m✘ 超时\033[0m\n");
            printf("  \033[1;33m  ⚠ 切换到下一个节点...\033[0m\n");
        }
        else if (status_code == 200)
        {
            host = domains[i];
            printf("\033[1;32m✔ 连接成功！\033[0m\n");
            printf("  \033[1;32m  ✔ 节点已就绪\033[0m\n");
            break;
        }
        else
        {
            printf("\033[1;33m状态码: %d\033[0m\n", status_code);
            printf("  \033[1;33m  ⚠ 切换到下一个节点...\033[0m\n");
        }
        if (i == domain_count - 1)
        {
            printf("\n  \033[1;31m┌──────────────────────────────────────────┐\033[0m\n");
            printf("  \033[1;31m│  ✘ 所有节点均无法连接！\033[0m                    │\n");
            printf("  \033[1;31m│  ⚠ 请检查网络连接后重试\033[0m                    │\n");
            printf("  \033[1;31m└──────────────────────────────────────────┘\033[0m\n");
            exit(0);
        }
    }
}

/**
 * 获取程序公告API
 */
void getNoticeApi()
{
    std::string params = reqCommonParams();
    std::string req_params = reqCommonInit(params + "&variableId=" + notice_id);

    printf("  \033[1;36m📢 正在获取服务器公告...\033[0m\n");
    fflush(stdout);

    std::string response = send_post(host, "/api/expand/variable", req_params);

    std::string decrypted = decrypt_response(response);

    // 验证响应数据
    resCommonInit(params, decrypted);

    // 开始处理业务逻辑
    cJSON *json = cJSON_Parse(decrypted.c_str());
    if (json == NULL)
    {
        printf("  \033[1;33m  ⚠ 解析公告数据失败，跳过公告\033[0m\n");
        return;
    }

    cJSON *code = cJSON_GetObjectItem(json, "code");
    if (code == NULL || !cJSON_IsNumber(code))
    {
        printf("  \033[1;33m  ⚠ 公告数据异常(code)，跳过公告\033[0m\n");
        cJSON_Delete(json);
        return;
    }

    if (code->valueint != 1)
    {
        cJSON *msg = cJSON_GetObjectItem(json, "msg");
        if (msg != NULL && cJSON_IsString(msg))
        {
            printf("  \033[1;33m  ⚠ 公告获取失败: %s\033[0m\n", msg->valuestring);
        }
        cJSON_Delete(json);
        return;
    }

    cJSON *data = cJSON_GetObjectItem(json, "data");
    if (data == NULL)
    {
        printf("  \033[1;33m  ⚠ 公告数据为空，跳过\033[0m\n");
        cJSON_Delete(json);
        return;
    }

    cJSON *content = cJSON_GetObjectItem(data, "content");
    if (content == NULL || !cJSON_IsString(content))
    {
        printf("  \033[1;33m  ⚠ 公告内容为空，跳过\033[0m\n");
        cJSON_Delete(json);
        return;
    }

    printf("\n");
    printf("  \033[1;35m┌──────────────────────────────────────────┐\033[0m\n");
    printf("  \033[1;35m│  📋 服务器公告\033[0m                                 │\n");
    printf("  \033[1;35m├──────────────────────────────────────────┤\033[0m\n");

    // 分行显示公告内容
    std::string noticeContent = content->valuestring;
    std::string line;
    for (char ch : noticeContent)
    {
        if (ch == '\n')
        {
            printf("  │  %-40s│\n", line.c_str());
            line.clear();
        }
        else
        {
            line += ch;
        }
    }
    if (!line.empty())
    {
        printf("  │  %-40s│\n", line.c_str());
    }
    printf("  \033[1;35m└──────────────────────────────────────────┘\033[0m\n\n");

    cJSON_Delete(json);
    return;
}

/**
 * 检查版本更新API
 */
void checkVersionApi()
{
    std::string params = reqCommonParams();
    std::string req_params = reqCommonInit(params);

    printf("  \033[1;36m🔍 正在检查版本更新...\033[0m\n");
    fflush(stdout);

    std::string response = send_post(host, "/api/expand/new-ver", req_params);

    std::string decrypted = decrypt_response(response);

    // 验证响应数据
    resCommonInit(params, decrypted);

    // 开始处理业务逻辑
    cJSON *json = cJSON_Parse(decrypted.c_str());
    if (json == NULL)
    {
        printf("  \033[1;33m  ⚠ 版本检查响应解析失败\033[0m\n");
        return;
    }

    cJSON *code = cJSON_GetObjectItem(json, "code");
    if (code == NULL || !cJSON_IsNumber(code))
    {
        printf("  \033[1;33m  ⚠ 版本检查数据异常(code)\033[0m\n");
        cJSON_Delete(json);
        return;
    }

    if (code->valueint != 1)
    {
        cJSON *msg = cJSON_GetObjectItem(json, "msg");
        if (msg != NULL && cJSON_IsString(msg))
        {
            printf("  \033[1;33m  ⚠ %s\033[0m\n", msg->valuestring);
        }
        cJSON_Delete(json);
        return;
    }

    // 检查num与app_version是否一致
    cJSON *data = cJSON_GetObjectItem(json, "data");
    if (data == NULL)
    {
        printf("  \033[1;33m  ⚠ 版本数据为空\033[0m\n");
        cJSON_Delete(json);
        return;
    }

    cJSON *num = cJSON_GetObjectItem(data, "num");
    if (num == NULL || !cJSON_IsString(num))
    {
        printf("  \033[1;33m  ⚠ 版本号数据异常\033[0m\n");
        cJSON_Delete(json);
        return;
    }

    if (num->valuestring == app_version)
    {
        printf("  \033[1;32m  ✔ 当前已是最新版本: %s\033[0m\n\n", app_version.c_str());
        cJSON_Delete(json);
        return;
    }
    else
    {
        printf("\n");
        printf("  \033[1;33m┌──────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;33m│  🆕 发现新版本！\033[0m                              │\n");
        printf("  \033[1;33m├──────────────────────────────────────────┤\033[0m\n");
        printf("  \033[1;33m│  当前版本: %-32s\033[0m│\n", app_version.c_str());
        printf("  \033[1;33m│  最新版本: %-32s\033[0m│\n", num->valuestring);
    }

    cJSON *name = cJSON_GetObjectItem(data, "name");
    if (name != NULL && cJSON_IsString(name))
    {
        printf("  \033[1;33m│  版本名称: %-32s\033[0m│\n", name->valuestring);
    }

    cJSON *updateTime = cJSON_GetObjectItem(data, "updateTime");
    if (updateTime != NULL && cJSON_IsString(updateTime))
    {
        printf("  \033[1;33m│  更新时间: %-32s\033[0m│\n", updateTime->valuestring);
    }

    cJSON *content = cJSON_GetObjectItem(data, "content");
    if (content != NULL && cJSON_IsString(content))
    {
        printf("  \033[1;33m├──────────────────────────────────────────┤\033[0m\n");
        printf("  \033[1;33m│  📝 更新内容:\033[0m                               │\n");
        std::string verContent = content->valuestring;
        std::string line;
        for (char ch : verContent)
        {
            if (ch == '\n')
            {
                printf("  \033[1;33m│  %-40s\033[0m│\n", line.c_str());
                line.clear();
            }
            else
            {
                line += ch;
            }
        }
        if (!line.empty())
        {
            printf("  \033[1;33m│  %-40s\033[0m│\n", line.c_str());
        }
    }

    cJSON *addr = cJSON_GetObjectItem(data, "addr");
    cJSON *forced = cJSON_GetObjectItem(data, "forced");

    printf("  \033[1;33m└──────────────────────────────────────────┘\033[0m\n\n");

    if (addr == NULL || !cJSON_IsString(addr))
    {
        printf("  \033[1;33m  ⚠ 更新地址为空\033[0m\n");
        cJSON_Delete(json);
        return;
    }

    // 使用 Updater 模块处理更新
    std::string mustUpdate = (forced != NULL && cJSON_IsNumber(forced) && forced->valueint == 1) ? "y" : "n";
    StartUpdate(app_version, num->valuestring, addr->valuestring, mustUpdate);

    cJSON_Delete(json);
    return;
}
/**
 * 单码登录API
 * 卡密为用户自己输入的
 */
int loginApi()
{
home_main:
    char card[40] = "";
    if (!ensureStorageDir())
        return 0;

    if (!readTokenFile(card_path, card, sizeof(card)))
    {
        printf("\n");
        printf("  \033[1;36m┌──────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;36m│  🔑 卡密验证\033[0m                                 │\n");
        printf("  \033[1;36m├──────────────────────────────────────────┤\033[0m\n");
        printf("  \033[1;36m│  \033[1;33m⮕ 请输入卡密: \033[0m");
        printf("[NEED_KAMI]\n");
        fflush(stdout);

        char _inputKm[128] = "";
        scanf("%127s", _inputKm);

        if (!writeTokenFile(card_path, _inputKm))
            return 0;

        snprintf(card, sizeof(card), "%s", _inputKm);

        FILE *fp = nullptr;
        if (fp != NULL)
        {
            fprintf(fp, "%s", _inputKm);
            fclose(fp);
            printf("  \033[1;36m│  \033[1;32m✔ 卡密已保存\033[0m                            │\n");
        }
        printf("  \033[1;36m└──────────────────────────────────────────┘\033[0m\n");
        printf("  \033[1;34m  ⏳ 正在验证卡密，请稍候...\033[0m\n\n");
    }
    else
    {
        printf("  \033[1;34m  ⏳ 正在验证已保存的卡密...\033[0m\n");
    }

    char imei[40] = "";
    if (!readTokenFile(imei_path, imei, sizeof(imei)))
    {
        printf("  \033[1;33m  ⚠ 未检测到设备标识，正在生成...\033[0m\n");
        srand(time(NULL));
        char *_Str = (char *)malloc((20 + 1) * sizeof(char));
        for (int i = 0; i < 20; i++)
        {
            int _randomNum = rand() % 26;
            _Str[i] = 'a' + _randomNum;
        }
        _Str[20] = '\0';

        if (!writeTokenFile(imei_path, _Str))
        {
            free(_Str);
            printf("  \033[1;31m  ✘ 设备标识文件创建失败\033[0m\n");
            return 0;
        }
        snprintf(imei, sizeof(imei), "%s", _Str);
        printf("  \033[1;32m  ✔ 设备标识已生成！\033[0m\n");
        free(_Str);
    }
    std::string params = reqCommonParams() + "&card=" + card + "&mac=" + imei;
    std::string req_params = reqCommonInit(params);

    std::string response = send_post(host, "/api/single/login", req_params);

    std::string decrypted = decrypt_response(response);

    // 验证响应数据
    resCommonInit(params, decrypted);

    // 开始处理业务逻辑
    cJSON *json = cJSON_Parse(decrypted.c_str());
    if (json == NULL)
    {
        printf("  \033[1;31m  ✘ 验证响应解析失败\033[0m\n");
        exit(0);
    }

    cJSON *code = cJSON_GetObjectItem(json, "code");
    if (code == NULL || !cJSON_IsNumber(code))
    {
        printf("  \033[1;31m  ✘ 验证数据异常(code)\033[0m\n");
        cJSON_Delete(json);
        exit(0);
    }

    if (code->valueint == 1)
    {
        cJSON *data = cJSON_GetObjectItem(json, "data");
        if (data == NULL)
        {
            printf("  \033[1;31m  ✘ 验证数据异常(data)\033[0m\n");
            cJSON_Delete(json);
            exit(0);
        }

        cJSON *token = cJSON_GetObjectItem(data, "token");
        if (token == NULL || !cJSON_IsString(token))
        {
            printf("  \033[1;31m  ✘ 验证数据异常(token)\033[0m\n");
            cJSON_Delete(json);
            exit(0);
        }

        cJSON *endTime = cJSON_GetObjectItem(data, "endTime");
        if (endTime == NULL || !cJSON_IsString(endTime))
        {
            printf("  \033[1;31m  ✘ 验证数据异常(endTime)\033[0m\n");
            cJSON_Delete(json);
            exit(0);
        }

        printf("\n");
        printf("  \033[1;32m┌──────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;32m│  ✅ 卡密验证成功！\033[0m                            │\n");
        printf("  \033[1;32m├──────────────────────────────────────────┤\033[0m\n");
        printf("  \033[1;32m│  📅 到期时间: %-31s\033[0m│\n", endTime->valuestring);
        printf("  \033[1;32m└──────────────────────────────────────────┘\033[0m\n\n");

        std::string token_str = token->valuestring;
        login_token = token_str;
        return 1;
    }
    else
    {
        cJSON *msg = cJSON_GetObjectItem(json, "msg");
        if (msg != NULL && cJSON_IsString(msg))
        {
            printf("\n");
            printf("  \033[1;31m┌──────────────────────────────────────────┐\033[0m\n");
            printf("  \033[1;31m│  ❌ 卡密验证失败\033[0m                           │\n");
            printf("  \033[1;31m├──────────────────────────────────────────┤\033[0m\n");
            printf("  \033[1;31m│  原因: %-36s\033[0m│\n", msg->valuestring);
            printf("  \033[1;31m└──────────────────────────────────────────┘\033[0m\n");
        }
        remove(card_path);
        if (imei[0] == '\0')
            remove(imei_path);
        printf("  \033[1;33m  ⏳ 请重新输入卡密...\033[0m\n\n");
        goto home_main;
    }

    cJSON_Delete(json);
    return 0;
}

void heart()
{
    if (errorCount > canError - 1)
    {
        printf("\n  \033[1;31m┌──────────────────────────────────────────┐\033[0m\n");
        printf("  \033[1;31m│  💔 心跳验证失败次数过多\033[0m                     │\n");
        printf("  \033[1;31m│  ⚠ 请确保网络连接稳定后重启\033[0m                   │\n");
        printf("  \033[1;31m└──────────────────────────────────────────┘\033[0m\n");
        exit(0);
    }

    printf("  \033[1;34m💓 正在心跳验证... [第%d次]\033[0m\r", errorCount + 1);
    fflush(stdout);

    std::string params = reqCommonParams();
    std::string req_params = reqCommonInit(params + "&token=" + login_token);

    std::string response = send_post(host, "/api/single/heart", req_params);

    std::string decrypted = decrypt_response(response);

    resCommonInit(params, decrypted);

    try
    {
        // 开始处理业务逻辑
        cJSON *json = cJSON_Parse(decrypted.c_str());
        if (json == NULL)
        {
            printf("  \033[1;33m  ⚠ 心跳响应解析失败\033[0m\n");
            errorCount = errorCount + 1;
            return;
        }

        cJSON *code = cJSON_GetObjectItem(json, "code");
        if (code == NULL || !cJSON_IsNumber(code))
        {
            printf("  \033[1;33m  ⚠ 心跳数据异常(code)\033[0m\n");
            errorCount = errorCount + 1;
            cJSON_Delete(json);
            return;
        }
        if (code->valueint == 1)
        {
            printf("  \033[1;32m💚 心跳正常 ✓\033[0m                                             \n");
            if (errorCount != 0)
            {
                errorCount = 0;
            }
        }
        else
        {
            printf("  \033[1;33m💛 心跳异常，剩余次数: %d\033[0m\n", canError - errorCount - 1);
            errorCount = errorCount + 1;
        }
        cJSON_Delete(json);
    }
    catch (...)
    {
        printf("  \033[1;31m  ✘ 心跳过程发生异常\033[0m\n");
        errorCount = errorCount + 1;
    }
}

void startHeartbeat()
{
    while (heartRate != 0)
    {
        heart();
        std::this_thread::sleep_for(std::chrono::seconds(heartRate));
    }
}

void heartController()
{
    // 运行到这里表示验证结束，开始心跳
    std::thread heartbeatThread(startHeartbeat);
    heartbeatThread.detach();
}

// 如果作为主程序运行，则取消注释即可
// int main()
// {
//     // 检测域名
//     check_host();

//     // 获取公告
//     if (!notice_id.empty())
//     {
//         getNoticeApi();
//     }

//     // 检查版本更新
//     if (!app_version.empty())
//     {
//         checkVersionApi();
//     }

//     // 调用 loginApi 函数
//     loginApi();

//     // 心跳验证 - 默认不进行心跳验证 如需开启心跳验证请将第49行的heartRate变量设置为根据你的业务所需的频率 单位:秒
//     heartController();

//     return 0;
// }

int network_verify()
{
    // 检测域名
    check_host();

    // 获取公告
    if (!notice_id.empty())
    {
        getNoticeApi();
    }

    // 检查版本更新
    if (!app_version.empty())
    {
        checkVersionApi();
    }

    // 调用 loginApi 函数
    int isLogin = false;
    while (!isLogin)
    {
        int mark = loginApi();
        if (mark == 1)
        {
            isLogin = true;
        }
    }

    // 心跳验证 - 默认不进行心跳验证 如需开启心跳验证请将第49行的heartRate变量设置为根据你的业务所需的频率 单位:秒
    heartController();

    return 0;
}
