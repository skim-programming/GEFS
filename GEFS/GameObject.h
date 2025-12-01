#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>
#include "Component.h"

class GameObject {
private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

public:
    template<typename T, typename... Args>
    void add_component(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value && !std::is_same<Component, T>, "T must inherit from Component (cannot be Component itself)");
        components[std::type_index(typeid(T))] = std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename Func>
    void edit_component(Func&& func) {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            auto comp = std::static_pointer_cast<T>(it->second);
            func(*comp);
        }
        else {
            throw std::runtime_error("Component not found");
        }
    }

    template<typename T>
    std::shared_ptr<T> get_component() {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template<typename T>
    bool has_component() const {
        return components.find(std::type_index(typeid(T))) != components.end();
    }
};
