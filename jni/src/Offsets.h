#pragma once
#include <cstdint>
#include <string>

namespace Offsets
{
    // ==================== libUE4.so 模块基址偏移 ====================
    extern uintptr_t GWorld;
    extern uintptr_t GName;
    extern uintptr_t MatrixChain1;
    extern uintptr_t MatrixChain2;
    extern uintptr_t ClassBase;
    extern uintptr_t GyroBase;
    // ==================== GWorld 内部偏移 ====================
    extern uintptr_t GWorld_PersistentLevel;
    extern uintptr_t GWorld_ActorsArray;
    extern uintptr_t GWorld_ActorsCount;
    extern uintptr_t GWorld_ActorsCountDec;
    extern uintptr_t GWorld_GameState;
    // ==================== Actor 通用偏移 ====================
    extern uintptr_t Actor_RootComponent;
    extern uintptr_t Actor_TeamID;
    extern uintptr_t Actor_Health;
    extern uintptr_t Actor_HealthMax;
    extern uintptr_t Actor_PlayerName;
    extern uintptr_t Actor_PlayerUID;
    extern uintptr_t Actor_Velocity;
    extern uintptr_t Actor_Mesh;
    extern uintptr_t Actor_PawnState;
    extern uintptr_t Actor_bIsAI;
    extern uintptr_t Actor_bIsGunADS;
    extern uintptr_t Actor_bIsWeaponFiring;
    extern uintptr_t Actor_CurrentWeapon;
    extern uintptr_t Actor_SpeedValue;
    extern uintptr_t Actor_HighWalkSpeed;
    extern uintptr_t Actor_Rotator;
    extern uintptr_t Actor_Vehicle;
    extern uintptr_t Actor_WeaponEntity;
    extern uintptr_t Actor_ClassID;
    // ==================== 玩家控制器偏移 ====================
    extern uintptr_t Controller_Offset;
    extern uintptr_t Controller_CameraManager;
    extern uintptr_t Controller_AimYaw;
    // ==================== 相机管理器偏移 ====================
    extern uintptr_t CameraManager_CameraPos;
    extern uintptr_t CameraManager_Rotation;
    extern uintptr_t CameraManager_FOV;
    // ==================== POV 结构偏移（新） ====================
    extern uintptr_t POV_Location;
    extern uintptr_t POV_Rotation;
    extern uintptr_t POV_FOV;
    // ==================== 武器偏移 ====================
    extern uintptr_t Weapon_RepID;
    extern uintptr_t Weapon_EntityComp;
    extern uintptr_t Weapon_BulletSpeed;
    extern uintptr_t Weapon_RecoilFactor;
    extern uintptr_t Weapon_ClipAmmo;
    extern uintptr_t Weapon_ClipMaxAmmo;
    extern uintptr_t Weapon_GripID;
    extern uintptr_t Weapon_CachedBulletTrack;
    extern uintptr_t Weapon_ShootBursts;
    // ==================== 载具偏移 ====================
    extern uintptr_t Vehicle_CommonData;
    extern uintptr_t Vehicle_CurrentHP;
    extern uintptr_t Vehicle_MaxHP;
    extern uintptr_t Vehicle_CurrentFuel;
    extern uintptr_t Vehicle_MaxFuel;
    // ==================== 盒子/物资偏移 ====================
    extern uintptr_t BoxPickUpDataList;
    extern uintptr_t PickUpDataList;
    extern uintptr_t Box_OpenState;
    extern uintptr_t Box_Open;
    // ==================== Mesh/骨骼偏移 ====================
    extern uintptr_t Mesh_ComponentToWorld;
    extern uintptr_t Mesh_BoneArray;
    extern uintptr_t Mesh_BoneCountOffset;
    // ==================== 全图人数偏移 ====================
    extern uintptr_t AliveNum;
    extern uintptr_t AllivePlayerNum;
    extern uintptr_t AliveRealPlayerNum;
    extern uintptr_t AliveTeamNum;
    // ==================== 矩阵链偏移 ====================
    extern uintptr_t Matrix_ViewMatrix;
    extern uintptr_t Matrix_Tol_Offset1;
    extern uintptr_t Matrix_Tol_Offset2;
    // ==================== 解密相关偏移 ====================
    extern uintptr_t Decrypt_Step1;
    extern uintptr_t Decrypt_Step2_Offset1;
    extern uintptr_t Decrypt_Step3_Offset2;
    // ==================== 其他常量 ====================
    extern float HumanHeight;
    extern float MaxDrawDistance;

    // ==================== 加载函数 ====================
    bool LoadFromRemote(const std::string &url);
    bool LoadFromJson(const std::string &jsonStr);
}
