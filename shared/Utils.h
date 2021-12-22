#pragma once

#include "cinttypes"
#include <string>
#include <random>

using namespace std;

uint64_t get_epoch_time_us();

string exec(const char* cmd);

uint64_t generate_local_uid();

uint64_t get_query_uid(uint64_t local_uid, uint64_t query_number);

void sigint_catcher(void (*handler)(int));

double fake_worker(float factor);

float normal_distributed_value(float mean, float std);