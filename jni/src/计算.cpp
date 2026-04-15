#include "辅助类.h"
extern 绘制 绘制;
int 计算::计算距离(FVector_class 自身坐标,FVector_class 对方坐标){
	return (int)sqrt(pow(自身坐标.X-对方坐标.X,2)
		+pow(自身坐标.Y-对方坐标.Y,2)
		+pow(自身坐标.Z-对方坐标.Z,2))*0.01;
}

D2DVector 计算::rotateCoord(float angle, float objRadar_x, float objRadar_y)
{
	D2DVector radarCoordinate;
	float s = sin(angle * 3.141592653589793238 / 180);
	float c = cos(angle * 3.141592653589793238 / 180);
	radarCoordinate.X = objRadar_x * c + objRadar_y * s;
	radarCoordinate.Y = -objRadar_x * s + objRadar_y * c;
	return radarCoordinate;
}

//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险//本项目仅用于学习和研究，不用于任何商业用途 否则自己承担所有风险

D2DVector 计算::rotateCoord(FVector_class Enemy, FVector_class RealPerson)
{
    float x = RealPerson.X - Enemy.X;
    float y = RealPerson.Y - Enemy.Y;
    float z = RealPerson.Z - Enemy.Z;
	D2DVector radarCoordinate;
	radarCoordinate.X = atan2(y,x)*180/3.141592653589793238;
	radarCoordinate.Y = atan2(z,sqrt(x*x+y*y)) * 180 / 3.141592653589793238;
	return radarCoordinate;
}

void 计算::GetDistance(D3DVector Object, D3DVector Self, float *Distance)
{
    float DistanceX = pow(Object.X - Self.X, 2);
    float DistanceY = pow(Object.Y - Self.Y, 2);
    float DistanceZ = pow(Object.Z - Self.Z, 2);
    *Distance = sqrt(DistanceX + DistanceY + DistanceZ) * 0.01f;
}

D4DVector 计算::计算屏幕坐标(float 矩阵[16],D3DVector 人物坐标,float px,float py){
	D4DVector 屏幕坐标;
	float camear_r = 矩阵[3] * 人物坐标.X + 矩阵[7] * 人物坐标.Y + 矩阵[11] * 人物坐标.Z + 矩阵[15];
	float r_x = px + (矩阵[0] * 人物坐标.X + 矩阵[4] * 人物坐标.Y + 矩阵[8] * 人物坐标.Z +矩阵[12]) / camear_r * px;
	float r_y = py - (矩阵[1] * 人物坐标.X + 矩阵[5] * 人物坐标.Y + 矩阵[9] * (人物坐标.Z - 5) +矩阵[13]) / camear_r * py;
	float r_z = py - (矩阵[1] * 人物坐标.X + 矩阵[5] * 人物坐标.Y + 矩阵[9] * (人物坐标.Z + 205) +矩阵[13]) / camear_r * py;
	屏幕坐标.X = r_x-(r_y-r_z)/4;
	屏幕坐标.Y = r_y;
	屏幕坐标.Z = (r_y-r_z)/2;
	屏幕坐标.W = r_y-r_z;
	return 屏幕坐标;
}

D2DVector 计算::计算屏幕坐标2(float 矩阵[16],D3DVector 人物坐标,float px,float py){
	D2DVector 屏幕坐标;
	float camear_r = 矩阵[3] * 人物坐标.X + 矩阵[7] * 人物坐标.Y + 矩阵[11] * 人物坐标.Z + 矩阵[15];
	屏幕坐标.X = px + (矩阵[0] * 人物坐标.X + 矩阵[4] * 人物坐标.Y + 矩阵[8] * 人物坐标.Z + 矩阵[12]) / camear_r * px;
    屏幕坐标.Y = py - (矩阵[1] * 人物坐标.X + 矩阵[5] * 人物坐标.Y + 矩阵[9] * 人物坐标.Z + 矩阵[13]) / camear_r * py;
	return 屏幕坐标;
}




float 计算::计算屏幕距离(D2DVector &坐标,float px,float py){
return sqrt(pow(px - 坐标.X, 2) + pow(py - 坐标.Y, 2));
}


骨骼数据 计算::计算骨骼(FVector_class 自身相机,FVector_class (&骨骼)[17],float px,float py){
骨骼数据 temp=
{
 绘制.WorldToScreen2(骨骼[0]),
 绘制.WorldToScreen2(骨骼[1]),
 绘制.WorldToScreen2(骨骼[2]),
 绘制.WorldToScreen2(骨骼[3]),
 绘制.WorldToScreen2(骨骼[4]),
 绘制.WorldToScreen2(骨骼[5]),
 绘制.WorldToScreen2(骨骼[6]),
 绘制.WorldToScreen2(骨骼[7]),
 绘制.WorldToScreen2(骨骼[8]),
 绘制.WorldToScreen2(骨骼[9]),
 绘制.WorldToScreen2(骨骼[10]),
 绘制.WorldToScreen2(骨骼[11]),
 绘制.WorldToScreen2(骨骼[12]),
 绘制.WorldToScreen2(骨骼[13]),
 绘制.WorldToScreen2(骨骼[14]),
};
return temp;
}
