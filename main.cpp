#include <windows.h>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <variant>

int width = 800;
int height = 600;

bool keys[256] = {}; // true if key is down

uint32_t* frameBuffer;
uint32_t* hitboxBuffer;
HDC hdc;
BITMAPINFO bmi;

enum class ObjectID : uint32_t
{
    EMPTY = 0,
    PLAYER = 1,
    ENEMY = 2,
    SOLID = 3,
    BULLET = 4,
};

enum class Shape : uint32_t 
{
    EMPTY = 0,
    RECTANGLE = 1,
    CIRCLE = 2,
    TRIANGLE = 3
};

class Object {
public:
    int x = 0; // default values
    int y = 0;
    uint32_t color = 000000;
    ObjectID id = ObjectID::EMPTY;
    Shape shape = Shape::EMPTY;

    virtual ~Object() = default;

    virtual bool checkCollision() {
        return false;
    };

    using Param = std::variant<int, uint32_t, ObjectID, Shape>;
    Object() = default;
    Object(std::initializer_list<Param> list){
        int intIndex = 0;
        for (const auto& p : list) {

            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, int>) {
                    switch (intIndex++) {
                    case 0: this->x = arg; break; 
                    case 1: this->y = arg; break; 
                    }
                }
                else if constexpr (std::is_same_v<T, uint32_t>) {
                    this->color = arg;
                }
                else if constexpr (std::is_same_v<T, ObjectID>) {
                    this->id = arg;
                }
                else if constexpr (std::is_same_v<T, Shape>) {
                    this->shape = arg;
                }

                }, p);
        }
    }
};

class rectangle : public Object {
public:
    int w = 5;
    int h = 5;

    using Param = std::variant<int, uint32_t, ObjectID>;

    rectangle() {
        this->shape = Shape::RECTANGLE;
    }

    bool checkCollision() override {
        for (int py = y; py < y + h; py++)
        {
            if (py < 0 || py >= height) continue;
            for (int px = x; px < x + w; px++)
            {
                if (px < 0 || px >= width) continue;

                ObjectID id = static_cast<ObjectID>(hitboxBuffer[py * width + px]);

                if (id == ObjectID::EMPTY || this->id == id) continue;

                switch (id)
                {
                case ObjectID::PLAYER:
                    std::cout << "Player hit \n";
                    return true;

                case ObjectID::ENEMY:
                    std::cout << "Enemy hit \n";
                    return true;

                case ObjectID::SOLID:
                    std::cout << "Solid hit \n";
                    return true;
                default:
                    return true;
                }
            }
        }
        return false;
    }

    /// <summary>
    /// Params are x, y, w, h, color(static cast uint32_t), id
    /// </summary>
    /// <param name="list"></param>
    rectangle(std::initializer_list<Param> list) {
        int intIndex = 0;

        for (const auto& p : list) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, int>) {
                    // sequentially assign ints:
                    // 0->x, 1->y, 2->w, 3->h, 4->size, 5->speed
                    switch (intIndex++) {
                    case 0: this->x = arg; break;
                    case 1: this->y = arg; break;
                    case 2: this->w = arg; break;
                    case 3: this->h = arg; break;
                    }
                }
                else if constexpr (std::is_same_v<T, uint32_t>) {
                    this->color = arg;
                }
                else if constexpr (std::is_same_v<T, ObjectID>) {
                    this->id = arg;
                }
                }, p);
        }

    }
};

class PlayerRect : public rectangle {
public:
    int speed = 4;

    using Param = std::variant<int, uint32_t, ObjectID, Shape>;

    PlayerRect(std::initializer_list<Param> list) {
        int intIndex = 0; // tracks sequential ints

        for (const auto& p : list) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, int>) {
                    // sequentially assign ints:
                    // 0->x, 1->y, 2->w, 3->h, 4->size, 5->speed
                    switch (intIndex++) {
                    case 0: this->x = arg; break;
                    case 1: this->y = arg; break;
                    case 2: this->w = arg; break;
                    case 3: this->h = arg; break;
                    case 5: this->speed = arg; break;
                    }
                }
                else if constexpr (std::is_same_v<T, uint32_t>) {
                    this->color = arg;
                }
                else if constexpr (std::is_same_v<T, ObjectID>) {
                    this->id = arg;
                }
            }, p);
        }
    }
};

class circle : public Object {
public:
    int r = 5;

    using Param = std::variant<int, uint32_t, ObjectID>;

    bool checkCollision() override {
        int r2 = r * r;

        for (int dy = -r; dy <= r; dy++) {
            int py = y + dy; // center-based
            if (py < 0 || py >= height) continue;

            for (int dx = -r; dx <= r; dx++) {
                int px = x + dx;
                if (px < 0 || px >= width) continue;

                if (dx * dx + dy * dy > r2) continue;

                ObjectID id = static_cast<ObjectID>(hitboxBuffer[py * width + px]);
                if (id == ObjectID::EMPTY || id == this->id) continue;

                switch (id) {
                case ObjectID::PLAYER: std::cout << "Player hit\n"; return true;
                case ObjectID::ENEMY:  std::cout << "Enemy hit\n"; return true;
                case ObjectID::SOLID:  std::cout << "Solid hit\n"; return true;
                default: return true;
                }
            }
        }
        return false;
    }


    /// <summary>
    /// Params are x, y, r, color(static cast uint32_t), id
    /// </summary>
    /// <param name="list"></param>
    circle(std::initializer_list<Param> list) {
        int intIndex = 0;
        this->shape = Shape::CIRCLE;

        for (const auto& p : list) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    switch (intIndex++) {
                    case 0: this->x = arg; break;
                    case 1: this->y = arg; break;
                    case 2: this->r = arg; break;
                    }
                }
                else if constexpr (std::is_same_v<T, uint32_t>) {
                    this->color = arg;
                }
                else if constexpr (std::is_same_v<T, ObjectID>) {
                    this->id = arg;
                }
            }, p);
        }
    }
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

            hitboxBuffer[py * width + px] = static_cast<uint32_t>(id);
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

void drawCircleHitbox(int cx, int cy, int r, ObjectID id) {
    int r2 = r * r;
    for (int dy = -r; dy <= r; dy++) {
        int py = cy + dy;
        if (py < 0 || py >= height) continue;
        for (int dx = -r; dx <= r; dx++) {
            int px = cx + dx;
            if (px < 0 || px >= width) continue;
            if (dx * dx + dy * dy <= r2)
                hitboxBuffer[py * width + px] = static_cast<uint32_t>(id);
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

void drawObjectWH(Object& object) {
    switch (object.shape) {
    case Shape::EMPTY:
        return;
    case Shape::RECTANGLE: {
        rectangle* rect = dynamic_cast<rectangle*>(&object);
        if (rect) {
            drawRectWH(rect->x, rect->y, rect->w, rect->h, rect->color, rect->id);
        }
        break;
    }
    case Shape::CIRCLE: {
        circle* c = dynamic_cast<circle*>(&object);
        if (c) {
            drawCircleWH(c->x, c->y, c->r, c->color, c->id);
        }
        break;
    }
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
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

    PlayerRect player({ width / 2, height / 2, 20, 20, 4, ObjectID::PLAYER, static_cast<uint32_t>(0xFFFF0000)});

    circle circ({ 400, 400, 40, static_cast<uint32_t>(0xFFFF0000), ObjectID::SOLID });

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
        for (int i = 0; i < width * height; i++)     {
            frameBuffer[i] = 0xFF101010; // dark background
            hitboxBuffer[i] = static_cast<uint32_t>(ObjectID::EMPTY);
        }

        // Draw player
        // drawRectWH(player.x, player.y, player.w, player.h, 0xFFFF0000, ObjectID::PLAYER); // red square

        drawObjectWH(player);

        drawObjectWH(circ);
        // drawCircleWH(400, 400, 40, 0xFFFF0000, ObjectID::SOLID);

        prevPos = { player.x, player.y };

        // Button handling
        if (keys[VK_LEFT] || keys[0x41])  player.x -= player.speed;
        if (keys[VK_RIGHT] || keys[0x44]) player.x += player.speed;
        if (keys[VK_UP] || keys[0x57])    player.y -= player.speed;
        if (keys[VK_DOWN] || keys[0x53])  player.y += player.speed;

        // Clamp to window boundaries
        if (player.x < 0 + player.w) player.x = player.w;
        if (player.x >= width - player.w) player.x = width - 1 - player.w;
        if (player.y < 0 + player.h) player.y = player.h;
        if (player.y >= height - player.h) player.y = height - 1 - player.h;

        if (player.checkCollision()) {
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
