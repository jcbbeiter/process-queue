#ifndef TYPES_H
#define TYPES_H

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
    std::string command;
};

#endif
