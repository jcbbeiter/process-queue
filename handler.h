#ifndef HANDLER_H
#define HANDLER_H

#include "types.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// handler.h

void handle_request(std::string message, int& client_fd);
void print_overview(FILE* client_stream);
void print_queue_header(FILE* client_stream,std::string title);
void print_queue(FILE* client_stream, std::deque<process>& queue);
void print_line(process& p);
int flush_queue(std::deque<process>& queue);

#endif
