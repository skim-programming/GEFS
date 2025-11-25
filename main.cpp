// Note: All in one file for now, not modularized, should work on all Windows x64 & x32 systems

#include <windows.h>
#include <stdint.h>
#include <vector>

int width = 800;
int height = 600;

bool keys[256] = {}; // true if key is down

uint32_t* frameBuffer;
uint32_t* hitboxBuffer;
HDC hdc;
BITMAPINFO bmi;

enum ObjectID : uint32_t
{
    EMPTY = 0,
    PLAYER = 1,
    ENEMY = 2,
    SOLID = 3,
    BULLET = 4,
};

class Object {
public:
    int x;
    int y;
    int w;
    int h;
    ObjectID id;

    Object(int x_, int y_, int w_, int h_, ObjectID id_)
        : x(x_), y(y_), w(w_), h(h_), id(id_) {}
};

class Player : public Object {
public:
    int size;
    int speed;
    Player(int x_, int y_, int size_, int speed_) 
        : Object(x_, y_, size_, size_, ObjectID::PLAYER), size(size_), speed(speed_) {}
};

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

void drawRectHitbox(int x, int y, int w, int h, ObjectID id)
{
    for (int j = 0; j < h; j++)
    {
        int py = y + j;
        if (py < 0 || py >= height) continue;

        for (int i = 0; i < w; i++)
        {
            int px = x + i;
            if (px < 0 || px >= width) continue;

            hitboxBuffer[py * width + px] = id;
        }
    }
}

void drawRectWH(int x, int y, int w, int h, uint32_t color, ObjectID id) // WH = with hitbox
{
    drawRectHitbox(x, y, w, h, id);
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

void drawCircleHitbox(int cx, int cy, int r, ObjectID id)
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

            if (x*x + y*y <= r2)
                hitboxBuffer[py * width + px] = id;
        }
    }
}

void drawCircleWH(int cx, int cy, int r, uint32_t color, ObjectID id)
{
    drawCircleHitbox(cx, cy, r, id);
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

bool CheckCollisions(Object& object)
{
    for (int py = object.y; py < object.y + object.h; py++)
    {
        if (py < 0 || py >= height) continue;
        for (int px = object.x; px < object.x + object.w; px++)
        {
            if (px < 0 || px >= width) continue;

            ObjectID id = static_cast<ObjectID>(hitboxBuffer[py * width + px]);

            if (id == ObjectID::EMPTY || id == object.id) continue;

            switch (id)
            {
            case ObjectID::PLAYER:
                return true;

            case ObjectID::ENEMY:
                return true;

            case ObjectID::SOLID:
                return true;
            default:
                return true;
            }
        }
    }
    return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
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

    // Allocate framebuffer
    frameBuffer = new uint32_t[width * height];
    hitboxBuffer = new uint32_t[width * height];


    // Setup BITMAPINFO (used by StretchDIBits)
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    MSG msg = {};

    Player player(width / 2, height / 2, 20, 4);

    std::pair<int, int> prevPos(width/2, height/2);

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
        for (int i = 0; i < width * height; i++) {
            frameBuffer[i] = 0xFF101010; // dark background
            hitboxBuffer[i] = ObjectID::EMPTY;
        }

        // Draw player
        drawRectWH(player.x, player.y, player.w, player.h, 0xFFFF0000, ObjectID::PLAYER); // red square

        drawCircleWH(400, 400, 40, 0xFFFF0000, ObjectID::SOLID);

        prevPos = { player.x, player.y };

        // Button handling
        if (keys[VK_LEFT] || keys[0x41])  player.x -= player.speed;
        if (keys[VK_RIGHT] || keys[0x44]) player.x += player.speed;
        if (keys[VK_UP] || keys[0x57])    player.y -= player.speed;
        if (keys[VK_DOWN] || keys[0x53])  player.y += player.speed;

        // Clamp to window boundaries
        if (player.x < 0 + player.size) player.x = player.size;
        if (player.x >= width - player.size) player.x = width - 1 - player.size;
        if (player.y < 0 + player.size) player.y = player.size;
        if (player.y >= height - player.size) player.y = height - 1 - player.size;

        if (CheckCollisions(player)) {
            player.x = prevPos.first;
            player.y = prevPos.second;
        }

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
