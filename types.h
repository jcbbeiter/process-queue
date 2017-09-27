#ifndef TYPES_H
#define TYPES_H

#include <deque>
#include <map>
#include <vector>

enum Scheduling_Policy {
    SCHEDULE_FIFO,
    SCHEDULE_RDRN,
    SCHEDULE_MLFQ
};

enum Log_Types {
    LOG_INFO,
    LOG_ERROR,
};

struct config_struct {
    std::string sock_path;
    int sock_fd;
    int ncpus;
    int policy;
    int microseconds;
    bool is_server;
    std::string client_option;
};

struct process {
    int pid;
    std::string command;
    double user_time;
    double threshold;
    double usage;
    time_t arrival_time;
    time_t start_time;
};

struct process_record {
    std::string command;
    time_t turnaround_time;
    time_t response_time;
};

struct scheduler_struct {
    int policy;
    std::vector<process_record> records;

    // FIFO
    std::map<int,process> running_map;
    std::deque<process> waiting_queue;
    
    // RDRN

    // MLFQ
};

#endif
