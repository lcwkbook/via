#pragma once


bool Touch_Init(int w, int h, uint32_t orientation_, bool readOnly);
void UpdateScreenData(int w, int h, uint32_t orientation_);

void Touch_Close();
void Touch_Down(float x, float y);
void Touch_Move(float x, float y);
void Touch_Up();

void Touch_Down1(float x, float y);
void Touch_Move1(float x, float y);
void Touch_Up1();
