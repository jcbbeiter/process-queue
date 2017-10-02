#ifndef TYPES_H
#define TYPES_H

#include <deque>
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
    int num;
    std::string command;
    std::string state;
    unsigned long user_time;
    unsigned long threshold;
    double usage;
    time_t arrival_time;
    time_t start_time;
    bool killed;
};

struct process_record {
    std::string command;
    time_t turnaround_time;
    time_t response_time;
};

struct scheduler_struct {
    int policy;
    int count;
    std::vector<process_record> records;
    std::deque<process> running_queue;

    // FIFO
    std::deque<process> waiting_queue;
    
    // RDRN

    // MLFQ
};

#endif
