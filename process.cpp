#include <sstream>
#include <unistd.h>
#include <cstring>

// Jacob Beiter
// Operating Systems: Project 02 - pq
// process.cpp

#include "types.h"
#include "log.h"
#include "process.h"

void update_usage(process& proc) {
}

void stop_process(process& proc) {
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
            return 0;
        }
    }
    else { // just need to send it SIGCONT
    }

    return 0;
}
