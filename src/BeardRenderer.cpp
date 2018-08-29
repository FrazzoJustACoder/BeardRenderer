#include <cstdint>
#include <cmath>
#include "BeardRenderer.h"
#include "lib.h"

#define min(a,b) (a<b?a:b)
#define max(a,b) (a<b?b:a)
//#define abs(a) (a<0?-a:a)

//#define MAX_DEPTH 0xFFFFFF00

//TODO this abstraction made it slower !
inline void setPixel(void *buf, void *zBuf, uint32_t color, uint32_t depth, int bpp, int bpz) {
	uint32_t t, mask;
	
	mask = 0xFFFFFFFF >> (32 - bpp);
	t = *(uint32_t*)buf & ~mask;
	t |= color & mask;
	*(uint32_t*)buf = t;
	
	mask = 0xFFFFFFFF >> (32 - bpz);
	t = *(uint32_t*)zBuf & ~mask;
	t |= depth & mask;
	*(uint32_t*)zBuf = t;
}

//returns true if depth < *inZ i.e. the pixel can be written
inline bool cmpDepth(void *zBuf, uint32_t depth, int bpz) {
	uint32_t t, mask = 0xFFFFFFFF >> (32 - bpz);
	depth &= mask;
	t = (*(uint32_t*)zBuf) & mask;
	return depth < t;
}

bool BRCInit(BRC *brc, void *buffer, void *zBuffer, int width, int height, int pitch, int zPitch, int bpp, int zDepth, Color (*s)(Color, float, float, float)) {
	//since this library is abstract, it doesn't allocate memory
	bpp &= ~7; //TODO add support for bpp < 8
	zDepth &= ~7;
	if(bpp < 8 || bpp > 32 || zDepth < 8 || zDepth > 32 || !buffer || !zBuffer)
		return false;
	brc->buffer = buffer;
	brc->zBuffer = zBuffer;
	brc->width = width;
	brc->height = height;
	brc->pitch = pitch;
	brc->zPitch = zPitch;
	brc->zDepth = zDepth;
	brc->bpp = bpp;
	brc->shader = s;
	return true;
}

void hline(BRC *brc, float x1, float y, float z1, float x2, float z2, Color c) {
	if(y <= -1.0f || y > 1.0f)
		return;
	Color cs; //shaded color
	int Bpp, Bpz; //bytes per pixel, bytes per zBuffer element
	Bpp = brc->bpp >> 3;
	Bpz = brc->zDepth >> 3;
	//maxDepth is 0xFC, 0xFFF0, 0xFFFFC0 or 0xFFFFFF00 based on bpz, to avoid little errors due to the use of float
	uint32_t maxDepth = (0xFFFFFFFF >> (32 - 6 * Bpz)) << (Bpz << 1);
	float t;
	if(x1 > x2) {
		t = x1;
		x1 = x2;
		x2 = t;
		t = z1;
		z1 = z2;
		z2 = t;
	}
	else if(x1 == x2) {
		float z = min(max(z1, z2), 1.0f);
		if(x1 >= -1.0f && x1 < 1.0f && z >= -1.0f && z <= 1.0f) {
			int h = (int)((y - 1.0f) / -2.0f * brc->height) * brc->pitch;
			int hz = (int)((y - 1.0f) / -2.0f * brc->height) * brc->zPitch;
			int x = (int)((x1 + 1.0f) / 2.0f * brc->width) * Bpp;
			int xz = (int)((x1 + 1.0f) / 2.0f * brc->width) * Bpz;
			uint32_t zz = (int)((z - 1.0f) / -2.0f * maxDepth);
			if(cmpDepth(brc->zBuffer + hz + xz, zz, brc->zDepth)) {
				if(brc->shader)
					cs = (*(brc->shader))(c, x1, y, z);
				else
					cs = c; //no shading
				setPixel(brc->buffer + h + x, brc->zBuffer + hz + xz, cs, zz, Bpp, Bpz);
			}
		}
		return;
	}
	if(x1 < -2.0f || x2 > 2.0f) //NOTE temporary clipping
		return;
	if(x1 >= 1.0f || x2 < -1.0f)
		return;
	float dx, dz;
	dx = (x2 - x1);
	dz = (z2 - z1);
	if(x1 < -1.0f) {
		z1 += (-1.0f - x1) * dz / dx;
		x1 = -1.0f;
	}
	if(x2 > 1.0f) {
		z2 += (1.0f - x2) * dz / dx;
		x2 = 1.0f;
	}
	if(z1 > 1.0f) {
		x1 += (1.0f - z1) * dx / dz;
		z1 = 1.0f;
	}
	else if(z1 < -1.0f) {
		x1 += (-1.0f - z1) * dx / dz;
		z1 = -1.0f;
	}
	if(z2 > 1.0f) {
		x2 += (1.0f - z2) * dx / dz;
		z2 = 1.0f;
	}
	else if(z2 < -1.0f) {
		x2 += (-1.0f - z2) * dx / dz;
		z2 = -1.0f;
	}
	if(x1 < -1.0f || x2 > 1.0f) //nothing to draw
		return;
	float z = (z1 - 1.0f) / -2.0f * maxDepth;
	int x = (x1 + 1.0f) / 2.0f * brc->width;
	int n = (x2 - x1) / 2.0f * brc->width;
	dz = (z2 - z1) / -2.0f * maxDepth / n;
	void *b = brc->buffer + (int)((y - 1.0f) / -2.0f * brc->height) * brc->pitch;
	void *zb = brc->zBuffer + (int)((y - 1.0f) / -2.0f * brc->height) * brc->zPitch;
	while(n > 0) {
		if(cmpDepth(zb + x * Bpz, z, brc->zDepth)) {
			if(brc->shader)
				cs = (*(brc->shader))(c, x1, y, z);
			else
				cs = c; //no shading
			setPixel(b + x * Bpp, zb + x * Bpz, cs, z, brc->bpp, brc->zDepth);
		}
		x++;
		z += dz;
		n--;
	}
}

void triangle(BRC *brc, const float *p1, const float *p2, const float *p3, Color c) {
	const float *t;
	//sort points by y
	int flag = (p1[1] < p2[1]) | ((p2[1] < p3[1]) << 1) | ((p1[1] < p3[1]) << 2);
	switch(flag) {
	case 7:
		break;
	case 0:
		t = p1;
		p1 = p3;
		p3 = t;
		break;
	case 1:
		t = p1;
		p1 = p3;
		p3 = p2;
		p2 = t;
		break;
	case 2:
		t = p1;
		p1 = p2;
		p2 = p3;
		p3 = t;
		break;
	//case 3:
	//case 4:
	case 5:
		t = p2;
		p2 = p3;
		p3 = t;
		break;
	case 6:
		t = p1;
		p1 = p2;
		p2 = t;
		break;
	}
	float x1, y, z1, x2, z2, dx1, dy, dz1, dx2, dz2;
	x1 = x2 = p1[0];
	y = p1[1];
	z1 = z2 = p1[2];
	dy = 2.0f / brc->height;
	 //TODO find a better way to avoid zero-division instead of the one below
	float n = (p2[1] - y + ((p2[1] == y) * 0.000001f)) / 2.0f * brc->height;
	dx1 = (p2[0] - x1) / n;
	dz1 = (p2[2] - z1) / n;
	float m = (p3[1] - y + ((p2[1] == y) * 0.000001f)) / 2.0f * brc->height;
	dx2 = (p3[0] - x1) / m;
	dz2 = (p3[2] - z1) / m;
	if(n > 10 * brc->height || m > 10 * brc->height) //
		return;
	while(y <= -1.0f && n > 0) {
		x1 += dx1;
		z1 += dz1;
		x2 += dx2;
		z2 += dz2;
		y += dy;
		n -= 1.0f;
		m -= 1.0f;
	}
	while(n > 0) {
		hline(brc, x1, y, z1, x2, z2, c);
		x1 += dx1;
		z1 += dz1;
		x2 += dx2;
		z2 += dz2;
		y += dy;
		n -= 1.0f;
		m -= 1.0f;
	}
	x1 = p2[0];
	z1 = p2[2];
	//TODO ^ this removes precision
	n = (p3[1] - p2[1] + ((p3[1] == p2[1]) * 0.000001f)) / 2.0f * brc->height;
	dx1 = (p3[0] - x1) / n;
	dz1 = (p3[2] - z1) / n;
	while(m > 0) {
		hline(brc, x1, y, z1, x2, z2, c);
		x1 += dx1;
		z1 += dz1;
		x2 += dx2;
		z2 += dz2;
		y += dy;
		m -= 1.0f;
	}
}

void (*background_shortcut[4])(void*, unsigned int, unsigned int) = {
	memset, memset2, memset3, memset4
};
void background(BRC *brc, Color c) {
	int Bpp = brc->bpp >> 3;
	void (*set)(void*, unsigned int, unsigned int);
	set = background_shortcut[Bpp - 1];
	for(int i = 0, n = brc->height * brc->pitch;
		i < n; i += brc->pitch)
		memset4(brc->buffer + i, c, brc->width);
		//(*set)(brc->buffer + i, c, brc->width);
}

void clearZbuf(BRC *brc) {
	int Bpz = brc->zDepth >> 3;
	for(int i = 0, n = brc->height * brc->zPitch;
		i < n; i += brc->zPitch)
		memset4(brc->zBuffer + i, 0xFFFFFFFF, brc->width);// * (Bpz) / 4);
}
