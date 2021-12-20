//
// Created by adrien on 19.12.21.
//
#include <chrono>
#include <signal.h>
#include <thread>
#include "Utils.h"

#include <cmath>

uint64_t get_epoch_time_us() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

uint64_t generate_local_uid(){
    uint64_t ip = 0;
    #ifdef LOCALHOST
        ip = hash<thread::id>()(this_thread::get_id());
    #else
        string sip = exec("hostname -i");
        uint64_t is[4] = {0,0,0,0};
        sscanf(sip.c_str(), "%ld.%ld.%ld.%ld", &(is[3]), &(is[2]), &(is[1]), is);
        ip = (is[3] << 24) | (is[3] << 16) | (is[3] << 8) | is[0];
    #endif
    srand(std::time(nullptr));
    uint64_t random = rand() % 16;
    ip = ((ip << 4) | random ) << 28;
    return ip;
}

uint64_t get_query_uid(uint64_t local_uid, uint64_t query_number){
    return local_uid | ((query_number << 36) >> 36);
}

void sigint_catcher(void (*handler)(int)){
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
}

string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// FUNCTION DEPENDENT ON THE HARDWARE
double fake_worker(float factor){
    double a = 1;
    // MAGIC FACTOR FOUND BY TESTING THE FUNCTION
    auto iterations = static_cast<uint64_t>(factor * 61.22369);
    for (uint64_t i=0; i < iterations; i++){
        // RANDOM GARBAGE OPERATIONS
        a += powf64(a, 6000);
        a -= powf64(a, 5999);
    }
    return a;
}

float normal_distributed_value(float mean, float std){
    //random_device dev;
    //mt19937 generator(dev());
    //normal_distribution<float> normal_dist(mean, std);
    // normal_dist(generator)
    return mean;
}
