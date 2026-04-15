#pragma once
#include "T3Data.h"
#include "Colors.h"
#include <thread>
#include <atomic>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <sys/system_properties.h>

class T3Api {
private:
    std::string card_kami;
    std::string machine_code;
    std::string heartbeat_code;
    std::string expire_time;
    bool is_logged_in;
    
    std::atomic<bool> heartbeat_running;
    std::thread heartbeat_thread;
    int heartbeat_interval;
    int heartbeat_fail_count;
    int heartbeat_max_fail;
    
public:
    T3Api() 
        : is_logged_in(false),
          heartbeat_running(false),
          heartbeat_interval(30),
          heartbeat_fail_count(0),
          heartbeat_max_fail(5) {
    }
    
    ~T3Api() {
        stopHeartbeat();
    }
    
    // 1. 单码登录
    bool singleLogin(const std::string& kami, const std::string& imei, std::string* errorMsg = nullptr) {
        card_kami = kami;
        machine_code = imei;
        
        T3DATA cardLoginApi;
        cardLoginApi.setRequestApi(T3Config::Path_SingleLogin);
        cardLoginApi.addRequestParam("kami", card_kami);
        cardLoginApi.addRequestParam("imei", machine_code);
        
        if (!cardLoginApi.sendRequest()) {
            if (errorMsg) *errorMsg = "网络请求失败";
            return false;
        }
        
        try {
            Json::Value responseJson = cardLoginApi.getResponseJsonObject();
            
            if (responseJson["code"].asInt() != 200) {
                if (errorMsg) *errorMsg = responseJson["msg"].asString();
                return false;
            }
            
            if (!cardLoginApi.requestDataSignatureVerify()) {
                if (errorMsg) *errorMsg = "数据验签失败";
                return false;
            }
            
            if (!cardLoginApi.requestSafeCodeVerify()) {
                if (errorMsg) *errorMsg = "数据被劫持篡改";
                return false;
            }
            
            if (!cardLoginApi.requestDataTimeDifferenceVerify()) {
                if (errorMsg) *errorMsg = "时间戳验证失败";
                return false;
            }
            
            expire_time = responseJson["end_time"].asString();
            heartbeat_code = responseJson["statecode"].asString();
            is_logged_in = true;
            
            return true;
        }
        catch (const std::exception& ex) {
            if (errorMsg) *errorMsg = std::string("异常: ") + ex.what();
            return false;
        }
    }
    
    // 2. 获取公告
    std::string getNotice() {
        T3DATA api;
        api.setRequestApi(T3Config::Path_GetProgramNotice);
        
        if (!api.sendRequest()) return "";
        
        try {
            Json::Value json = api.getResponseJsonObject();
            if (json["code"].asInt() != 200) return "";
            if (!api.requestSafeCodeVerify() || !api.requestDataTimeDifferenceVerify()) return "";
            return json["msg"].asString();
        }
        catch (const std::exception&) {
            return "";
        }
    }
    
    // 3. 获取到期时间
    std::string getExpireTime() {
        return is_logged_in ? expire_time : "";
    }
    
    // 4. 获取服务器版本
    std::string getServerVersion() {
        T3DATA api;
        api.setRequestApi(T3Config::Path_GetProgramVersionNumber);
        
        if (!api.sendRequest()) return "";
        
        try {
            Json::Value json = api.getResponseJsonObject();
            if (json["code"].asInt() != 200) return "";
            if (!api.requestSafeCodeVerify() || !api.requestDataTimeDifferenceVerify()) return "";
            return json["msg"].asString();
        }
        catch (const std::exception&) {
            return "";
        }
    }
    
    // 5. 版本检查
    bool checkVersion(const std::string& localVersion) {
        std::string serverVersion = getServerVersion();
        if (serverVersion.empty()) return false;
        return localVersion >= serverVersion;
    }
    
    // 6. 获取变量内容
    std::string getValueContent(const std::string& kami, const std::string& valueId, const std::string& valueName) {
        T3DATA api;
        api.setRequestApi(T3Config::Path_GetValueContent);
        api.addRequestParam("kami", kami);
        api.addRequestParam("valueid", valueId);
        api.addRequestParam("valuename", valueName);
        
        if (!api.sendRequest()) return "";
        
        try {
            Json::Value json = api.getResponseJsonObject();
            if (json["code"].asInt() != 200) return "";
            if (!api.requestSafeCodeVerify() || !api.requestDataTimeDifferenceVerify()) return "";
            return json["msg"].asString();
        }
        catch (const std::exception&) {
            return "";
        }
    }
    
    // 7. 开启心跳验证
    void startHeartbeat(int interval = 30, int maxFailCount = 5) {
        if (!is_logged_in || heartbeat_running) return;
        
        heartbeat_interval = interval;
        heartbeat_max_fail = maxFailCount;
        heartbeat_fail_count = 0;
        heartbeat_running = true;
        heartbeat_thread = std::thread(&T3Api::heartbeatLoop, this);
        
        printSuccess("心跳验证已启动");
    }
    
    // 停止心跳
    void stopHeartbeat() {
        if (heartbeat_running) {
            heartbeat_running = false;
            if (heartbeat_thread.joinable()) {
                heartbeat_thread.join();
            }
        }
    }
    
    // 8. 解绑设备
    bool unbindDevice(const std::string& kami) {
        T3DATA api;
        api.setRequestApi(T3Config::Path_UnbindSingleLogin);
        api.addRequestParam("kami", kami);
        
        if (!api.sendRequest()) return false;
        
        try {
            Json::Value json = api.getResponseJsonObject();
            if (json["code"].asInt() != 200) return false;
            if (!api.requestSafeCodeVerify() || !api.requestDataTimeDifferenceVerify()) return false;
            
            is_logged_in = false;
            heartbeat_code.clear();
            expire_time.clear();
            
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }
    
    // 获取设备码
    static std::string getDeviceCode() {
        std::string deviceCode;
        char value[PROP_VALUE_MAX] = {0};
        
        if (__system_property_get("ro.serialno", value) > 0 && strlen(value) > 0) {
            deviceCode = value;
        }
        else if (__system_property_get("ro.boot.serialno", value) > 0 && strlen(value) > 0) {
            deviceCode = value;
        }
        else if (__system_property_get("ro.build.id", value) > 0 && strlen(value) > 0) {
            deviceCode = value;
        }
        else {
            std::stringstream ss;
            if (__system_property_get("ro.product.model", value) > 0) {
                ss << value << "_";
            }
            if (__system_property_get("ro.product.brand", value) > 0) {
                ss << value << "_";
            }
            if (__system_property_get("ro.build.fingerprint", value) > 0) {
                ss << value;
            }
            deviceCode = ss.str();
            if (deviceCode.empty()) {
                deviceCode = "UNKNOWN_DEVICE";
            }
        }
        
        deviceCode.erase(std::remove_if(deviceCode.begin(), deviceCode.end(), ::isspace), deviceCode.end());
        return deviceCode;
    }
    
private:
    // 心跳验证
    bool doHeartbeat() {
        T3DATA api;
        api.setRequestApi(T3Config::Path_IsSingleLoginStatus);
        api.addRequestParam("kami", card_kami);
        api.addRequestParam("statecode", heartbeat_code);
        
        if (!api.sendRequest()) return false;
        
        try {
            Json::Value json = api.getResponseJsonObject();
            if (json["code"].asInt() != 200) return false;
            if (!api.requestSafeCodeVerify() || !api.requestDataTimeDifferenceVerify()) return false;
            
            std::string msg = json["msg"].asString();
            return (msg == "心跳验证成功" || msg.rfind("心跳验证成功:", 0) == 0);
        }
        catch (const std::exception&) {
            return false;
        }
    }
    
    // 心跳线程
    void heartbeatLoop() {
        while (heartbeat_running) {
            bool success = doHeartbeat();
            
            if (success) {
                heartbeat_fail_count = 0;
                std::cout << Colors::GREEN << "  " << Colors::ICON_SUCCESS << " 心跳验证通过" << Colors::RESET << std::endl;
            } else {
                heartbeat_fail_count++;
                std::cout << Colors::YELLOW << "  " << Colors::ICON_WARNING << " 心跳失败 " << heartbeat_fail_count << "/" << heartbeat_max_fail << Colors::RESET << std::endl;
                
                if (heartbeat_fail_count >= heartbeat_max_fail) {
                    std::cout << Colors::RED << "  " << Colors::ICON_ERROR << " 心跳失败次数过多，程序退出" << Colors::RESET << std::endl;
                    exit(1);
                }
            }
            
            for (int i = 0; i < heartbeat_interval && heartbeat_running; i++) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
};
