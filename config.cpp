#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>

#include "config.h"
#include "log.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// config.cpp

void usage(int exit_code) {

    std::string usage_str = 
    "Usage: ./pq [Options]\n"
    "\n"
    "General Options:\n"
    "   -h                 Print this help message\n"
    "   -f PATH            Path to IPC channel\n"
    "\n"
    "Client Options:\n"
    "   add COMMAND        Add COMMAND to queue\n"
    "   status             Query status of queue\n"
    "   running            Query running jobs\n"
    "   waiting            Query waiting jobs\n"
    "   flush              Remove all jobs from queue\n"
    "\n"
    "Server Options:\n"
    "   -n NCPUS           Number of CPUs\n"
    "   -p POLICY          Scheduling policy (fifo, rdrn, mlfq)\n"
    "   -t MICROSECONDS    Time between scheduling\n";

    std::cout << usage_str << std::endl;

    exit(exit_code);
}

config_struct read_args(int argc, char* argv[]) {

    config_struct config;

    //read command line arguments
    int argind = 1;
    
    //read all flags starting with '-', anything after is client commands
    while (argind < argc) {

        char* arg = argv[argind];

        if (arg[0] != '-')
            break;

        switch (arg[1]) {
            case 'h':
                usage(0);
                break;
            case 'f':
                config.sock_path = argv[++argind];
                break;
            case 'n':
                config.ncpus = std::stoi(argv[++argind]);
                break;
            case 'p':
                    argind++;
                    if(strcmp(argv[argind],"fifo") == 0)
                        config.policy = SCHEDULE_FIFO;
                    else if(strcmp(argv[argind],"rdrn") == 0)
                        config.policy = SCHEDULE_RDRN;
                    else if(strcmp(argv[argind],"mlfq") == 0)
                        config.policy = SCHEDULE_MLFQ;
                    else {
                        log_exit(LOG_ERROR, "Unknown scheduling policy \"" + std::string(argv[argind]) + "\"");
                    }
                break;
            case 't':
                config.microseconds = std::stoi(argv[++argind]);
                break;
            default:
                usage(1);
        }
        argind++;
    }

    //if we've read all arguments, should run process as the server
    if(argind == argc) {
        config.is_server = true;
        return config;
    }
    else { //otherwise, it's the client
        config.is_server = false;
        config.client_option = argv[argind];
        if(config.client_option == "add") {
            config.command = "";
            while(++argind < argc) {
                config.command += argv[argind];
            }
            if(config.command == "") {
                log_exit(LOG_ERROR, "No command to add to server, exiting...");
            }
        }
        else if (config.client_option != "status" &&
                    config.client_option != "running" &&
                    config.client_option != "waiting" &&
                    config.client_option != "flush") {
            log_exit(LOG_ERROR, "Invalid client command \"" + config.client_option + "\", exiting...");
        }
        else if (argind != argc-1) { //none of the other options should have anything after
            log_exit(LOG_ERROR,"Too many command line arguments, exiting...");
        }
    }

    return config;
}
