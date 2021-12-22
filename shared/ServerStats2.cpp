//
// Created by adrien on 19.12.21.
//

#include "ServerStats2.h"
#include <thread>
#include <utility>
#include <iostream>
#include <fstream>

ServerStats2::ServerStats2(std::string filepath, bool erase_previous_file)
        : filepath(std::move(filepath)), stop(false), batch_write_interval(1000) {
    if (erase_previous_file) {
        std::ofstream file;
        file.open(this->filepath, std::ofstream::trunc);
        file.close();
    }

    thread1 = std::thread([&]() {
        while (!stop) {
            std::this_thread::sleep_for(batch_write_interval);

            dump_server_stats();
        }
    });
}

ServerStats2::~ServerStats2() {
    stop = true;
    thread1.join();
}

void ServerStats2::add_entry(uint64_t query_uid, uint64_t timestamp) {

    if (mutex_.try_lock_shared()) { // If it can't lock just return without waiting. This skip a few datapoints but nothing terrible

        auto search = hashMap.find(std::this_thread::get_id());
        if (search != hashMap.end()) {
            search->second.emplace_back(std::make_tuple(query_uid, timestamp));
            mutex_.unlock_shared();
        } else {
            mutex_.unlock_shared();
            mutex_.lock();

            std::vector<std::tuple<uint64_t, uint64_t>> newlog;
            newlog.emplace_back(std::make_tuple(query_uid, timestamp));
            hashMap.insert({std::this_thread::get_id(), newlog});

            mutex_.unlock();
        }
    }
}

void ServerStats2::dump_server_stats(bool overwrite) {
    mutex_.lock();

    std::ofstream file;
    file.open(filepath, overwrite ? std::ofstream::trunc : std::ofstream::app);
    for (auto &it: hashMap) {
        for (auto &elem: it.second) {
            file << std::get<0>(elem) << "," << std::get<1>(elem) << "\n";
        }
        it.second.clear();
    }
    file.close();

    mutex_.unlock();
}

void ServerStats2::set_batch_write_interval(std::chrono::milliseconds batch_write_interval) {
    this->batch_write_interval = batch_write_interval;
}


