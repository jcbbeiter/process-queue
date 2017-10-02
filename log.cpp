#include <iostream>
#include <unistd.h>

#include "types.h"
#include "log.h"
#include "scheduler.h"
#include "handler.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// log.cpp

extern config_struct config;
extern scheduler_struct scheduler;

void log(int type, std::string message) {
    time_t time_val;
    time(&time_val);

    std::cout << '[' << time_val << "] ";
    if(type == LOG_INFO)
        std::cout << "INFO  ";
    else if(type == LOG_ERROR)
        std::cout << "ERROR ";

    std::cout << message << std::endl;
}

void log_exit(int type, std::string message) {
    log(type, message);
    cleanup(EXIT_FAILURE);
}

void cleanup(int exit_code) {

    if(config.is_server) {
        log(LOG_INFO, "Cleaning up...");

        if(config.sock_fd > 0) {
            log(LOG_INFO, "Removing socket \"" + config.sock_path + "\" ...");
            unlink(config.sock_path.c_str());
            close(config.sock_fd);
        }
    }

    int running;
    int waiting;
    switch(scheduler.policy) {
        case SCHEDULE_FIFO:
        case SCHEDULE_RDRN:
            running = flush_queue(scheduler.running_queue);
            waiting = flush_queue(scheduler.waiting_queue);
            break;
        case SCHEDULE_MLFQ:
            running = flush_queue(scheduler.running_queue);
            for(int i = 0; i < scheduler.levels; i++) {
            waiting += flush_queue(scheduler.waiting_queues[i]);
            }
            break;
    }
    log(LOG_INFO,"Flushed " + std::to_string(running) + " running and " + std::to_string(waiting) + " waiting processes");

    log(LOG_INFO, "Goodbye!");
    exit(exit_code);
}
