#pragma once

#include <atomic>

// ★ 触摸原始状态：触摸线程(TypeA)写，渲染线程(悬浮窗.cpp 的“触摸滑动”)读。
//   由“触摸滑动”逻辑决定什么时候把它交给 ImGui —— 滑动=只滚内容，点按=补一次点击。
extern std::atomic<bool>  g_触摸屏按下;
extern std::atomic<float> g_触摸屏X;
extern std::atomic<float> g_触摸屏Y;

bool Touch_Init(int w, int h, uint32_t orientation_, bool readOnly);
void UpdateScreenData(int w, int h, uint32_t orientation_);

void Touch_Close();
void Touch_Down(float x, float y);
void Touch_Move(float x, float y);
void Touch_Up();

void Touch_Down1(float x, float y);
void Touch_Move1(float x, float y);
void Touch_Up1();
