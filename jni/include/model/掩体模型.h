#pragma once
// ============================================================================
//  掩体渲染模型（币子开源实现移植版）—— 对外接口
//  本头文件不含任何第三方类型，可以直接被 绘制.cpp 等工程代码包含。
//
//  对接方式（三步）：
//    1) 启动时（读到 libUE4 基址之后）：
//          掩体模型::设置读取函数(&读内存回调);
//          掩体模型::设置屏幕(宽, 高);
//          掩体模型::初始化(地址.libue4);
//    2) 每帧绘制时：
//          掩体模型::设置屏幕(宽, 高);
//          掩体模型::绘制();
//    3) 掩体变色（原来 LinePosition 的位置）：
//          LineOfSightToTab[i] = 掩体模型::射线遮挡(相机X,相机Y,相机Z, 骨骼X,骨骼Y,骨骼Z);
//          // 返回 true = 被掩体挡住
// ============================================================================
#include <cstdint>

namespace 掩体模型 {

    // 注入内存读取函数（用你自己的内核驱动/Kernel 读取，避免进程权限问题）
    void 设置读取函数(bool (*读内存)(uint64_t, void *, size_t));

    // 屏幕分辨率（每帧可更新）
    void 设置屏幕(int 宽, int 高);

    // 启动时调用一次：读取 libUE4 里的 PhysX 实例指针并开始扫描地图碰撞网格（起 3 个工作线程）
    bool 初始化(uintptr_t libUE4);

    // 每帧调用：读相机 → 准星射线 → 画命中的掩体三角形
    void 绘制();

    // 射线遮挡判定：true = 被掩体挡住（等价于原来的 LinePosition）
    bool 射线遮挡(float 起点X, float 起点Y, float 起点Z,
                 float 终点X, float 终点Y, float 终点Z);

    // 是否已经初始化并且 PhysX 场景指针有效
    bool 已就绪();

    // 可选：覆盖游戏偏移（默认值已与服务器 offsets.json 的一致，一般不用调）
    void 设置偏移(uintptr_t GWorld, uintptr_t PersistentLevel,
                 uintptr_t 自身链1, uintptr_t 自身链2, uintptr_t 自身链3, uintptr_t 自身链4,
                 uintptr_t 控制器偏移, uintptr_t 相机管理器偏移, uintptr_t POV偏移);
}
