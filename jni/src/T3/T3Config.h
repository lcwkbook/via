#pragma once

#include <string>

/**
 * T3网络验证配置文件
 * 
 * ⚠️ 重要：使用前需要在T3网络验证后台管理系统配置以下内容
 * 
 * 🔑 第一步：基础配置（必须）
 * 1. RSA密钥对配置：
 *    - 在后台管理系统（程序列表 → 我的程序 → 传输配置）生成RSA密钥对
 *    - 生成器默认生成的就是1024位，无需特别设置
 *    - 私钥：保存在后台（系统自动保存）
 *    - 公钥：复制公钥内容，填写到下面的 RsaPublicKey
 * 
 * 2. AppKey配置：
 *    - 在后台管理系统中找到应用密钥（AppKey）
 *    - 将AppKey填写到下面的 AppKey
 * 
 * ✨ 第二步：程序传输配置（全局）：
 * 在后台管理系统的"传输配置"页面配置：
 * - 全局数据加密：开启
 * - 加解密类型：RSA非对称加密算法（1024位，生成器默认）
 * - 请求值加密：开启
 * - 请求值编码：Base64编码
 * - 返回值加密：开启
 * - 时间戳效验：开启
 * - 时间戳效验增强：开启
 * - 时间戳通道：系统时间
 * - 签名效验：双向签名
 * - 返回值格式：JSON
 * - JSON返回时间戳：开启
 * - JSON_code类型：int
 * 
 * 🔧 第三步：接口配置（以下所有接口都需要配置）：
 * ⚠️ 警告：下面的接口路径标识（如 C03471D9BC12F7C6）只是示例值！
 * 您需要：
 * 1. 在后台管理系统中创建对应的接口
 * 2. 获取您自己的接口路径标识
 * 3. 将路径标识填写到下面对应的配置项中
 * 
 * 每个接口都需要设置：
 * - HTTP 状态：开启
 * - 安全传输：开启
 * - 返回时间戳：开启
 * 
 * 需要配置的接口：
 * 1. 单码卡密登录 (Path_SingleLogin) - 示例：8D5F2182C3921D03
 * 2. 单码卡密登录状态查询 (Path_IsSingleLoginStatus) - 示例：BC3A9FFE68EAA45D
 * 3. 获取程序公告 (Path_GetProgramNotice) - 示例：C03471D9BC12F7C6
 * 4. 获取程序版本号 (Path_GetProgramVersionNumber) - 示例：A5CB5FC469AC850A
 * 5. 获取变量内容 (Path_GetValueContent) - 示例：A0F55102104FCA0D
 * 6. 解绑单码卡密 (Path_UnbindSingleLogin) - 示例：0C20B9CA9170F84E
 * 
 * 详细配置说明请查看：后台配置说明.md
 */

namespace T3Config {
    
    // t3网络验证配置
    static const std::string Host = "http://w.t3yanzheng.com"; // t3网络验证线路地址
    
    // ⚠️ 必须配置：在后台管理系统获取AppKey，填写到此处
    static const std::string AppKey = "380392dc5949af7f601962bc8c53bdaf"; // ⚠️ 示例值，需要替换为您自己的AppKey
    
    // ⚠️ 必须配置：在后台管理系统（程序列表 → 我的程序 → 传输配置）生成RSA密钥对
    // 生成器默认生成的就是1024位，无需特别设置
    // 私钥保存在后台，公钥填写在这里
    static const std::string RsaPublicKey = "-----BEGIN PUBLIC KEY-----\n"
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCNOhbjXy5JAssNH/Ezywo5Rj8Q\n"
"9m0A2mfqz2VNHMOUgxK+UIZFy74F1bm1037/FPZ7pQbZNJA4X7kwJ/lK8p4ZAvfR\n"
"a3Da4RjrXNNQ1AdE3uzI7xIiKlravVWkCFanNPQUehJFj593DuEKWnUMA1cj2QkR\n"
"vQcQLoNgBiDVhjhV6wIDAQAB\n"
"-----END PUBLIC KEY-----"; // ⚠️ 示例值，需要替换为您自己的公钥
    
    // API路径 - ⚠️ 警告：以下路径标识都是示例值，需要在后台创建接口后获取您自己的路径标识
    static const std::string Path_SingleLogin = "ECCE0C5BDC516F8E"; // ⚠️ 示例值，需要替换为您自己的单码卡密登录接口路径标识
    static const std::string Path_IsSingleLoginStatus = "D9BBDD2C250A3581"; // ⚠️ 示例值，需要替换为您自己的单码卡密登录状态查询接口路径标识
    static const std::string Path_GetProgramNotice = "F8A84E182029CE31"; // ⚠️ 示例值，需要替换为您自己的获取程序公告接口路径标识
    static const std::string Path_GetProgramVersionNumber = "3D8D10B85F74138B"; // ⚠️ 示例值，需要替换为您自己的获取程序版本号接口路径标识
    static const std::string Path_GetValueContent = "294198F946246099"; // ⚠️ 示例值，需要替换为您自己的获取变量内容接口路径标识
    static const std::string Path_UnbindSingleLogin = "D938D1269A223D2E"; // ⚠️ 示例值，需要替换为您自己的解绑单码卡密接口路径标识
    
    // 版本和变量配置
    static const std::string LocalVersion = "1000"; // 本地版本号
    static const std::string ValueId = "3247"; // 变量ID
    static const std::string ValueName = "nb666"; // 变量名称
    
    // 心跳配置
    static const bool EnableHeartbeat = true; // 是否开启心跳验证
    static const int HeartbeatInterval = 30; // 心跳间隔（秒）
    static const int HeartbeatMaxFail = 5; // 心跳失败最大次数
    
    // 行为配置
    static const bool ExitOnNoticeFail = false; // 获取公告失败时是否退出程序
    static const bool ExitOnVersionMismatch = true; // 版本不一致时是否退出程序
}