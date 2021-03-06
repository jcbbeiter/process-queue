#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <unistd.h>
#include <csignal>

#include "types.h"
#include "log.h"
#include "server.h"
#include "scheduler.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// server.cpp

extern config_struct config;
extern scheduler_struct scheduler;

void block_child() {
    return;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

void unblock_child() {
    return;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask,SIGCHLD);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

void SIGPIPE_handler(int signal) {
    log(LOG_ERROR,"Tried to write, client closed connection");
}

int create_socket() {

    log(LOG_INFO, "Attempting to create unix socket \"" + config.sock_path + "\" ...");

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

// use poll() to check if there's a pending connection
bool socket_has_data() {
    struct pollfd fds[1];
    fds[0].fd  = config.sock_fd;
    fds[0].events = POLLIN | POLLPRI;

    int ret;

    while((ret = poll(fds, 1, config.microseconds/1000)) == -1) {
        if(errno == EINTR)
            continue;
        log(LOG_ERROR, "Error: " + std::string(strerror(errno)));
        return 0;
    }
    if(ret == 0) {
        return 0;
    }
    else
        return 1;
}

std::string read_client_message(int& client_fd, FILE* &client_stream) {
    char buf [BUFSIZ];
    memset(buf,0,BUFSIZ);

    //log(LOG_INFO,"Accepting connection...");
    if((client_fd = accept(config.sock_fd,NULL,NULL)) == -1) {
        log(LOG_ERROR, "Error accepting connection: " + std::string(strerror(errno)));
        return "";
    }

    client_stream = fdopen(client_fd,"w");

    //only one line needs to be read -- client rejects message > 1024 chars
    if(read(client_fd, buf, BUFSIZ) == -1) {
        log(LOG_ERROR,"Error receiving message");
        return "";
    }

    return std::string(buf);
}

void server() {
    log(LOG_INFO, "Starting process server...");

    if(create_socket() == -1) {
        cleanup(EXIT_FAILURE);
    }

    /* ------------------------------------------------------------------------------
     * On the design for the scheduler -- having a struct that functions as all
     * of the different types, and only using certain parts of it depending on scheduler
     * type -- This project is a project that would normally be done in C, but done
     * in C++ for certain conveniences (esp. STL data structures). The
     * design decision to have it this way is an effort to stick closer to the C
     * standard practices, which most of this program tends towards. A better
     * solution would likely to be have a Scheduler abstract parent class, and the 
     * different types of scheduler inherit from it so that they can be used 
     * polymorphically in the server loop.
     * ------------------------------------------------------------------------------ */

    //initialize scheduler, differently for different types
    scheduler.policy = config.policy;
    scheduler.records = std::vector<process_record>();

    switch(scheduler.policy) {
        case SCHEDULE_FIFO:
        case SCHEDULE_RDRN:
            scheduler.running_queue = std::deque<process>(0);
            scheduler.waiting_queue = std::deque<process>(0);
            break;
        case SCHEDULE_MLFQ:
            scheduler.levels = 8;
            scheduler.running_queue = std::deque<process>(0);
            scheduler.waiting_queues = std::vector<std::deque<process>> (scheduler.levels,std::deque<process>());
            time(&scheduler.last_boost);
            break;
    }

    signal(SIGPIPE,SIGPIPE_handler);
    //signal(SIGCHLD, reap_child);

    struct sigaction sa;
    sa.sa_handler = &reap_child;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        log_exit(LOG_ERROR,"Error setting SIGCHLD handler");
    }

    // server main loop
    while(true) {
        //read the data from the client, call handler, and close connection
        if(socket_has_data()) {

            block_child();
            int client_fd;
            FILE* client_stream;
            std::string message = read_client_message(client_fd, client_stream);

            if(!message.empty()) {
                handle_request(message,client_stream);
            }
            
            //flush and close client connection
            fflush(client_stream);
            close(client_fd);
            fclose(client_stream);
            unblock_child();
        }

        // call scheduler
        schedule();
    }
}

