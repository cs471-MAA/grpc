//
// Created by adrien on 19.12.21.
//
#include <chrono>
#include "Utils.h"

uint64_t get_epoch_time_us() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}