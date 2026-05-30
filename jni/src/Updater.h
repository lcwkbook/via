#ifndef UPDATER_H
#define UPDATER_H

#include <string>

/**
 * @brief 检查并执行自动更新
 * @param currentVersion 当前程序版本（例如 "1.36.4.68"）
 * @param serverVersion  服务端最新版本
 * @param downloadUrl    新版本下载直链
 * @param mustUpdate     是否强制更新 ("y" 表示强制)
 */
void StartUpdate(const std::string &currentVersion,
                 const std::string &serverVersion,
                 const std::string &downloadUrl,
                 const std::string &mustUpdate);

#endif