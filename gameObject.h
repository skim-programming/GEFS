#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>
#include "Component.h"

class GameObject {
private:
    std::unordered_map<ComponentTypeID, std::shared_ptr<Component>> components;

public:
    template<typename T, typename... Args>
    T& add_component(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value && !std::is_same_v<Component, T>, "T must inherit from Component (cannot be Component itself)");
        
        for (ComponentTypeID ex : T::Exclusions()) {
            if (this->has_component(ex)) {
                throw std::runtime_error("Component is mutually exclusive");
            }
        }

        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *comp;

        components[GetComponentTypeID<T>()] = std::move(comp);
        return ref;
    }

    template<typename T, typename Func>
    T* edit_component(Func&& func) { // lambda or function that takes T&
        auto it = components.find(GetComponentTypeID<T>);
        if (it != components.end()) {
            auto comp = static_cast<T*>(it->second.get());
            it->second = func(*comp);
        }
        else {
            throw std::runtime_error("Component not found");
        }
    }

    template<typename T>
    T* get_component() {
        auto it = components.find(GetComponentTypeID<T>);
        return it == components.end() ? nullptr : static_cast<T*>(it->second.get());
    }

    bool has_component(ComponentTypeID id) const {
        return components.contains(id);
    }
};
