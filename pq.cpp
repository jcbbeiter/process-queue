#include <iostream>
#include <csignal>

#include "types.h"
#include "config.h"
#include "log.h"
#include "server.h"
#include "client.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// pq.cpp

// config is global - to be universally accessible
config_struct config;

void sigint_handler(int signal) {
    log(LOG_INFO, "Caught SIGINT");
    cleanup(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    //set interrupt handler
    signal(SIGINT,sigint_handler);

    //read command line arguments into config struct
    read_args(argc, argv);

    /*std::cout << "sock_path: " << config.sock_path << std::endl;
    std::cout << "ncpus: " << config.ncpus << std::endl;
    std::cout << "policy: " << config.policy << std::endl;
    std::cout << "microseconds: " << config.microseconds << std::endl;
    std::cout << "is_server: " << config.is_server << std::endl;
    std::cout << "client_option: " << config.client_option << std::endl;
    std::cout << "command: " << config.command << std::endl;*/

    if(config.is_server) {
        server();
    }
    else {
        client();
    }

    cleanup(EXIT_SUCCESS);

    return 0;
}
