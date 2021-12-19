#pragma once

#include <future>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <mutex>

using namespace std;

#define VECTOR_BUFFER_VERSION

// Other path to try:
// https://stackoverflow.com/questions/21126950/asynchronously-writing-to-a-file-in-c-unix


#define SERVER_STATS_BLOCK(MANAGER, BODY) \
    MANAGER.add_entry(query_uid, get_epoch_time_us()); \
    BODY \
    MANAGER.add_entry(query_uid, get_epoch_time_us());

class ServerStatsManager;

void save_test_file(uint32_t row_count = 10,
                    uint32_t col_count = 2);

float server_stats_stress_measurement(ServerStatsManager &manager,
                                      uint32_t entry_test_number,
                                      chrono::microseconds waiting_time);

void server_stats_stress(ServerStatsManager &manager,
                         uint32_t entry_test_number = 5000,
                         chrono::microseconds waiting_time = 1000us);


void dump_matrix(const string &filename,
                 const vector<vector<uint64_t>> &data,
                 int64_t until = -1,
                 bool overwrite = false);

#ifdef VECTOR_BUFFER_VERSION

class ServerStatsManager {
public:
    static const uint32_t DEFAULT_ENTRY_COUNT = 32768;
private:
    vector<vector<vector<uint64_t>>> data_buffers;
    string filename;
    bool overwrite;
    bool async_dumping;
    uint32_t active_buffer;
    uint32_t entry_cursor;
    uint32_t entry_count;
    uint32_t buffer_count;
    mutex entry_saving_mutex;
public:
    ServerStatsManager(const string &filename,
                       bool async_dumping = false,
                       uint32_t entry_count = DEFAULT_ENTRY_COUNT,
                       uint32_t buffer_count = 2,
                       bool overwrite = false,
                       bool erase_previous_file = true) :
            data_buffers(buffer_count, vector<vector<uint64_t>>(entry_count, vector<uint64_t>(2, 0))),
            filename(filename),
            overwrite(overwrite),
            async_dumping(async_dumping),
            active_buffer(0),
            entry_cursor(0),
            entry_count(entry_count),
            buffer_count(buffer_count) {
        if (erase_previous_file) {
            ofstream file;
            file.open(filename, ofstream::trunc);
            file.close();
        }

    }

    ~ServerStatsManager() {
        this->dump_server_stats(filename, entry_cursor);
    }

    void dump_server_stats(const string &filename = "", uint32_t until = -1);

    void add_entry(uint64_t query_id, uint64_t epoch_time_ms);

    uint32_t get_entry_count();

};

#else
class ServerStatsManager{
    public:
        static const uint32_t DEFAULT_ENTRY_COUNT = 32768;
    private:
        ofstream file_stream;
        uint32_t entry_cursor;
        uint32_t entry_count;
    public:
        ServerStatsManager(const string& filename,
                           uint32_t entry_count=DEFAULT_ENTRY_COUNT,
                           bool overwrite=false,
                           bool erase_previous_file=true):
            entry_cursor(0),
            entry_count(entry_count)
        {
            if (erase_previous_file){
                ofstream file;
                file.open(filename, ofstream::trunc);
                file.close();
            }

            file_stream.open(filename, overwrite ? ofstream::trunc : ofstream::app);


        }
        ~ServerStatsManager(){
            file_stream.close();
        }

        void add_entry(uint64_t query_id, uint64_t epoch_time_ms);

};
#endif