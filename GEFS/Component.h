#include "vectors.h"
#include <unordered_set>

class Component { // ALL COMPONENTS MUST INHERIT THIS CLASS OR NOT COMPATIBLE WITH GAMEOBJECTS
public:
	virtual ~Component() = default;
	template<typename T>
	bool getInMEC() const { return (muteExComponents.find(std::type_index(typeid(T))) != muteExComponents.end()); }
	std::unordered_set<std::type_index> getMEC() const { return muteExComponents; }; // mutually exclusive components
protected:
	std::unordered_set<std::type_index> muteExComponents;
	template <typename... Args>
	bool addToMEC(Args&&... args) {
		static_assert((std::is_base_of<Component, std::decay_t<Args>>::value && !std::is_same<Component, std::decay_t<Args>>::value && ...), "Args must inherit from Component (cannot be Component itself)")
			try {
			(muteExComponents.emplace(std::type_index(typeid(args))), ...);
			return true;
		}
		catch (const std::exception e) {
			std::cerr << "Error in addToMEC: " << e.what() << "\n";
			return false;
		}
		catch (...) {
			std::cerr << "Unknown error in addToMEC" << "\n";
			return false;
		}
	}
};

class Rectangle2d : public Component {
public:
	Vector2d size;
	Rectangle2d(Vector2d size_) : size(size_) { addToMEC(Component()); };
};
