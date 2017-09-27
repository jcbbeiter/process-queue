#ifndef SCHEDULER_H
#define SCHEDULER_H

// Jacob Beiter
// Operating Systems - Project 02: pq
// scheduler.h

void add_process(std::string command);
void schedule();
void reap_child(int signal);

#endif
