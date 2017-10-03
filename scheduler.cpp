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
#include "server.h"

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
    new_process.state = "Sleeping";
    new_process.user_time = 0;
    new_process.threshold = 0;
    new_process.usage = 0;
    new_process.level = 0;
    new_process.arrival_time = time_val;
    new_process.start_time = 0;
    new_process.num = ++scheduler.count;
    new_process.failed_usage = false;

    if(scheduler.policy == SCHEDULE_MLFQ) {
        new_process.threshold = scheduler.thresholds[0];
    }

    switch(scheduler.policy) {
        case SCHEDULE_FIFO:
        case SCHEDULE_RDRN:
            scheduler.waiting_queue.push_back(new_process);
            break;
        case SCHEDULE_MLFQ:
            scheduler.waiting_queues[0].push_back(new_process);
            break;
    }
    
    log(LOG_INFO, "Added process " + std::to_string(scheduler.count) + ": " + command); 
}

void reap_child(int signal) {
    block_child();

    int saved_errno = errno;
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);

    if(pid == -1) {
        log(LOG_ERROR,"Error waiting for child: " + std::string(strerror(errno)));
    }
    else if(pid == 0) {
        log(LOG_INFO,"Exiting reap early; PID returned was 0");
        return;
    }

    errno = saved_errno;

    time_t finish_time;
    time(&finish_time);

    //find process in running queue
    process doomed;
    bool found = false;

    for(auto it = scheduler.running_queue.begin(); it < scheduler.running_queue.end(); it++) {
        if((*it).pid == pid) {
            doomed = *it;
            scheduler.running_queue.erase(it);
            found = true;
            break;
        }
    }

    //need to search in waiting queue(s) for the record if it already got scheduled out
    if(!found) {
        switch(scheduler.policy) {
            case SCHEDULE_FIFO: 
            case SCHEDULE_RDRN:
                for(auto it = scheduler.waiting_queue.begin(); it < scheduler.waiting_queue.end(); it++) {
                    if((*it).pid == pid) {
                        doomed = *it;
                        scheduler.waiting_queue.erase(it);
                        found = true;
                        break;
                    }
                }
                break;
            case SCHEDULE_MLFQ:
                for(int i = 0; i < scheduler.levels && !found; i++) {
                    for(auto it = scheduler.waiting_queues[i].begin(); it < scheduler.waiting_queues[i].end(); it++) {
                        if((*it).pid == pid) {
                            doomed = *it;
                            scheduler.waiting_queues[i].erase(it);
                            found = true;
                            break;
                        }
                    }
                }
                break;
        }
    }

    if(found && !WIFSIGNALED(status)) {
        time_t arrival_time = doomed.arrival_time;
        time_t start_time = doomed.start_time;

        process_record record;

        record.command = doomed.command;
        record.turnaround_time = finish_time - arrival_time;
        record.response_time = start_time - arrival_time;

        char buf[BUFSIZ];
        sprintf(buf,"Reaped process %d: %s, Turnaround = %ld, Response = %ld",pid,record.command.c_str(),record.turnaround_time,record.response_time);

        log(LOG_INFO,std::string(buf));

        scheduler.records.push_back(record);
    }
    else {
        log(LOG_INFO,"Couldn't find process record for pid " + std::to_string(pid));
    }
    unblock_child();
}

void schedule() {

    block_child();

    int to_start;
    int started;
    switch(scheduler.policy) {
        case SCHEDULE_FIFO:
            // start processes running if there are process waiting and any free cores
            while(!scheduler.waiting_queue.empty() && 
                scheduler.running_queue.size() < config.ncpus) {

                // get process to start
                process proc = scheduler.waiting_queue.front();
                scheduler.waiting_queue.pop_front();

                // start it
                if(start_process(proc) == -1) {
                    log(LOG_ERROR,"Couldn't create process : " + proc.command + ", removing...");
                }
                else {
                    //add entry to running queue
                    scheduler.running_queue.push_back(proc);
                }
            }
            break;
        case SCHEDULE_RDRN:
            // the number of processes to start from the waiting queue is either the
            // whole queue or how many cores we have, whichever is smaller
            if(scheduler.waiting_queue.size() < config.ncpus) {
                to_start = scheduler.waiting_queue.size();
            }
            else {
                to_start = config.ncpus;
            }

            for(int i = 0; i < to_start; i++) {

                // pause process to free core if necessary
                if(scheduler.running_queue.size() == config.ncpus) {
                    process to_pause = scheduler.running_queue.front();
                    scheduler.running_queue.pop_front();
                    stop_process(to_pause);
                    scheduler.waiting_queue.push_back(to_pause);
                }

                // get process to start
                process proc = scheduler.waiting_queue.front();
                scheduler.waiting_queue.pop_front();

                // start it
                if(start_process(proc) == -1) {
                    log(LOG_ERROR,"Couldn't create process : " + proc.command + ", removing...");
                }
                else {
                    //add entry to running queue
                    scheduler.running_queue.push_back(proc);
                }
            }

            break;
        case SCHEDULE_MLFQ:
            // clear the running queue, updating thresholds and 
            // lowering priority if necessary
            while(!scheduler.running_queue.empty()) {
                process to_pause = scheduler.running_queue.front();
                scheduler.running_queue.pop_front();
                if(update_usage(to_pause) == -1) {
                    log(LOG_ERROR,"Consecutive failures to read usage of process " + 
                                    std::to_string(to_pause.pid) + ": " + 
                                    to_pause.command + ", removing...");
                    kill(to_pause.pid, SIGKILL);
                    continue;
                }
                //lower priority if gone over usage threshold
                if(to_pause.user_time > to_pause.threshold && 
                                        to_pause.level < (scheduler.levels-1)) {
                    to_pause.level++;
                    to_pause.threshold = to_pause.user_time + scheduler.thresholds[to_pause.level];
                }
                stop_process(to_pause);
                scheduler.waiting_queues[to_pause.level].push_back(to_pause);
            }

            //priority boost if it's been long enough since the last one
            //interval is 20 seconds, just use system time to track
            time_t cur_time;
            time(&cur_time);
            if(cur_time - scheduler.last_boost >= 20) {
                scheduler.last_boost = cur_time;
                log(LOG_INFO,"Performing priority boost...");

                //reset threshold for all processes in first queue
                for(process& proc : scheduler.waiting_queues[0]) {
                    proc.threshold = proc.user_time + scheduler.thresholds[0];
                }

                //move processes from all other queues to first level
                for(int i = 1; i < scheduler.levels; i++) {
                    while(!scheduler.waiting_queues[i].empty()) {
                        process proc = scheduler.waiting_queues[i].front();
                        scheduler.waiting_queues[i].pop_front();
                        proc.threshold = proc.user_time + scheduler.thresholds[0];
                        proc.level = 0;
                        scheduler.waiting_queues[0].push_back(proc);
                    }
                }
            }

            to_start = config.ncpus;
            started = 0;

            //start the first ncpus processes, lower queues first
            for(int i = 0; i < scheduler.levels && started < to_start; i++) {
                while(started < to_start && !scheduler.waiting_queues[i].empty()) {
                    process proc = scheduler.waiting_queues[i].front();
                    scheduler.waiting_queues[i].pop_front();
                    // start it
                    if(start_process(proc) == -1) {
                        log(LOG_ERROR,"Couldn't create process : " + proc.command + ", removing...");
                    }
                    else {
                        //add entry to running queue
                        scheduler.running_queue.push_back(proc);
                        started++;
                    }
                }
            }
            //if there are no processes running, reset priority boost counter
            if(started == 0) {
                time(&scheduler.last_boost);
            }
            
            break;
    }

    unblock_child();
}
