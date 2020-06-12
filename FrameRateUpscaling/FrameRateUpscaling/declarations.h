#pragma once
#define W 352
#define H 288
typedef unsigned char byte;

void GetSAD(byte* prev, byte* curr, int x, int y, int& best_dx, int& best_dy);
void MotionCompensation(byte* intrBuf, byte* currBuf, int x, int y, int best_dx, int best_dy, int frameWidth);