#include <sstream>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <iostream>

// Jacob Beiter
// Operating Systems: Project 02 - pq
// process.cpp

#include "types.h"
#include "log.h"
#include "process.h"

double get_uptime() {
    int fd = 0;
    if((fd = open("/proc/uptime",O_RDONLY)) == -1) {
        log(LOG_ERROR,"Couldn't open /proc/uptime");
    }

    FILE* uptime_file = fdopen(fd,"r");
    char buf[BUFSIZ];

     if(fgets(buf,BUFSIZ,uptime_file) <= 0) {
        log(LOG_ERROR, "Error reading uptime from /proc/uptime");
    }

    double uptime;
    std::stringstream ss(buf);
    ss >> uptime;

    close(fd);
    fclose(uptime_file);

    return uptime;
}

void update_usage(process& proc) {
    if(proc.pid == 0)
        return;

    int fd;

    std::string file_name = "/proc/" + std::to_string(proc.pid) + "/stat";
    if((fd = open(file_name.c_str(),O_RDONLY)) == -1) {
        log(LOG_ERROR, "Couldn't open stat file for pid " + std::to_string(proc.pid));
    }

    FILE* stat_file = fdopen(fd,"r");
    char buf[BUFSIZ];

    if(fgets(buf,BUFSIZ,stat_file) <= 0) {
        log(LOG_ERROR, "Error reading stat file for pid " + std::to_string(proc.pid));
    }

    std::stringstream ss(buf);
    std::string temp;

    for(int i = 1; i <= 13; i++) {
        ss >> temp;
    }

    unsigned long utime;
    unsigned long stime;
    unsigned long cutime;
    unsigned long cstime;

    ss >> utime >> stime >> cutime >> cstime;

    for(int i = 18; i <= 21; i++) {
        ss >> temp;
    }

    unsigned long start_time;
    ss >> start_time;

    double uptime = get_uptime();
    long clock_rate = sysconf(_SC_CLK_TCK);

    unsigned long total_time = utime + stime;
    double seconds_since_start = uptime - (start_time / clock_rate);

    proc.user_time = utime;
    proc.usage = 100 * ((total_time / clock_rate) / seconds_since_start);

    close(fd);
    fclose(stat_file);
}

void stop_process(process& proc) {
    kill(proc.pid, SIGSTOP);
    proc.state = "Sleeping";
}

int start_process(process& proc) {

    // if it doesn't have a pid yet, need to fork and create it
    if(proc.pid == 0) {
        pid_t pid = fork();

        if(pid < 0) {
            log(LOG_ERROR, "Couldn't fork");
            return -1;
        }
        else if(pid == 0) { // child

            //parse args from command into argv for exec
            std::vector<char*> args;
            std::stringstream ss(proc.command);

            std::string temp;
            while(ss >> temp) {
                char* arg = (char*)malloc(temp.length() + 1);
                strcpy(arg, temp.c_str());
                arg[temp.length()] = '\0';
                args.push_back(arg);
            }

            int argc = args.size();
            char** argv = (char**)malloc((argc+1)*sizeof(char*));

            for(int i = 0; i < argc; i++) {
                argv[i] = args[i];
            }

            //terminate argv
            argv[argc] = NULL;

            execvp(argv[0],argv);

            log(LOG_ERROR,"Failed to exec in child: " + std::string(strerror(errno)));
            for(int i = 0; i < argc; i++) {
                free(argv[i]);
            }
            free(argv);

            exit(EXIT_FAILURE);
        }
        else { // parent
            proc.pid = pid;
            proc.state = "Running";
            time_t time_val;
            time(&time_val);
            proc.start_time = time_val;
            log(LOG_INFO,"Started process " + std::to_string(proc.pid) + ": " + proc.command);
            return 0;
        }
    }
    else { // just need to send it SIGCONT
    }

    return 0;
}
