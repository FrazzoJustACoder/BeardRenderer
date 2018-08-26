#include <windows.h>
#include <math.h>
#include "BeardMatrix.h"
#include "BeardRenderer.h"
#include "WinDebug.h"

//window
HWND hwnd;
HINSTANCE hInst;
const char _class[] = "aeaffei";
const char _title[] = "BR test";
RECT client;
#define TIMER_ID 10
int timerPeriod = 15;
#define DELTA_PERIOD 1

//stuff
BRC dc1, dc2, dc3, dc4;
HBITMAP buffer;
int showZbuf;
double radius;

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

long int _stdcall WndProc(HWND, UINT, WPARAM, LPARAM);

int main() {
	//debug
	initDebug(GetStdHandle(STD_OUTPUT_HANDLE));
	LARGE_INTEGER t;
	QueryPerformanceFrequency(&t);
	nl();
	print("frequency: ");
	printi(*(int*)(void*)(&t));
	nl();
	
	//window
	hInst = GetModuleHandle(0);
	WNDCLASS wc = { };
	wc.hInstance = hInst;
	wc.lpszClassName = _class;
	wc.lpfnWndProc = WndProc;
	RegisterClass(&wc);
	hwnd = CreateWindow(_class, _title, WS_VISIBLE|WS_SYSMENU,
		100, 100, 646, 509, 0, 0, hInst, 0);
	
	//stuff
	showZbuf = 0;
	radius = 0.0;
	GetClientRect(hwnd, &client);
	printi(client.right);
	print("x");
	printi(client.bottom);
	nl();
	void *tmp1 = VirtualAlloc(0, client.right * client.bottom * 4, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	void *tmp2 = VirtualAlloc(0, client.right * client.bottom * 4, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	BRCInit(&dc1, tmp1, tmp2, client.right >> 1, client.bottom >> 1, client.right << 2, client.right << 2, 32, 32);
	BRCInit(&dc2, tmp1, tmp2, client.right >> 1, client.bottom >> 1, client.right << 2, client.right << 2, 32, 32);
	BRCInit(&dc3, tmp1, tmp2, client.right >> 1, client.bottom >> 1, client.right << 2, client.right << 2, 32, 32);
	BRCInit(&dc4, tmp1, tmp2, client.right >> 1, client.bottom >> 1, client.right << 2, client.right << 2, 32, 32);
	dc2.buffer = (int*)dc1.buffer + dc1.width;
	dc3.buffer = (int*)dc1.buffer + dc1.width * dc1.height * 2;
	dc4.buffer = (int*)dc3.buffer + dc3.width;
	dc2.zBuffer = (int*)dc1.zBuffer + dc1.width;
	dc3.zBuffer = (int*)dc1.zBuffer + dc1.width * dc1.height * 2;
	dc4.zBuffer = (int*)dc3.zBuffer + dc3.width;
	buffer = CreateBitmap(client.right, client.bottom, 1, 32, 0);
	
	//windows II
	SetTimer(hwnd, TIMER_ID, timerPeriod, 0);
	
	//window loop
	MSG msg;
	while(GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}

long int _stdcall WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc, hdcMem;
	HPEN pen, old;
	LARGE_INTEGER t1, t2;
	switch(msg) {
	case WM_TIMER:
		QueryPerformanceCounter(&t1);
		float a[16], b[16], c[16];
		float buf[4*3*3];
		
		//1
		background(&dc1, 0);
		clearZbuf(&dc1);
		
		camera(a, 0, 1, 0, 0, 0, 0, 0, 0, -1);
		rotateY(b, radius);
		mulmat(a, b, c, 4, 4, 4);
		applyMatrixToVec3(buf, _triangles, 3*3, c);
		
		triangle(&dc1, buf, buf+4, buf+8, 0xFF0000);
		triangle(&dc1, buf+12, buf+16, buf+20, 0x00FF00);
		triangle(&dc1, buf+24, buf+28, buf+32, 0x0000FF);
		
		//2
		background(&dc2, 0);
		clearZbuf(&dc2);
		
		camera(a, 0, 0, 1, 0, 0, 0, 0, 1, 0);
		rotateY(b, radius);
		mulmat(a, b, c, 4, 4, 4);
		applyMatrixToVec3(buf, _triangles, 3*3, c);
		
		triangle(&dc2, buf, buf+4, buf+8, 0xFF0000);
		triangle(&dc2, buf+12, buf+16, buf+20, 0x00FF00);
		triangle(&dc2, buf+24, buf+28, buf+32, 0x0000FF);
		
		//3
		background(&dc3, 0);
		clearZbuf(&dc3);
		
		camera(a, 0, 0, 1, 0, 0, 0, 1, 0, 0);
		rotateY(b, radius);
		mulmat(a, b, c, 4, 4, 4);
		applyMatrixToVec3(buf, _triangles, 3*3, c);
		
		triangle(&dc3, buf, buf+4, buf+8, 0xFF0000);
		triangle(&dc3, buf+12, buf+16, buf+20, 0x00FF00);
		triangle(&dc3, buf+24, buf+28, buf+32, 0x0000FF);
		
		//4
		background(&dc4, 0);
		clearZbuf(&dc4);
		
		camera(a, 1, 1, 1, 0, 0, 0, sin(radius * 2.0) * 0.3, 1, -sin(radius * 2.0) * 0.3);
		//identity(a);
		rotateY(b, radius);
		mulmat(a, b, c, 4, 4, 4);
		applyMatrixToVec3(buf, _triangles, 3*3, c);
		
		triangle(&dc4, buf, buf+4, buf+8, 0xFF0000);
		triangle(&dc4, buf+12, buf+16, buf+20, 0x00FF00);
		triangle(&dc4, buf+24, buf+28, buf+32, 0x0000FF);
		
		radius += M_PI/120.0;
		
		InvalidateRect(hwnd, 0, 0);
		QueryPerformanceCounter(&t2);
		//t2 -= t1;
		*(unsigned int*)(void*)&t2 -= *(unsigned int*)(void*)&t1;
		print("Timer ");
		printi(*(int*)(void*)&t2);
		nl();
		break;
	case WM_PAINT:
		QueryPerformanceCounter(&t1);
		hdc = BeginPaint(hwnd, &ps);
		hdcMem = CreateCompatibleDC(hdc);
		if(!showZbuf)
			SetBitmapBits(buffer, client.bottom * client.right * 4, dc1.buffer);
		else {
			unsigned int *buf = (unsigned int*)VirtualAlloc(0, client.right * client.bottom * 4, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			for(int i = 0; i < client.bottom; i++) {
				unsigned int a;
				for(int j = 0; j < client.right; j++) {
					a = ((unsigned char*)dc1.zBuffer)[i * client.right + j];
					a = 255 - (a >> 24);
					a |= (a << 8) | (a << 16);
					buf[i * client.right + j] = a;
				}
			}
			SetBitmapBits(buffer, client.bottom * client.right * 4, buf);
			VirtualFree(buf, 0, MEM_RELEASE);
		}
		SelectObject(hdcMem, buffer);
		BitBlt(hdc, 0, 0, client.right, client.bottom, hdcMem, 0, 0, SRCCOPY);
		pen = CreatePen(PS_SOLID, 1, 0xFFFFFF);
		old = (HPEN)SelectObject(hdc, pen);
		MoveToEx(hdc, client.right >> 1, 0, 0);
		LineTo(hdc, client.right >> 1, client.bottom);
		MoveToEx(hdc, 0, client.bottom >> 1, 0);
		LineTo(hdc, client.right, client.bottom >> 1);
		SelectObject(hdc, old);
		DeleteObject(pen);
		DeleteDC(hdcMem);
		EndPaint(hwnd, &ps);
		QueryPerformanceCounter(&t2);
		//t2 -= t1;
		*(unsigned int*)(void*)&t2 -= *(unsigned int*)(void*)&t1;
		print("Paint ");
		printi(*(int*)(void*)&t2);
		nl();
		break;
	case WM_KEYDOWN:
		switch(wParam) {
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case VK_SPACE:
			showZbuf ^= 1;
			break;
		case VK_LEFT:
			timerPeriod -= DELTA_PERIOD;
			print("New timer period ");
			printi(timerPeriod);
			nl();
			KillTimer(hwnd, TIMER_ID);
			SetTimer(hwnd, TIMER_ID, timerPeriod, 0);
			break;
		case VK_RIGHT:
			timerPeriod += DELTA_PERIOD;
			print("New timer period ");
			printi(timerPeriod);
			nl();
			KillTimer(hwnd, TIMER_ID);
			SetTimer(hwnd, TIMER_ID, timerPeriod, 0);
			break;
		}
		InvalidateRect(hwnd, 0, 0);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 1;
}