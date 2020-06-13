#include <iostream>
#include <algorithm>
#include "declarations.h"
using namespace std;


void GetSAD(byte* prevY, byte* currY, int x, int y, int& best_dx, int& best_dy) {
	int SAD_min = 10000;
	best_dx = 100;
	best_dy = 100;
	int SAD_thresh = 20;

	for (int dx_temp = -15; dx_temp < 16; dx_temp++) {
		for (int dy_temp = -15; dy_temp < 16; dy_temp++) {

			int SAD_temp = 0;
			bool overflowFlag = 0;

			/* Compute SAD */
			for (int i = 0; i < 8; i++) {
				int idx_prevY = y + dy_temp + i;
				int idx_currY = y - dy_temp + i;

				if (idx_prevY < 0 || idx_prevY > H - 1 || idx_currY < 0 || idx_currY > H - 1) {
					/* If 16 is beyond the edge of frame, ignore this pixel */
					overflowFlag = 1;
					break;
				}


				for (int j = 0; j < 8; j++) {
					int idx_prevX = x + dx_temp + j;
					int idx_currX = x - dx_temp + j;

					if (idx_prevX < 0 || idx_prevX > W - 1 || idx_currX < 0 || idx_currX > W - 1) {
						overflowFlag = 1;
						break;
					}

					SAD_temp += abs(prevY[idx_prevY * W + idx_prevX] - currY[idx_currY * W + idx_currX]);
				}
			}

			if ((SAD_temp < SAD_min) && (overflowFlag == 0)) {
				/* Update if a smaller SAD is found and no overflow occured */
				SAD_min = SAD_temp;
				if (SAD_min > SAD_thresh) {
					best_dx = 0;
					best_dy = 0;
				} else {
					best_dx = dx_temp;
					best_dy = dy_temp;
				}
			}
		}
	}
}


void MotionCompensation(byte* intrBuf, byte* currBuf, int x, int y, int best_dx, int best_dy, int frameWidth) {
	for (int i = 0; i < 8; i++) {
		int idx_intrY = y + i;
		int idx_currY = y + best_dy + i;

		for (int j = 0; j < 8; j++) {
			int idx_intrX = x + j;
			int idx_currX = x + best_dx + j;

			intrBuf[idx_intrY * frameWidth + idx_intrX] = currBuf[idx_currY * frameWidth + idx_currX];
		}
	}
}