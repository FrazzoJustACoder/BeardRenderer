#include "BeardRenderer.h"
#include "lib.h"

#define min(a,b) (a<b?a:b)
#define abs(a) (a<0?-a:a)

#define MAX_DEPTH 0xFFFFFF00

void hline(BRC *brc, float x1, float y, float z1, float x2, float z2, Color c) {
	if(y <= -1.0f || y > 1.0f)
		return;
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
		float z = min(z1, z2);
		if(x1 >= -1.0f && x1 < 1.0f && z >= -1.0f && z <= 1.0f) {
			int h = (int)((y - 1.0f) / -2.0f * brc->height) * (brc->pitch >> 2);
			int x = (int)((x1 + 1.0f) / 2.0f * brc->width);
			int z = (int)((z - 1.0f) / -2.0f * MAX_DEPTH);
			if(z < brc->zBuffer[h + x]) {
				brc->zBuffer[h + x] = z;
				brc->buffer[h + x] = c;
			}
		}
		return;
	}
	if(x1 < -3.0f || x2 > 3.0f) //NOTE problems with perspective matrix
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
	float z = (z1 - 1.0f) / -2.0f * MAX_DEPTH;
	int x = (x1 + 1.0f) / 2.0f * brc->width;
	int n = (x2 - x1) / 2.0f * brc->width;
	dz = (z2 - z1) / -2.0f * MAX_DEPTH / n;
	Color *b = brc->buffer;
	unsigned int *zb = brc->zBuffer;
	int h = (int)((y - 1.0f) / -2.0f * brc->height) * (brc->pitch >> 2);
	int zh = (int)((y - 1.0f) / -2.0f * brc->height) * (brc->zPitch >> 2);
	while(n > 0) {
		if((unsigned int)z < zb[h + x]) {
			//here goes shading code
			b[h + x] = c;
			zb[zh + x] = z;
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

void background(BRC *brc, Color c) {
	for(int i = 0, n = brc->height * (brc->pitch >> 2);
		i < n; i += (brc->pitch >> 2))
		memset4(brc->buffer + i, c, brc->width);
	//memset4(brc->buffer, c, brc->width * brc->height);
}

void clearZbuf(BRC *brc) {
	for(int i = 0, n = brc->height * (brc->zPitch >> 2);
		i < n; i+= (brc->zPitch >> 2))
		memset4(brc->zBuffer + i, MAX_DEPTH, brc->width);
	//memset4(brc->zBuffer, MAX_DEPTH, brc->width * brc->height);
}
