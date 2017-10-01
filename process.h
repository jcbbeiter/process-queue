#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"
#include "log.h"

int start_process(process& proc);
void stop_process(process& proc);
void update_usage(process& proc);
double get_uptime();

#endif
