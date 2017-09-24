#ifndef TYPES_H
#define TYPES_H

enum Scheduling_Policy {
    SCHEDULE_FIFO,
    SCHEDULE_RDRN,
    SCHEDULE_MLFQ
};

struct config_struct {
    //initializer with defaults
    config_struct() {
        sock_path = "./pq_socket";
        ncpus = 1;
        policy = SCHEDULE_FIFO;
        microseconds = 10;
    }

    std::string sock_path;
    int ncpus;
    int policy;
    int microseconds;
    bool server;
    std::string client_option;
    std::string command;
};

#endif
