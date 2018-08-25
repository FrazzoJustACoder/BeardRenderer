#include "WinDebug.h"

namespace debug {
	HANDLE h;
}
using namespace debug;

void initDebug(HANDLE a) {
	h = a;
}

const char _shortcut[] = "0123456789ABCDEF";
void int2hex(unsigned int n, char *d, int b) {
	b &= ~3;
	while(b) {
		b -= 4;
		*(d++) = _shortcut[(n >> b) & 15];
	}
	*d = 0;
}

void int2str(int n, char *d) {
	if(n == 0) {
		*((short*)d) = '0';
		return;
	}
	if(n < 0) {
		*(d++) = '-';
		n = -n;
	}
	int t = 1000000000;
	while(t > n) t /= 10;
	while(t) {
		*(d++) = n / t + '0';
		n %= t;
		t /= 10;
	}
	*d = 0;
}

void dump0(const void* s, char* d) {
	unsigned char *p = (unsigned char*)s;
	int2hex((int)p, d, 32);
	d[8] = '|';
	d += 9;
	for(int i = 0; i < 4; i++) {
		for(int j= 0; j < 4; j++, p++) {
			int2hex(*p, d, 8);
			d[2] = ' ';
			d += 3;
		}
		d[-1] = '-';
	}
	d[-1] = '|';
	p -= 16;
	for(int i = 0; i < 16; i++, p++)
		*(d++) = (signed char)*p < ' ' ? '.' : *p;
	d[0] = 10;
	d[1] = 0;
}

void print(const char *c) {
	WriteConsole(h, c, strlen(c), 0, 0);
}

void printx(int n) {
	char buf[12];
	int2hex(n, buf, 32);
	print(buf);
}

void printi(int n) {
	char buf[12];
	int2str(n, buf);
	print(buf);
}

void printd(double n) {
	if(abs(n) > 100.0)
		print("big");
	else if(abs(n) < 0.000001 && abs(n) != 0.0)
		print("small");
	else
		printi((int)(n * 1000000));
}

void printDump(void *src, int lines) {
	char buf[80];
	char *s = (char*)src;
	while(lines) {
		dump0(s, buf);
		s += 16;
		print(buf);
		lines--;
	}
}

void nl() {
	char a = 10;
	WriteConsole(h, &a, 1, 0, 0);
}

void space() {
	char a = 32;
	WriteConsole(h, &a, 1, 0, 0);
}
