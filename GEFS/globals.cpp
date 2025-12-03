#include "globals.h"

// initializing all global vars
int width = 800;
int height = 600;

// Allocate framebuffer
uint32_t* frameBuffer = new uint32_t[width * height];
uint32_t* hitboxBuffer = new uint32_t[width * height];

HDC hdc;

BITMAPINFO bmi;

bool keys[256] = {}; // true if key is down

void setupGlobals() {
	// Setup BITMAPINFO (used by StretchDIBits)
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
}

void cleanupGlobals() { // Just for safety, OS does it anyways
	delete[] frameBuffer;
	delete[] hitboxBuffer;
}