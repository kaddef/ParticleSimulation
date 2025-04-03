//
// Created by erkan on 19.03.2025.
//

#ifndef KEY_H
#define KEY_H
#include <functional>

struct Key {
    int x;
    int y;

    Key() = default;
    Key(int x_val = 0, int y_val = 0) : x(x_val), y(y_val) {}
    // Equality operator
    bool operator==(const Key& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Key& other) const {
        return x != other.x || y != other.y;
    }
};

struct KeyHasher {
    std::size_t operator()(const Key& k) const {
        // Combine x and y to create a hash value
        return std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1);
    }
};
#endif //KEY_H
