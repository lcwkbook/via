#include "图片调用.h"
#include "物资ID.h"
#include "辅助类.h"

extern 绘制 绘制;

std::map<int, MyTextureData> 手持图片;
// 本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险
static int getLength(const char *str)
{
    int len = 0;
    while (*str)
    {
        if ((*str & 0xC0) != 0x80)
        {
            len++;
        }
        ++str;
    }
    return len;
}

void 绘图::初始化绘图(int X, int Y)
{
    if (Y < X)
    {
        this->PX = X / 2;
        this->PY = Y / 2;
    }
    else
    {
        this->PX = Y / 2;
        this->PY = X / 2;
    }
}

void 绘图::绘制瞄准信息()
{
    // 获取屏幕宽度，用于居中计算
    float screenWidth = ImGui::GetIO().DisplaySize.x;

    for (int i = 0; i < 绘制.被瞄准对象数量; i++)
    {
        string 距离显示 = to_string(static_cast<int>(绘制.被瞄信息[i].距离)) + "米";

        // 计算所有文本尺寸
        ImVec2 textSizeName = ImGui::CalcTextSize(绘制.被瞄信息[i].名字.c_str());
        ImVec2 textSizeUse = ImGui::CalcTextSize("使用");
        ImVec2 textSizeWeapon = ImGui::CalcTextSize(绘制.被瞄信息[i].瞄准武器.c_str());
        ImVec2 textSizeAim = ImGui::CalcTextSize("瞄准你");
        ImVec2 textSizeDistance = ImGui::CalcTextSize(距离显示.c_str());

        // 样式参数
        float spacing = 15.0f;      // 文字间距
        float rounding = 8.0f;      // 圆角半径
        float paddingH = 20.0f;     // 水平内边距
        float paddingV = 12.0f;     // 垂直内边距
        float topMargin = 20.0f;    // 距离屏幕顶部的间距（保留）
        float panelSpacing = 10.0f; // 多个面板之间的垂直间距

        // 动态计算总宽度
        float totalTextWidth = textSizeName.x + textSizeUse.x + textSizeWeapon.x +
                               textSizeAim.x + textSizeDistance.x + (4 * spacing);
        float totalWidth = totalTextWidth + 2 * paddingH;
        float totalHeight = std::max({textSizeName.y, textSizeWeapon.y, textSizeDistance.y}) + 2 * paddingV;

        // 确保最小宽度
        float minWidth = 300.0f;
        if (totalWidth < minWidth)
        {
            paddingH = (minWidth - totalTextWidth) / 2;
            totalWidth = minWidth;
        }

        // ========== 关键修改：顶部水平居中 ==========
        // startX 改为 (屏幕宽度 - 面板宽度) / 2，leftMargin 不再需要
        float startX = (screenWidth - totalWidth) / 2.0f;
        float startY = topMargin + i * (totalHeight + panelSpacing);

        // 背景与边框绘制（完全不变）
        ImColor backgroundColor = ImColor(20, 20, 30, 220);
        ImColor borderColor = ImColor(80, 80, 120, 255);

        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(startX, startY),
            ImVec2(startX + totalWidth, startY + totalHeight),
            backgroundColor, rounding);

        ImGui::GetBackgroundDrawList()->AddRect(
            ImVec2(startX, startY),
            ImVec2(startX + totalWidth, startY + totalHeight),
            borderColor, rounding, 0, 2.0f);

        // 文字绘制（相对位置不变）
        float textY = startY + paddingV + (totalHeight - 2 * paddingV - textSizeName.y) / 2;
        float currentX = startX + paddingH;

        ImGui::GetBackgroundDrawList()->AddText(ImVec2(currentX, textY), ImColor(255, 255, 255), 绘制.被瞄信息[i].名字.c_str());
        currentX += textSizeName.x + spacing;
        ImGui::GetBackgroundDrawList()->AddText(ImVec2(currentX, textY), ImColor(180, 180, 180), "使用");
        currentX += textSizeUse.x + spacing;
        ImGui::GetBackgroundDrawList()->AddText(ImVec2(currentX, textY), ImColor(255, 80, 80), 绘制.被瞄信息[i].瞄准武器.c_str());
        currentX += textSizeWeapon.x + spacing;
        ImGui::GetBackgroundDrawList()->AddText(ImVec2(currentX, textY), ImColor(255, 255, 0), "瞄准你");
        currentX += textSizeAim.x + spacing;
        ImGui::GetBackgroundDrawList()->AddText(ImVec2(currentX, textY), ImColor(100, 200, 255), 距离显示.c_str());

        // 威胁指示条（跟随面板位置）
        float threatIndicatorHeight = totalHeight - 10;
        ImColor threatColor = 绘制.被瞄信息[i].距离 < 50 ? ImColor(255, 50, 50) : 绘制.被瞄信息[i].距离 < 100 ? ImColor(255, 150, 50)
                                                                                                              : ImColor(100, 200, 100);
        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(startX + 3, startY + 5),
            ImVec2(startX + 6, startY + 5 + threatIndicatorHeight),
            threatColor, 2.0f);
    }
}

void 绘图::RenderRadarScan(ImDrawList *draw_list, ImVec2 center, float radius, int numSegments, float &rotationAngle, float lineLength, float yawRotation)
{
    // 计算延伸到外圈外的长度
    float extendedLength = radius + 10.0f;

    // 使用传入的yawRotation作为基础旋转角度
    float baseRotation = yawRotation * (M_PI / 180.0f);

    ImVec2 outerCirclePos = ImVec2(center.x + radius * cos(rotationAngle), center.y + radius * sin(rotationAngle));

    // 绘制雷达背景和内圈（不旋转）
    draw_list->AddCircleFilled(center, radius, IM_COL32(0, 0, 0, 51), numSegments);
    draw_list->AddCircle(center, radius * 0.5f, IM_COL32(255, 0, 0, 180), numSegments); // 内圈红色

    // 外圈加粗并旋转
    draw_list->AddCircle(center, radius, IM_COL32(255, 255, 255, 255), numSegments, 3.0f); // 外圈加粗到3.0

    // 横竖线条 - 保留完整的线条（圈内+圈外）
    // 水平线：完整的从左到右
    draw_list->AddLine(
        ImVec2(center.x - extendedLength, center.y),
        ImVec2(center.x + extendedLength, center.y),
        IM_COL32_WHITE);
    // 垂直线：完整的从上到下
    draw_list->AddLine(
        ImVec2(center.x, center.y - extendedLength),
        ImVec2(center.x, center.y + extendedLength),
        IM_COL32_WHITE);

    // 删除撇捺两条交叉线

    // 添加八个方向标志（应用旋转）
    const char *directions[] = {"E", "NE", "N", "NW", "W", "SW", "S", "SE"};
    float directionAngles[] = {
        baseRotation,                // E
        baseRotation + 0.25f * M_PI, // NE
        baseRotation + 0.5f * M_PI,  // N
        baseRotation + 0.75f * M_PI, // NW
        baseRotation + M_PI,         // W
        baseRotation + 1.25f * M_PI, // SW
        baseRotation + 1.5f * M_PI,  // S
        baseRotation + 1.75f * M_PI  // SE
    };

    float textOffset = 15.0f; // 文字距离线条末端的偏移量

    for (int i = 0; i < 8; i++)
    {
        float angle = directionAngles[i];
        ImVec2 textPos(
            center.x + (extendedLength + textOffset) * cos(angle),
            center.y + (extendedLength + textOffset) * sin(angle));

        // 调整文字位置使其居中
        ImVec2 textSize = ImGui::CalcTextSize(directions[i]);
        textPos.x -= textSize.x * 0.5f;
        textPos.y -= textSize.y * 0.5f;

        draw_list->AddText(textPos, IM_COL32_WHITE, directions[i]);
    }

    // 扫描线（不旋转）
    draw_list->AddLine(center, outerCirclePos, IM_COL32_WHITE);
    float newRotationAngle = rotationAngle + 0.02f;
    float rotationAngle2 = rotationAngle;
    outerCirclePos = ImVec2(center.x + radius * cos(newRotationAngle), center.y + radius * sin(newRotationAngle));
    draw_list->AddLine(center, outerCirclePos, IM_COL32_WHITE);
    newRotationAngle = rotationAngle + 0.02f;
    outerCirclePos = ImVec2(center.x + radius * cos(newRotationAngle), center.y + radius * sin(newRotationAngle));
    draw_list->AddLine(center, outerCirclePos, IM_COL32_WHITE);

    // 扫描阴影效果（不旋转）
    ImU32 shadowColor = IM_COL32(220, 212, 202, 255);
    for (float alpha = 1.0f; alpha >= 0.0f; alpha -= 0.003f)
    {
        shadowColor = IM_COL32(220, 212, 202, static_cast<int>(128 * alpha));
        outerCirclePos = ImVec2(center.x + radius * cos(rotationAngle2), center.y + radius * sin(rotationAngle2));
        draw_list->AddLine(center, outerCirclePos, shadowColor);
        rotationAngle2 -= 0.02f;
    }

    rotationAngle = newRotationAngle;
}
void 绘图::初始化坐标(D4DVector &屏幕坐标, 骨骼数据 &骨骼)
{
    left = 骨骼.Head.X - 屏幕坐标.Z / 2;
    right = 骨骼.Head.X + 屏幕坐标.Z / 2;
    top = 骨骼.Pelvis.Y - ((骨骼.Head.X != 0) ? (骨骼.Pelvis.Y - 骨骼.Head.Y) : (骨骼.Pelvis.Y - 骨骼.Chest.Y)) - 屏幕坐标.Z / 5;
    bottom = (骨骼.Left_Ankle.Y < 骨骼.Right_Ankle.Y) ? (骨骼.Right_Ankle.Y + 屏幕坐标.Z / 10) : (骨骼.Left_Ankle.Y + 屏幕坐标.Z / 10);
    MIDDLE = 屏幕坐标.X + 屏幕坐标.Z / 2;
    BOTTOM = 屏幕坐标.Y + 屏幕坐标.Z;
    TOP = 屏幕坐标.Y - 屏幕坐标.Z;
    屏幕坐标.X += 屏幕坐标.Z / 2; // 身位矫正，向右挪移半个身位
}

void 绘图::绘制头甲包(int id)
{
    string 头信息 = tou(id);
    string 甲信息 = jia(id);
    string 包信息 = bao(id);
    if (!头信息.empty() && 头信息 != "NULL" || !甲信息.empty() && 甲信息 != "NULL" || !包信息.empty() && 包信息 != "NULL")
    {
        string 显示信息 = "";
        if (!头信息.empty() && 头信息 != "NULL")
            显示信息 += 头信息 + " ";
        if (!甲信息.empty() && 甲信息 != "NULL")
            显示信息 += 甲信息 + " ";
        if (!包信息.empty() && 包信息 != "NULL")
            显示信息 += 包信息;
        auto textSize = ImGui::CalcTextSize(显示信息.c_str(), 0, 15);
        绘制字体描边(15, (right + 30) - (textSize.x / 4), top + 10 - (绘制.头甲包文本高度 * textSize.y / 2), ImColor(255, 255, 0, 255), 显示信息.c_str());
        绘制.头甲包文本高度++;
    }
}

void 绘图::绘制方框(bool 是否可见, bool isboot)
{
    if (绘制.对象信息.敌人信息.距离 > 绘制.按钮.绘制最大距离)
        return;            // 新增
    float 缩放比例 = 0.2f; // 例如 1.0 代表不缩放。0.5代表缩放到原始的50%
    // 计算方块的边长，根据矩形的尺寸调整
    float 方块边长比例 = 0.8f; // 方块边长占矩形宽度的比例（例如5%）
    float 方块长度 = (right - left) * 方块边长比例 * 缩放比例;

    // 设置方框的颜色
    ImColor 方框color = ImColor(static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框颜色[0] * 255 + 0.5),
                                static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框颜色[1] * 255 + 0.5),
                                static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框颜色[2] * 255 + 0.5),
                                static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框颜色[3] * 255 + 0.5));
    if (是否可见)
    {
        方框color = ImColor(static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框掩体颜色[0] * 255 + 0.5),
                            static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框掩体颜色[1] * 255 + 0.5),
                            static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框掩体颜色[2] * 255 + 0.5),
                            static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框掩体颜色[3] * 255 + 0.5));
    }
    // 绘制四角方块
    ImGui::GetForegroundDrawList()->AddLine({left, top}, {left + 方块长度, top}, 方框color, 绘制.按钮.方框粗细);
    ImGui::GetForegroundDrawList()->AddLine({right, top}, {right - 方块长度, top}, 方框color, 绘制.按钮.方框粗细);
    ImGui::GetForegroundDrawList()->AddLine({left, bottom}, {left + 方块长度, bottom}, 方框color, 绘制.按钮.方框粗细);
    ImGui::GetForegroundDrawList()->AddLine({right, bottom}, {right - 方块长度, bottom}, 方框color, 绘制.按钮.方框粗细);

    ImGui::GetForegroundDrawList()->AddLine({left, top}, {left, top + 方块长度}, 方框color, 绘制.按钮.方框粗细);
    ImGui::GetForegroundDrawList()->AddLine({right, top}, {right, top + 方块长度}, 方框color, 绘制.按钮.方框粗细);
    ImGui::GetForegroundDrawList()->AddLine({left, bottom}, {left, bottom - 方块长度}, 方框color, 绘制.按钮.方框粗细);
    ImGui::GetForegroundDrawList()->AddLine({right, bottom}, {right, bottom - 方块长度}, 方框color, 绘制.按钮.方框粗细);
}

void 绘图::绘制人数(int 人机, int 真人)
{
    // ========== 获取屏幕信息 ==========
    float screenW = ImGui::GetIO().DisplaySize.x;
    float screenH = ImGui::GetIO().DisplaySize.y;
    float time = ImGui::GetTime();

    // ========== 灵动岛样式参数 ==========
    float capsuleWidth = 210.0f;
    float capsuleHeight = 54.0f;
    float cornerRadius = capsuleHeight / 2.0f;
    float centerX = screenW / 2.0f;
    float topY = 80.0f;
    float numberFontSize = 26.0f;
    float labelFontSize = 12.0f;

    ImVec2 cMin = {centerX - capsuleWidth / 2, topY};
    ImVec2 cMax = {centerX + capsuleWidth / 2, topY + capsuleHeight};

    auto *draw = ImGui::GetForegroundDrawList();

    int totalPlayers = 人机 + 真人;

    // ================================================================
    // 1. 动态外发光 - 脉冲呼吸效果
    // ================================================================
    ImColor glowColor;
    float pulse = sinf(time * 1.8f) * 0.25f + 0.75f; // 0.5 ~ 1.0 脉冲

    if (绘制.地址.世界地址 == 0) {
        glowColor = ImColor(60, 120, 255, (int)(70 * pulse));
    } else if (totalPlayers == 0) {
        glowColor = ImColor(0, 255, 140, (int)(90 * pulse));
    } else if (真人 > 0) {
        // 有真人时红色脉冲更剧烈
        float dangerPulse = sinf(time * 2.5f) * 0.35f + 0.65f;
        glowColor = ImColor(255, 40, 40, (int)(100 * dangerPulse));
    } else {
        glowColor = ImColor(255, 200, 30, (int)(60 * pulse));
    }

    // 多层外发光（带脉冲动画）
    for (int i = 3; i >= 1; i--) {
        float pr = pulse * 0.3f + 0.7f;
        float r = 20.0f * (i / 3.0f) * pr;
        ImColor c = glowColor;
        c.Value.w = glowColor.Value.w * (i / 3.0f) * 0.4f;
        draw->AddRectFilled(
            ImVec2(cMin.x - r, cMin.y - r),
            ImVec2(cMax.x + r, cMax.y + r),
            c, cornerRadius + r);
    }

    // ================================================================
    // 2. 主背景 - 毛玻璃效果
    // ================================================================
    draw->AddRectFilled(cMin, cMax, ImColor(8, 8, 18, 240), cornerRadius);

    // 顶部高光反射（动态扫光效果）
    float scanY = fmod(time * 30.0f, capsuleHeight);
    float scanAlpha = (scanY < capsuleHeight * 0.3f)
                          ? (scanY / (capsuleHeight * 0.3f)) * 30
                          : 30.0f - (scanY - capsuleHeight * 0.3f) / (capsuleHeight * 0.7f) * 30;
    if (scanAlpha > 0) {
        draw->AddRectFilled(
            ImVec2(cMin.x + 6, cMin.y + scanY),
            ImVec2(cMax.x - 6, cMin.y + scanY + 2),
            ImColor(255, 255, 255, (int)scanAlpha));
    }

    // 顶部静态高光层
    float highlightH = capsuleHeight * 0.35f;
    draw->AddRectFilledMultiColor(
        ImVec2(cMin.x + 4, cMin.y + 2),
        ImVec2(cMax.x - 4, cMin.y + highlightH),
        ImColor(255, 255, 255, 20), ImColor(255, 255, 255, 20),
        ImColor(255, 255, 255, 3), ImColor(255, 255, 255, 3));

    // 底部微弱反光
    draw->AddRectFilledMultiColor(
        ImVec2(cMin.x + 4, cMax.y - highlightH * 0.5f),
        ImVec2(cMax.x - 4, cMax.y - 2),
        ImColor(255, 255, 255, 2), ImColor(255, 255, 255, 2),
        ImColor(255, 255, 255, 12), ImColor(255, 255, 255, 12));

    // ================================================================
    // 3. 动态边框
    // ================================================================
    // 外边框颜色随状态变化
    ImColor borderColor;
    if (绘制.地址.世界地址 == 0)
        borderColor = ImColor(80, 140, 255, (int)(50 * pulse + 30));
    else if (totalPlayers == 0)
        borderColor = ImColor(0, 255, 140, (int)(40 * pulse + 30));
    else if (真人 > 0)
        borderColor = ImColor(255, 60, 60, (int)(60 * pulse + 30));
    else
        borderColor = ImColor(255, 200, 30, (int)(40 * pulse + 30));

    draw->AddRect(cMin, cMax, borderColor, cornerRadius, 0, 1.5f);

    // 内层亮边
    draw->AddRect(
        ImVec2(cMin.x + 1.5f, cMin.y + 1.5f),
        ImVec2(cMax.x - 1.5f, cMax.y - 1.5f),
        ImColor(255, 255, 255, 10), cornerRadius, 0, 0.8f);

    // ================================================================
    // 4. 四角装饰呼吸灯
    // ================================================================
    float cornerGlow = sinf(time * 2.2f) * 0.4f + 0.6f;
    int cornerAlpha = (int)(cornerGlow * 80);
    float cornerLen = 12.0f;

    // 左上角
    draw->AddLine(ImVec2(cMin.x + 2, cMin.y + 2), ImVec2(cMin.x + cornerLen, cMin.y + 2),
                  ImColor(255, 255, 255, cornerAlpha), 1.8f);
    draw->AddLine(ImVec2(cMin.x + 2, cMin.y + 2), ImVec2(cMin.x + 2, cMin.y + cornerLen),
                  ImColor(255, 255, 255, cornerAlpha), 1.8f);
    // 右上角
    draw->AddLine(ImVec2(cMax.x - 2, cMin.y + 2), ImVec2(cMax.x - cornerLen, cMin.y + 2),
                  ImColor(255, 255, 255, cornerAlpha), 1.8f);
    draw->AddLine(ImVec2(cMax.x - 2, cMin.y + 2), ImVec2(cMax.x - 2, cMin.y + cornerLen),
                  ImColor(255, 255, 255, cornerAlpha), 1.8f);
    // 左下角
    draw->AddLine(ImVec2(cMin.x + 2, cMax.y - 2), ImVec2(cMin.x + cornerLen, cMax.y - 2),
                  ImColor(255, 255, 255, cornerAlpha), 1.8f);
    draw->AddLine(ImVec2(cMin.x + 2, cMax.y - 2), ImVec2(cMin.x + 2, cMax.y - cornerLen),
                  ImColor(255, 255, 255, cornerAlpha), 1.8f);
    // 右下角
    draw->AddLine(ImVec2(cMax.x - 2, cMax.y - 2), ImVec2(cMax.x - cornerLen, cMax.y - 2),
                  ImColor(255, 255, 255, cornerAlpha), 1.8f);
    draw->AddLine(ImVec2(cMax.x - 2, cMax.y - 2), ImVec2(cMax.x - 2, cMax.y - cornerLen),
                  ImColor(255, 255, 255, cornerAlpha), 1.8f);

    // ================================================================
    // 5. 内容区域 - 根据状态
    // ================================================================

    // ---------- 状态1: 未进入对局 ----------
    if (绘制.地址.世界地址 == 0) {
        string text = "等待进入对局";
        auto textSize = ImGui::CalcTextSize(text.c_str(), 0, numberFontSize);
        float textX = centerX - textSize.x / 2;
        float textY = topY + (capsuleHeight - textSize.y) / 2;

        // 文字描边
        draw->AddText(nullptr, numberFontSize, ImVec2(textX - 1, textY - 1), ImColor(0, 0, 0, 200), text.c_str());
        draw->AddText(nullptr, numberFontSize, ImVec2(textX + 1, textY + 1), ImColor(0, 0, 0, 200), text.c_str());

        // 主文字 - 蓝色呼吸
        float blueBreathe = sinf(time * 2.0f) * 40.0f + 200.0f;
        draw->AddText(nullptr, numberFontSize, ImVec2(textX, textY),
                      ImColor((int)(blueBreathe * 0.7f), (int)(blueBreathe * 0.85f), (int)blueBreathe, 255), text.c_str());

        // --- 加载动画点（三点跳跃式） ---
        float dotSpacing = 10.0f;
        float dotRadius = 3.5f;
        float dotBaseX = centerX + textSize.x / 2 + 20.0f;
        float dotY = topY + capsuleHeight / 2.0f;

        for (int d = 0; d < 3; d++) {
            // 每个点依次跳跃
            float jumpPhase = fmod(time * 2.5f + d * 0.8f, 3.0f);
            float jumpOffset = 0;
            float alpha = 100;

            if (jumpPhase < 1.0f) {
                // 上升阶段
                jumpOffset = -sinf(jumpPhase * 3.14159f) * 5.0f;
                alpha = 180 + sinf(jumpPhase * 3.14159f) * 75;
            } else {
                alpha = 120;
            }

            draw->AddCircleFilled(
                ImVec2(dotBaseX + d * dotSpacing, dotY + jumpOffset),
                dotRadius, ImColor(100, 160, 255, (int)alpha));

            // 小光晕
            if (jumpPhase < 1.0f) {
                float halo = sinf(jumpPhase * 3.14159f) * 4.0f;
                draw->AddCircleFilled(
                    ImVec2(dotBaseX + d * dotSpacing, dotY + jumpOffset),
                    dotRadius + halo, ImColor(100, 160, 255, 20));
            }
        }
        return;
    }

    // ---------- 状态2: 安全 ----------
    if (totalPlayers == 0) {
        string text = "✓ 安全区域";
        auto textSize = ImGui::CalcTextSize(text.c_str(), 0, numberFontSize);
        float textX = centerX - textSize.x / 2;
        float textY = topY + (capsuleHeight - textSize.y) / 2;

        draw->AddText(nullptr, numberFontSize, ImVec2(textX - 1, textY - 1), ImColor(0, 0, 0, 200), text.c_str());
        draw->AddText(nullptr, numberFontSize, ImVec2(textX + 1, textY + 1), ImColor(0, 0, 0, 200), text.c_str());

        // 绿色呼吸文字
        float greenBreathe = sinf(time * 1.8f) * 35.0f + 220.0f;
        draw->AddText(nullptr, numberFontSize, ImVec2(textX, textY),
                      ImColor(0, (int)greenBreathe, 100, 255), text.c_str());

        // --- 安全盾牌图标（带脉冲光环） ---
        float iconCX = textX - 18.0f;
        float iconCY = topY + capsuleHeight / 2.0f;
        float shieldSize = 14.0f;

        // 外圈脉冲光晕
        float shieldPulse = sinf(time * 2.0f) * 0.3f + 0.7f;
        draw->AddCircleFilled(ImVec2(iconCX, iconCY), shieldSize + 4.0f + shieldPulse * 3.0f,
                              ImColor(0, 255, 120, (int)(20 * shieldPulse)));

        // 盾牌主体（用圆角矩形+三角模拟）
        draw->AddRectFilled(
            ImVec2(iconCX - shieldSize / 2, iconCY - shieldSize / 2),
            ImVec2(iconCX + shieldSize / 2, iconCY + shieldSize / 2),
            ImColor(0, 255, 120, 40), 3.0f);
        draw->AddRect(
            ImVec2(iconCX - shieldSize / 2, iconCY - shieldSize / 2),
            ImVec2(iconCX + shieldSize / 2, iconCY + shieldSize / 2),
            ImColor(0, 255, 120, (int)(150 * shieldPulse)), 3.0f, 0, 1.5f);

        // 中间勾号(用两个小线)
        draw->AddLine(ImVec2(iconCX - 3, iconCY), ImVec2(iconCX - 1, iconCY + 3),
                      ImColor(0, 255, 120, 200), 2.0f);
        draw->AddLine(ImVec2(iconCX - 1, iconCY + 3), ImVec2(iconCX + 4, iconCY - 2),
                      ImColor(0, 255, 120, 200), 2.0f);

        return;
    }

    // ================================================================
    // ---------- 状态3: 正常显示人数（核心界面） ----------
    // ================================================================

    // --- 中心动态雷达环 ---
    float ringPulse = sinf(time * 2.2f) * 0.3f + 0.7f;
    float ringMaxR = 16.0f + ringPulse * 8.0f;

    // 外圈扩散环（逐渐扩散消失）
    for (int ri = 0; ri < 3; ri++) {
        float ringPhase = fmod(time * 1.5f + ri * 0.6f, 1.0f);
        float ringR = ringPhase * ringMaxR;
        int ringA = (int)((1.0f - ringPhase) * 40);
        ImColor ringColor = (真人 > 0) ? ImColor(255, 60, 60, ringA) : ImColor(255, 200, 30, ringA);
        draw->AddCircle(ImVec2(centerX, topY + capsuleHeight / 2), ringR, ringColor, 0, 1.5f);
    }

    // 中心小圆点(带脉冲)
    float dotCenterR = 4.0f + sinf(time * 3.0f) * 0.5f;
    ImColor dotColor = (真人 > 0) ? ImColor(255, 80, 80, 230) : ImColor(255, 200, 50, 230);
    draw->AddCircleFilled(ImVec2(centerX, topY + capsuleHeight / 2), dotCenterR, dotColor);

    // 中心圆点内层高光
    draw->AddCircleFilled(ImVec2(centerX - 1, topY + capsuleHeight / 2 - 1), dotCenterR * 0.4f,
                          ImColor(255, 255, 255, 60));

    // 中心圆点外圈稳定光晕
    draw->AddCircleFilled(ImVec2(centerX, topY + capsuleHeight / 2), dotCenterR + 6.0f,
                          ImColor(dotColor.Value.x, dotColor.Value.y, dotColor.Value.z, 30.0f));

    // 中心装饰短横线（两侧带呼吸）
    float dashLen = 22.0f + sinf(time * 1.5f) * 3.0f;
    int dashAlpha = (int)(sinf(time * 1.2f) * 20 + 50);
    draw->AddLine(ImVec2(centerX - dotCenterR - 5, topY + capsuleHeight / 2),
                  ImVec2(centerX - dashLen, topY + capsuleHeight / 2),
                  ImColor(255, 255, 255, dashAlpha), 1.2f);
    draw->AddLine(ImVec2(centerX + dotCenterR + 5, topY + capsuleHeight / 2),
                  ImVec2(centerX + dashLen, topY + capsuleHeight / 2),
                  ImColor(255, 255, 255, dashAlpha), 1.2f);

    // --- 左侧：真人 ---
    {
        string numStr = std::to_string(真人);
        auto numSize = ImGui::CalcTextSize(numStr.c_str(), 0, numberFontSize);
        string labelStr = "真人";
        auto labelSize = ImGui::CalcTextSize(labelStr.c_str(), 0, labelFontSize);

        float leftCenterX = centerX - capsuleWidth / 4;

        float numX = leftCenterX - numSize.x / 2;
        float numY = topY + 7.0f;

        float labelX = leftCenterX - labelSize.x / 2;
        float labelY = numY + numSize.y - 2.0f;

        // 数字描边
        draw->AddText(nullptr, numberFontSize, ImVec2(numX - 1, numY - 1), ImColor(0, 0, 0, 180), numStr.c_str());
        draw->AddText(nullptr, numberFontSize, ImVec2(numX + 1, numY + 1), ImColor(0, 0, 0, 180), numStr.c_str());

        // 数字颜色 - 如果数量>0则红色脉冲
        float redPulse = 1.0f;
        if (真人 > 0) {
            redPulse = sinf(time * 2.8f) * 0.15f + 0.85f;
        }
        int rVal = (int)(200 * redPulse) + (真人 > 0 ? 55 : 0);
        int gVal = (int)(50 * redPulse);
        int bVal = (int)(50 * redPulse);
        draw->AddText(nullptr, numberFontSize, ImVec2(numX, numY),
                      ImColor(rVal, gVal, bVal, 255), numStr.c_str());

        // 标签文字
        draw->AddText(nullptr, labelFontSize, ImVec2(labelX, labelY),
                      ImColor(200, 120, 120, 200), labelStr.c_str());

        // 左侧状态指示点（真人图标）
        if (真人 > 0) {
            float iconX = leftCenterX + numSize.x / 2 + 14.0f;
            float iconY = numY + numSize.y / 2 - 2;
            // 主圆点 + 脉冲
            draw->AddCircleFilled(ImVec2(iconX, iconY), 4.5f, ImColor(255, 60, 60, 220));
            draw->AddCircleFilled(ImVec2(iconX, iconY), 7.0f + sinf(time * 2.5f) * 2.0f,
                                  ImColor(255, 60, 60, 25));
        } else {
            float iconX = leftCenterX + numSize.x / 2 + 14.0f;
            float iconY = numY + numSize.y / 2 - 2;
            draw->AddCircleFilled(ImVec2(iconX, iconY), 3.0f, ImColor(100, 100, 100, 150));
        }
    }

    // --- 右侧：人机 ---
    {
        string numStr = std::to_string(人机);
        auto numSize = ImGui::CalcTextSize(numStr.c_str(), 0, numberFontSize);
        string labelStr = "人机";
        auto labelSize = ImGui::CalcTextSize(labelStr.c_str(), 0, labelFontSize);

        float rightCenterX = centerX + capsuleWidth / 4;

        float numX = rightCenterX - numSize.x / 2;
        float numY = topY + 7.0f;

        float labelX = rightCenterX - labelSize.x / 2;
        float labelY = numY + numSize.y - 2.0f;

        // 数字描边
        draw->AddText(nullptr, numberFontSize, ImVec2(numX - 1, numY - 1), ImColor(0, 0, 0, 180), numStr.c_str());
        draw->AddText(nullptr, numberFontSize, ImVec2(numX + 1, numY + 1), ImColor(0, 0, 0, 180), numStr.c_str());

        // 数字颜色 - 蓝白呼吸
        float bluePulse = sinf(time * 1.5f) * 20.0f + 220.0f;
        draw->AddText(nullptr, numberFontSize, ImVec2(numX, numY),
                      ImColor((int)(bluePulse * 0.85f), (int)bluePulse, 255, 255), numStr.c_str());

        // 标签
        draw->AddText(nullptr, labelFontSize, ImVec2(labelX, labelY),
                      ImColor(140, 170, 210, 200), labelStr.c_str());

        // 右侧状态点
        if (人机 > 0) {
            float iconX = rightCenterX + numSize.x / 2 + 14.0f;
            float iconY = numY + numSize.y / 2 - 2;
            draw->AddCircleFilled(ImVec2(iconX, iconY), 4.0f, ImColor(120, 180, 255, 200));
            draw->AddCircleFilled(ImVec2(iconX, iconY), 6.0f + sinf(time * 2.0f) * 1.5f,
                                  ImColor(120, 180, 255, 20));
        } else {
            float iconX = rightCenterX + numSize.x / 2 + 14.0f;
            float iconY = numY + numSize.y / 2 - 2;
            draw->AddCircleFilled(ImVec2(iconX, iconY), 3.0f, ImColor(100, 100, 100, 150));
        }
    }

    // ================================================================
    // 6. 顶部呼吸灯 - 带能量环效果
    // ================================================================
    float breather = sinf(time * 1.5f) * 0.3f + 0.7f;
    int breathAlpha = (int)(breather * 70);

    float topDotY = topY - 10.0f;

    // 外圈光晕
    if (真人 > 0) {
        draw->AddCircleFilled(ImVec2(centerX, topDotY), 5.0f + breather * 3.0f,
                              ImColor(255, 60, 60, (int)(breather * 25)));
        draw->AddCircleFilled(ImVec2(centerX, topDotY), 3.0f,
                              ImColor(255, 60, 60, breathAlpha));
    } else if (totalPlayers > 0) {
        draw->AddCircleFilled(ImVec2(centerX, topDotY), 5.0f + breather * 3.0f,
                              ImColor(255, 200, 50, (int)(breather * 25)));
        draw->AddCircleFilled(ImVec2(centerX, topDotY), 3.0f,
                              ImColor(255, 200, 50, breathAlpha));
    }

    // ================================================================
    // 7. 底部流光小尾巴（动态装饰线）
    // ================================================================
    float tailPhase = fmod(time * 1.2f, 1.0f);
    float tailX = cMin.x + 10.0f + tailPhase * (capsuleWidth - 20.0f);
    float tailLen = 30.0f;

    ImColor tailColor = (真人 > 0) ? ImColor(255, 60, 60, 60) : ImColor(255, 200, 50, 50);
    draw->AddLine(ImVec2(tailX - tailLen / 2, cMax.y - 2),
                  ImVec2(tailX + tailLen / 2, cMax.y - 2),
                  tailColor, 1.2f);

    // ================================================================
    // 8. 浮动粒子 (环绕灵动岛的小光点)
    // ================================================================
    for (int p = 0; p < 4; p++) {
        float pAngle = time * 0.6f + p * 1.57f;
        float pRadius = capsuleWidth / 2 + 12.0f + sinf(time * 0.8f + p) * 4.0f;
        float px = centerX + cosf(pAngle) * pRadius;
        float py = topY + capsuleHeight / 2 + sinf(pAngle) * (capsuleHeight / 2 + 8.0f);
        float pSize = 1.5f + sinf(time * 1.3f + p * 2.0f) * 0.8f;
        int pAlpha = (int)(sinf(time * 1.1f + p * 1.7f) * 30 + 50);

        if (px >= cMin.x - 10 && px <= cMax.x + 10 &&
            py >= cMin.y - 10 && py <= cMax.y + 10) {
            continue; // 不在岛内显示
        }

        draw->AddCircleFilled(ImVec2(px, py), pSize,
                              ImColor(255, 255, 255, pAlpha));
    }

    // ================================================================
    // 9. 总人数角标（右上角小气泡）
    // ================================================================
    {
        string totalStr = std::to_string(totalPlayers);
        auto totalSize = ImGui::CalcTextSize(totalStr.c_str(), 0, 10.0f);
        float badgeW = totalSize.x + 10.0f;
        float badgeH = 16.0f;
        float badgeX = cMax.x - badgeW - 4.0f;
        float badgeY = topY - badgeH - 2.0f;

        // 小气泡背景
        draw->AddRectFilled(
            ImVec2(badgeX, badgeY),
            ImVec2(badgeX + badgeW, badgeY + badgeH),
            ImColor(0, 0, 0, 160), 8.0f);

        // 边框
        draw->AddRect(
            ImVec2(badgeX, badgeY),
            ImVec2(badgeX + badgeW, badgeY + badgeH),
            ImColor(255, 255, 255, 30), 8.0f, 0, 0.8f);

        // 文字
        draw->AddText(nullptr, 10.0f,
                      ImVec2(badgeX + badgeW / 2 - totalSize.x / 2, badgeY + (badgeH - totalSize.y) / 2),
                      ImColor(180, 180, 180, 180), totalStr.c_str());
    }
}


void 绘图::绘制距离(int 距离, int 队伍)
{
    string 距离文本 = to_string((int)距离) + "M";
    auto Size = ImGui::CalcTextSize(距离文本.c_str(), 0, 绘制.距离字体大小);

    // ============== 修复：读取配置中的距离颜色 ==============
    // 根据人机/真人获取对应颜色配置
    float *距离颜色配置 = 绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].距离颜色;
    ImVec4 距离颜色 = ImVec4(距离颜色配置[0], 距离颜色配置[1], 距离颜色配置[2], 距离颜色配置[3]);

    // 替换原来的 颜色.白色 为配置颜色
    绘制字体描边(绘制.距离字体大小, MIDDLE - (Size.x / 4), bottom, 距离颜色, 距离文本.c_str());
}

void 绘图::绘制射线(bool 是否可见, 骨骼数据 &骨骼)
{
    auto &颜色 = 绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].射线颜色;
    ImColor 默认颜色 = ImColor(
        static_cast<int>(颜色[0] * 255 + 0.5),
        static_cast<int>(颜色[1] * 255 + 0.5),
        static_cast<int>(颜色[2] * 255 + 0.5),
        static_cast<int>(颜色[3] * 255 + 0.5));
    auto &掩体颜色 = 绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].射线掩体颜色;
    if (是否可见)
    {
        默认颜色 = ImColor(
            static_cast<int>(掩体颜色[0] * 255 + 0.5),
            static_cast<int>(掩体颜色[1] * 255 + 0.5),
            static_cast<int>(掩体颜色[2] * 255 + 0.5),
            static_cast<int>(掩体颜色[3] * 255 + 0.5));
    }
    ImGui::GetForegroundDrawList()->AddLine({PX, 0}, {骨骼.Head.X, top - 70}, 默认颜色, 绘制.按钮.射线粗细); // 使用白色，并增加线宽
}
void 绘图::漏手模式()
{
    // 绘制半透明深灰色圆点 (RGBA: 0.2, 0.2, 0.2, 0.5)
    ImGui::GetForegroundDrawList()->AddCircleFilled(
        {MIDDLE, top},
        10.0f,                          // 半径
        ImColor(0.2f, 0.2f, 0.2f, 0.5f) // 深灰色半透明
    );
}

void 绘图::绘制自救(float 自救倒计时)
{
    string 计时文本 = "";
    char buffer[50];
    std::sprintf(buffer, "%.1f", 自救倒计时);
    计时文本 = "正在自救-[" + std::string(buffer) + "]";
    auto textSize = ImGui::CalcTextSize(计时文本.c_str());
    ImVec2 计时位置 = ImVec2(MIDDLE - (textSize.x / 4) - 20, top - 70);
    绘制字体描边(30, 计时位置.x, 计时位置.y,
                 ImColor(255, 0, 0, 255),
                 计时文本.c_str());
}

void 绘图::绘制名字(string 名字, bool isboot, float 计时, bool 是否掐雷, char *类名, int 阵营, int Bonecount, bool 是否自救, int 高级人机)
{
    string a = to_string((int)阵营) + "·";
    a += isboot ? "人机" : 名字;
    string 计时文本 = "";
    if (高级人机 != 0)
    {
        a = "高级人机[粑粑打我]";
    }
    if (是否掐雷)
    {
        char buffer[50];
        std::sprintf(buffer, "%.1f", 计时);
        计时文本 = " 掐雷中-[" + std::string(buffer) + "]";
    }
    auto textSize = ImGui::CalcTextSize(a.c_str(), 0, 绘制.名称字体大小);

    // 名字位置
    ImVec2 名字位置 = ImVec2(MIDDLE - (textSize.x / 4) - 13, top - 37);
    ImVec2 计时位置 = ImVec2(MIDDLE - (textSize.x / 4) - 20, top - 70);
    绘制字体描边(绘制.名称字体大小, 名字位置.x, 名字位置.y,
                 ImColor(
                     static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].名称颜色[0] * 255 + 0.5f),
                     static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].名称颜色[1] * 255 + 0.5f),
                     static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].名称颜色[2] * 255 + 0.5f),
                     static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].名称颜色[3] * 255 + 0.5f)),
                 a.c_str());

    if (是否掐雷)
    {
        绘制字体描边(30, 计时位置.x, 计时位置.y,
                     ImColor(255, 0, 0, 255),
                     计时文本.c_str());
    }
}

void 绘图::绘制血量(float 最大血量, float 当前血量, bool isbot)
{

    float 血量 = 当前血量 / 最大血量 * 100;
    if (绘制.按钮.血条绘图 == 2) // 简约
    {
        string duo = to_string((int)当前血量);
        auto textSize = ImGui::CalcTextSize(duo.c_str());
        float hpRatio = 当前血量 / 最大血量;   // 血量比例
        float healthBarWidth = 114.0f * 0.56f; // 114*0.7*0.8=63.84≈114*0.56
        float healthBarHeight = 11.0f * 0.4f;  // 11*0.5*0.8=4.4≈11*0.4
        float verticalOffset = (18 * 0.2f);    // 18*(1-0.8)=3.6的偏移调整

        ImGui::GetForegroundDrawList()->AddRectFilled({MIDDLE - healthBarWidth / 2, top - 18 + verticalOffset},
                                                      {MIDDLE + healthBarWidth / 2, top - 18 + healthBarHeight + verticalOffset},
                                                      IM_COL32(0, 0, 0, 0), 0);
        ImGui::GetForegroundDrawList()->AddRectFilled({MIDDLE - healthBarWidth / 2, top - 18 + verticalOffset},
                                                      {MIDDLE - healthBarWidth / 2 + healthBarWidth * hpRatio, top - 18 + healthBarHeight + verticalOffset},
                                                      IM_COL32(173, 255, 47, 255), 5.0f);
        ImGui::GetForegroundDrawList()->AddRect({MIDDLE - healthBarWidth / 2, top - 18 + verticalOffset},
                                                {MIDDLE + healthBarWidth / 2, top - 18 + healthBarHeight + verticalOffset},
                                                ImColor(255, 192, 203, 1), 5.0f, 0, 1.5f);
    }
    if (绘制.按钮.血条绘图 == 1) // 赛事血条样式
    {
        if (当前血量 > 0)
        {
            float x = MIDDLE;    // 使用方框中心X坐标
            float y = top - -10; // 调整Y坐标位置

            // 根据血量设置颜色渐变
            ImColor 联赛health =
                当前血量 == 100.0f ? ImColor(255, 255, 255, 155) : 当前血量 >= 90.0f ? ImColor(255, 255, 220, 255)
                                                               : 当前血量 >= 80.0f   ? ImColor(255, 255, 180, 255)
                                                               : 当前血量 >= 70.0f   ? ImColor(255, 255, 140, 255)
                                                               : 当前血量 >= 60.0f   ? ImColor(255, 200, 100, 255)
                                                               : 当前血量 >= 50.0f   ? ImColor(255, 150, 150, 255)
                                                               : 当前血量 >= 40.0f   ? ImColor(255, 120, 120, 255)
                                                               : 当前血量 >= 30.0f   ? ImColor(255, 90, 90, 255)
                                                               : 当前血量 >= 20.0f   ? ImColor(255, 60, 60, 255)
                                                               : 当前血量 >= 10.0f   ? ImColor(255, 30, 30, 255)
                                                               : 当前血量 >= 5.0f    ? ImColor(255, 0, 0, 200)
                                                                                     : ImColor(255, 0, 0, 255);

            // 队伍ID显示区域
            auto textSize_id = ImGui::GetFont()->CalcTextSizeA(23, FLT_MAX, -1,
                                                               to_string(绘制.对象信息.敌人信息.队伍).c_str(), NULL, NULL);

            // 绘制队伍ID背景（左侧彩色区域）
            ImGui::GetBackgroundDrawList()->AddRectFilled(
                {x - 75, y - 43},
                {x - 60 + textSize_id.x, y - 17},
                ImColor(static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框颜色[0] * 255 + 0.5),
                        static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框颜色[1] * 255 + 0.5),
                        static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框颜色[2] * 255 + 0.5),
                        static_cast<int>(绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].方框颜色[3] * 255 + 0.5)),
                0);

            // 根据队伍ID确定名称背景颜色
            ImColor nameFillColor;
            switch (绘制.对象信息.敌人信息.队伍 % 7)
            {
            case 0:
                nameFillColor = ImColor(0, 255, 0, 80);
                break; // 绿色半透明
            case 1:
                nameFillColor = ImColor(0, 0, 255, 80);
                break; // 蓝色半透明
            case 2:
                nameFillColor = ImColor(255, 0, 255, 80);
                break; // 紫色半透明
            case 3:
                nameFillColor = ImColor(255, 165, 0, 80);
                break; // 橙色半透明
            case 4:
                nameFillColor = ImColor(0, 255, 255, 80);
                break; // 青色半透明
            case 5:
                nameFillColor = ImColor(255, 192, 203, 80);
                break; // 粉色半透明
            default:
                nameFillColor = ImColor(255, 255, 0, 80);
                break; // 黄色半透明
            }

            // 绘制名称背景（右侧区域）
            ImGui::GetBackgroundDrawList()->AddRectFilled(
                {x - 60 + textSize_id.x, y - 43},
                {x + 75, y - 17},
                nameFillColor,
                0);

            // 绘制血量条背景
            ImGui::GetBackgroundDrawList()->AddRectFilled(
                {x - 75, y - 17},
                {x + 75, y - 11},
                ImColor(50, 50, 50, 200),
                0);

            // 绘制当前血量
            ImGui::GetBackgroundDrawList()->AddRectFilled(
                {x - 75, y - 17},
                {x - 75 + (当前血量 * 150 / 100), y - 11},
                联赛health,
                0);

            // 绘制队伍ID文字
            绘制字体描边(
                23,
                x - 67.5f,
                y - 29 - (textSize_id.y / 2),
                ImVec4{255, 255, 255, 255},
                to_string(绘制.对象信息.敌人信息.队伍).c_str());

            // 绘制玩家名称
            auto textSize_信息 = ImGui::GetFont()->CalcTextSizeA(23, FLT_MAX, -1,
                                                                 绘制.对象信息.敌人信息.名字.c_str(), NULL, NULL);
            绘制字体描边(
                23,
                x - 53 + textSize_id.x,
                y - 29 - (textSize_信息.y / 2),
                ImVec4{255, 255, 255, 255},
                绘制.对象信息.敌人信息.名字.c_str());
        }
    }

    if (绘制.按钮.血条绘图 == 0) // 分格血条
    {
        const float 当前血量百分比 = 当前血量 / 100.0f;
        const float healthBarWidth = 114.0f * 0.8f;
        const float healthBarHeight = 11.0f * 0.5f; // 5.5像素高
        const float borderRadius = 3.0f;
        const ImU32 lineColor = IM_COL32(0, 0, 0, 255);
        const float barStartX = MIDDLE - healthBarWidth / 2;
        const float barEndX = MIDDLE + healthBarWidth / 2;

        // 绘制血条背景
        ImGui::GetForegroundDrawList()->AddRectFilled(
            {barStartX, top - 18},
            {barEndX, top - 18 + healthBarHeight},
            IM_COL32(220, 220, 220, 150),
            borderRadius);

        // 绘制黑色描边
        ImGui::GetForegroundDrawList()->AddRect(
            {barStartX, top - 18},
            {barEndX, top - 18 + healthBarHeight},
            lineColor,
            borderRadius, 0, 2.5f);

        // 计算填充宽度
        const float filledWidth = healthBarWidth * 当前血量百分比;
        const float actualFilledWidth = std::min(filledWidth, healthBarWidth);

        // 血量低于35%爆闪效果
        static float breathTimer = 0.0f;
        breathTimer += 0.05f;
        if (breathTimer > 6.283185f)
            breathTimer -= 6.283185f;

        ImU32 fillColor = IM_COL32(220, 50, 50, 255);
        if (当前血量百分比 <= 0.35f)
        {
            float flash = sin(breathTimer * 6.0f);
            if (flash > 0.3f)
            {
                fillColor = IM_COL32(255, 80, 80, 255);
            }
            else if (flash < -0.3f)
            {
                fillColor = IM_COL32(180, 30, 30, 200);
            }
        }

        // 绘制血条填充
        ImGui::GetForegroundDrawList()->AddRectFilled(
            {barStartX, top - 18},
            {barStartX + actualFilledWidth, top - 18 + healthBarHeight},
            fillColor,
            borderRadius);

        // 绘制分格线
        for (int i = 1; i < 5; i++)
        {
            const float linePos = barStartX + (healthBarWidth / 5.0f) * i;
            ImGui::GetForegroundDrawList()->AddLine(
                {linePos, top - 18},
                {linePos, top - 18 + healthBarHeight},
                lineColor,
                2.0f);
        }
    }
}

void 绘图::绘制手持(int 手持, int 状态, int 子弹, int 最大子弹)
{
    // 手持图片绘制 - 由 "手持图片" 开关控制
    if (绘制.按钮.手持)
    {
        手持 = heldconversion(手持);
        if (手持图片.find(手持) != 手持图片.end())
        {
            ImGui::GetForegroundDrawList()->AddImage(
                手持图片[手持].DS,
                ImVec2(MIDDLE - 75 * 0.7f, top - 135 * 0.7f),
                ImVec2(MIDDLE + 75 * 0.7f, top - 100 * 0.7f),
                ImVec2(0, 0), ImVec2(1, 1),
                ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
        }
    }

    // 手持文字绘制 - 由 "手持文字" 开关控制
    if (绘制.按钮.手持2)
    {
        string b = GetHolGunItem(手持);
        b += " [" + to_string(子弹) + "/" + to_string(最大子弹) + "]";
        auto textSize = ImGui::CalcTextSize(b.c_str(), 0, 绘制.手持字体大小);
        绘制字体描边(绘制.手持字体大小,
                     (MIDDLE + 20) - (textSize.x / 2),
                     top - 88,
                     ImColor(255, 255, 255),
                     b.c_str());
    }
}

void 绘图::绘制动作(int 状态)
{
    string a = GetHol(状态);
    auto Size = ImGui::CalcTextSize(a.c_str(), 0, 绘制.动作字体大小);
    绘制字体描边(绘制.动作字体大小, MIDDLE + 5 - (Size.x / 2), top - 60, 颜色.白色, a.c_str());
}

void 绘图::绘制骨骼(骨骼数据 &骨骼, D4DVector &屏幕坐标, bool LineOfSightTo[15], int 距离, int Bonecount)
{
    if (距离 <= (int)绘制.骨骼距离限制)
    {
        auto &颜色1 = 绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].骨骼颜色;
        ImColor 可见颜色 = ImColor(
            static_cast<int>(颜色1[0] * 255 + 0.5),
            static_cast<int>(颜色1[1] * 255 + 0.5),
            static_cast<int>(颜色1[2] * 255 + 0.5),
            static_cast<int>(颜色1[3] * 255 + 0.5));

        auto &颜色2 = 绘制.Colorset[(int)绘制.对象信息.敌人信息.isboot].骨骼掩体颜色;
        ImColor 掩体颜色 = ImColor(
            static_cast<int>(颜色2[0] * 255 + 0.5),
            static_cast<int>(颜色2[1] * 255 + 0.5),
            static_cast<int>(颜色2[2] * 255 + 0.5),
            static_cast<int>(颜色2[3] * 255 + 0.5));
        auto *背景绘制 = ImGui::GetBackgroundDrawList();
        auto *前景绘制 = ImGui::GetForegroundDrawList();

        // 绘制头部
        背景绘制->AddCircle(
            ImVec2(骨骼.Head.X, 骨骼.Head.Y),
            屏幕坐标.W / 13,
            LineOfSightTo[0] ? 掩体颜色 : 可见颜色,
            0,
            绘制.按钮.骨骼粗细);

        // 绘制线段（只判断起点 LineOfSightTo）
        auto 绘制线段 = [&](int 起点idx, D2DVector &点1, D2DVector &点2)
        {
            float dx = 点2.X - 点1.X;
            float dy = 点2.Y - 点1.Y;
            if ((dx * dx + dy * dy) < 10000)
            {
                ImColor color = LineOfSightTo[起点idx] ? 掩体颜色 : 可见颜色;
                前景绘制->AddLine(ImVec2(点1.X, 点1.Y), ImVec2(点2.X, 点2.Y), color, 绘制.按钮.骨骼粗细);
            }
        };

        // 上半身
        绘制线段(1, 骨骼.Chest, 骨骼.Pelvis);
        绘制线段(1, 骨骼.Chest, 骨骼.Left_Shoulder);
        绘制线段(3, 骨骼.Left_Shoulder, 骨骼.Left_Elbow);
        绘制线段(5, 骨骼.Left_Elbow, 骨骼.Left_Wrist);
        绘制线段(1, 骨骼.Chest, 骨骼.Right_Shoulder);
        绘制线段(4, 骨骼.Right_Shoulder, 骨骼.Right_Elbow);
        绘制线段(6, 骨骼.Right_Elbow, 骨骼.Right_Wrist);

        // 下半身
        绘制线段(2, 骨骼.Pelvis, 骨骼.Left_Thigh);
        绘制线段(9, 骨骼.Left_Thigh, 骨骼.Left_Knee);
        绘制线段(11, 骨骼.Left_Knee, 骨骼.Left_Ankle);
        绘制线段(2, 骨骼.Pelvis, 骨骼.Right_Thigh);
        绘制线段(10, 骨骼.Right_Thigh, 骨骼.Right_Knee);
        绘制线段(12, 骨骼.Right_Knee, 骨骼.Right_Ankle);

        // 绘制骨盆延伸线（如果开启）
        if (绘制.按钮.延伸)
        {
            // 简单的向下延伸
            float 延伸长度 = 50.0f;
            D2DVector 延伸点;

            // 向下延伸：X坐标不变，Y坐标增加
            延伸点.X = 骨骼.Pelvis.X;
            延伸点.Y = 骨骼.Pelvis.Y + 延伸长度; // 注意：屏幕坐标系中Y增加是向下

            // 绘制延伸线 - 固定绿色，粗细3.0
            前景绘制->AddLine(
                ImVec2(骨骼.Pelvis.X, 骨骼.Pelvis.Y),
                ImVec2(延伸点.X, 延伸点.Y),
                ImColor(0, 255, 0, 255), // 固定绿色 (R=0, G=255, B=0, A=255)
                6.0f                     // 固定粗细3.0
            );
        }
    }
}

void 绘图::绘制字体描边(float size, int x, int y, ImVec4 color, const char *str)
{
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x - 0.1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)), str);
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(color), str);
}

void 绘图::绘制加粗字体(float size, float x, float y, ImColor color, ImColor color1, const char *str)
{
    ImGui::GetBackgroundDrawList()->AddText(nullptr, size, ImVec2(x, y), color, str);
}

// 世界坐标转屏幕坐标
bool 绘图::WorldTurnScreen(VecTor2 &Screen, VecTor3 World, float Matrix[])
{
    float Camera = Matrix[3] * World.x + Matrix[7] * World.y + Matrix[11] * World.z + Matrix[15];
    if (Camera < 0.03)
    {
        return false;
    }
    Screen.x = PX + (Matrix[0] * World.x + Matrix[4] * World.y + Matrix[8] * World.z + Matrix[12]) / Camera * PX;
    Screen.y = PY - (Matrix[1] * World.x + Matrix[5] * World.y + Matrix[9] * World.z + Matrix[13]) / Camera * PY;
    return true;
}

// 爆炸范围
void 绘图::ExplosionRange(D3DVector Obj, ImColor color, float Range, float thickn, float Matrix[])
{
    constexpr int rings = 6;
    constexpr int sectors = 32;
    const float heightFactor = 1.0f;
    const float flashFreq = 0.3f;

    static float pulse = 0.0f;
    pulse += 0.03f * (pulse > IM_PI ? -1.0f : 1.0f);
    const float pulseFactor = 0.97f + 0.03f * sinf(pulse * 6.0f);

    const ImColor sphereFill(255, 255, 100, 40);  // 球体填充色(浅黄半透明)
    const ImColor outlineColor(255, 255, 0, 220); // 轮廓线颜色(亮黄)

    const bool flashState = static_cast<int>(ImGui::GetTime() / flashFreq) % 2 == 0;
    const ImColor flashColor = ImColor(255, 80, 80, 180);

    std::vector<ImVec2> points2D;
    points2D.reserve((rings + 1) * sectors);

    for (int i = 0; i <= rings; ++i)
    {
        const float phi = IM_PI / 2 * i / rings;
        const float radius = Range * sinf(phi) * pulseFactor;
        const float z = Obj.Z + Range * cosf(phi) * heightFactor;

        for (int j = 0; j < sectors; ++j)
        {
            const float theta = 2 * IM_PI * j / sectors;
            VecTor2 screen;
            WorldTurnScreen(screen, {Obj.X + radius * cosf(theta), Obj.Y + radius * sinf(theta), z}, Matrix);
            points2D.emplace_back(screen.x, screen.y);
        }
    }

    for (int i = 0; i < rings; ++i)
    {
        const int currRing = i * sectors;
        const int nextRing = (i + 1) * sectors;

        for (int j = 0; j < sectors; ++j)
        {
            const int nextJ = (j + 1) % sectors;
            ImVec2 quad[4] = {
                points2D[currRing + j],
                points2D[currRing + nextJ],
                points2D[nextRing + nextJ],
                points2D[nextRing + j]};
            ImGui::GetForegroundDrawList()->AddConvexPolyFilled(quad, 4, sphereFill);
        }
    }

    for (int i = 0; i <= rings; ++i)
    {
        const int ringStart = i * sectors;

        for (int j = 0; j < sectors; ++j)
        {
            const int curr = ringStart + j;
            const int next = ringStart + (j + 1) % sectors;

            if (i > 0)
            {
                ImGui::GetForegroundDrawList()->AddLine(
                    points2D[curr], points2D[next],
                    outlineColor, thickn * 0.8f);
            }

            if (i < rings)
            {
                const int above = curr + sectors;
                const bool isBase = (i == 0);
                ImGui::GetForegroundDrawList()->AddLine(
                    points2D[curr], points2D[above],
                    isBase ? outlineColor : ImColor(255, 255, 0, 160),
                    isBase ? thickn * 1.2f : thickn * 0.6f);
            }
        }
    }

    std::vector<ImVec2> bottomCircle(sectors);
    for (int j = 0; j < sectors; ++j)
    {
        const float theta = 2 * IM_PI * j / sectors;
        VecTor2 screen;
        WorldTurnScreen(screen, {Obj.X + Range * cosf(theta) * pulseFactor, Obj.Y + Range * sinf(theta) * pulseFactor, Obj.Z}, Matrix);
        bottomCircle[j] = ImVec2(screen.x, screen.y);
    }
    ImGui::GetForegroundDrawList()->AddPolyline(
        bottomCircle.data(), sectors, outlineColor, true, thickn * 1.5f);

    if (pulse < IM_PI / 2)
    {
        const float waveProgress = pulse / (IM_PI / 2);
        const float waveRadius = Range * (1.0f + 0.4f * waveProgress);
        const float waveHeight = Range * 0.6f * waveProgress;

        const ImColor waveColor(
            255,
            flashState ? 100 : 200,
            100,
            static_cast<int>(200 * (1.0f - waveProgress)));

        std::vector<ImVec2> wavePoints(sectors);
        for (int j = 0; j < sectors; ++j)
        {
            const float theta = 2 * IM_PI * j / sectors;
            VecTor2 screen;
            WorldTurnScreen(screen, {Obj.X + waveRadius * cosf(theta), Obj.Y + waveRadius * sinf(theta), Obj.Z + waveHeight}, Matrix);
            wavePoints[j] = ImVec2(screen.x, screen.y);
        }

        ImGui::GetForegroundDrawList()->AddPolyline(
            wavePoints.data(), sectors,
            waveColor,
            true,
            thickn * (1.0f - waveProgress * 0.5f));
    }
}
void 绘图::瞬爆雷预测(int 距离, float 计时, bool 是否掐雷)
{
    if (距离 > 55 || !是否掐雷)
    {
        return;
    }
    float 倒计时 = static_cast<float>(距离) / 15.0f;
    float 瞬爆时间 = 计时 - 倒计时 - 0.3f;
    if (瞬爆时间 < 0)
    {
        瞬爆时间 = 0;
    }
    char buffer[50];
    std::sprintf(buffer, "%.1f秒", 瞬爆时间);
    if (瞬爆时间 != 0)
    {
        绘制字体描边(40, left - 10, top + 10, ImColor(255, 255, 0, 255), buffer);
    }
    else
    {
        绘制字体描边(40, left, top + 10, ImColor(255, 0, 0, 255), "丢出");
    }
}

void 绘图::ExplosionRange1(D3DVector Obj, ImColor color, float Range, float thickn, float Matrix[])
{
    VecTor3 l1, l2, l3, l4, l5, l6, l7, l8;
    VecTor2 lw1, lw2, lw3, lw4, lw5, lw6, lw7, lw8;
    l1 = VecTor3(Obj.X - Range, Obj.Y - Range, Obj.Z);
    l2 = VecTor3(Obj.X, Obj.Y - Range, Obj.Z);
    l3 = VecTor3(Obj.X + Range, Obj.Y - Range, Obj.Z);
    l4 = VecTor3(Obj.X - Range, Obj.Y, Obj.Z);
    l5 = VecTor3(Obj.X + Range, Obj.Y, Obj.Z);
    l6 = VecTor3(Obj.X - Range, Obj.Y + Range, Obj.Z);
    l7 = VecTor3(Obj.X, Obj.Y + Range, Obj.Z);
    l8 = VecTor3(Obj.X + Range, Obj.Y + Range, Obj.Z);
    WorldTurnScreen(lw1, l1, Matrix);
    WorldTurnScreen(lw2, l2, Matrix);
    WorldTurnScreen(lw3, l3, Matrix);
    WorldTurnScreen(lw4, l4, Matrix);
    WorldTurnScreen(lw5, l5, Matrix);
    WorldTurnScreen(lw6, l6, Matrix);
    WorldTurnScreen(lw7, l7, Matrix);
    WorldTurnScreen(lw8, l8, Matrix);

    // 绘制曲线
    ImGui::GetForegroundDrawList()->AddBezierCubic({lw4.x, lw4.y}, {lw1.x, lw1.y}, {lw2.x, lw2.y}, {lw2.x, lw2.y}, color, thickn);
    ImGui::GetForegroundDrawList()->AddBezierCubic({lw2.x, lw2.y}, {lw3.x, lw3.y}, {lw5.x, lw5.y}, {lw5.x, lw5.y}, color, thickn);
    ImGui::GetForegroundDrawList()->AddBezierCubic({lw5.x, lw5.y}, {lw8.x, lw8.y}, {lw7.x, lw7.y}, {lw7.x, lw7.y}, color, thickn);
    ImGui::GetForegroundDrawList()->AddBezierCubic({lw7.x, lw7.y}, {lw6.x, lw6.y}, {lw4.x, lw4.y}, {lw4.x, lw4.y}, color, thickn);
}
