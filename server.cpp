#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>

#include "types.h"
#include "log.h"
#include "server.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// server.cpp

extern config_struct config;

int create_socket() {

    log(LOG_INFO, "Attempting to create unix socket at \"" + config.sock_path + "\" ...");

    sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, config.sock_path.c_str(), config.sock_path.length()+1);

    int fd;

    if((fd = socket(AF_UNIX, SOCK_STREAM,0)) == -1)
        return -1;

    if(bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1)
        return -1;

    //store in config only if successfully bind and need to delete on cleanup
    config.sock_fd = fd;

    if(listen(config.sock_fd,1) == -1)
        return -1;
    
    return 0;
}

void server() {
    log(LOG_INFO, "Starting process server...");

    if(create_socket() == -1) {
        log_exit(LOG_ERROR, "Error creating socket: " + std::string(strerror(errno)));
    }

    log(LOG_INFO, "Successfully created socket");

    while(true) {


    }
}

