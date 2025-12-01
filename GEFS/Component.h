#include "vectors.h"

class Component { // ALL COMPONENTS MUST INHERIT THIS CLASS OR NOT COMPATIBLE WITH GAMEOBJECTS
public:
	virtual ~Component() = default;
};

class Rectangle2d : public Component {
public:
	Vector2d size;
	
	Rectangle2d(Vector2d size_) : size(size_) {};
};