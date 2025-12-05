//#include "object.h"
//#include "globals.h"
//#include <iostream>
//
//Object::Object(std::initializer_list<Param> list) {
//    int intIndex = 0;
//    for (const auto& p : list) {
//
//        std::visit([&](auto&& arg) {
//            using T = std::decay_t<decltype(arg)>;
//
//            if constexpr (std::is_same_v<T, int>) {
//                switch (intIndex++) {
//                case 0: this->x = arg; break;
//                case 1: this->y = arg; break;
//                }
//            }
//            else if constexpr (std::is_same_v<T, uint32_t>) {
//                this->color = arg;
//            }
//            else if constexpr (std::is_same_v<T, ObjectID>) {
//                this->id = arg;
//            }
//            else if constexpr (std::is_same_v<T, Shape>) {
//                this->shape = arg;
//            }
//
//            }, p);
//    }
//}
//
//rectangle::rectangle(std::initializer_list<Param> list) {
//    int intIndex = 0;
//
//    for (const auto& p : list) {
//        std::visit([&](auto&& arg) {
//            using T = std::decay_t<decltype(arg)>;
//
//            if constexpr (std::is_same_v<T, int>) {
//                // sequentially assign ints:
//                // 0->x, 1->y, 2->w, 3->h, 4->size, 5->speed
//                switch (intIndex++) {
//                case 0: this->x = arg; break;
//                case 1: this->y = arg; break;
//                case 2: this->w = arg; break;
//                case 3: this->h = arg; break;
//                }
//            }
//            else if constexpr (std::is_same_v<T, uint32_t>) {
//                this->color = arg;
//            }
//            else if constexpr (std::is_same_v<T, ObjectID>) {
//                this->id = arg;
//            }
//            }, p);
//    }
//
//}
//
//bool rectangle::checkCollision() {
//    {
//        for (int py = y; py < y + h; py++)
//        {
//            if (py < 0 || py >= h) continue;
//            for (int px = x; px < x + w; px++)
//            {
//                if (px < 0 || px >= w) continue;
//
//                ObjectID id = static_cast<ObjectID>(hitboxBuffer[py * width + px]);
//
//                if (id == ObjectID::EMPTY || this->id == id) continue;
//
//                switch (id)
//                {
//                case ObjectID::PLAYER: // logging
//                    std::cout << "Player hit \n";
//                    return true;
//
//                case ObjectID::ENEMY:
//                    std::cout << "Enemy hit \n";
//                    return true;
//
//                case ObjectID::SOLID:
//                    std::cout << "Solid hit \n";
//                    return true;
//                default:
//                    return true;
//                }
//            }
//        }
//        return false;
//    }
//}
//
//circle::circle(std::initializer_list<Param> list) {
//    int intIndex = 0;
//    this->shape = Shape::CIRCLE;
//
//    for (const auto& p : list) {
//        std::visit([&](auto&& arg) {
//            using T = std::decay_t<decltype(arg)>;
//            if constexpr (std::is_same_v<T, int>) {
//                switch (intIndex++) {
//                case 0: this->x = arg; break;
//                case 1: this->y = arg; break;
//                case 2: this->r = arg; break;
//                }
//            }
//            else if constexpr (std::is_same_v<T, uint32_t>) {
//                this->color = arg;
//            }
//            else if constexpr (std::is_same_v<T, ObjectID>) {
//                this->id = arg;
//            }
//            }, p);
//    }
//}
//
//bool circle::checkCollision() {
//    int r2 = r * r;
//
//    for (int dy = -r; dy <= r; dy++) {
//        int py = y + dy; // center-based
//        if (py < 0 || py >= height) continue;
//
//        for (int dx = -r; dx <= r; dx++) {
//            int px = x + dx;
//            if (px < 0 || px >= width) continue;
//
//            if (dx * dx + dy * dy > r2) continue;
//
//            ObjectID id = static_cast<ObjectID>(hitboxBuffer[py * width + px]);
//            if (id == ObjectID::EMPTY || id == this->id) continue;
//
//            switch (id) {
//            case ObjectID::PLAYER: std::cout << "Player hit\n"; return true;
//            case ObjectID::ENEMY:  std::cout << "Enemy hit\n"; return true;
//            case ObjectID::SOLID:  std::cout << "Solid hit\n"; return true;
//            default: return true;
//            }
//        }
//    }
//    return false;
//}