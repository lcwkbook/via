#pragma once
#include <cstdint>

namespace Offsets {
    // ==================== libUE4.so 模块基址偏移 ====================
    constexpr uintptr_t GWorld             = 0x15772758;
    constexpr uintptr_t GName              = 0x154DAD38;
    constexpr uintptr_t MatrixChain1       = 0x1573BEE8;
    constexpr uintptr_t MatrixChain2       = 0x154DAD38;
    constexpr uintptr_t ClassBase          = 0x14E33C18;
    constexpr uintptr_t GyroBase           = 0x12F65848;
    // ==================== GWorld 内部偏移 ====================
    constexpr uintptr_t GWorld_PersistentLevel = 0xB0;
    constexpr uintptr_t GWorld_ActorsArray      = 0xA0;
    constexpr uintptr_t GWorld_ActorsCount      = 0xA8;
    constexpr uintptr_t GWorld_ActorsCountDec    = 0xB8;
    constexpr uintptr_t GWorld_GameState        = 0xAB8;
    // ==================== Actor 通用偏移 ====================
    constexpr uintptr_t Actor_RootComponent     = 0x260;
    constexpr uintptr_t Actor_TeamID            = 0xB78;
    constexpr uintptr_t Actor_Health            = 0xFF8;
    constexpr uintptr_t Actor_HealthMax         = 0x1000;
    constexpr uintptr_t Actor_PlayerName        = 0xAF8;
    constexpr uintptr_t Actor_PlayerUID         = 0xB10;
    constexpr uintptr_t Actor_Velocity          = 0x10FC;
    constexpr uintptr_t Actor_Mesh              = 0x658;
    constexpr uintptr_t Actor_PawnState         = 0x1680;
    constexpr uintptr_t Actor_bIsAI             = 0xB94;
    constexpr uintptr_t Actor_bIsGunADS         = 0x17B8;
    constexpr uintptr_t Actor_bIsWeaponFiring   = 0x2608;
    constexpr uintptr_t Actor_CurrentWeapon     = 0x1108;
    constexpr uintptr_t Actor_SpeedValue        = 0x1018;
    constexpr uintptr_t Actor_HighWalkSpeed     = 0x38C4;
    constexpr uintptr_t Actor_Rotator           = 0x198;
    constexpr uintptr_t Actor_Vehicle           = 0x1B8;
    constexpr uintptr_t Actor_WeaponEntity      = 0x10E8;
    constexpr uintptr_t Actor_ClassID           = 0x18;
    // ==================== 玩家控制器偏移 ====================
    constexpr uintptr_t Controller_Offset       = 0x5C58;
    constexpr uintptr_t Controller_CameraManager = 0x660;
    constexpr uintptr_t Controller_AimYaw       = 0x604;
    // ==================== 相机管理器偏移 ====================
    constexpr uintptr_t CameraManager_CameraPos = 0x650;
    constexpr uintptr_t CameraManager_Rotation  = 0x668;
    constexpr uintptr_t CameraManager_FOV       = 0x680;
    // ==================== 武器偏移 ====================
    constexpr uintptr_t Weapon_RepID            = 0xDA8;
    constexpr uintptr_t Weapon_EntityComp       = 0xC68;
    constexpr uintptr_t Weapon_BulletSpeed      = 0x15CC;
    constexpr uintptr_t Weapon_RecoilFactor     = 0x1EC8;
    constexpr uintptr_t Weapon_ClipAmmo         = 0x1F80;
    constexpr uintptr_t Weapon_ClipMaxAmmo      = 0x1F84;
    constexpr uintptr_t Weapon_GripID           = 0xEA0;
    constexpr uintptr_t Weapon_CachedBulletTrack = 0x1F60;
    constexpr uintptr_t Weapon_ShootBursts      = 0x23C;
    // ==================== 载具偏移 ====================
    constexpr uintptr_t Vehicle_CommonData      = 0xBD8;
    constexpr uintptr_t Vehicle_CurrentHP       = 0x1F8;
    constexpr uintptr_t Vehicle_MaxHP           = 0x1F4;
    constexpr uintptr_t Vehicle_CurrentFuel     = 0x21C;
    constexpr uintptr_t Vehicle_MaxFuel         = 0x218;
    // ==================== 盒子/物资偏移 ====================
    constexpr uintptr_t PickUpDataList          = 0xD88;
    constexpr uintptr_t Box_OpenState           = 0x270;
    constexpr uintptr_t Box_Open           = 0x6f4;
    // ==================== Mesh/骨骼偏移 ====================
    constexpr uintptr_t Mesh_ComponentToWorld   = 0x1F0;
    constexpr uintptr_t Mesh_BoneArray          = 0x828;
    constexpr uintptr_t Mesh_BoneCountOffset    = 0x8;
    // ==================== 全图人数偏移 ====================
    constexpr uintptr_t AliveNum = 0xac0;
    constexpr uintptr_t AllivePlayerNum = 0x129c;    //全图人数
    constexpr uintptr_t AliveRealPlayerNum = 0x12a0;    //真人数量
    // constexpr uintptr_t AlivePlayerNum = 0x12ac;    //人机数量
    constexpr uintptr_t AliveTeamNum= 0x131c;   //队伍数量
    // ==================== 矩阵链偏移 ====================
    constexpr uintptr_t Matrix_ViewMatrix       = 0x270;
    constexpr uintptr_t Matrix_Tol_Offset1      = 0x98;
    constexpr uintptr_t Matrix_Tol_Offset2      = 0x10440;
    // ==================== 陀螺仪灵敏度偏移 ====================
    // constexpr uintptr_t Gyro_ThirdPerson        = 0x58C;
    // constexpr uintptr_t Gyro_FirstPerson        = 0x5B0;
    // constexpr uintptr_t Gyro_RedDot             = 0x590;
    // constexpr uintptr_t Gyro_2x                 = 0x594;
    // constexpr uintptr_t Gyro_3x                 = 0x5A0;
    // constexpr uintptr_t Gyro_4x                 = 0x598;
    // constexpr uintptr_t Gyro_6x                 = 0x5A4;
    // constexpr uintptr_t Gyro_8x                 = 0x59C;
    // ==================== 解密相关偏移 ====================
    constexpr uintptr_t Decrypt_Step1           = 0x1495C2A0;
    constexpr uintptr_t Decrypt_Step2_Offset1   = 0x30;
    constexpr uintptr_t Decrypt_Step3_Offset2   = 0x5D0;
    // ==================== 其他常用常量 ====================
    constexpr float HumanHeight                 = 205.0f;
    constexpr float MaxDrawDistance             = 1500.0f; //空投绘制距离
}