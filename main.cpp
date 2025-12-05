#include <windows.h>
#include <vector>
#include <iostream>
#include <variant>
#include "object.h"
#include "globals.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        ReleaseDC(hwnd, hdc);
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        keys[wParam] = true;
        return 0;

    case WM_KEYUP:
        keys[wParam] = false;
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void drawRect(int x, int y, int w, int h, uint32_t color)
{
    for (int j = 0; j < h; j++)
    {
        int py = y + j;
        if (py < 0 || py >= height) continue;

        for (int i = 0; i < w; i++)
        {
            int px = x + i;
            if (px < 0 || px >= width) continue;

            frameBuffer[py * width + px] = color;
        }
    }
}

//void drawRectHitbox(int x, int y, int w, int h, ObjectID id)
//{
//    for (int j = 0; j < h; j++)
//    {
//        int py = y + j;
//        if (py < 0 || py >= height) continue;
//
//        for (int i = 0; i < w; i++)
//        {
//            int px = x + i;
//            if (px < 0 || px >= width) continue;
//
//            hitboxBuffer[py * width + px] = static_cast<uint32_t>(id);
//        }
//    }
//}

//void drawRectWH(int x, int y, int w, int h, uint32_t color, ObjectID id) // WH = with hitbox
//{
//    drawRectHitbox(x, y, w, h, id);
//    for (int j = 0; j < h; j++)
//    {
//        int py = y + j;
//        if (py < 0 || py >= height) continue;
//
//        for (int i = 0; i < w; i++)
//        {
//            int px = x + i;
//            if (px < 0 || px >= width) continue;
//
//            frameBuffer[py * width + px] = color;
//        }
//    }
//}


void drawCircle(int cx, int cy, int r, uint32_t color)
{
    int r2 = r * r;

    for (int y = -r; y <= r; y++)
    {
        int py = cy + y;
        if (py < 0 || py >= height) continue;

        for (int x = -r; x <= r; x++)
        {
            int px = cx + x;
            if (px < 0 || px >= width) continue;

            if (x * x + y * y <= r2)
                frameBuffer[py * width + px] = color;
        }
    }
}

//void drawCircleHitbox(int cx, int cy, int r, ObjectID id) {
//    int r2 = r * r;
//    for (int dy = -r; dy <= r; dy++) {
//        int py = cy + dy;
//        if (py < 0 || py >= height) continue;
//        for (int dx = -r; dx <= r; dx++) {
//            int px = cx + dx;
//            if (px < 0 || px >= width) continue;
//            if (dx * dx + dy * dy <= r2)
//                hitboxBuffer[py * width + px] = static_cast<uint32_t>(id);
//        }
//    }
//}

//void drawCircleWH(int cx, int cy, int r, uint32_t color, ObjectID id)
//{
//    drawCircleHitbox(cx, cy, r, id);
//    int r2 = r * r;
//
//    for (int y = -r; y <= r; y++)
//    {
//        int py = cy + y;
//        if (py < 0 || py >= height) continue;
//
//        for (int x = -r; x <= r; x++)
//        {
//            int px = cx + x;
//            if (px < 0 || px >= width) continue;
//
//            if (x * x + y * y <= r2)
//                frameBuffer[py * width + px] = color;
//        }
//    }
//}

//void drawObjectWH(Object& object) {
//    switch (object.shape) {
//    case Shape::EMPTY:
//        return;
//    case Shape::RECTANGLE: {
//        rectangle* rect = dynamic_cast<rectangle*>(&object);
//        if (rect) {
//            drawRectWH(rect->x, rect->y, rect->w, rect->h, rect->color, rect->id);
//        }
//        break;
//    }
//    case Shape::CIRCLE: {
//        circle* c = dynamic_cast<circle*>(&object);
//        if (c) {
//            drawCircleWH(c->x, c->y, c->r, c->color, c->id);
//        }
//        break;
//    }
//    }
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    setupGlobals();
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    // Register window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SoftRenderWindow";
    RegisterClass(&wc);

    // Create window    
    HWND hwnd = CreateWindowEx(
        0,
        L"SoftRenderWindow",
        L"Software Renderer",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        0, 0, hInstance, 0
    );

    hdc = GetDC(hwnd);

    MSG msg = {};

    // PlayerRect player({ width / 2, height / 2, 20, 20, 4, ObjectID::PLAYER, static_cast<uint32_t>(0xFFFF0000)});

//    circle circ({ 400, 400, 40, static_cast<uint32_t>(0xFFFF0000), ObjectID::SOLID });

    std::pair<int, int> prevPos(width/2, height/2);

    GameObject obj = GameObject();
    auto& catSprite = obj.add_component<SpriteRenderer>();
    catSprite.SetSprite("hairy-fluffy-cat-playing-png.png", width / 2, height / 2);
    catSprite.SetSpriteScale(0.5, 0.5);

    int xForce = 2, yForce = 2;

    // Game/render loop
    while (true)
    {
        // Handle Windows messages
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) 
            {
                ReleaseDC(hwnd, hdc);
                return 0;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Clear framebuffer
        for (int i = 0; i < width * height; i++)     {
            frameBuffer[i] = 0xFF101010; // dark background
            //hitboxBuffer[i] = static_cast<uint32_t>(ObjectID::EMPTY);
        }

        DrawSprite(frameBuffer, width, height, catSprite.pixels.data(), catSprite.w, catSprite.h, catSprite.x, catSprite.y, catSprite.sw, catSprite.sh);

        if (catSprite.y + catSprite.h*catSprite.sh > height) {
            			yForce = -yForce;
		}
        /*else if (catSprite.y - catSprite.h*catSprite.sh < 0) {
            yForce = -yForce;
        }*/
        if (catSprite.x + catSprite.w*catSprite.sw > width) {
			xForce = -xForce;
		}
		else if (catSprite.x - catSprite.w*catSprite.sw < 0) {
			xForce = -xForce;
		}

        catSprite.x += xForce;
        catSprite.y += yForce;

        // Draw player
        // drawRectWH(player.x, player.y, player.w, player.h, 0xFFFF0000, ObjectID::PLAYER); // red square

        // drawObjectWH(player);

        // drawObjectWH(circ);
        // drawCircleWH(400, 400, 40, 0xFFFF0000, ObjectID::SOLID);

        // prevPos = { player.x, player.y };

        // Button handling
        //if (keys[VK_LEFT] || keys[0x41])  player.x -= player.speed;
        //if (keys[VK_RIGHT] || keys[0x44]) player.x += player.speed;
        //if (keys[VK_UP] || keys[0x57])    player.y -= player.speed;
        //if (keys[VK_DOWN] || keys[0x53])  player.y += player.speed;

        //// Clamp to window boundaries
        //if (player.x < 0 + player.w) player.x = player.w;
        //if (player.x >= width - player.w) player.x = width - 1 - player.w;
        //if (player.y < 0 + player.h) player.y = player.h;
        //if (player.y >= height - player.h) player.y = height - 1 - player.h;

        //if (player.checkCollision()) {
        //    player.x = prevPos.first;
        //    player.y = prevPos.second;
        //}

        // Actual displaying framebuffer to window
        StretchDIBits(
            hdc,
            0, 0, width, height,
            0, 0, width, height,
            frameBuffer,
            &bmi,
            DIB_RGB_COLORS,
            SRCCOPY
        );
        Sleep(8);
    }

    return 0;
}
