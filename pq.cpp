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

// config and scheduler are global - to be universally accessible
config_struct config;
scheduler_struct scheduler;

void sigint_handler(int signal) {
    log(LOG_INFO, "Caught SIGINT");
    cleanup(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    //set interrupt handler
    signal(SIGINT,sigint_handler);

    //read command line arguments into config struct
    read_args(argc, argv);

    if(config.is_server) {
        server();
    }
    else {
        client();
    }

    cleanup(EXIT_SUCCESS);

    return 0;
}
