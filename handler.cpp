#include <unistd.h>
#include <signal.h>
#include <sstream>
#include <iomanip>
#include <cstring>

#include "types.h"
#include "log.h"
#include "handler.h"
#include "scheduler.h"
#include "process.h"
#include "server.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// handler.cpp

extern config_struct config;
extern scheduler_struct scheduler;

void print_overview(FILE* client_stream) {
    
    int waiting_count = 0;
    int running_count = 0;
    int levels;
    double avg_response = 0;
    double avg_turnaround = 0;

    switch(scheduler.policy) {
        case SCHEDULE_FIFO:
        case SCHEDULE_RDRN:
            levels = 0;
            waiting_count = scheduler.waiting_queue.size();
            running_count = scheduler.running_queue.size();
            break;
        case SCHEDULE_MLFQ:
            running_count = scheduler.running_queue.size();

            levels = 0;
            for(int i = 0; i < scheduler.levels; i++) {
                if(!scheduler.waiting_queues[i].empty()) {
                    levels++;
                    waiting_count += scheduler.waiting_queues[i].size();
                }
            }
            break;
    }

    //calculate average times from completed processes
    for(const process_record &p : scheduler.records) {
        avg_response += p.response_time;
        avg_turnaround += p.turnaround_time;
    }
    avg_response /= scheduler.records.size();
    avg_turnaround /= scheduler.records.size();

    std::stringstream out;

    out << "Running =" << std::setw(5) << running_count << ", ";
    out << "Waiting =" << std::setw(5) << waiting_count << ", ";
    out << "Levels =" << std::setw(5) << levels << ", ";
    out << "Turnaround = " << std::setw(5) << std::fixed << std::setprecision(2) << avg_turnaround << ", ";
    out << "Response = " << std::setw(5) << std::fixed << std::setprecision(2) << avg_response << "\n\n";

    std::string out_str = out.str();
    if(fputs(out_str.c_str(),client_stream) <= 0) {
        log(LOG_ERROR,"Error writing overview to client: " + std::string(strerror(errno)));
    }

}

void print_queue_header(FILE* client_stream, std::string title) {

    std::stringstream out;

    out << title << ":\n";
    out << std::setw(6) << "PID" << ' ';
    out << std::setw(20) << std::left << "COMMAND" << ' ';
    out << std::setw(8) << std::left << "STATE" << ' ';
    out << std::setw(8) << std::left << "USER" << ' ';
    out << std::setw(7) << std::left << "THRESHOLD" << ' ';
    out << std::setw(7) << std::left << "USAGE" << ' ';
    out << std::setw(10) << std::left << "ARRIVAL" << ' ';
    out << std::setw(10) << std::left << "START" << '\n';

    std::string out_str = out.str();
    if(fputs(out_str.c_str(),client_stream) <= 0) {
        log(LOG_ERROR,"Error writing header \"" + title + "\" to client: " + std::string(strerror(errno)));
    }
}

int flush_queue(std::deque<process>& queue) {
    int count = 0;

    while(!queue.empty()) {
        if(queue.front().pid == 0) {
            log(LOG_INFO,"Removed unstarted process: " + queue.front().command);
        }
        else {
            log(LOG_INFO,"Terminated unfinished process " + std::to_string(queue.front().pid) + ": " + queue.front().command);
            kill(queue.front().pid, SIGTERM);
        }
        queue.pop_front();
        count++;
    }

    return count;
}

void print_line(FILE* client_stream, process& p) {
    
    std::stringstream out;
    out << std::setw(6) << p.pid << ' ';
    out << std::setw(20) << std::left << p.command << ' ';
    out << std::setw(8) << std::left << p.state << ' ';
    out << std::setw(8) << std::left << p.user_time << ' ';
    out << std::setw(9) << std::left << p.threshold << ' ';
    out << std::setw(7) << std::left << std::fixed << std::setprecision(2) << p.usage << ' ';
    out << std::setw(10) << std::left << p.arrival_time << ' ';
    out << std::setw(10) << std::left << p.start_time << '\n';
    
    std::string out_str = out.str();
    if(fputs(out_str.c_str(),client_stream) <= 0) {
        log(LOG_ERROR,"Error writing process record to client: " + std::string(strerror(errno)));
    }
}

void print_queue(FILE* client_stream, std::deque<process>& queue) {
    for(process& p : queue) {
        update_usage(p);
        print_line(client_stream, p);
    }
    fputs("\n",client_stream);
}

void handle_request(std::string message, FILE* &client_stream) {
    block_child();
    if(message == "status") {

        print_overview(client_stream);

        switch(scheduler.policy) {
            case SCHEDULE_FIFO:
            case SCHEDULE_RDRN:

                if(!scheduler.running_queue.empty()) {
                    print_queue_header(client_stream,"Running Queue");
                    print_queue(client_stream,scheduler.running_queue);
                }

                if(!scheduler.waiting_queue.empty()) {
                    print_queue_header(client_stream,"Waiting Queue");
                    print_queue(client_stream,scheduler.waiting_queue);
                }
                
                break;
            case SCHEDULE_MLFQ:
                if(!scheduler.running_queue.empty()) {
                    print_queue_header(client_stream,"Running Queue");
                    print_queue(client_stream,scheduler.running_queue);
                }

                for(int i = 0; i < scheduler.levels; i++) {
                    if(!scheduler.waiting_queues[i].empty()) {
                        print_queue_header(client_stream,"Level " + std::to_string(i));
                        print_queue(client_stream,scheduler.waiting_queues[i]);
                    }
                }
                break;
        }
    }
    else if(message == "running") {

        if(!scheduler.running_queue.empty()) {
            print_queue_header(client_stream,"Running Queue");
            print_queue(client_stream,scheduler.running_queue);
        }
        else {
            fputs("No processes running\n",client_stream);
        }
    }
    else if(message == "waiting") {
        bool printed = 0;
        switch(scheduler.policy) {
            case SCHEDULE_FIFO:
            case SCHEDULE_RDRN:
                if(!scheduler.waiting_queue.empty()) {
                    print_queue_header(client_stream,"Waiting Queue");
                    print_queue(client_stream,scheduler.waiting_queue);
                }
                else {
                    fputs("No processes waiting\n",client_stream);
                }
                break;
            case SCHEDULE_MLFQ:
                for(int i = 0; i < scheduler.levels; i++) {
                    if(!scheduler.waiting_queues[i].empty()) {
                        printed =1;
                        print_queue_header(client_stream,"Level " + std::to_string(i));
                        print_queue(client_stream,scheduler.waiting_queue);
                    }
                }
                if(!printed) {
                    fputs("No processes waiting\n",client_stream);
                }
            break;
        }
    }
    else if(message == "flush") {
        log(LOG_INFO,"Flushing process queues and records...");
        scheduler.records.clear();
        int running;
        int waiting;
        switch(scheduler.policy) {
            case SCHEDULE_FIFO:
            case SCHEDULE_RDRN:
                running = flush_queue(scheduler.running_queue);
                waiting = flush_queue(scheduler.waiting_queue);
                break;
            case SCHEDULE_MLFQ:
                running = flush_queue(scheduler.running_queue);
                for(int i = 0; i < scheduler.levels; i++) {
                    waiting += flush_queue(scheduler.waiting_queues[i]);
                }
            break;
        }
        std::string message = "Flushed " + std::to_string(running) + " running processes and " + std::to_string(waiting) + " waiting processes\n";
        fputs(message.c_str(),client_stream);
    }
    else { // command is add
        add_process(message.substr(4,message.length()-4));
    }
    unblock_child();
}
