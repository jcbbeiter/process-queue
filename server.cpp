#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <unistd.h>

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

    if((fd = socket(AF_UNIX, SOCK_STREAM,0)) == -1) {
        log(LOG_ERROR, "Error creating socket: " + std::string(strerror(errno)));
        return -1;
    }

    if(bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        log(LOG_ERROR, "Error binding socket: " + std::string(strerror(errno)));
        return -1;
    }

    //store in config only if successfully bind and need to delete on cleanup
    config.sock_fd = fd;

    if(listen(config.sock_fd,1) == -1) {
        log(LOG_ERROR, "Error listening on socket: " + std::string(strerror(errno)));
        return -1;
    }
    
    log(LOG_INFO, "Successfully created socket");
    return 0;
}

bool socket_has_data() {
    struct pollfd fds[1];
    fds[0].fd  = config.sock_fd;
    fds[0].events = POLLIN | POLLPRI;

    int ret;

    if((ret = poll(fds, 1, config.microseconds/1000)) == -1) {
        log(LOG_ERROR, "Error: " + std::string(strerror(errno)));
        return 0;
    }
    else if(ret == 0) {
        return 0;
    }
    else
        return 1;
}

std::string read_client_message(int& client_fd) {
    char buf [BUFSIZ];

    log(LOG_INFO,"Accepting connection...");
    if((client_fd = accept(config.sock_fd,NULL,NULL)) == -1) {
        log(LOG_ERROR, "Error accepting connection: " + std::string(strerror(errno)));
        return "";
    }

    //only one line needs to be read -- client rejects message > 1024 chars
    if(read(client_fd, buf, BUFSIZ) == -1) {
        log(LOG_ERROR,"Error receiving message");
        return "";
    }

    return std::string(buf);
}

void handle_request(std::string message, int& client_fd) {
    if(message == "status") {
        log(LOG_INFO, "Handling status request...");
        std::string message = "Your write request was handled";
        write(client_fd, message.c_str(), message.length() + 1);
    }
    else if(message == "running") {
        log(LOG_INFO, "Handling running request...");
        std::string message = "Your running request was handled";
        write(client_fd, message.c_str(), message.length() + 1);
    }
    else if(message == "waiting") {
        log(LOG_INFO, "Handling waiting request...");
        std::string message = "Your waiting request was handled";
        write(client_fd, message.c_str(), message.length() + 1);
    }
    else if(message == "flush") {
        log(LOG_INFO, "Handling flush request...");
        std::string message = "Your flush request was handled";
        write(client_fd, message.c_str(), message.length() + 1);
    }
    else { // command is add
        log(LOG_INFO, "Handling add request...");
        std::string message = "Your add request was handled";
        write(client_fd, message.c_str(), message.length() + 1);
    }
}

void server() {
    log(LOG_INFO, "Starting process server...");

    if(create_socket() == -1) {
        cleanup(EXIT_FAILURE);
    }

    // server main loop
    while(true) {
        if(socket_has_data()) {

            int client_fd;
            std::string message = read_client_message(client_fd);
            log(LOG_INFO,"Received: " + message);

            if(!message.empty()) {
                handle_request(message,client_fd);
            }
            
            log(LOG_INFO,"Closing connection...");
            close(client_fd);
        }
    }
}

