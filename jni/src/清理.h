


#include <cstdio>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


int 修改ID() {
    std::cout << "开始执行Shell脚本..." << std::endl;

    // 脚本内容
    std::string script = R"(
        PKG=com.tencent.tmgp.pubgmhd
        ID=$(grep $PKG /data/system/users/0/settings_ssaid.xml | awk -F'"' '{print $6}')
        for i in $(seq 16)
        do P=$P$(uuidgen|head -c 1|tr '-' -d)
        done
        sed -i s/$ID/$P/g /data/system/users/0/settings_ssaid.xml

        echo -e "\033[41m---安卓id更改成功---\033[0m"
        echo -e "\033[41m---重启后生效---\033[0m"
        echo -e "\033[41m---务必安装游戏并打开一次游戏后，安卓id修改才生效!!!---\033[0m"
        echo -e "\033[41m---修改后再次执行一次清理游戏sh并重启设备---\033[0m"
        echo -e "\033[35m 如果有异样提示，则修改不成功，建议百度或者酷安app下载爱玩机工具箱或者设备id修改器，自行修改，修改后并重启   \033[5m"
        // echo -e "\033[41m---公益频道 ---\033[0m"
    )";

    // 创建临时文件
    char tempFileName[256];
    snprintf(tempFileName, sizeof(tempFileName), "/data/local/tmp/scriptXXXXXX");
    int fd = mkstemp(tempFileName);
    if (fd == -1) {
        std::cerr << "无法创建临时文件！" << std::endl;
        return 1;
    }

    // 写入脚本内容
    write(fd, script.c_str(), script.size());
    close(fd);

    // 设置临时文件权限为可读可写可执行
    chmod(tempFileName, 0777);

    // 执行临时文件
    FILE* pipe = popen(("sh " + std::string(tempFileName)).c_str(), "r");
    if (!pipe) {
        std::cerr << "无法打开管道！" << std::endl;
        return 1;
    }

    // 读取脚本的输出
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::cout << buffer;
    }

    // 关闭管道
    pclose(pipe);

    // 删除临时文件
    remove(tempFileName);

    std::cout << "脚本执行完成！" << std::endl;

    return 0;
}




int 执行清理脚本1() {
    std::string script = R"(
        #!/bin/bash

        TARGET_DIR1="/data/data/com.tencent.tmgp.pubgmhd/"
        TARGET_DIR2="/storage/emulated/0/Android/data/com.tencent.tmgp.pubgmhd/cache/"
        TARGET_DIR3="/storage/emulated/0/Android/data/com.tencent.tmgp.pubgmhd/files/"
        TARGET_DIR4="/storage/emulated/0/Android/data/com.tencent.tmgp.pubgmhd/files/UE4Game/ShadowTrackerExtra/"
        TARGET_DIR5="/storage/emulated/0/Android/data/com.tencent.tmgp.pubgmhd/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/"
        TARGET_DIR6="/storage/emulated/0/Android/data/com.tencent.tmgp.pubgmhd/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/Paks/"
        TARGET_DIR7="/storage/emulated/0/Android/data/com.tencent.tmgp.pubgmhd/files/ProgramBinaryCache/"
        TARGET_DIR8="/storage/emulated/0/Android/data/com.tencent.tmgp.pubgmhd/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/SaveGames/"
        TARGET_DIR9="/storage/emulated/0/Android/data/com.tencent.tmgp.pubgmhd/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/Config/"
        TARGET_DIR10="/storage/emulated/0/Android/data/com.tencent.tmgp.pubgmhd/files/UE4Game/ShadowTrackerExtra/ShadowTrackerExtra/Saved/Paks/avatarpaks/"

        if [ ! -d "$TARGET_DIR1" ]; then
            echo "第一个目标目录不存在: $TARGET_DIR1"
            exit 1
        fi

        echo "开始删除 $TARGET_DIR1 下的所有子文件夹..."
        for dir in "$TARGET_DIR1"*/; do
            if [ -d "$dir" ]; then
                echo "删除文件夹: $dir"
                rm -rf "$dir"
            fi
        done

        if [ -d "$TARGET_DIR2" ]; then
            echo "删除 $TARGET_DIR2 下的cache文件夹..."
            rm -rf "$TARGET_DIR2"
        else
            echo "第二个目标目录下的cache文件夹不存在: $TARGET_DIR2"
        fi

        if [ -d "$TARGET_DIR7" ]; then
            echo "删除 $TARGET_DIR7 下的cache文件夹..."
            rm -rf "$TARGET_DIR7"
        else
            echo "第7个目标目录下的cache文件夹不存在: $TARGET_DIR7"
        fi

        if [ -d "$TARGET_DIR8" ]; then
            echo "删除 $TARGET_DIR8 下的cache文件夹..."
            rm -rf "$TARGET_DIR8"
        else
            echo "第8个目标目录下的cache文件夹不存在: $TARGET_DIR8"
        fi

        if [ -d "$TARGET_DIR9" ]; then
            echo "删除 $TARGET_DIR9 下的cache文件夹..."
            rm -rf "$TARGET_DIR9"
        else
            echo "第9个目标目录下的cache文件夹不存在: $TARGET_DIR9"
        fi

        if [ -d "$TARGET_DIR10" ]; then
            echo "删除 $TARGET_DIR10 下的cache文件夹..."
            rm -rf "$TARGET_DIR10"
        else
            echo "第10个目标目录下的cache文件夹不存在: $TARGET_DIR10"
        fi

        if [ -d "$TARGET_DIR3" ]; then
            echo "开始删除 $TARGET_DIR3 目录下除了UE4Game和ProgramBinaryCache之外的所有文件和文件夹..."
            find "$TARGET_DIR3" -mindepth 1 -maxdepth 1 ! -name 'UE4Game' ! -name 'ProgramBinaryCache' -exec rm -rf {} \;
            echo "删除完成"
        else
            echo "第三个目标目录不存在: $TARGET_DIR3"
        fi

        if [ -d "$TARGET_DIR4" ]; then
            echo "开始删除 $TARGET_DIR4 目录下除了ShadowTrackerExtra之外的所有文件夹..."
            find "$TARGET_DIR4" -mindepth 1 -maxdepth 1 ! -name '.' ! -name '..' ! -name 'ShadowTrackerExtra' -exec rm -rf {} \;
            echo "删除完成"
        else
            echo "第四个目标目录不存在: $TARGET_DIR4"
        fi

        if [ -d "$TARGET_DIR5" ]; then
            echo "开始删除 $TARGET_DIR5 目录下除了Config、Paks、SaveGames和SrcVersion.ini之外的所有文件和文件夹..."
            find "$TARGET_DIR5" -mindepth 1 -maxdepth 1 \
                ! -name 'Config' ! -name 'Paks' ! -name 'SaveGames' ! -name 'SrcVersion.ini' \
                -exec rm -rf {} \;
            echo "删除完成"
        else
            echo "第五个目标目录不存在: $TARGET_DIR5"
        fi

        if [ -d "$TARGET_DIR6" ]; then
            echo "开始删除 $TARGET_DIR6 目录下的eifsCache1、eifsCache2、eifsCache3和eifsCache5文件夹..."
            for cache in eifsCache1 eifsCache2 eifsCache3 eifsCache5; do
                if [ -d "$TARGET_DIR6/$cache" ]; then
                    echo "删除文件夹: $TARGET_DIR6/$cache"
                    rm -rf "$TARGET_DIR6/$cache"
                else
                    echo "文件夹不存在: $TARGET_DIR6/$cache"
                fi
            done
            echo "删除完成 "
        else
            echo "第六个目标目录不存在: $TARGET_DIR6"
        fi

        echo "操作完成 认准无忧 "
    )";

    char tempFileName[256];
    snprintf(tempFileName, sizeof(tempFileName), "/data/local/tmp/scriptXXXXXX");
    int fd = mkstemp(tempFileName);
    if (fd == -1) {
        std::cerr << "无法创建临时文件！" << std::endl;
        return 1;
    }

    write(fd, script.c_str(), script.size());
    close(fd);
    chmod(tempFileName, 0777);

    FILE* pipe = popen(("sh " + std::string(tempFileName)).c_str(), "r");
    if (!pipe) {
        std::cerr << "无法打开管道！" << std::endl;
        return 1;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::cout << buffer;
    }

    pclose(pipe);
    remove(tempFileName);

    return 0;
}



int 执行清理脚本2() {
    std::string script = R"(
        #!/bin/bash

        TARGET_DIR1="/data/data/com.tencent.tmgp.pubgmhd/"
        TARGET_DIR2="/storage/emulated/0/Android/data/com.tencent.tmgp.pubgmhd/"

        if [ ! -d "$TARGET_DIR1" ]; then
            echo "第一个目标目录不存在: $TARGET_DIR1"
            exit 1
        fi

        echo "开始删除 $TARGET_DIR1 下的所有子文件夹..."
        for dir in "$TARGET_DIR1"*/; do
            if [ -d "$dir" ]; then
                echo "删除文件夹: $dir"
                rm -rf "$dir"
            fi
        done

        if [ -d "$TARGET_DIR2" ]; then
            echo "删除 $TARGET_DIR2 下的cache文件夹..."
            rm -rf "$TARGET_DIR2"
        else
            echo "第二个目标目录下的cache文件夹不存在: $TARGET_DIR2"
        fi

        echo "高级清理删除操作完成 "
        echo "认准无忧 "
    )";

    char tempFileName[256];
    snprintf(tempFileName, sizeof(tempFileName), "/data/local/tmp/scriptXXXXXX");
    int fd = mkstemp(tempFileName);
    if (fd == -1) {
        std::cerr << "无法创建临时文件！" << std::endl;
        return 1;
    }

    write(fd, script.c_str(), script.size());
    close(fd);
    chmod(tempFileName, 0777);

    FILE* pipe = popen(("sh " + std::string(tempFileName)).c_str(), "r");
    if (!pipe) {
        std::cerr << "无法打开管道！" << std::endl;
        return 1;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::cout << buffer;
    }

    pclose(pipe);
    remove(tempFileName);

    return 0;
}

int 安卓15() {
    std::cout << "开始伪装Android 15..." << std::endl;

    std::string script = R"(
        #!/system/bin/sh
        resetprop ro.build.version.release 14
        resetprop ro.build.version.sdk 34
        echo -e "\033[42mAndroid版本伪装完成！当前显示为Android 14 (API 34)\033[0m"
    )";

    char tempFileName[256];
    snprintf(tempFileName, sizeof(tempFileName), "/data/local/tmp/scriptXXXXXX");
    int fd = mkstemp(tempFileName);
    if (fd == -1) {
        std::cerr << "无法创建临时文件！" << std::endl;
        return 1;
    }

    write(fd, script.c_str(), script.size());
    close(fd);
    chmod(tempFileName, 0777);

    FILE* pipe = popen(("sh " + std::string(tempFileName)).c_str(), "r");
    if (!pipe) {
        std::cerr << "无法执行伪装操作！" << std::endl;
        return 1;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::cout << buffer;
    }

    pclose(pipe);
    remove(tempFileName);

    std::cout << "Android版本伪装完成！" << std::endl;
    return 0;
}

int 解除触控卡屏() {
    std::cout << "开始解除触控卡屏限制..." << std::endl;

    std::string script = R"(
        #!/system/bin/sh
        settings put system block_untrusted_touches 0
        settings put global block_untrusted_touches 0
        settings put secure block_untrusted_touches 0
        echo -e "\033[42m触控限制已解除！\033[0m"
        echo -e "\033[33m可能需要重启生效\033[0m"
    )";

    char tempFileName[256];
    snprintf(tempFileName, sizeof(tempFileName), "/data/local/tmp/scriptXXXXXX");
    int fd = mkstemp(tempFileName);
    if (fd == -1) {
        std::cerr << "无法创建临时文件！" << std::endl;
        return 1;
    }

    write(fd, script.c_str(), script.size());
    close(fd);
    chmod(tempFileName, 0777);

    FILE* pipe = popen(("sh " + std::string(tempFileName)).c_str(), "r");
    if (!pipe) {
        std::cerr << "无法执行触控解除操作！" << std::endl;
        return 1;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::cout << buffer;
    }

    pclose(pipe);
    remove(tempFileName);

    std::cout << "触控限制解除操作完成！" << std::endl;
    return 0;
}
