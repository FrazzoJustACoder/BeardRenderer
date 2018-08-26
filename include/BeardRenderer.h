//TODO add more abstraction (e.g. bpp abstraction)

#pragma once

typedef unsigned int Color;

//beard renderer context
typedef struct {
	void *buffer;
	void *zBuffer;
	int width, height, pitch, zPitch;
	int bpp; //bits per pixel
	int zDepth; //zBuffer bits depth
} BRC;

//returns true on success
bool BRCInit(BRC*, void*, void*, int, int, int, int, int, int);

void triangle(BRC*, const float*, const float*, const float*, Color);

void background(BRC*, Color);
void clearZbuf(BRC*);
