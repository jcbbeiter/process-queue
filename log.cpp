#include <iostream>
#include <unistd.h>

#include "types.h"
#include "log.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// log.cpp

extern config_struct config;

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

    if(config.sock_fd > 0) {
        log(LOG_INFO, "Removing socket \"" + config.sock_path + "\" ...");
        std::cout << "config.sock_fd: " << config.sock_fd << std::endl;
        unlink(config.sock_path.c_str());
    }

    log(LOG_INFO, "Goodbye!");
    exit(exit_code);
}
