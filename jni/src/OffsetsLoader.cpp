#include "Offsets.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <unordered_map>
#include <openssl/ssl.h>
#include <openssl/err.h>

// ==================== 定义变量（不初始化，加载失败就 exit） ====================
namespace Offsets
{
    uintptr_t GWorld;
    uintptr_t GName;
    uintptr_t MatrixChain1;
    uintptr_t MatrixChain2;
    uintptr_t ClassBase;
    uintptr_t GyroBase;

    uintptr_t GWorld_PersistentLevel;
    uintptr_t GWorld_ActorsArray;
    uintptr_t GWorld_ActorsCount;
    uintptr_t GWorld_ActorsCountDec;
    uintptr_t GWorld_GameState;

    uintptr_t Actor_RootComponent;
    uintptr_t Actor_TeamID;
    uintptr_t Actor_Health;
    uintptr_t Actor_HealthMax;
    uintptr_t Actor_PlayerName;
    uintptr_t Actor_PlayerUID;
    uintptr_t Actor_Velocity;
    uintptr_t Actor_Mesh;
    uintptr_t Actor_PawnState;
    uintptr_t Actor_bIsAI;
    uintptr_t Actor_bIsGunADS;
    uintptr_t Actor_bIsWeaponFiring;
    uintptr_t Actor_CurrentWeapon;
    uintptr_t Actor_SpeedValue;
    uintptr_t Actor_HighWalkSpeed;
    uintptr_t Actor_Rotator;
    uintptr_t Actor_Vehicle;
    uintptr_t Actor_WeaponEntity;
    uintptr_t Actor_ClassID;

    uintptr_t Controller_Offset;
    uintptr_t Controller_CameraManager;
    uintptr_t Controller_AimYaw;

    uintptr_t CameraManager_CameraPos;
    uintptr_t CameraManager_Rotation;
    uintptr_t CameraManager_FOV;

    uintptr_t POV_Location;
    uintptr_t POV_Rotation;
    uintptr_t POV_FOV;

    uintptr_t Weapon_RepID;
    uintptr_t Weapon_EntityComp;
    uintptr_t Weapon_BulletSpeed;
    uintptr_t Weapon_RecoilFactor;
    uintptr_t Weapon_ClipAmmo;
    uintptr_t Weapon_ClipMaxAmmo;
    uintptr_t Weapon_GripID;
    uintptr_t Weapon_CachedBulletTrack;
    uintptr_t Weapon_ShootBursts;

    uintptr_t Vehicle_CommonData;
    uintptr_t Vehicle_CurrentHP;
    uintptr_t Vehicle_MaxHP;
    uintptr_t Vehicle_CurrentFuel;
    uintptr_t Vehicle_MaxFuel;

    uintptr_t BoxPickUpDataList;
    uintptr_t FPickUpItemData_Count;
    uintptr_t PickUpDataList;
    uintptr_t Box_OpenState;
    uintptr_t Box_Open;

    uintptr_t Mesh_ComponentToWorld;
    uintptr_t Mesh_BoneArray;
    uintptr_t Mesh_BoneCountOffset;

    uintptr_t AliveNum;
    uintptr_t AllivePlayerNum;
    uintptr_t AliveRealPlayerNum;
    uintptr_t AliveTeamNum;

    uintptr_t Matrix_ViewMatrix;
    uintptr_t Matrix_Tol_Offset1;
    uintptr_t Matrix_Tol_Offset2;

    uintptr_t Decrypt_Step1;
    uintptr_t Decrypt_Step2_Offset1;
    uintptr_t Decrypt_Step3_Offset2;

    float HumanHeight;
    float MaxDrawDistance;
    uintptr_t SelfChain_Hop1;
    uintptr_t SelfChain_Hop2;
    uintptr_t SelfChain_Hop3;
    uintptr_t SelfChain_Hop4;
    uintptr_t Matrix_Offset1;
    uintptr_t Actor_CoordChain2;
    uintptr_t Actor_Height;
    uintptr_t Actor_AdvancedBot;
    uintptr_t Actor_GrenadeID;
    uintptr_t Enemy_EquipEntity;
    uintptr_t Enemy_EquipTable;
    uintptr_t Mesh_BoneCount;
    uintptr_t Mesh_BoneStart;
    uintptr_t FName_Chunk;
    uintptr_t FName_NameOff;
    uintptr_t DecryptArray_Base;
    uintptr_t DecryptArray_Hop1;
    uintptr_t DecryptArray_Hop2;
    uintptr_t DecryptArray_ArrayOff;
    uintptr_t DecryptArray_CountOff;
    uintptr_t Feature_Base;
    uintptr_t Feature_Hop1;
    uintptr_t Feature_Hop2;
    uintptr_t Feature_Hop3;
    uintptr_t Flash_CoordBase;
    uintptr_t Flash_CoordOff;
    uintptr_t Flash_ItemWrapper;
    uintptr_t Flash_BoxComp;
    uintptr_t Flash_ListOff;
}

// ---------- HTTPS GET (使用 OpenSSL) ----------
static std::string httpsGet(const std::string &url, int timeoutSec = 8)
{
    std::string host, path = "/";
    int port = 443;

    // 解析 URL
    size_t schemePos = url.find("://");
    size_t pos = (schemePos == std::string::npos) ? 0 : schemePos + 3;

    // 判断是否 HTTPS
    bool isHttps = true;
    if (schemePos != std::string::npos)
    {
        std::string scheme = url.substr(0, schemePos);
        isHttps = (scheme == "https");
        port = isHttps ? 443 : 80;
    }

    size_t colon = url.find(':', pos);
    size_t slash = url.find('/', pos);

    if (colon != std::string::npos && (slash == std::string::npos || colon < slash))
    {
        host = url.substr(pos, colon - pos);
        if (isHttps)
            port = 443; // 忽略 URL 中的端口，强制 443
        else
            port = std::stoi(url.substr(colon + 1, slash - colon - 1));
    }
    else if (slash != std::string::npos)
    {
        host = url.substr(pos, slash - pos);
    }
    else
    {
        host = url.substr(pos);
    }

    if (slash != std::string::npos)
        path = url.substr(slash);

    // ---------- 建立 TCP 连接 ----------
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return "";

    struct timeval tv;
    tv.tv_sec = timeoutSec;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    struct hostent *server = gethostbyname(host.c_str());
    if (!server)
    {
        close(sock);
        return "";
    }

    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    memcpy(&servAddr.sin_addr.s_addr, server->h_addr, server->h_length);
    servAddr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        close(sock);
        return "";
    }

    // ---------- SSL 握手 ----------
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ctx)
    {
        close(sock);
        return "";
    }

    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

    SSL *ssl = SSL_new(ctx);
    if (!ssl)
    {
        SSL_CTX_free(ctx);
        close(sock);
        return "";
    }

    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) != 1)
    {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        return "";
    }

    // ---------- 发送 HTTP GET 请求 ----------
    std::string request = "GET " + path + " HTTP/1.1\r\n"
                                          "Host: " +
                          host + "\r\n"
                                 "Connection: close\r\n"
                                 "User-Agent: AuraKernel/1.0\r\n"
                                 "\r\n";

    SSL_write(ssl, request.c_str(), request.size());

    // ---------- 接收响应 ----------
    std::string response;
    char buf[4096];
    int n;
    while ((n = SSL_read(ssl, buf, sizeof(buf) - 1)) > 0)
    {
        buf[n] = '\0';
        response += buf;
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sock);

    // 提取 body（跳过 HTTP 头）
    size_t bodyStart = response.find("\r\n\r\n");
    if (bodyStart != std::string::npos)
    {
        std::string body = response.substr(bodyStart + 4);
        // 调试：打印前200个字符
        // printf("  [调试] 服务器返回内容(前200字符): %s\n", body.substr(0, 200).c_str());
        return body;
    }
    return response;
}

// ---------- 简易 JSON 解析 ----------
static std::string trim(const std::string &s)
{
    size_t l = s.find_first_not_of(" \t\r\n\"");
    size_t r = s.find_last_not_of(" \t\r\n\"");
    return (l == std::string::npos) ? "" : s.substr(l, r - l + 1);
}

static uintptr_t parseHex(const std::string &s)
{
    return strtoull(s.c_str(), nullptr, 16);
}

static float parseFloat(const std::string &s)
{
    return strtof(s.c_str(), nullptr);
}

bool Offsets::LoadFromJson(const std::string &jsonStr)
{
    std::string s = jsonStr;
    size_t start = s.find('{');
    size_t end = s.rfind('}');
    if (start == std::string::npos || end == std::string::npos)
        return false;
    s = s.substr(start + 1, end - start - 1);

    std::unordered_map<std::string, std::string> kv;

    // 改进的解析：按行处理
    size_t pos = 0;
    while (pos < s.size())
    {
        // 找下一个 key 的开始引号
        size_t quoteStart = s.find('"', pos);
        if (quoteStart == std::string::npos)
            break;

        // 找 key 的结束引号
        size_t quoteEnd = s.find('"', quoteStart + 1);
        if (quoteEnd == std::string::npos)
            break;

        std::string key = s.substr(quoteStart + 1, quoteEnd - quoteStart - 1);

        // 找冒号
        size_t colon = s.find(':', quoteEnd + 1);
        if (colon == std::string::npos)
            break;

        // 找值的开始
        size_t valStart = s.find_first_not_of(" \t\r\n", colon + 1);
        if (valStart == std::string::npos)
            break;

        // 判断值类型：字符串（引号开头）还是数字
        std::string value;
        if (s[valStart] == '"')
        {
            // 字符串值，找结束引号
            size_t valEnd = s.find('"', valStart + 1);
            if (valEnd == std::string::npos)
                break;
            value = s.substr(valStart + 1, valEnd - valStart - 1);
            pos = valEnd + 1;
        }
        else
        {
            // 数值，找逗号或结束
            size_t comma = s.find(',', valStart);
            size_t brace = s.find('}', valStart);
            size_t valEnd = (comma != std::string::npos && comma < brace) ? comma : brace;
            if (valEnd == std::string::npos)
            {
                // 可能是最后一个值，取到末尾
                value = s.substr(valStart);
                pos = s.size();
            }
            else
            {
                value = s.substr(valStart, valEnd - valStart);
                pos = valEnd + 1;
            }
            // 去除多余空白
            value = trim(value);
        }

        kv[key] = value;
    }

#define SET_UINT(key, var)                         \
    do                                             \
    {                                              \
        auto it = kv.find(key);                    \
        if (it != kv.end() && !it->second.empty()) \
            var = parseHex(it->second);            \
    } while (0)

#define SET_FLOAT(key, var)                        \
    do                                             \
    {                                              \
        auto it = kv.find(key);                    \
        if (it != kv.end() && !it->second.empty()) \
            var = parseFloat(it->second);          \
    } while (0)

    SET_UINT("GWorld", GWorld);
    SET_UINT("GName", GName);
    SET_UINT("MatrixChain1", MatrixChain1);
    SET_UINT("MatrixChain2", MatrixChain2);
    SET_UINT("ClassBase", ClassBase);
    SET_UINT("GyroBase", GyroBase);
    SET_UINT("GWorld_PersistentLevel", GWorld_PersistentLevel);
    SET_UINT("GWorld_ActorsArray", GWorld_ActorsArray);
    SET_UINT("GWorld_ActorsCount", GWorld_ActorsCount);
    SET_UINT("GWorld_ActorsCountDec", GWorld_ActorsCountDec);
    SET_UINT("GWorld_GameState", GWorld_GameState);
    SET_UINT("Actor_RootComponent", Actor_RootComponent);
    SET_UINT("Actor_TeamID", Actor_TeamID);
    SET_UINT("Actor_Health", Actor_Health);
    SET_UINT("Actor_HealthMax", Actor_HealthMax);
    SET_UINT("Actor_PlayerName", Actor_PlayerName);
    SET_UINT("Actor_PlayerUID", Actor_PlayerUID);
    SET_UINT("Actor_Velocity", Actor_Velocity);
    SET_UINT("Actor_Mesh", Actor_Mesh);
    SET_UINT("Actor_PawnState", Actor_PawnState);
    SET_UINT("Actor_bIsAI", Actor_bIsAI);
    SET_UINT("Actor_bIsGunADS", Actor_bIsGunADS);
    SET_UINT("Actor_bIsWeaponFiring", Actor_bIsWeaponFiring);
    SET_UINT("Actor_CurrentWeapon", Actor_CurrentWeapon);
    SET_UINT("Actor_SpeedValue", Actor_SpeedValue);
    SET_UINT("Actor_HighWalkSpeed", Actor_HighWalkSpeed);
    SET_UINT("Actor_Rotator", Actor_Rotator);
    SET_UINT("Actor_Vehicle", Actor_Vehicle);
    SET_UINT("Actor_WeaponEntity", Actor_WeaponEntity);
    SET_UINT("Actor_ClassID", Actor_ClassID);
    SET_UINT("Controller_Offset", Controller_Offset);
    SET_UINT("Controller_CameraManager", Controller_CameraManager);
    SET_UINT("Controller_AimYaw", Controller_AimYaw);
    SET_UINT("CameraManager_CameraPos", CameraManager_CameraPos);
    SET_UINT("CameraManager_Rotation", CameraManager_Rotation);
    SET_UINT("CameraManager_FOV", CameraManager_FOV);
    SET_UINT("POV_Location", POV_Location);
    SET_UINT("POV_Rotation", POV_Rotation);
    SET_UINT("POV_FOV", POV_FOV);
    SET_UINT("Weapon_RepID", Weapon_RepID);
    SET_UINT("Weapon_EntityComp", Weapon_EntityComp);
    SET_UINT("Weapon_BulletSpeed", Weapon_BulletSpeed);
    SET_UINT("Weapon_RecoilFactor", Weapon_RecoilFactor);
    SET_UINT("Weapon_ClipAmmo", Weapon_ClipAmmo);
    SET_UINT("Weapon_ClipMaxAmmo", Weapon_ClipMaxAmmo);
    SET_UINT("Weapon_GripID", Weapon_GripID);
    SET_UINT("Weapon_CachedBulletTrack", Weapon_CachedBulletTrack);
    SET_UINT("Weapon_ShootBursts", Weapon_ShootBursts);
    SET_UINT("Vehicle_CommonData", Vehicle_CommonData);
    SET_UINT("Vehicle_CurrentHP", Vehicle_CurrentHP);
    SET_UINT("Vehicle_MaxHP", Vehicle_MaxHP);
    SET_UINT("Vehicle_CurrentFuel", Vehicle_CurrentFuel);
    SET_UINT("Vehicle_MaxFuel", Vehicle_MaxFuel);
    SET_UINT("BoxPickUpDataList", BoxPickUpDataList);
    SET_UINT("FPickUpItemData_Count", FPickUpItemData_Count);
    SET_UINT("PickUpDataList", PickUpDataList);
    SET_UINT("Box_OpenState", Box_OpenState);
    SET_UINT("Box_Open", Box_Open);
    SET_UINT("Mesh_ComponentToWorld", Mesh_ComponentToWorld);
    SET_UINT("Mesh_BoneArray", Mesh_BoneArray);
    SET_UINT("Mesh_BoneCountOffset", Mesh_BoneCountOffset);
    SET_UINT("AliveNum", AliveNum);
    SET_UINT("AllivePlayerNum", AllivePlayerNum);
    SET_UINT("AliveRealPlayerNum", AliveRealPlayerNum);
    SET_UINT("AliveTeamNum", AliveTeamNum);
    SET_UINT("Matrix_ViewMatrix", Matrix_ViewMatrix);
    SET_UINT("Matrix_Tol_Offset1", Matrix_Tol_Offset1);
    SET_UINT("Matrix_Tol_Offset2", Matrix_Tol_Offset2);
    SET_UINT("Decrypt_Step1", Decrypt_Step1);
    SET_UINT("Decrypt_Step2_Offset1", Decrypt_Step2_Offset1);
    SET_UINT("Decrypt_Step3_Offset2", Decrypt_Step3_Offset2);
    SET_FLOAT("HumanHeight", HumanHeight);
    SET_FLOAT("MaxDrawDistance", MaxDrawDistance);
    SET_UINT("SelfChain_Hop1", SelfChain_Hop1);
    SET_UINT("SelfChain_Hop2", SelfChain_Hop2);
    SET_UINT("SelfChain_Hop3", SelfChain_Hop3);
    SET_UINT("SelfChain_Hop4", SelfChain_Hop4);
    SET_UINT("Matrix_Offset1", Matrix_Offset1);
    SET_UINT("Actor_CoordChain2", Actor_CoordChain2);
    SET_UINT("Actor_Height", Actor_Height);
    SET_UINT("Actor_AdvancedBot", Actor_AdvancedBot);
    SET_UINT("Actor_GrenadeID", Actor_GrenadeID);
    SET_UINT("Enemy_EquipEntity", Enemy_EquipEntity);
    SET_UINT("Enemy_EquipTable", Enemy_EquipTable);
    SET_UINT("Mesh_BoneCount", Mesh_BoneCount);
    SET_UINT("Mesh_BoneStart", Mesh_BoneStart);
    SET_UINT("FName_Chunk", FName_Chunk);
    SET_UINT("FName_NameOff", FName_NameOff);
    SET_UINT("DecryptArray_Base", DecryptArray_Base);
    SET_UINT("DecryptArray_Hop1", DecryptArray_Hop1);
    SET_UINT("DecryptArray_Hop2", DecryptArray_Hop2);
    SET_UINT("DecryptArray_ArrayOff", DecryptArray_ArrayOff);
    SET_UINT("DecryptArray_CountOff", DecryptArray_CountOff);
    SET_UINT("Feature_Base", Feature_Base);
    SET_UINT("Feature_Hop1", Feature_Hop1);
    SET_UINT("Feature_Hop2", Feature_Hop2);
    SET_UINT("Feature_Hop3", Feature_Hop3);
    SET_UINT("Flash_CoordBase", Flash_CoordBase);
    SET_UINT("Flash_CoordOff", Flash_CoordOff);
    SET_UINT("Flash_ItemWrapper", Flash_ItemWrapper);
    SET_UINT("Flash_BoxComp", Flash_BoxComp);
    SET_UINT("Flash_ListOff", Flash_ListOff);

#undef SET_UINT
#undef SET_FLOAT

    return true;
}

bool Offsets::LoadFromRemote(const std::string &url)
{
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    std::string jsonStr = httpsGet(url, 8); // 8秒超时
    if (jsonStr.empty())
    {
        printf("  \033[1;31m  ✘ 服务器连接失败！\033[0m\n");
        printf("  \033[1;31m  ✘ 授权验证不通过，程序退出\033[0m\n");
        return false;
    }

    printf("  \033[1;32m  ✔ 服务器连接成功...\033[0m\n");

    if (!LoadFromJson(jsonStr))
    {
        printf("  \033[1;31m  ✘ 数据解析失败！\033[0m\n");
        printf("  \033[1;33m  [调试] 服务器返回(前300字符):\033[0m\n");
        printf("  \033[1;33m  ---BEGIN---\033[0m\n");
        printf("  %s\n", jsonStr.substr(0, 300).c_str());
        printf("  \033[1;33m  ---END---\033[0m\n");
        return false;
    }

    printf("  \033[1;32m  ✔ 加载完成，授权通过！\033[0m\n");
    return true;
}
