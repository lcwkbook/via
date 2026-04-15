#include <string>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_tricks.hpp"

using namespace ImGui;

inline int tab = 2;
inline int subtab = 1;
inline float content_anim = 0.f;
inline int rotation_start_index;

namespace custom
{
inline void ImRotateStart()
{
	rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

inline ImVec2 ImRotationCenter()
{
	ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

	const auto &buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

	return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}
inline void ImRotateEnd(float rad, ImVec2 center)
{
	float s = sin(rad), c = cos(rad);
	center = ImVec2(ImRotate(center, s, c).x - center.x, ImRotate(center, s, c).y - center.y);

	auto &buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		buf[i].pos = ImVec2(ImRotate(buf[i].pos, s, c).x - center.x, ImRotate(buf[i].pos, s, c).y - center.y);
}

inline bool tab(ImTextureID texture, const char *label, bool selected)
{
    constexpr float TAB_HEIGHT = 80.0f;  // 这个控制侧边栏的大小
    
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = GetCurrentWindow();

    ImGuiID id = window->GetID(label);

    ImVec2 p = window->DC.CursorPos;
    ImVec2 size({window->Size.x, TAB_HEIGHT});
    ImRect bb(p, ImVec2(p.x + size.x, p.y + size.y));

    ItemSize(bb);
    ItemAdd(bb, id);

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    float anim = ImTricks::Animations::FastFloatLerp(
        std::string(label).append("tab.anim"), 
        selected, 
        0.f, 
        1.f, 
        0.05f  // 插值因子(越大速度越快，越小速度越慢)
    );
    auto col = ImTricks::Animations::FastColorLerp(GetColorU32(ImGuiCol_TextDisabled), GetColorU32(ImGuiCol_Text), anim);

    if (pressed)
        content_anim = 0.f;

    static float line_pos = 0.f;
    line_pos = ImLerp(line_pos, selected ? bb.Min.y - window->Pos.y : line_pos, 0.04f);
    window->DrawList->AddRectFilled({bb.Max.x - 2, window->Pos.y + line_pos}, {bb.Max.x, window->Pos.y + line_pos + TAB_HEIGHT}, GetColorU32(ImGuiCol_Text, anim), 2, ImDrawFlags_RoundCornersLeft);

    // 绘制图片纹理
    const float icon_size = 50.0f; // 图片大小
    window->DrawList->AddImage(
        texture,
        {bb.Min.x + 19, bb.GetCenter().y - icon_size / 2},
        {bb.Min.x + 19 + icon_size, bb.GetCenter().y + icon_size / 2}
    );

    // 绘制标签文本
    PushStyleColor(ImGuiCol_Text, col.Value);
    RenderText({bb.Min.x + 100, bb.GetCenter().y - CalcTextSize(label).y / 2}, label);
    PopStyleColor();

    return pressed;
}





inline bool subtab(const char *label, bool selected)
{
	ImGuiContext &g = *GImGui;
	ImGuiWindow *window = GetCurrentWindow();

	ImGuiID id = window->GetID(label);

	ImVec2 p = window->DC.CursorPos;
	ImVec2 size({CalcTextSize(label, 0, true).x + 10, window->Size.y});
	ImRect bb(p, {p.x + size.x, p.y + size.y});

	ItemSize(bb);
	ItemAdd(bb, id);

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	float anim = ImTricks::Animations::FastFloatLerp(std::string(label).append("subtab.anim"), selected, 0.f, 1.f, 0.04f);
	auto col = ImTricks::Animations::FastColorLerp(GetColorU32(ImGuiCol_TextDisabled), GetColorU32(ImGuiCol_Text), anim);

	window->DrawList->AddRectFilled({bb.Min.x, bb.Max.y - 2}, bb.Max, GetColorU32(ImGuiCol_Text, anim), 2, ImDrawFlags_RoundCornersTop);

	PushStyleColor(ImGuiCol_Text, col.Value);
	RenderText(ImVec2(bb.GetCenter().x - CalcTextSize(label, 0, true).x / 2, bb.GetCenter().y - CalcTextSize(label, 0, true).y / 2), label);
	PopStyleColor();

	return pressed;
}

inline void begin_child(const char *name, ImVec2 size)
{
	ImGuiContext &g = *GImGui;
	ImGuiWindow *window = GetCurrentWindow();

	ImVec2 p = window->DC.CursorPos;

	BeginChild(std::string(name).append("main").c_str(), size, true, ImGuiWindowFlags_NoScrollbar);
	GetWindowDrawList()->AddText(nullptr, ImGui::GetFont()->FontSize, {p.x + 15, p.y + 15}, GetColorU32(ImGuiCol_Text), name);
	SetCursorPosY(50);
	BeginChild(name, {size.x, size.y == 0 ? size.y : size.y - 50}, false, ImGuiWindowFlags_NoBackground);
	SetCursorPosX(15);

	BeginGroup();

	PushStyleVar(ImGuiStyleVar_ItemSpacing, {10, 12});
}

inline void end_child()
{
	PopStyleVar();
	EndGroup();
	EndChild();
	EndChild();
}

inline bool collapse_button(bool collapsed)
{
	BeginChild("123", {40, 40});

	ImGuiWindow *window = GetCurrentWindow();

	ImGuiID id = window->GetID("collapse_button");

	ImVec2 p = window->DC.CursorPos;
	ImVec2 size(GetWindowSize());
	ImRect bb(p, ImVec2(p.x + size.x, p.y + size.y));

	ItemSize(bb);
	ItemAdd(bb, id);

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	window->DrawList->AddCircleFilled(bb.GetCenter(), bb.GetWidth() / 2, IM_COL32(50, 120, 200, 255)); // 修改背景颜色
	window->DrawList->AddCircle(bb.GetCenter(), bb.GetWidth() / 2, IM_COL32(150, 200, 250, 255));     // 修改边框颜色

	float anim = ImTricks::Animations::FastFloatLerp("collapse_button.anim", collapsed, 0.f, 1.f, 0.04f);

	// 调整箭头大小（原15.f → 20.f）和位置偏移（原4 → 6）
	ImRotateStart();
	RenderArrow(window->DrawList, 
		ImVec2(bb.GetCenter().x - ImVec2(6 + 6 * !anim, 15).x,  // 增加X轴偏移量
			   bb.GetCenter().y - ImVec2(6 + 6 * !anim, 15).y), // 增加Y轴偏移量
		IM_COL32(255, 255, 255, 255),  // 修改箭头颜色为白色
		ImGuiDir_Down, 
		20.f);  // 增大箭头尺寸
	ImRotateEnd(3.14f * 0.7f * anim, bb.GetCenter());  // 调整旋转角度系数

	EndChild();

	return pressed;
}

} // namespace custom