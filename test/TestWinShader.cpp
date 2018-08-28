#include <windows.h>
#include <math.h>
#include "BeardMatrix.h"
#include "BeardRenderer.h"
#include "WinDebug.h"

//window
HWND hwnd;
HINSTANCE hInst;
const char _class[] = "test";
const char _title[] = "BR3D test";
RECT client;
#define TIMER_ID 10
int timerPeriod = 15;
#define DELTA_PERIOD 1

long int _stdcall WndProc(HWND, UINT, WPARAM, LPARAM);

//stuff
BRC brc;
HBITMAP buffer;
int showZbuf;
double radius;
float x, y, z, pitch, yaw;
int oldX, oldY;
int keys;
#define KEY_W 1
#define KEY_S 2
#define KEY_A 4
#define KEY_D 8
#define KEY_Q 0x10
#define KEY_E 0x20

//notice that in order to make normals poining towards "out" direction, the right hand rule has to be applied
#define TETRA1 0.0, +1.0, 1.0/M_SQRT2
#define TETRA2 0.0, -1.0, 1.0/M_SQRT2
#define TETRA3 +1.0, 0.0, -1.0/M_SQRT2
#define TETRA4 -1.0, 0.0, -1.0/M_SQRT2
const float _tetrahedron[] = {
TETRA1, TETRA4, TETRA2,
TETRA2, TETRA3, TETRA1,
TETRA4, TETRA1, TETRA3,
TETRA3, TETRA2, TETRA4
};

const float _up[] = {0.0f, 1.0f, 0.0f};

float fooShader_cos;
Color fooShader(Color c, float x, float y, float z) {
	unsigned char *out = (unsigned char*)&c;
	float &s = fooShader_cos;
	if(s > 0.0) {
		//out[0] = (out[0] * (1.0 - s)) + (255.0 * s); o = o - os + 255s, o += s(255-o)
		out[0] += s * (255 - out[0]);
		out[1] += s * (255 - out[1]);
		out[2] += s * (255 - out[2]);
	}
	else if(s < 0.0) {
		//out[0] = (out[0] * (s + 1.0)) + (0 * s); o = o * (s + 1)
		out[0] *= s + 1.0f;
		out[1] *= s + 1.0f;
		out[2] *= s + 1.0f;
	}
	return c;
}

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
	x = 0.0f;
	y = 0.0f;
	z = 2.5f;
	pitch = 0.0f;
	yaw = 0.0f;
	keys = 0;
	oldX = 320;
	oldY = 240;
	GetClientRect(hwnd, &client);
	printi(client.right);
	print("x");
	printi(client.bottom);
	nl();
	if(!
		BRCInit(&brc,
			VirtualAlloc(0, client.right * client.bottom * 4, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE),
			VirtualAlloc(0, client.right * client.bottom * 4, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE),
			client.right, client.bottom, client.right << 2, client.right << 2, 32, 32, fooShader
			)
	) {
		MessageBox(0, "!", "error", MB_OK);
		return 1;
	}
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
		float buf[4*4*3];
		float buf0[4*4*3];
		
		float lx, lz;
		lx = (((keys & KEY_D) != 0) - ((keys & KEY_A) != 0)) * 0.01f;
		lz = (((keys & KEY_S) != 0) - ((keys & KEY_W) != 0)) * 0.01f;
		x += lx * cos(pitch) + lz * sin(pitch);
		y += (((keys & KEY_E) != 0) - ((keys & KEY_Q) != 0)) * 0.01f;
		z += -lx * sin(pitch) + lz * cos(pitch);
		
		float s[3], u[3], temp[3];
		s[0] = - sin(pitch) * cos(yaw);
		s[1] = + sin(yaw);
		s[2] = - cos(pitch) * cos(yaw);
		u[0] = 0.0f;
		u[1] = 1.0f,
		u[2] = 0.0f;
		mulvecV(s, u, temp);
		mulvecV(temp, s, u);
		s[0] += x;
		s[1] += y;
		s[2] += z;
		
		rotateY(c, radius);
		rotateZ(b, radius * (10.0/17.0));
		mulmat(b, c, a, 4, 4, 4);
		applyMatrixToVec3(buf0, _tetrahedron, 4*3, a);
		camera(b, x, y, z, s[0], s[1], s[2], u[0], u[1], u[2]);
		mulmat(b, a, c, 4, 4, 4);
		perspective(b, M_PI / 3.0, 4.0f / 3.0f, 0.1f, 3.0f);
		mulmat(b, c, a, 4, 4, 4);
		applyMatrixToVec3(buf, _tetrahedron, 4*3, a);
		
		background(&brc, 0);
		clearZbuf(&brc);
		
		float n[3];
		for(int i = 0; i < 4; i++) {
			triNorm(n, buf0+i*12, buf0+i*12+4, buf0+i*12+8);
			fooShader_cos = cosV(n, _up) * 0.3f;
			//triangle(&brc, buf+i*12, buf+i*12+4, buf+i*12+8, (i/2 ? 0xFF0000 : 0) + (i%2 ? 0xFF : 0) + (!i ? 0xFF00 : 0));
			triangle(&brc, buf+i*12, buf+i*12+4, buf+i*12+8, 0xFF0000);
		}
		
		radius += M_PI / 120.0;
		
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
			SetBitmapBits(buffer, client.bottom * client.right * 4, brc.buffer);
		else {
			unsigned int *buf = (unsigned int*)VirtualAlloc(0, client.right * client.bottom * 4, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			for(int i = 0; i < client.bottom; i++) {
				unsigned int a;
				for(int j = 0; j < client.right; j++) {
					a = ((unsigned int*)(brc.zBuffer))[i * client.right + j];
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
			InvalidateRect(hwnd, 0, 0);
			break;
		case VK_LEFT:
			timerPeriod -= DELTA_PERIOD;
			print("New timer period ");
			printi(timerPeriod);
			nl();
			KillTimer(hwnd, TIMER_ID);
			SetTimer(hwnd, TIMER_ID, timerPeriod, 0);
			InvalidateRect(hwnd, 0, 0);
			break;
		case VK_RIGHT:
			timerPeriod += DELTA_PERIOD;
			print("New timer period ");
			printi(timerPeriod);
			nl();
			KillTimer(hwnd, TIMER_ID);
			SetTimer(hwnd, TIMER_ID, timerPeriod, 0);
			InvalidateRect(hwnd, 0, 0);
			break;
		case 'W':
			keys |= KEY_W;
			break;
		case 'S':
			keys |= KEY_S;
			break;
		case 'A':
			keys |= KEY_A;
			break;
		case 'D':
			keys |= KEY_D;
			break;
		case 'Q':
			keys |= KEY_Q;
			break;
		case 'E':
			keys |= KEY_E;
			break;
		}
		break;
	case WM_KEYUP:
		switch(wParam) {
		case 'W':
			keys &= ~KEY_W;
			break;
		case 'S':
			keys &= ~KEY_S;
			break;
		case 'A':
			keys &= ~KEY_A;
			break;
		case 'D':
			keys &= ~KEY_D;
			break;
		case 'Q':
			keys &= ~KEY_Q;
			break;
		case 'E':
			keys &= ~KEY_E;
			break;
		}
		break;
	case WM_MOUSEMOVE: {
		int x = LOWORD(lParam), y = HIWORD(lParam);
		pitch -= (x - oldX) * M_PI / 80.0;
		yaw = max(min(yaw - (y - oldY) * M_PI / 80.0f, M_PI / 2.0 - 0.0001), -M_PI / 2.0 + 0.0001);
		oldX = x;
		oldY = y;
		break;
	}
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