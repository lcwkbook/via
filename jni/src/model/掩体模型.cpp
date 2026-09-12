// ============================================================================
//  掩体渲染模型（币子开源实现移植版）—— 实现
//  依赖：include/model/PhysX.h（全部实现都在头里）+ include/model/{ReadGame.h,
//        VectorHelper.h, struct.h, VectorStruct.h, Log.h} + include/{embree3,foundation}
//        + jni/lib/{libembree3.a, libsys.a, libmath.a, libtasking.a, libsimd.a, liblexers.a}
// ============================================================================
#include "掩体模型.h"
#include "掩体配置.h"
#include "ReadGame.h"
#include <cstdint>
#include <cstdio>

// 下面两个全局定义在 model/PhysX.h 里（该头只在 ReadGame.cpp 中包含一次）
extern bool (*g_掩体读内存)(uint64_t, void *, size_t);
extern uint64_t PhysxInstancePtr;

// ---------------------------------------------------------------------------
// 运行时配置实体
// ---------------------------------------------------------------------------
namespace 掩体配置 {
    uintptr_t libUE4 = 0;
    uintptr_t 物理引擎指针 = 0;
    bool (*读内存)(uint64_t, void *, size_t) = nullptr;
    int 屏幕宽 = 0, 屏幕高 = 0;

    // 默认值与服务器 offsets.json（1.38）一致
    uintptr_t GWorld = 0x1771CC38;
    uintptr_t PersistentLevel = 0xB8;
    uintptr_t 自身链1 = 0xC0;
    uintptr_t 自身链2 = 0x88;
    uintptr_t 自身链3 = 0x30;
    uintptr_t 自身链4 = 0x3540;
    uintptr_t 控制器偏移 = 0x60C8;
    uintptr_t 相机管理器偏移 = 0x680;
    uintptr_t POV偏移 = 0x650;
}

namespace 掩体模型 {

    static bool g_已初始化 = false;

    void 设置读取函数(bool (*读内存)(uint64_t, void *, size_t)) {
        掩体配置::读内存 = 读内存;
        g_掩体读内存 = 读内存;
    }

    void 设置屏幕(int 宽, int 高) {
        掩体配置::屏幕宽 = 宽 > 0 ? 宽 : 掩体配置::屏幕宽;
        掩体配置::屏幕高 = 高 > 0 ? 高 : 掩体配置::屏幕高;
    }

    bool 初始化(uintptr_t libUE4) {
        if (g_已初始化) return true;
        if (libUE4 == 0) return false;
        掩体配置::libUE4 = libUE4;
        ReadGame::Initialization();   // 读 PhysX 实例指针 + 启动 3 个扫描线程
        g_已初始化 = true;
        printf("[掩体模型] 初始化完成, libUE4=0x%lX, PhysX=0x%lX\n",
               (unsigned long)libUE4, (unsigned long)PhysxInstancePtr);
        return true;
    }

    void 绘制() {
        if (!g_已初始化) return;

        // PhysX 实例指针会随地图/对局变化，低频刷新一次，扫描线程会自动跟着换场景
        static int 计数 = 0;
        if (掩体配置::物理引擎指针 == 0 && 掩体配置::读内存 && 掩体配置::libUE4 != 0 && (计数++ % 120) == 0) {
            uint64_t v = 0;
            if (掩体配置::读内存(掩体配置::libUE4 + 掩体配置::PhysX实例偏移, &v, sizeof(v)) && v != 0) {
                PhysxInstancePtr = v;
            }
        }

        ReadGame::ReadGameData();     // 更新相机 POV
        ReadGame::Raycast();          // 准星射线 + 画三角形
    }

    bool 射线遮挡(float 起点X, float 起点Y, float 起点Z,
                 float 终点X, float 终点Y, float 终点Z) {
        if (!g_已初始化) return false;
        // CustomLineTrace 返回 true = 没打到东西 = 没被遮挡
        return !CustomLineTrace(Vector3(起点X, 起点Y, 起点Z), Vector3(终点X, 终点Y, 终点Z));
    }

    bool 已就绪() {
        return g_已初始化 && PhysxInstancePtr != 0;
    }

    void 设置偏移(uintptr_t GWorld, uintptr_t PersistentLevel,
                 uintptr_t 自身链1, uintptr_t 自身链2, uintptr_t 自身链3, uintptr_t 自身链4,
                 uintptr_t 控制器偏移, uintptr_t 相机管理器偏移, uintptr_t POV偏移) {
        掩体配置::GWorld = GWorld;
        掩体配置::PersistentLevel = PersistentLevel;
        掩体配置::自身链1 = 自身链1;
        掩体配置::自身链2 = 自身链2;
        掩体配置::自身链3 = 自身链3;
        掩体配置::自身链4 = 自身链4;
        掩体配置::控制器偏移 = 控制器偏移;
        掩体配置::相机管理器偏移 = 相机管理器偏移;
        掩体配置::POV偏移 = POV偏移;
    }
}
