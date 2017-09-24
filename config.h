#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

// Jacob Beiter
// Operating Systems - Project 02: pq
// config.h

void usage(int exit_code);
void log(int type, std::string message);
config_struct read_args(int argc, char* argv[]);

#endif
