#include "辅助类.h"
#include "辅助类.h"
#include "map"

class 骨骼
{
public:
  std::unordered_map<std::string, std::string> BossName;
  std::vector<std::string> BossArray;
  Kernel *读写;

  骨骼(Kernel *读写) : 读写(读写)
  {
    BossName = {
        {"Pawn_Escape_RD_Grenade_C", "巡卫长·玄铁"},
        {"Pawn_Escape_RD_RoyalGuards_C", "影卫·银星"},
        {"Pawn_Escape_Boss_Robocop_C", "V-34机械警"},
        {"Pawn_Escape_BOSS_Claws_C", "钢爪·安德烈"},
        {"Pawn_Escape_RD_SupplyBoss_C", "辎重使·墨守"},
    };
  }

  bool isBoss(char &className)
  {
    for (size_t i = 0; i < BossArray.size(); i++)
    {
      if (strstr((char *)className, BossArray[i].c_str()))
        return true;
    }
    return false;
  }

  bool getBossName(char &className, std::string &name)
  {
    auto point = this->BossName.find((char *)className);
    if (point != this->BossName.end())
    {
      name = point->second;
      return true;
    }
    return false;
  }

  FTransform getBone(uintptr_t addr)
  {
    FTransform transform;
    读写->readv(addr, &transform, sizeof(FTransform));
    return transform;
  }

  D3DVector MarixToVector(FMatrix matrix)
  {
    return D3DVector{matrix.M[3][0], matrix.M[3][1], matrix.M[3][2]};
  }

  FMatrix MatrixMulti(const FMatrix &m1, const FMatrix &m2)
  {
    FMatrix matrix;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        matrix.M[i][j] = 0;
        for (int k = 0; k < 4; k++)
        {
          matrix.M[i][j] += m1.M[i][k] * m2.M[k][j];
        }
      }
    }
    return matrix;
  }

  FMatrix TransformToMatrix(FTransform transform)
  {
    FMatrix matrix;
    matrix.M[3][0] = transform.Translation.X;
    matrix.M[3][1] = transform.Translation.Y;
    matrix.M[3][2] = transform.Translation.Z;
    float x2 = transform.Rotation.X + transform.Rotation.X;
    float y2 = transform.Rotation.Y + transform.Rotation.Y;
    float z2 = transform.Rotation.Z + transform.Rotation.Z;
    float xx2 = transform.Rotation.X * x2;
    float yy2 = transform.Rotation.Y * y2;
    float zz2 = transform.Rotation.Z * z2;
    matrix.M[0][0] = (1 - (yy2 + zz2)) * transform.Scale3D.X;
    matrix.M[1][1] = (1 - (xx2 + zz2)) * transform.Scale3D.Y;
    matrix.M[2][2] = (1 - (xx2 + yy2)) * transform.Scale3D.Z;
    float yz2 = transform.Rotation.Y * z2;
    float wx2 = transform.Rotation.W * x2;
    matrix.M[2][1] = (yz2 - wx2) * transform.Scale3D.Z;
    matrix.M[1][2] = (yz2 + wx2) * transform.Scale3D.Y;
    float xy2 = transform.Rotation.X * y2;
    float wz2 = transform.Rotation.W * z2;
    matrix.M[1][0] = (xy2 - wz2) * transform.Scale3D.Y;
    matrix.M[0][1] = (xy2 + wz2) * transform.Scale3D.X;
    float xz2 = transform.Rotation.X * z2;
    float wy2 = transform.Rotation.W * y2;
    matrix.M[2][0] = (xz2 + wy2) * transform.Scale3D.Z;
    matrix.M[0][2] = (xz2 - wy2) * transform.Scale3D.X;
    matrix.M[0][3] = 0;
    matrix.M[1][3] = 0;
    matrix.M[2][3] = 0;
    matrix.M[3][3] = 1;
    return matrix;
  }

  D3DXMATRIX ToMatrixWithScale(D3DXVECTOR4 Rotation, D3DVector Translation, D3DVector Scale3D)
  {
    D3DXMATRIX M;
    float X2, Y2, Z2, xX2, Yy2, Zz2, Zy2, Wx2, Xy2, Wz2, Zx2, Wy2;
    M._41 = Translation.X;
    M._42 = Translation.Y;
    M._43 = Translation.Z;
    X2 = Rotation.X + Rotation.X;
    Y2 = Rotation.Y + Rotation.Y;
    Z2 = Rotation.Z + Rotation.Z;
    xX2 = Rotation.X * X2;
    Yy2 = Rotation.Y * Y2;
    Zz2 = Rotation.Z * Z2;
    M._11 = (1 - (Yy2 + Zz2)) * Scale3D.X;
    M._22 = (1 - (xX2 + Zz2)) * Scale3D.Y;
    M._33 = (1 - (xX2 + Yy2)) * Scale3D.Z;
    Zy2 = Rotation.Y * Z2;
    Wx2 = Rotation.W * X2;
    M._32 = (Zy2 - Wx2) * Scale3D.Z;
    M._23 = (Zy2 + Wx2) * Scale3D.Y;
    Xy2 = Rotation.X * Y2;
    Wz2 = Rotation.W * Z2;
    M._21 = (Xy2 - Wz2) * Scale3D.Y;
    M._12 = (Xy2 + Wz2) * Scale3D.X;
    Zx2 = Rotation.X * Z2;
    Wy2 = Rotation.W * Y2;
    M._31 = (Zx2 + Wy2) * Scale3D.Z;
    M._13 = (Zx2 - Wy2) * Scale3D.X;
    M._14 = 0;
    M._24 = 0;
    M._34 = 0;
    M._44 = 1;
    return M;
  }

  FTransform1 ReadFTransform(long int address)
  {
    FTransform1 Result;
    Result.Rotation.X = 读写->getFloat(address);         // Rotation_X
    Result.Rotation.Y = 读写->getFloat(address + 4);     // Rotation_y
    Result.Rotation.Z = 读写->getFloat(address + 8);     // Rotation_z
    Result.Rotation.W = 读写->getFloat(address + 12);    // Rotation_w
    Result.Translation.X = 读写->getFloat(address + 16); // /Translation_X
    Result.Translation.Y = 读写->getFloat(address + 20); // Translation_y
    Result.Translation.Z = 读写->getFloat(address + 24); // Translation_z
    Result.Scale3D.X = 读写->getFloat(address + 32);
    ; // Scale_X
    Result.Scale3D.Y = 读写->getFloat(address + 36);
    ; // Scale_y
    Result.Scale3D.Z = 读写->getFloat(address + 40);
    ; // Scale_z
    return Result;
  }

  // 获取骨骼3d坐标
  D3DVector D3dMatrixMultiply(D3DXMATRIX bonematrix, D3DXMATRIX actormatrix)
  {
    D3DVector result;
    result.X =
        bonematrix._41 * actormatrix._11 + bonematrix._42 * actormatrix._21 +
        bonematrix._43 * actormatrix._31 + bonematrix._44 * actormatrix._41;
    result.Y =
        bonematrix._41 * actormatrix._12 + bonematrix._42 * actormatrix._22 +
        bonematrix._43 * actormatrix._32 + bonematrix._44 * actormatrix._42;
    result.Z =
        bonematrix._41 * actormatrix._13 + bonematrix._42 * actormatrix._23 +
        bonematrix._43 * actormatrix._33 + bonematrix._44 * actormatrix._43;
    return result;
  }

  D3DVector getBoneXYZ(long int humanAddr, long int boneAddr, int Part)
  {
    // 获取Bone数据
    FTransform1 Bone = ReadFTransform(boneAddr + Part * 48);
    // 获取Actor数据
    FTransform1 Actor = ReadFTransform(humanAddr);
    D3DXMATRIX Bone_Matrix = ToMatrixWithScale(Bone.Rotation, Bone.Translation, Bone.Scale3D);
    D3DXMATRIX Component_ToWorld_Matrix =
        ToMatrixWithScale(Actor.Rotation, Actor.Translation, Actor.Scale3D);
    D3DVector result = D3dMatrixMultiply(Bone_Matrix, Component_ToWorld_Matrix);
    return result;
  }

  D2DVector getPointingAngle(long int SelfAddress, float object_x, float object_y, float object_z, float Self_x, float Self_y, float Self_z, D3DVector Movement, float distance, float 预判力度)
  {
    D2DVector PointingAngle;
    float bulletVelocity = 读写->getFloat(读写->getPtr64(读写->getPtr64(SelfAddress + 0x10c8) + 0xb60) + 0x158c); // 子弹速度
    float FlyTime = distance / (bulletVelocity * 0.01f) * 预判力度;

    // float FlyTime = (distance >= 60) ? (distance / (bulletVelocity * 0.01f) * 预判力度) : (distance / (bulletVelocity * 0.0055f) * 预判力度);

    float DropM = 500.0f * FlyTime * FlyTime;
    float zbcx = object_x + (Movement.X * FlyTime) - Self_x;
    float zbcy = object_y + (Movement.Y * FlyTime) - Self_y;
    float zbcz = object_z + (Movement.Z * FlyTime * FlyTime) - Self_z;
    long int pfg = sqrt((zbcx * zbcx) + (zbcy * zbcy));
    PointingAngle.X = atan2(zbcy, zbcx) * 180 / 3.141592653589793238;
    PointingAngle.Y = atan2(zbcz, pfg) * 180 / 3.141592653589793238;
    return PointingAngle;
  }

  // ============================================================
  // 自动识别骨骼索引（终极版）
  // 根据15个骨骼的世界坐标位置分布，自动识别出正确的索引
  // 无需任何硬编码，兼容所有角色模型
  // ============================================================
  std::vector<int> 自动识别骨骼索引(D3DVector *所有骨骼位置, int 骨骼数量)
  {
    // 输出: [0头,1胸,2盆骨,3左肩,4右肩,5左肘,6右肘,7左腕,8右腕,
    //        9左大腿,10右大腿,11左膝,12右膝,13左踝,14右踝]
    std::vector<int> 结果(15, -1);

    // ===== 第一阶段：过滤无效骨骼 =====
    // 找出最高点（头的位置）
    float 最高Z = -99999.0f;
    for (int i = 0; i < 骨骼数量; i++)
      if (所有骨骼位置[i].Z > 最高Z)
        最高Z = 所有骨骼位置[i].Z;

    // 收集有效骨骼（在角色高度范围内的）
    struct 骨骼信息
    {
      int 索引;
      D3DVector 位置;
      float 距中心距离;
    };
    std::vector<骨骼信息> 有效骨骼;
    for (int i = 0; i < 骨骼数量; i++)
    {
      // 跳过无效骨骼（在地面附近的）
      if ((最高Z - 所有骨骼位置[i].Z) > 280.0f)
        continue;
      // 跳过原点附近的
      if (fabs(所有骨骼位置[i].X) < 0.5f && fabs(所有骨骼位置[i].Y) < 0.5f)
        continue;
      有效骨骼.push_back({i, 所有骨骼位置[i], 0});
    }
    if (有效骨骼.size() < 15)
    { // 过滤太狠了就放宽
      有效骨骼.clear();
      for (int i = 0; i < 骨骼数量; i++)
        有效骨骼.push_back({i, 所有骨骼位置[i], 0});
    }

    // ===== 第二阶段：计算中心点 =====
    float 中心X = 0, 中心Y = 0;
    for (auto &b : 有效骨骼)
    {
      中心X += b.位置.X;
      中心Y += b.位置.Y;
    }
    中心X /= 有效骨骼.size();
    中心Y /= 有效骨骼.size();

    // 计算每个骨骼到中心的距离
    for (auto &b : 有效骨骼)
      b.距中心距离 = sqrtf(powf(b.位置.X - 中心X, 2) + powf(b.位置.Y - 中心Y, 2));

    // ===== 第三阶段：按Z高度从高到低排序 =====
    std::sort(有效骨骼.begin(), 有效骨骼.end(), [](auto &a, auto &b)
              { return a.位置.Z > b.位置.Z; });

    float 最高 = 有效骨骼[0].位置.Z;
    float 最低 = 有效骨骼.back().位置.Z;
    float 范围 = 最高 - 最低;
    if (范围 < 50.0f)
      return 结果; // 高度范围太小，不可能是一个站立人物

    // ===== 第四阶段：识别各个身体部位 =====

    // --- 4.1 头：全局最高点 ---
    结果[0] = 有效骨骼[0].索引;

    // --- 4.2 胸：靠近顶部且在中心的骨骼 ---
    float 胸高度上限 = 最高;
    float 胸高度下限 = 最高 - 范围 * 0.20f;
    float 最小距离 = 99999.0f;
    for (auto &b : 有效骨骼)
    {
      if (b.索引 == 结果[0])
        continue;
      if (b.位置.Z < 胸高度下限)
        break;
      if (b.距中心距离 < 最小距离)
      {
        最小距离 = b.距中心距离;
        结果[1] = b.索引;
      }
    }
    if (结果[1] == -1)
    { // 兜底：用第二高的
      for (auto &b : 有效骨骼)
      {
        if (b.索引 != 结果[0])
        {
          结果[1] = b.索引;
          break;
        }
      }
    }

    // --- 4.3 盆骨：中间高度的中心骨骼 ---
    float 盆骨目标高 = 最高 - 范围 * 0.45f;
    最小距离 = 99999.0f;
    for (auto &b : 有效骨骼)
    {
      if (b.索引 == 结果[0] || b.索引 == 结果[1])
        continue;
      float 离目标 = fabs(b.位置.Z - 盆骨目标高);
      float 评分 = 离目标 * 0.6f + b.距中心距离 * 0.4f;
      if (评分 < 最小距离)
      {
        最小距离 = 评分;
        结果[2] = b.索引;
      }
    }

    // --- 4.4 左肩(3)、右肩(4)、左肘(5)、右肘(6)、左腕(7)、右腕(8) ---
    float 盆骨Z = 所有骨骼位置[结果[2]].Z;

    // 收集上半身骨骼（胸以下、盆骨以上的区域内，排除已识别的中心骨骼）
    std::vector<骨骼信息> 上半身;
    float 胸Z = 所有骨骼位置[结果[1]].Z;
    for (auto &b : 有效骨骼)
    {
      if (b.索引 == 结果[0] || b.索引 == 结果[1] || b.索引 == 结果[2])
        continue;
      // 上半身范围：从胸稍下到盆骨稍上
      if (b.位置.Z < 胸Z + 范围 * 0.05f && b.位置.Z > 盆骨Z - 范围 * 0.05f)
        上半身.push_back(b);
    }

    // 如果上半身骨骼不够，放宽范围
    if (上半身.size() < 6)
    {
      上半身.clear();
      for (auto &b : 有效骨骼)
      {
        if (b.索引 == 结果[0] || b.索引 == 结果[1] || b.索引 == 结果[2])
          continue;
        if (b.位置.Z < 最高 - 范围 * 0.1f && b.位置.Z > 盆骨Z - 范围 * 0.1f)
          上半身.push_back(b);
      }
    }

    // 上半身按距中心距离从远到近排序（远的是手，近的是肩膀）
    std::sort(上半身.begin(), 上半身.end(), [](auto &a, auto &b)
              { return a.距中心距离 > b.距中心距离; });

    // 从远到近分配：手腕(最远2个)→手肘(次远2个)→肩膀(最近2个)
    // 左右区分：X < 中心X = 左，X >= 中心X = 右
    for (size_t i = 0; i < 上半身.size() && i < 6; i++)
    {
      int &目标索引 = (i < 2) ? (上半身[i].位置.X < 中心X ? 结果[7] : 结果[8]) : // 手腕
                          (i < 4) ? (上半身[i].位置.X < 中心X ? 结果[5] : 结果[6])
                                  :                                       // 手肘
                          (上半身[i].位置.X < 中心X ? 结果[3] : 结果[4]); // 肩膀
      if (目标索引 == -1)
        目标索引 = 上半身[i].索引;
    }

    // --- 4.5 下半身：左大腿(9)、右大腿(10)、左膝(11)、右膝(12)、左踝(13)、右踝(14) ---
    std::vector<骨骼信息> 下半身;
    for (auto &b : 有效骨骼)
    {
      bool 已用 = false;
      for (int j = 0; j < 9; j++)
        if (b.索引 == 结果[j])
        {
          已用 = true;
          break;
        }
      if (已用)
        continue;
      if (b.位置.Z <= 盆骨Z + 范围 * 0.1f)
        下半身.push_back(b);
    }

    if (下半身.size() >= 6)
    {
      // 按距中心距离从近到远排序（腿靠近中心，脚踝略远）
      std::sort(下半身.begin(), 下半身.end(), [](auto &a, auto &b)
                { return a.距中心距离 < b.距中心距离; });

      // 靠近中心的=大腿/膝盖区域，按Z从高到低分
      std::sort(下半身.begin(), 下半身.end(), [](auto &a, auto &b)
                { return a.位置.Z > b.位置.Z; });

      int n = 下半身.size();
      int 每段 = n / 3;
      if (每段 < 2)
        每段 = 2;

      for (int i = 0; i < n; i++)
      {
        int 段 = (i < 每段) ? 0 : (i < 每段 * 2) ? 1
                                                 : 2;
        int &目标 = (段 == 0) ? (下半身[i].位置.X < 中心X ? 结果[9] : 结果[10]) : // 大腿
                        (段 == 1) ? (下半身[i].位置.X < 中心X ? 结果[11] : 结果[12])
                                  :                                       // 膝盖
                        (下半身[i].位置.X < 中心X ? 结果[13] : 结果[14]); // 脚踝
        if (目标 == -1)
          目标 = 下半身[i].索引;
      }
    }
    else if (下半身.size() >= 4)
    {
      // 少一些骨骼的情况，分为2段
      std::sort(下半身.begin(), 下半身.end(), [](auto &a, auto &b)
                { return a.位置.Z > b.位置.Z; });
      int n = 下半身.size();
      int 每段 = n / 2;
      for (int i = 0; i < n; i++)
      {
        int 段 = (i < 每段) ? 0 : 1;
        int &目标 = (段 == 0) ? (下半身[i].位置.X < 中心X ? 结果[9] : 结果[10]) : (下半身[i].位置.X < 中心X ? 结果[13] : 结果[14]);
        if (目标 == -1)
          目标 = 下半身[i].索引;
      }
    }

    return 结果;
  }

  void 更新骨骼数据(uintptr_t MeshAddress, uintptr_t Bone, D3DVector (&骨骼坐标)[17],
                    int Bonecount, int Team, char *类名)
  {
    // ★★★ 安全防护 ★★★
    if (Bonecount <= 5 || Bonecount > 200)
    {
      memset(骨骼坐标, 0, sizeof(D3DVector) * 17);
      return;
    }

    FTransform meshtrans = getBone(MeshAddress);
    FMatrix c2wMatrix = TransformToMatrix(meshtrans);

    // 一次性读取所有骨骼位置（仅用于调试/识别新模型）
    // 正式使用时去掉这个循环，只读需要的15个
    std::vector<int> boneIndices;
    bool isboss = isBoss(*类名);

    if (!isboss)
    {
      if (Bonecount == 68 || Bonecount == 69)
      {
        boneIndices = {5, 4, 1, 11, 33, 12, 34, 13, 35, 55, 59, 56, 60, 57, 61};
      }
      else if (Bonecount == 70)
      {
        // BP_TrainPlayerPawn_C 特殊布局：头在29/30，胸在5，盆骨在0
        boneIndices = {30, 5, 0, 31, 33, 7, 34, 8, 35, 55, 59, 56, 60, 57, 61};
      }
      else if (Bonecount == 71)
      {
        boneIndices = {5, 4, 1, 6, 34, 7, 35, 8, 36, 55, 59, 56, 60, 57, 61};
      }
      else if (Bonecount == 73)
      {
        boneIndices = {5, 4, 1, 12, 34, 13, 35, 14, 36, 57, 61, 58, 62, 59, 63};
      }
      // ... 你的其他分支 ...
      else if (Bonecount == 65)
      {
        // 基于数据：头=7(2330.6), 胸=4(2307.5), 盆骨=0(2255.7)
        // 但需要更多数据来确认肩膀/手肘/手腕/腿的索引
        boneIndices = {7, 4, 0, 31, 33, 11, 34, 13, 35, 53, 55, 53, 54, 57, 58};
      }
      else if (Bonecount == 63)
      {
        boneIndices = {5, 4, 0, 8, 30, 9, 31, 10, 32, 50, 54, 51, 55, 52, 56};
      }
      else if (Bonecount == 64)
      {
        boneIndices = {5, 4, 0, 7, 29, 8, 30, 9, 31, 51, 57, 52, 58, 53, 59};
      }
      else if (Bonecount == 38)
      {
        boneIndices = {5, 4, 0, 7, 17, 9, 19, 11, 21, 27, 34, 28, 35, 30, 36};
      }
      else if (Bonecount == 30)
      {
        boneIndices = {5, 4, 0, 9, 14, 10, 15, 11, 16, 18, 22, 19, 23, 20, 24};
      }
      else if (Bonecount == 26)
      {
        boneIndices = {5, 4, 0, 13, 6, 14, 7, 15, 8, 21, 18, 22, 19, 23, 20};
      }
      else if (Bonecount == 27)
      {
        boneIndices = {5, 4, 0, 7, 10, 8, 11, 9, 12, 14, 18, 15, 19, 16, 20};
      }
      else if (Bonecount == 46)
      {
        boneIndices = {5, 4, 0, 21, 8, 22, 9, 23, 10, 35, 40, 36, 41, 38, 44};
      }
      else if (Bonecount == 42)
      {
        boneIndices = {16, 15, 0, 20, 31, 21, 32, 22, 33, 3, 8, 4, 9, 5, 10};
      }
      else if (Bonecount == 76)
      {
        boneIndices = {5, 4, 0, 7, 27, 8, 28, 9, 29, 48, 52, 49, 53, 50, 54};
      }
      else if (Bonecount == 61)
      {
        boneIndices = {5, 4, 0, 6, 27, 7, 28, 8, 29, 48, 52, 49, 53, 50, 54};
      }
      else
      {
        // ★ 安全默认值：保证不越界 ★
        int safeIdx[] = {
            min(5, Bonecount - 1), min(4, Bonecount - 1), min(1, Bonecount - 1),
            min(11, Bonecount - 1), min(32, Bonecount - 1),
            min(12, Bonecount - 1), min(33, Bonecount - 1),
            min(13, Bonecount - 1), min(35, Bonecount - 1),
            min(55, Bonecount - 1), min(59, Bonecount - 1),
            min(56, Bonecount - 1), min(60, Bonecount - 1),
            min(57, Bonecount - 1), min(61, Bonecount - 1)};
        boneIndices.assign(safeIdx, safeIdx + 15);
      }
    }
    else
    {
      std::string name;
      getBossName(*类名, name);
      if (name == "钢爪·安德烈")
        boneIndices = {5, 4, 1, 28, 7, 29, 8, 30, 9, 57, 50, 58, 51, 59, 52};
      else
        boneIndices = {5, 4, 1, 28, 7, 29, 8, 30, 9, 52, 48, 53, 49, 54, 50};
    }

    // ==== 逐个读取15个骨骼坐标（不读全部骨骼，省内存！）====
    for (size_t i = 0; i < boneIndices.size() && i < 15; i++)
    {
      int idx = boneIndices[i];
      if (idx >= 0 && idx < Bonecount)
      {
        FTransform boneTrans = getBone(Bone + idx * 48);
        FMatrix boneMatrix = TransformToMatrix(boneTrans);
        骨骼坐标[i] = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
      }
    }
    骨骼坐标[0].Z += 7;
    if (骨骼坐标[13].Z < 骨骼坐标[14].Z)
      骨骼坐标[14].Z = 骨骼坐标[13].Z;
  }
};