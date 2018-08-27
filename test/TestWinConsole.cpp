#include <windows.h>
#include <cmath>
#include <cstring>
#include "BeardRenderer.h"
#include "BeardMatrix.h"

#ifndef DISABLE_NEWLINE_AUTO_RETURN
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008 //should be defined int wincon.h but my compiler complains
#endif //DISABLE_NEWLINE_AUTO_RETURN

//TODO find a way to determine the character w/h ratio, this shall be valid just on my machine
#define CHAR_RATIO 0.5

const float _triangles[] = {
-1.0f, 1.0f, -1.0f,
1.0f, 0.0f, 1.0f,
0.5f, -1.0f, 1.0f,
1.0f, 1.0f, -1.0f,
-1.0f, 0.0f, 1.0f,
-0.5f, -1.0f, 1.0f,
0.0f, 1.0f, 1.0f,
1.0f, -0.5f, -1.0f,
-1.0f, -0.5f, -1.0f
};

int main() {
	int T, t;
	LARGE_INTEGER large;
	HANDLE out;
	CONSOLE_SCREEN_BUFFER_INFO bufInfo, newInfo;
	BRC brc;
	COORD coord, coord0;
	int w, h, p;
	float a[16], b[16], c[16];
	double radius = 0.0;
	float vertices[4*3*3];

	coord0.X = 0;
	coord0.Y = 0;

	QueryPerformanceFrequency(&large);
	T = *(int*)(void*)&large;
	
	FreeConsole();
	AllocConsole();
	out = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleMode(out, ENABLE_WRAP_AT_EOL_OUTPUT|DISABLE_NEWLINE_AUTO_RETURN);
	GetConsoleScreenBufferInfo(out, &bufInfo);
	w = bufInfo.srWindow.Right - bufInfo.srWindow.Left;
	h = bufInfo.srWindow.Bottom - bufInfo.srWindow.Top;
	p = w + 1;
	if(!BRCInit(&brc, VirtualAlloc(0, p * h, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE),
		VirtualAlloc(0, w * h, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE),
		w, h, p, w, 8, 8)) {
		MessageBox(0, "!", "error", MB_OK);
		return 1;
	}
	
	QueryPerformanceCounter(&large);
	t = *(int*)(void*)&large;
	while(1) {
		GetConsoleScreenBufferInfo(out, &newInfo);
		if(memcmp(&bufInfo, &newInfo, sizeof(bufInfo))) {
			int newW, newH;
			newW = newInfo.srWindow.Right - newInfo.srWindow.Left;
			newH = newInfo.srWindow.Bottom - newInfo.srWindow.Top;
			if((newW + 1) * newH / 4096 != p * h / 4096) {
				VirtualFree(brc.buffer, 0, MEM_RELEASE);
				VirtualFree(brc.zBuffer, 0, MEM_RELEASE);
				brc.buffer = VirtualAlloc(0, (newW + 1) * newH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				brc.zBuffer = VirtualAlloc(0, newW * newH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			}
			memcpy(&bufInfo, &newInfo, sizeof(bufInfo));
			w = brc.width = brc.zPitch = newW;
			h = brc.height = newH;
			p = brc.pitch = w + 1;
		}
		
		//render
		clearZbuf(&brc);
		background(&brc, ' ');
		
		rotateY(a, radius);
		camera(b, 1.5, 1, 1.5, 0, 0, 0, 0, 1, 0);
		mulmat(b, a, c, 4, 4, 4);
		perspective(b, M_PI / 3.0, (float)w/(float)h*CHAR_RATIO, 0.1, 5.0);
		mulmat(b, c, a, 4, 4, 4);
		applyMatrixToVec3(vertices, _triangles, 3*3, a);
		radius += M_PI / 120.0;
		
		for(int i = 0; i < 3; i++)
			triangle(&brc, vertices + i * 12, vertices + i * 12 + 4, vertices + i * 12 + 8, 0x30 + i - 1);//0xB0 + i
		
		//update
		SetConsoleCursorPosition(out, coord0);
		WriteConsole(out, brc.buffer, p * h, 0, 0);
		
		//sync TODO this huge CPU time-consuming method is so bad
		do QueryPerformanceCounter(&large);
		while(T / (*(int*)(void*)&large - t) > 30);
		t = *(int*)(void*)&large;
	}
}