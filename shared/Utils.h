#include "cinttypes"

uint64_t get_epoch_time_us(){
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}