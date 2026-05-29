#ifndef UPDATER_H
#define UPDATER_H

#include <string>

/**
 * @brief 检查并执行自动更新（需 root）
 * @param serverVersion 服务端最新版本字符串
 * @param downloadUrl   新版本下载直链
 * @param mustUpdate    是否强制更新 ("y" 表示强制)
 * @note  该函数在更新成功后会直接 exit(0)，强制更新失败则 exit(1)
 */
void StartUpdate(const std::string &serverVersion,
                 const std::string &downloadUrl,
                 const std::string &mustUpdate);

#endif // UPDATER_H