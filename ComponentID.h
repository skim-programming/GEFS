#pragma once
#include <type_traits>
#include <cstddef>

class Component;

using ComponentTypeID = size_t;

inline ComponentTypeID GetNextComponentTypeID() {
    static ComponentTypeID lastID = 0;
    return lastID++;
}

template<typename T>
ComponentTypeID GetComponentTypeID() {
    static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");

    static ComponentTypeID typeID = GetNextComponentTypeID();
    return typeID;
}