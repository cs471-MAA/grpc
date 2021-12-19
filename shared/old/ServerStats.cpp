#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "ServerStats.h"

using namespace std;


// int main(){
//     // save_test_file();
//     uint32_t entry_number = 10000;
//     chrono::microseconds waiting_time = 100us;
//     uint32_t iterations = 20;

//     #ifdef VECTOR_BUFFER_VERSION
//         vector<ServerStatsManager> managers{
//             ServerStatsManager("dump_file_1.csv", false),
//             ServerStatsManager("dump_file_2.csv", true),
//             // ServerStatsManager("dump_file_3.csv", false, 1024, 2),
//             // ServerStatsManager("dump_file_4.csv", true, 1024, 2),
//             // ServerStatsManager("dump_file_5.csv", false, 16, 16),
//             // ServerStatsManager("dump_file_6.csv", true, 16, 16),
//         };

//         vector<float> averages = vector<float>(managers.size(), 0);

//         for (int m = 0; m < managers.size(); m++){
//             for (int i = 0; i < iterations; i++) {
//                 averages[m] += server_stats_stress_measurement(managers[m], entry_number, waiting_time);
//             }
//             averages[m] /= iterations;
//             cout << averages[m] << " ms\n";
//         }
//     #else
//         auto manager = ServerStatsManager("dump_file_1.csv");
//         auto manager2 = ServerStatsManager("dump_file_2.csv", 512);
//         float average = 0.0f;

//         for (int i = 0; i < iterations; i++) {
//             average += server_stats_stress_measurement(manager, entry_number, waiting_time);
//         }
//         average /= iterations;
//         cout << average << " ms\n";
//         average = 0.0f;
//         for (int i = 0; i < iterations; i++) {
//             average += server_stats_stress_measurement(manager2, entry_number, waiting_time);
//         }
//         average /= iterations;
//         cout << average << " ms\n";

//     #endif 
//     return 0;
// }

float server_stats_stress_measurement(ServerStatsManager& manager, 
                                     uint32_t entry_number, 
                                     chrono::microseconds waiting_time){
    auto start = chrono::system_clock::now();
    server_stats_stress(manager, entry_number, waiting_time);
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start);
    
    auto expected_time = chrono::duration_cast<chrono::milliseconds>(entry_number * waiting_time); 
    // cout << elapsed.count() - expected_time.count() << " ms\n";
    return (float) elapsed.count() - expected_time.count();
}

void server_stats_stress(ServerStatsManager& manager,
                         uint32_t entry_test_number,
                         chrono::microseconds waiting_time){

    uint64_t epoch_time_ms = 0;
    for (uint64_t i = 0; i < entry_test_number; i++){
        uint64_t epoch_time_ms = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
        manager.add_entry(i, epoch_time_ms);
        this_thread::sleep_for(waiting_time);
        manager.add_entry(i, epoch_time_ms);
    }
}


void save_test_file(uint32_t row_count, uint32_t col_count){
    // Line from local computer to get the files
    // docker cp <Container_ID>:/app/src/build/shared/example.txt example.txt
    
    vector<vector<uint64_t>> test_data(row_count, vector<uint64_t>(col_count, 0));

    for (uint32_t i = 0; i < row_count; i++){
        for (uint32_t j = 0; j < col_count; j++){
            test_data[i][j] = i + j;
        }
    }

    dump_matrix("example.csv", test_data, -1, true);

}

void dump_matrix(const string& filename, 
                 const vector<vector<uint64_t>>& data, 
                 int64_t until,
                 bool overwrite){
    uint32_t row_count = data.size();
    if (row_count <= 0)
        return;
    
    uint32_t col_count = data[0].size();
    if (until >= 0 && until < row_count){
        row_count = until;
    }

    ofstream file;
    file.open(filename, overwrite ? ofstream::trunc : ofstream::app);
    for (uint32_t i = 0; i < row_count; i++){
        for (uint32_t j = 0; j < col_count - 1; j++){
            file << data[i][j] << ",";
        }
        file << data[i][col_count - 1] << "\n";
    }
    file.close();
}

#ifdef VECTOR_BUFFER_VERSION
    void ServerStatsManager::dump_server_stats(const string& filename, uint32_t until){
        auto start = chrono::system_clock::now();
        if (async_dumping){
            auto a = async(std::launch::async, dump_matrix, filename, data_buffers[active_buffer], until, overwrite);
            // auto a = async(std::launch::deferred, dump_matrix, filename, data_buffers[active_buffer], until, overwrite);
            // auto a = async(dump_matrix, filename, data_buffers[active_buffer], until, overwrite);
        } else{
            dump_matrix((filename == "") ? this->filename : filename, data_buffers[active_buffer], until, overwrite);
        }
        auto elapsed = chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now() - start);
        cout << elapsed.count() /1000.f << " us\n";
    }

    void ServerStatsManager::add_entry(uint64_t query_id, uint64_t epoch_time_ms){
        if (entry_cursor >= entry_count){
            // dump_server_stats();
            entry_cursor = 0;
            active_buffer = (active_buffer + 1) % buffer_count;    
        }

        unique_lock<mutex> entry_saving_lock(entry_saving_mutex, defer_lock);
        entry_saving_lock.lock();
            data_buffers[active_buffer][entry_cursor][0] = query_id;
            data_buffers[active_buffer][entry_cursor][1] = epoch_time_ms;
            entry_cursor += 1;
        entry_saving_lock.unlock();
    }

    uint32_t ServerStatsManager::get_entry_count(){
        return entry_count;
    }
#else

    void ServerStatsManager::add_entry(uint64_t query_id, uint64_t epoch_time_ms){
        if (entry_cursor >= entry_count){
            file_stream.flush();
            entry_cursor = 0;
        }

        file_stream << query_id << "," << epoch_time_ms << "\n";
        entry_cursor += 1;
    }
#endif