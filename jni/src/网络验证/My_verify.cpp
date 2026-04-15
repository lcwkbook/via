#include <string>
#include <time.h>           //读时间获取
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/system_properties.h>
#include <chrono>
#include <thread>

#include "My_verify.h"

#include "cJSON.h"
#include "Encrypt.h"
#include "Mysshttp.h"
#include "obfuscate.h" //混淆字符串

#include <iostream>
#include <string>
#include <random>
#include <ctime>

// 随机数生成器初始化
std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));

// 生成随机字母的函数
char getRandomChar() {
    const char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::uniform_int_distribution<int> dist(0, sizeof(letters) - 2); // -2 to exclude the null terminator
    return letters[dist(rng)];
}

// 生成随机域名的函数，并返回char*
char* getRandomDomain() {
    std::string domain = "";
    int length = 8; // 假设我们想要8个随机字符
    for (int i = 0; i < length; ++i) {
        domain += getRandomChar();
    }
    domain += ".wy.llua.cn";
    return strdup(domain.c_str()); // 使用strdup复制字符串到新的内存区域
}

// 使用AY_OBFUSCATE宏来调用getRandomDomain函数
#define AY_OBFUSCATE(domain) (char*)(domain)

// 在实际使用中，你可能需要根据程序的逻辑来决定如何组织这些代码。

            // 禁用特定的警告
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"

long *IsLoGin_2 = NULL;
static time_t Maturity_time = 0;
static const char *OwnVersion = (const char *)AY_OBFUSCATE("2.6");


char *my_shell(const char *command) {
	FILE *pipe = NULL;
	char line[256] = { };
    char *result = (char *) calloc(2048, sizeof(char));
	memset(result, 0, 2048);
	pipe = popen(command, (const char *)AY_OBFUSCATE("r"));
    if (!pipe) {
        return {};
    }
    while (!feof(pipe)) {
        if (fgets(line, sizeof(line), pipe) != NULL) {
    		strncat(result, line, strlen(line));
        } else {
        
        }
    }
	pclose(pipe);
	return result;
}

bool loadConfig_ver(char *cfgPath, void *data, size_t size) {
    int fd = open(cfgPath, O_RDONLY);
   	if (fd > 0) {
   		read(fd, data, size);
   		close(fd);
    	return true;
    } else {
   	    return false;
    }
}
bool saveConfig_ver(char *cfgPath, void *data, size_t size) {
    int fd = open(cfgPath, O_TRUNC | O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd > 0) {
    	write(fd, data, size);	// 写入文本 // close(fd); 
    	close(fd);
   		return true;
   	} else {
 	    return false;
    }
}     

char *getAndroid_boot_serialno() {
    char *prop_value = (char *) calloc(256, sizeof(char));
    ::__system_property_get((const char *)AY_OBFUSCATE("ro.boot.serialno"), prop_value);
    return prop_value;
}
char* getMacAddresses(const char *str_name) {
    char *macAddress;
    struct dirent *entry;
    DIR *dir = opendir((const char *)AY_OBFUSCATE("/sys/class/net"));
    if (dir == NULL) {
        perror((const char *)AY_OBFUSCATE("opendir"));
        return (char *)"";
    }
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, str_name, strlen(str_name)) == 0) {
            char macFile[128] = {0};
            snprintf(macFile, sizeof(macFile), (const char *)AY_OBFUSCATE("/sys/class/net/%s/address"), entry->d_name);
            FILE *file = fopen(macFile, (const char *)AY_OBFUSCATE("r"));
            if (file) {
                char local_str[64] = {0};
                macAddress = (char *)calloc(64, sizeof(char));
                fgets(local_str, 64, file);
                fclose(file);
                snprintf(macAddress, strlen(local_str), (const char *)AY_OBFUSCATE("%s"), local_str);
            }
        }
    }
    closedir(dir);
    return macAddress;
}


int wbfd3408c915617b54992c2adf5c9127a(all_tinyCheck *all_check, int *yes, char *ret_err) {
    char* host = AY_OBFUSCATE(getRandomDomain());
    //std::cout << "云随机分配成功: " << host << std::endl;
	char *APPID = (char *) & all_check->APPID;
	char *APPKEY = (char *) & all_check->APPKEY;
	char *RC4KEY = (char *) & all_check->RC4KEY;
    char *imei = (char *) & all_check->imei_;
    char *km = (char *) & all_check->key_;

	//printf("卡密： %s\n设备码： %s\n\n", km, imei);
	if (strlen(km) == 0 || strlen(imei) == 0) {
        sprintf(ret_err, (const char *)AY_OBFUSCATE("无设备码或者卡密!"));   
    	return 0;
	}
	
	time_t t; //时间戳
	t = time(NULL);
	int ii = time(&t);
    srand(time(NULL));
	// 合并数据
	char local_value[256];
	char local_sign[512];
	char local_data[768];
	sprintf(local_value, (const char *)AY_OBFUSCATE("%d%d"), ii, ::rand());
	sprintf(local_sign, (const char *)AY_OBFUSCATE("kami=%s&markcode=%s&t=%d&%s"), km, imei, ii, APPKEY);

	MD5_CTX md5c;
	MD5Init(&md5c);
	unsigned char decrypt[16];
	MD5Update(&md5c, (unsigned char *)&local_sign, strlen(local_sign));
	MD5Final(&md5c, decrypt);
	char lkey[32+1] = { 0 };
	for (int i = 0; i < 16; i++) {
		sprintf(&lkey[i * 2], (const char *)AY_OBFUSCATE("%02x"), decrypt[i]);
	}

	// RC4加密
	sprintf(local_data, (const char *)AY_OBFUSCATE("kami=%s&markcode=%s&t=%d&sign=%s&value=%s"), km, imei, ii, lkey, local_value);
    char *dataa = Encrypt(local_data, RC4KEY);
	// 合并数据
	char cs[1024];
	sprintf(cs, (const char *)AY_OBFUSCATE("&data=%s"), dataa);
	free(dataa);
	dataa = NULL;
	
	char url[1024];
	sprintf(url, (const char *)AY_OBFUSCATE("api/?id=kmlogon&app=%s"), APPID);	
	// 提交数据
	char *tijiao = httppost(host, url, cs);
	if (tijiao == NULL) {
        sprintf(ret_err, (const char *)AY_OBFUSCATE("err_登录_请求失败!"));
        return 0;	 	
	}

	// RC4解密
	char* tijiaoo = Decrypt(tijiao, RC4KEY);	
    free(tijiao);
    tijiao = NULL;
	if (tijiaoo == NULL || strlen(tijiaoo) == 0) {
        sprintf(ret_err, (const char *)AY_OBFUSCATE("err_登录_RC4失败!"));
        return 0;	 		
	}

	//解析JSON
	cJSON *cjson = cJSON_Parse(tijiaoo);	
    free(tijiaoo);
    tijiaoo = NULL;

	
	// 读取状态码
	int code = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("code"))->valueint;
	// 服务器时间
	int time = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("time"))->valueint;
	// 错误信息
	char *msg = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("msg"))->valuestring;	
	// 登录校验
	char *check = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("check"))->valuestring;

	if (all_check->Login_code == code) {//code判断	
		cJSON *msgdata = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("msg"));
		long vip = cJSON_GetObjectItem(msgdata, (const char *)AY_OBFUSCATE("vip"))->valuedouble;
		char weijy[512];
		sprintf(weijy, (const char *)AY_OBFUSCATE("%d%s%s"), time, APPKEY, local_value);
		// md5验证签名
		MD5_CTX md5c;
		MD5Init(&md5c);
		unsigned char decrypt[16];
		MD5Update(&md5c, (unsigned char *)&weijy, strlen(weijy));
		MD5Final(&md5c, decrypt);
		char ykey[32+1] = { 0 };
		for (int i = 0; i < 16; i++) {
			sprintf(&ykey[i * 2], (const char *)AY_OBFUSCATE("%02x"), decrypt[i]);
		}
		// md5验证签名
		if (strncmp(check, ykey, strlen(ykey)) == 0) {
			//printf("登录成功\n");
            if (IsLoGin_2 == NULL) {
                ::IsLoGin_2 = (long *) calloc(1, sizeof(long));
            }
            *IsLoGin_2 = 1330000009;
			if (vip) {
				::Maturity_time = vip;
				char buffer[128];
                struct tm *info = ::localtime(&vip);
                ::strftime(buffer, sizeof(buffer), (const char *)AY_OBFUSCATE("%Y-%m-%d %H:%M:%S"), info);
                sprintf(ret_err, (const char *)AY_OBFUSCATE("%s"), buffer);   
			} else {
                sprintf(ret_err, (const char *)AY_OBFUSCATE("免费模式?"));   
			}
			*yes = 10086;
		} else {
            cJSON_Delete(cjson);
            sprintf(ret_err, (const char *)AY_OBFUSCATE("校验失败!"));   
          	return 0;
 		}
	} else {
        cJSON_Delete(cjson);
        sprintf(ret_err, (const char *)AY_OBFUSCATE("%s"), msg);   
     	return 0;
	}

    cJSON_Delete(cjson);
    free(host);
	return 1;
}

//解绑
void bbsjs4646ekkdbdbdbdb(all_tinyCheck *all_check, char *ret_err) {
    char* host = AY_OBFUSCATE(getRandomDomain());
    //std::cout << "云随机分配成功: " << host << std::endl;
	char *APPID = (char *) & all_check->APPID;
	char *APPKEY = (char *) & all_check->APPKEY;
	char *RC4KEY = (char *) & all_check->RC4KEY;
    char *imei = (char *) & all_check->imei_;
    char *kami = (char *) & all_check->key_;
    time_t t;
    t = time(NULL);
    int ii = time(&t);

    // 合并数据
    char local_value[256];
    char local_sign[512];
    char local_data[512];
    sprintf(local_value, (const char *)AY_OBFUSCATE("%d"), ii);
    sprintf(local_sign, (const char *)AY_OBFUSCATE("kami=%s&markcode=%s&t=%d&%s"), kami, imei, ii, APPKEY);

    MD5_CTX md5c;
    MD5Init(&md5c);
    unsigned char decrypt[16];
    MD5Update(&md5c, (unsigned char *)&local_sign, strlen(local_sign));
    MD5Final(&md5c, decrypt);
    char lkey[32+1] = { 0 };
    for (int i = 0; i < 16; i++) {
        sprintf(&lkey[i * 2], (const char *)AY_OBFUSCATE("%02x"), decrypt[i]);
    }
    char weiyan[512];
    sprintf(weiyan, (const char *)AY_OBFUSCATE("%d%s%s"), ii, APPKEY, local_value);
    sprintf(local_data, (const char *)AY_OBFUSCATE("kami=%s&markcode=%s&t=%d&sign=%s&value=%s"), kami, imei, ii, lkey, local_value);
    char *new_local_data = Encrypt(local_data, RC4KEY);
    char cs[1024];
    sprintf(cs, (const char *)AY_OBFUSCATE("&data=%s"), new_local_data);
    free(new_local_data);
    new_local_data = NULL;
    char url[1024];
    sprintf(url, (const char *)AY_OBFUSCATE("api/?id=kmdismiss&app=%s"), APPID);
     
    char *tijiao = httppost(host, url, cs);
	if (tijiao == NULL) {
        sprintf(ret_err, (const char *)AY_OBFUSCATE("err_解绑_请求失败!"));
        return;	 	
	}
    //RC4操作
    char *tijiaoo = Decrypt(tijiao, RC4KEY);
    free(tijiao);
    tijiao = NULL;
	if (tijiaoo == NULL || strlen(tijiaoo) == 0) {
        sprintf(ret_err, (const char *)AY_OBFUSCATE("err_解绑_RC4失败!"));
        return;	 		
	}

    cJSON *cjson = cJSON_Parse(tijiaoo);
    free(tijiaoo);
    tijiaoo = NULL;

    // 读取状态码
    int code = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("code"))->valueint;
    char *msg = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("msg"))->valuestring;
    if (code == 84565) { // 200是你的状态码即是code
        sprintf(ret_err, (const char *)AY_OBFUSCATE("解绑成功"));   
    } else {
        sprintf(ret_err, (const char *)AY_OBFUSCATE("%s"), msg);   
    }
    free(host);
    cJSON_Delete(cjson);
}

//公告
int get_Announcement_data(all_tinyCheck *all_check, char *ret_str) {
    char* host = AY_OBFUSCATE(getRandomDomain());
    std::cout << "云随机分配成功: " << host << std::endl;
	char *APPID = (char *) & all_check->APPID;
	char *APPKEY = (char *) & all_check->APPKEY;
	char *RC4KEY = (char *) & all_check->RC4KEY;


    char *app_gg = NULL;
    char dmdlData[128];
    char local_data[512];
    char local_url1[1024];
    time_t t;
    t = time(NULL);
    int ii = time(&t);
    sprintf(dmdlData, (const char *)AY_OBFUSCATE("api/?id=notice&app=%s"), APPID);
    sprintf(local_data, (const char *)AY_OBFUSCATE("%d%s"), ii, APPKEY);
    char* adga1 = Encrypt(local_data, RC4KEY);
    sprintf(local_url1, (const char *)AY_OBFUSCATE("&data=%s"), adga1);
    free(adga1);
    adga1 = NULL;    
    char *ztm1 = httppost(host, (char *)&dmdlData, (char *)&local_url1);
	if (ztm1 == NULL) {
        sprintf(ret_str, (const char *)AY_OBFUSCATE("err_公告_请求失败!"));
        return 0;	 	
	}

    char* abcdstr = Decrypt(ztm1, RC4KEY);
    free(ztm1);
    ztm1 = NULL;
	if (abcdstr == NULL || strlen(abcdstr) == 0) {
        sprintf(ret_str, (const char *)AY_OBFUSCATE("err_公告_RC4失败!"));
        return 0;	 		
	}

    cJSON *cjson = cJSON_Parse(abcdstr);
    free(abcdstr);
    abcdstr = NULL;
    
    if (cjson == NULL) {
        sprintf(ret_str, (const char *)AY_OBFUSCATE("无法解析域名"));   
        return 0;        
    }
    int code = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("code"))->valueint;
    if (code == 84565) { // 200是你的状态码即是code
        char strDay[1024] = {0};
        cJSON *pValue = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("msg")); // 解析value字段内容
        if (!pValue) {  // 判断value字段是否json格式
            cJSON_Delete(cjson);
            return 0;
        } else {
            cJSON *pDay = cJSON_GetObjectItem(pValue, (const char *)AY_OBFUSCATE("app_gg"));  // 解析子节点pValue的day字段字符串内容
            if (!pDay) { // 判断day字段是否json格式
                cJSON_Delete(cjson);
                return 0;
            } else {
                if (cJSON_IsString(pDay)) {// 判断day字段是否string类型
                   
   app_gg = strcpy(strDay, pDay->valuestring);
   
   // 改为（二选一）：

                //    strcpy(strDay, pDay->valuestring); 
                    // 只复制不赋值
   // 或者
   app_gg = strDay;  // 如果需要赋值目标指针
                }
            }
        }
    }

    sprintf(ret_str, (const char *)AY_OBFUSCATE("%s"), app_gg);   
    cJSON_Delete(cjson);
    free(host);
    return 1;

}

int get_app_version(all_tinyCheck *all_check, Version_information *ret_data) {
    char* host = AY_OBFUSCATE(getRandomDomain());
    //std::cout << "云随机分配成功: " << host << std::endl;
	char *APPID = (char *) & all_check->APPID;
	char *APPKEY = (char *) & all_check->APPKEY;
	char *RC4KEY = (char *) & all_check->RC4KEY;
    

    char url[256];
    sprintf(url, (const char *)AY_OBFUSCATE("api/?id=ini&app=%s"), APPID);
    char *ztm = httppost(host, url, (char *)AY_OBFUSCATE(""));
	if (ztm == NULL) {
        sprintf(ret_data->Latest_version, (const char *)AY_OBFUSCATE("err_版本_请求失败!"));
        return 0;	 	
	}

    char* abcdstr = Decrypt(ztm, RC4KEY);
    free(ztm);
    ztm = NULL;
	if (abcdstr == NULL || strlen(abcdstr) == 0) {
        sprintf(ret_data->Latest_version, (const char *)AY_OBFUSCATE("err_版本_RC4失败!"));
        return 0;	 		
	}

    cJSON *cjson = cJSON_Parse(abcdstr);
    free(abcdstr);
    abcdstr = NULL;
    if (cjson == NULL) {
        sprintf(ret_data->Latest_version, (const char *)AY_OBFUSCATE("err_版本_js失败!"));
        return 0;	 		    
    }
    int code = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("code"))->valueint;
    cJSON *json = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("msg"));
    char *local_version = cJSON_GetObjectItem(json, (const char *)AY_OBFUSCATE("version"))->valuestring;
    char *local_update_link = cJSON_GetObjectItem(json, (const char *)AY_OBFUSCATE("app_update_url"))->valuestring;
    char *temp_是否强制 = cJSON_GetObjectItem(json, (const char *)AY_OBFUSCATE("app_update_must"))->valuestring;
    char *temp_Updata = cJSON_GetObjectItem(json, (const char *)AY_OBFUSCATE("app_update_show"))->valuestring;
    bool local_is_force_update;
    if (strcmp(temp_是否强制, (const char *)AY_OBFUSCATE("y")) == 0 && strcmp(local_version, OwnVersion) != 0) {
        local_is_force_update = true;
    } else {
        local_is_force_update = false;
    }

	snprintf(ret_data->Latest_version, sizeof(ret_data->Latest_version), (const char *)AY_OBFUSCATE("%s"), local_version);
	snprintf(ret_data->Latest_version_link, sizeof(ret_data->Latest_version_link), (const char *)AY_OBFUSCATE("%s"), local_update_link);
	snprintf(ret_data->Update_Content, sizeof(ret_data->Update_Content), (const char *)AY_OBFUSCATE("%s"), temp_Updata);
    ret_data->is_tforce_update = local_is_force_update;

    cJSON_Delete(cjson);
    free(host);
    return 1;      
}



//心跳
void *Circular_authentication(void *arg) {
    int fault_tolerant = 0;
    int *local_successful_operation = (int *)arg;
    all_tinyCheck *data_rt = (all_tinyCheck *) calloc(1, sizeof(all_tinyCheck));

    while (1) {
        if (*local_successful_operation == -1) {
            std::this_thread::sleep_for(std::chrono::seconds(4)); //这个你不用管
            continue;        
        }        

        if (fault_tolerant >= 2) {
            *IsLoGin_2 = -678;
            printf((const char *)AY_OBFUSCATE("err_心跳不通过\n"));
            printf((const char *)AY_OBFUSCATE("err_exit__\n"));
            if (getuid() == 0) {
                //system((const char *)AY_OBFUSCATE("am force-stop com.uapp.baiyueguang"));   
            }
            exit(-1);                                    
        }
		
        //std::this_thread::sleep_for(std::chrono::seconds(5));     //五秒
        std::this_thread::sleep_for(std::chrono::seconds(60 * 3));// 等待3分钟
        data_rt->Login_code = 11617;
        snprintf(data_rt->APPID, sizeof(data_rt->APPID), (char *)AY_OBFUSCATE("%d"), 70506);
        snprintf(data_rt->APPKEY, sizeof(data_rt->APPKEY), (char *)AY_OBFUSCATE("%s"), (char *)AY_OBFUSCATE("zNAugVlGw9gsZqM9"));
        snprintf(data_rt->RC4KEY, sizeof(data_rt->RC4KEY), (char *)AY_OBFUSCATE("%s"), (char *)AY_OBFUSCATE("pd2SdbMMR6MtBZA"));	
        snprintf(data_rt->imei_, sizeof(data_rt->imei_), (const char *)AY_OBFUSCATE("%s"), get_all_tinyCheckData()->imei_);
        snprintf(data_rt->key_, sizeof(data_rt->key_), (const char *)AY_OBFUSCATE("%s"), get_all_tinyCheckData()->key_);

        char* host = AY_OBFUSCATE(getRandomDomain());
    //std::cout << "云随机分配成功: " << host << std::endl;
     	char *APPID = (char *) & data_rt->APPID;
     	char *APPKEY = (char *) & data_rt->APPKEY;
     	char *RC4KEY = (char *) & data_rt->RC4KEY;
        char *imei = (char *) & data_rt->imei_;
        char *km = (char *) & data_rt->key_;

       	if (strlen(km) == 0 || strlen(imei) == 0) {
            //sprintf(ret_err, (const char *)AY_OBFUSCATE("无设备码或者卡密!"));   
     	    fault_tolerant++;
     	    continue;        
     	}
	
    	time_t t = time(NULL);
     	int ii = time(&t);
        srand(time(NULL));
    	// 合并数据
     	char local_value[256];
     	char local_sign[512];
     	char local_data[768];
      	sprintf(local_value, (const char *)AY_OBFUSCATE("%d%d"), ii, ::rand());
     	sprintf(local_sign, (const char *)AY_OBFUSCATE("kami=%s&markcode=%s&t=%d&%s"), km, imei, ii, APPKEY);

     	MD5_CTX md5c;
     	MD5Init(&md5c);
    	unsigned char decrypt[16];
     	MD5Update(&md5c, (unsigned char *)&local_sign, strlen(local_sign));
     	MD5Final(&md5c, decrypt);
        char lkey[32+1] = { 0 };
      	for (int i = 0; i < 16; i++) {
	     	sprintf(&lkey[i * 2], (const char *)AY_OBFUSCATE("%02x"), decrypt[i]);
     	}

     	// RC4加密
     	sprintf(local_data, (const char *)AY_OBFUSCATE("kami=%s&markcode=%s&t=%d&sign=%s&value=%s"), km, imei, ii, lkey, local_value);
        char *dataa = Encrypt(local_data, RC4KEY);
     	// 合并数据
    	char cs[1024];
     	sprintf(cs, (const char *)AY_OBFUSCATE("&data=%s"), dataa);
     	free(dataa);
     	dataa = NULL;
	
     	char url[1024];
     	sprintf(url, (const char *)AY_OBFUSCATE("api/?id=kmlogon&app=%s"), APPID);	
      	// 提交数据
     	char *tijiao = httppost(host, url, cs);
     	if (tijiao == NULL) {
            //sprintf(ret_err, (const char *)AY_OBFUSCATE("err_登录_请求失败!"));
     	    fault_tolerant++;
     	    continue;            
    	}

     	// RC4解密
     	char* tijiaoo = Decrypt(tijiao, RC4KEY);	
        free(tijiao);
        tijiao = NULL;
     	if (tijiaoo == NULL || strlen(tijiaoo) == 0) {
            //sprintf(ret_err, (const char *)AY_OBFUSCATE("err_登录_RC4失败!"));
     	    fault_tolerant++;
     	    continue;            
     	}

     	//解析JSON
       	cJSON *cjson = cJSON_Parse(tijiaoo);	
        free(tijiaoo);
        tijiaoo = NULL;
        if (cjson == NULL) {
     	    fault_tolerant++;
     	    continue;            
        }
	
    	// 读取状态码
     	int code = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("code"))->valueint;
     	// 服务器时间
     	int time = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("time"))->valueint;
     	// 错误信息
     	char *msg = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("msg"))->valuestring;	
     	// 登录校验
     	char *check = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("check"))->valuestring;

     	if (data_rt->Login_code == code) {//code判断	
    		cJSON *msgdata = cJSON_GetObjectItem(cjson, (const char *)AY_OBFUSCATE("msg"));
    		long vip = cJSON_GetObjectItem(msgdata, (const char *)AY_OBFUSCATE("vip"))->valuedouble;
     		char weijy[512];
     		sprintf(weijy, (const char *)AY_OBFUSCATE("%d%s%s"), time, APPKEY, local_value);
    		// md5验证签名
      		MD5_CTX md5c;
     		MD5Init(&md5c);
     		unsigned char decrypt[16];
     		MD5Update(&md5c, (unsigned char *)&weijy, strlen(weijy));
     		MD5Final(&md5c, decrypt);
     		char ykey[32+1] = { 0 };
     		for (int i = 0; i < 16; i++) {
     			sprintf(&ykey[i * 2], (const char *)AY_OBFUSCATE("%02x"), decrypt[i]);
    		}
     		// md5验证签名
    		if (strncmp(check, ykey, strlen(ykey)) == 0) {
                fault_tolerant = 0;   
     			//printf("心跳通过\n");                    
		    } else {
                cJSON_Delete(cjson);
                //sprintf(ret_err, (const char *)AY_OBFUSCATE("校验失败!"));   
     	        fault_tolerant++;
          	    continue;            
      		}
     	} else {
            cJSON_Delete(cjson);
            //sprintf(ret_err, (const char *)AY_OBFUSCATE("%s"), msg);   
     	    fault_tolerant++;
     	    continue;            
	    }
        cJSON_Delete(cjson);
        
    }
	return NULL;
}

void 微验验证(int *yes, char *str_err) {
    verify_Start:
    
    *yes = 0; 
    char *android_id = NULL;
    char Kami[128] = {0};   
    char *key_path = (char *)((const char *)AY_OBFUSCATE("/storage/emulated/0/Monarch卡密"));   
    loadConfig_ver(key_path, (void *)Kami, sizeof(Kami));
    if (strlen(Kami) == 0) {
    		printf("\033[0;1m");
	//	printf("- Please enter the card password.:");
        printf((const char *)AY_OBFUSCATE("[+] 请输入卡密：\n"));
        scanf((const char *)AY_OBFUSCATE("%s"), Kami);  // 从标准输入流中读取字符串    
        if (strlen(Kami) == 0) {
            printf((const char *)AY_OBFUSCATE("什么都没输入 exit\n"));
            goto verify_Start;
    		//exit(1); 	
        } 
    }


    if (android_id == NULL || strlen(android_id) == 0) {
        char *local_ret = my_shell((const char *)AY_OBFUSCATE("settings get secure android_id"));
        if (strlen(local_ret) > 4) {
            local_ret[strlen(local_ret)-1] = '\0'; //去掉换行
            android_id = local_ret;
            //printf("android_id : %s (%d)\n", android_id, strlen(local_ret));
        } else {
            free(local_ret);
            local_ret = NULL;
        }
    }
    if (android_id == NULL || strlen(android_id) == 0) {
        android_id = getAndroid_boot_serialno();
    }    
    /*
    if (android_id == NULL || strlen(android_id) == 0) {
        android_id = getMacAddresses("wlan1");
    }
    if (android_id == NULL || strlen(android_id) == 0) {
        android_id = getMacAddresses("p2p0");
    }
    if (android_id == NULL || strlen(android_id) == 0) {
        android_id = getMacAddresses("wifi-aware0");
    }
    if (android_id == NULL || strlen(android_id) == 0) {
        android_id = getMacAddresses("bond0");
    }
    if (android_id == NULL || strlen(android_id) == 0) {
        android_id = getMacAddresses("wlan0");
    }
    */ 
    if (android_id == NULL || strlen(android_id) == 0) {
        printf((const char *)AY_OBFUSCATE("设备码获取错误\n"));
        exit(0);    
    }   


    all_tinyCheck *local_CheckData = get_all_tinyCheckData();
    snprintf(local_CheckData->imei_, sizeof(local_CheckData->imei_), (const char *)AY_OBFUSCATE("%s"), android_id);
    snprintf(local_CheckData->key_, sizeof(local_CheckData->key_), (const char *)AY_OBFUSCATE("%s"), Kami);
    int ret_it = ::wbfd3408c915617b54992c2adf5c9127a(local_CheckData, (int *)yes, (char *)str_err);
    if (*yes == 10086 && ret_it == 1) { //成功
        saveConfig_ver(key_path, (void *)Kami, strlen(Kami));
    } else { //不成功
     	remove(key_path);
        printf((const char *)AY_OBFUSCATE("登陆状态 : %s\n"), str_err);   
        goto verify_Start;
        //exit(0);    
    }
}    

time_t get_Maturity_time() {
    return Maturity_time;
}

all_tinyCheck* get_all_tinyCheckData() {
	static all_tinyCheck *local_CheckData = (all_tinyCheck *) calloc(1, sizeof(all_tinyCheck));
	if (local_CheckData->Login_code == 0) {
	/*    local_CheckData->Login_code = 615;
    snprintf(local_CheckData->APPID, sizeof(local_CheckData->APPID), (const char *)AY_OBFUSCATE("%d"), 69446);
    snprintf(local_CheckData->APPKEY, sizeof(local_CheckData->APPKEY), (const char *)AY_OBFUSCATE("%s"), (char *)AY_OBFUSCATE("j2dQJcVoayTFHYf"));
    snprintf(local_CheckData->RC4KEY, sizeof(local_CheckData->RC4KEY), (const char *)AY_OBFUSCATE("%s"), (char *)AY_OBFUSCATE("GDmEk1gQ21MPteJ"));*/
    	local_CheckData->Login_code = 11617;
     	snprintf(local_CheckData->APPID, sizeof(local_CheckData->APPID), "%d", 70506);
     	snprintf(local_CheckData->APPKEY, sizeof(local_CheckData->APPKEY), "%s", "zNAugVlGw9gsZqM9");
    	snprintf(local_CheckData->RC4KEY, sizeof(local_CheckData->RC4KEY), "%s", "pd2SdbMMR6MtBZA");	
    }
    return local_CheckData;
}


            // 恢复之前的警告设置
#pragma GCC diagnostic pop
