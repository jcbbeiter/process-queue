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
    //create socket address to bind to (hopefully) existing unix socket
    sockaddr_un addr;
    memset (&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, config.sock_path.c_str(), config.sock_path.length()+1);

    int fd;

    // connect to unix domain socket
    if(( fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        log_exit(LOG_ERROR, "Error creating socket: " + std::string(strerror(errno)));
    }

    if(connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        log_exit(LOG_ERROR, "Error connecting: " + std::string(strerror(errno)));
    }


    //write message to server

    if(write(fd, config.client_option.c_str(),config.client_option.length()) <= 0) {
        fprintf(stderr,"Error, couldn't write to server: %s\n",strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    FILE* fstream = fdopen(fd,"r");

    //get response and print to stdout
    char buf[BUFSIZ];
    memset(buf,0,BUFSIZ);

    //print server response
    while(fgets(buf,BUFSIZ,fstream) != NULL) {
        printf("%s",buf);
        memset(buf,0,BUFSIZ);
    }

    close(fd);
    fclose(fstream);

    //client has no cleanup to do
    exit(EXIT_SUCCESS);
    
}
