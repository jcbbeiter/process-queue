#ifndef HANDLER_H
#define HANDLER_H

#include "types.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// handler.h

void handle_request(std::string message, int& client_fd);
void get_overview(int client_fd);
void print_queue_header(int client_fd,std::string title);
void print_queue(int client_fd, std::deque<process>& queue);
void print_line(process& p);

#endif
