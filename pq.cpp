#include <iostream>
#include <csignal>

#include "types.h"
#include "config.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// pq.cpp

void sigint_handler(int signal) {
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    //set interrupt handler
    signal(SIGINT,sigint_handler);

    //read command line arguments into config struct
    config_struct config = read_args(argc, argv);

    std::cout << "sock_path: " << config.sock_path << std::endl;
    std::cout << "ncpus: " << config.ncpus << std::endl;
    std::cout << "policy: " << config.policy << std::endl;
    std::cout << "microseconds: " << config.microseconds << std::endl;
    std::cout << "server: " << config.server << std::endl;
    std::cout << "client_option: " << config.client_option << std::endl;
    std::cout << "command: " << config.command << std::endl;


    return 0;
}