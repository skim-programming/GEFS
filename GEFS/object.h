#include <variant>


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
    uint32_t color = 000000;
    ObjectID id = ObjectID::EMPTY;
    Shape shape = Shape::EMPTY;

    virtual ~Object() = default;

    virtual bool checkCollision() {
        return false;
    };

    using Param = std::variant<int, uint32_t, ObjectID, Shape>;
    Object() = default;
    Object(std::initializer_list<Param> list);
};

class rectangle : public Object {
public:
    int x{0};
    int y{0};
    int w{0};
    int h{0};

    using Param = std::variant<int, uint32_t, ObjectID>;

    rectangle() : Object({ Shape::RECTANGLE }) {};

    bool checkCollision() override;

    /// <summary>
    /// Params are x, y, w, h, color(static cast uint32_t), id
    /// </summary>
    /// <param name="list"></param>
    rectangle(std::initializer_list<Param> list);
};

class circle : public Object {
public:
    int r{0};

    using Param = std::variant<int, uint32_t, ObjectID>;

    bool checkCollision() override;

    circle() : Object({ Shape::CIRCLE }) {};

    /// <summary>
    /// Params are x, y, r, color(static cast uint32_t), id
    /// </summary>
    /// <param name="list"></param>
    circle(std::initializer_list<Param> list);
};