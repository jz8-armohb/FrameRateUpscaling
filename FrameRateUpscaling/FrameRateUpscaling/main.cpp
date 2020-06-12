#include <iostream>
#include "declarations.h"
using namespace std;

int main(int argc, char** argv) {
	/********** Preparations **********/
	FILE* srcFp;
	FILE* dstFp;
	const char* srcFileName = "miss.cif";
	const char* dstFileName = "miss_x2fps.cif";
	int srcFileSize;
	int frameSize = W * H * 3 / 2;
	int yFrameSize = W * H;
	int chrFrameSize = W * H / 4;	// Chrominance frame size
	int frameCnt;

	/* Open files */
	if (fopen_s(&srcFp, srcFileName, "rb") == 0) {
		cout << "Successfully opened '" << srcFileName << "'.\n";
	} else {
		cout << "WARNING!! Failed to open '" << srcFileName << "'.\n";
	}
	if (fopen_s(&dstFp, dstFileName, "wb") == 0) {
		cout << "Successfully opened '" << dstFileName << "'.\n";
	} else {
		cout << "WARNING!! Failed to open '" << dstFileName << "'.\n";
	}

	/* Space allocation */	
	byte* prevY = new byte[yFrameSize];
	byte* currY = new byte[yFrameSize];
	byte* intrY = new byte[yFrameSize];
	byte* prevU = new byte[yFrameSize];
	byte* currU = new byte[chrFrameSize];
	byte* intrU = new byte[chrFrameSize];
	byte* prevV = new byte[chrFrameSize];
	byte* currV = new byte[chrFrameSize];
	byte* intrV = new byte[chrFrameSize];

	/* Compute the size of original video file */
	fseek(srcFp, 0L, SEEK_END);
	srcFileSize = ftell(srcFp);
	rewind(srcFp);
	cout << "Size of source file: " << (double)srcFileSize / 1024 << " kB.\n";
	frameCnt = srcFileSize / frameSize;
	cout << "Total frame count: " << frameCnt << ".\n";


	/********** Frame rate upscaling **********/
	/* Read the first frame into buffers, and directly write into destination file */
	fread(prevY, sizeof(byte), yFrameSize, srcFp);
	fread(prevU, sizeof(byte), chrFrameSize, srcFp);
	fread(prevV, sizeof(byte), chrFrameSize, srcFp);
	fwrite(prevY, sizeof(byte), yFrameSize, dstFp);
	fwrite(prevU, sizeof(byte), chrFrameSize, dstFp);
	fwrite(prevV, sizeof(byte), chrFrameSize, dstFp);

	int xBlockNum = W / 8;
	int yBlockNum = H / 8;

	/* For each frame */
	for (int frameNum = 1; frameNum < frameCnt - 1; frameNum++) {
		fread(currY, sizeof(byte), yFrameSize, srcFp);
		fread(currU, sizeof(byte), chrFrameSize, srcFp);
		fread(currV, sizeof(byte), chrFrameSize, srcFp);

		for (int i = 0; i < H - 7; i += 8) {
			for (int j = 0; j < W - 7; j += 8) {
				int best_dx, best_dy;
				GetSAD(prevY, currY, j, i, best_dx, best_dy);
				MotionCompensation(intrY, currY, j, i, best_dx, best_dy, W);
				MotionCompensation(intrU, currU, j / 2, i / 2, best_dx / 2, best_dy / 2, W / 2);
				MotionCompensation(intrV, currV, j / 2, i / 2, best_dx / 2, best_dy / 2, W / 2);
			}
		}
		/* Write interpolated frame into destination file */
		fwrite(intrY, sizeof(byte), yFrameSize, dstFp);
		fwrite(intrU, sizeof(byte), chrFrameSize, dstFp);
		fwrite(intrV, sizeof(byte), chrFrameSize, dstFp);

		/* Write current frame into destination file */
		fwrite(currY, sizeof(byte), yFrameSize, dstFp);
		fwrite(currU, sizeof(byte), chrFrameSize, dstFp);
		fwrite(currV, sizeof(byte), chrFrameSize, dstFp);

		/* Set current frames as previous frames */
		for (int i = 0; i < yFrameSize; i++) {
			prevY[i] = currY[i];
		}
		for (int i = 0; i < chrFrameSize; i++) {
			prevU[i] = currU[i];
			prevV[i] = prevV[i];
		}
	}



	/* Close files and clear buffers */
	//fclose(srcFp);
	//fclose(dstFp);
	//delete[]currY;
	//delete[]currU;
	//delete[]currV;
	//delete[]prevY;
	//delete[]prevU;
	//delete[]prevV;
	//delete[]intrY;
	//delete[]intrU;
	//delete[]intrV;
}