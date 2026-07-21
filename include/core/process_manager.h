#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "backend.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int pid;
    int ppid;
    char name[64];
    long memory_rss; 
} ProcessInfo;

ProcessInfo* get_process_list(int* count, CError* error);

int send_signal_to_process(int pid, int signal_num, CError* error);

#ifdef __cplusplus
}
#endif

#endif // PROCESS_MANAGER_H
