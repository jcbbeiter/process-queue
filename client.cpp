#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>

#include "types.h"
#include "log.h"
#include "client.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// client.cpp

extern config_struct config;

void client() {
    sockaddr_un addr;
    memset (&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, config.sock_path.c_str(), config.sock_path.length()+1);

    int fd;

    if(( fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        log_exit(LOG_ERROR, "Error creating socket: " + std::string(strerror(errno)));
    }

    if(connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        log_exit(LOG_ERROR, "Error connecting: " + std::string(strerror(errno)));
    }

    int count;

    if(config.client_option == "add") {
        std::string message = config.client_option + " " + config.command;
        count = write(fd, message.c_str(), message.length()+1);
        log(LOG_INFO, "Wrote " + std::to_string(count) + " chars to server: " + message);
    }
    else {
        count = write(fd, config.client_option.c_str(), config.client_option.length()+1);
        log(LOG_INFO, "Wrote " + std::to_string(count) + " chars to server: " + config.client_option);
    }
    //TODO: remove this, and count

    //get response and print to stdout
    char buf[BUFSIZ];

    //print server response
    while(read(fd,buf,BUFSIZ) > 0) {
        printf("%s\n",buf);
    }

    close(fd);

    //client has no cleanup to do
    exit(EXIT_SUCCESS);
    
}
