#include "mbed.h"

#ifndef __TYPES_HPP__
#define __TYPES_HPP__

typedef enum : uint8_t { PINSTAT_HIGH, PINSTAT_LOW, PINSTAT_HIZ } pinstat_t;

template <typename T>
class Array {
public:
    Array(T buf[], size_t size) {
        this->buf = buf;
        this->size = size;
        this->allocated = false;
    }

    Array(size_t size) {
        this->buf = new T[size];
        this->size = size;
        this->allocated = true;
    }

    ~Array() {
        if (allocated) {
            delete[] buf;
        }
    }

    T& operator[](size_t i) {
        return this->buf[i];
    }

    T*     buf;
    size_t size;
    bool   allocated;
};

constexpr uint32_t BIT(uint32_t pos) {
    return 1u << pos;
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#endif // __TYPES_HPP__
