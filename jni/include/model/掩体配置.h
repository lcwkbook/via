// 掩体模型（来自币子开源实现）—— 运行时配置
// 由主程序(main / 绘制.cpp)填充：内存读取函数、屏幕尺寸、服务器下发的游戏偏移
#pragma once
#include <cstdint>

namespace 掩体配置 {

    // ---- 环境 ----
    extern uintptr_t libUE4;                        // libUE4.so 基址
    extern uintptr_t 物理引擎指针;                     // PhysX 实例指针（0 = 用 libUE4 + 偏移自动取）
    extern bool (*读内存)(uint64_t, void *, size_t);  // 内存读取回调
    extern int 屏幕宽, 屏幕高;

    // ---- 游戏偏移（与服务器 offsets.json 对齐）----
    extern uintptr_t GWorld;             // GWorld
    extern uintptr_t PersistentLevel;    // GWorld_PersistentLevel
    extern uintptr_t 自身链1;              // SelfChain_Hop1
    extern uintptr_t 自身链2;              // SelfChain_Hop2
    extern uintptr_t 自身链3;              // SelfChain_Hop3
    extern uintptr_t 自身链4;              // SelfChain_Hop4
    extern uintptr_t 控制器偏移;            // Controller_Offset
    extern uintptr_t 相机管理器偏移;         // Controller_CameraManager
    extern uintptr_t POV偏移;             // POV_Location

    // PhysX 实例指针在 libUE4 里的偏移（games 更新需改；当前版本 0x16bae798）
    constexpr uintptr_t PhysX实例偏移 = 0x16bae798;
}
