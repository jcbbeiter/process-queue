#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sstream>
#include <cstring>

#include "types.h"
#include "log.h"
#include "scheduler.h"
#include "process.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// scheduler.cpp

extern config_struct config;
extern scheduler_struct scheduler;

void add_process(std::string command) {
    time_t time_val;
    time(&time_val);

    process new_process;
    new_process.pid = 0;
    new_process.command = command;
    new_process.arrival_time = time_val;

    switch(scheduler.policy) {
        case SCHEDULE_FIFO:
            scheduler.waiting_queue.push_back(new_process);
            break;
        case SCHEDULE_RDRN:
            log(LOG_INFO, "adding for this policy not implemented yet");
            break;
        case SCHEDULE_MLFQ:
            log(LOG_INFO, "adding for this policy not implemented yet");
            break;
    }

    log(LOG_INFO, "Added process: " + command); 
}

void reap_child(int signal) {
    int status;
    pid_t pid = waitpid(-1, &status, 0);

    time_t finish_time;
    time(&finish_time);

    time_t arrival_time = scheduler.running_map[pid].arrival_time;
    time_t start_time = scheduler.running_map[pid].start_time;

    process_record record;

    record.command = scheduler.running_map[pid].command;
    record.turnaround_time = finish_time - arrival_time;
    record.response_time = finish_time - start_time;

    char buf[BUFSIZ];
    sprintf(buf,"Reaped process %d: %s, Turnaround = %ld, Response = %ld",pid,record.command.c_str(),record.turnaround_time,record.response_time);

    log(LOG_INFO,std::string(buf));

    scheduler.records.push_back(record);

    switch(scheduler.policy) {
        case SCHEDULE_FIFO:
            scheduler.running_map.erase(pid);
            break;
        case SCHEDULE_RDRN:
            log(LOG_INFO,"Implement reaping!");
            break;
        case SCHEDULE_MLFQ:
            log(LOG_INFO,"Implement reaping!");
            break;
    }
}

void schedule() {

    switch(scheduler.policy) {
        case SCHEDULE_FIFO:
            // start processes running if there are process waiting and any free cores
            while(!scheduler.waiting_queue.empty() && 
                (int) scheduler.running_map.size() < config.ncpus) {

                // get process to start
                process proc = scheduler.waiting_queue.front();
                scheduler.waiting_queue.pop_front();

                // start it
                if(start_process(proc) == -1) {
                    log(LOG_ERROR,"Couldn't create process \"" + proc.command + "\", removing...");
                }
                else {
                    //add entry to running map
                    scheduler.running_map[proc.pid] = proc;
                }
            }
            break;
        case SCHEDULE_RDRN:
            break;
        case SCHEDULE_MLFQ:
            break;
    }
}
