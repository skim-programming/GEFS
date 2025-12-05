#include "vectors.h"
#include "sprite.h"
#include <unordered_set>

class Rectangle2d;
class Circle2d;

class Component { // ALL COMPONENTS MUST INHERIT THIS CLASS OR NOT COMPATIBLE WITH GAMEOBJECTS
public:
	virtual ~Component() = default;
};

#include "ComponentID.h"

class Rectangle2d : public Component {
public:
	Vector2d dimensions;
	Vector2d pos;
	static std::vector<ComponentTypeID> Exclusions() {
		return { GetComponentTypeID<Circle2d>() };
	}
	Rectangle2d() = default;
	Rectangle2d(Vector2d dimensions_, Vector2d pos_) : dimensions(dimensions_), pos(pos_) {};
};

class Circle2d : public Component {
public:
	float radius;
	Vector2d pos;
	static std::vector<ComponentTypeID> Exclusions() {
		return { GetComponentTypeID<Rectangle2d>() };
	}
	Circle2d() = default;
	Circle2d(float radius_, Vector2d pos_) : radius(radius_), pos(pos_) {};
};

class SpriteRenderer : public Component {
public:
	int w, h, c;
	float sw = 1.0f, sh = 1.0f; // scaled w/h, leave 1 if no scaling
	int x, y;
	std::vector<unsigned char> pixels;

	SpriteRenderer* SetSprite(std::string path, int x, int y);
	SpriteRenderer* SetSpriteScale(float sw, float sh);

	static std::vector<ComponentTypeID> Exclusions() {
		return {};
	}

	SpriteRenderer() = default;
	SpriteRenderer(std::string path, int x, int y);
};