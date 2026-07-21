#ifndef COMMAND_CONSOLE_H
#define COMMAND_CONSOLE_H

#include "backend.h"

#ifdef __cplusplus
extern "C" {
#endif

int execute_command_async(const char* cmd, const char* working_dir, int* out_fd, int* process_pid, CError* error);
int read_command_output(int fd, char* buffer, int max_len, CError* error);

#ifdef __cplusplus
}
#endif

#endif // COMMAND_CONSOLE_H