#include <iostream>

#include "types.h"
#include "log.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// log.cpp

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

    log(LOG_INFO, "Cleaning up...");

    log(LOG_INFO, "Goodbye!");
    exit(exit_code);
}
