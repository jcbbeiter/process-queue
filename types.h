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
    //initializer with defaults
    config_struct() {
        sock_path = "./pq.socket";
        sock_created = false;
        ncpus = 1;
        policy = SCHEDULE_FIFO;
        microseconds = 10;
    }

    std::string sock_path;
    bool sock_created;
    int ncpus;
    int policy;
    int microseconds;
    bool is_server;
    std::string client_option;
    std::string command;
};

#endif
