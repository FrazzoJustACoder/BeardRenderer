//TODO add more abstraction (e.g. bpp abstraction)

#pragma once

typedef unsigned int Color;

//beard renderer context
typedef struct {
	Color *buffer;
	unsigned int *zBuffer;
	int width, height, pitch, zPitch;
} BRC;

void hline(BRC *dc, float x1, float y, float z1, float x2, float z2, Color c);

void triangle(BRC*, const float*, const float*, const float*, Color);

void background(BRC*, Color);
void clearZbuf(BRC*);
