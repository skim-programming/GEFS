#include <stdint.h>
#include <windows.h>
#include <initializer_list>
#include "ComponentID.h"
#include "gameObject.h"
#include "stb_image.h"

extern int height;
extern int width;
extern uint32_t* hitboxBuffer;
extern uint32_t* frameBuffer;
extern HDC hdc;
extern BITMAPINFO bmi;
extern bool keys[256];

void setupGlobals();