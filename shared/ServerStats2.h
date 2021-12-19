#pragma once

#include <thread>
#include "HashMap.h"
#include "cinttypes"

class ServerStats2 {
public:
    explicit ServerStats2(std::string filepath, bool erase_previous_file = true);

    ~ServerStats2();

    void add_entry(uint64_t query_uid, uint64_t timestamp);

    void dump_server_stats(bool overwrite = false);

    void set_batch_write_interval(std::chrono::milliseconds batch_write_interval);

private:
    //                                                   query_uid, timestamp in us
    mutable std::shared_mutex mutex_;
    std::unordered_map<std::thread::id, std::vector<std::tuple<uint64_t, uint64_t>>> hashMap;
    const std::string filepath;

    std::chrono::milliseconds batch_write_interval;
    std::thread thread1;
    bool stop;
};
