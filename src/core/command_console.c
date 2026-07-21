#include "command_console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

int execute_command_async(const char* cmd, const char* working_dir, int* out_fd, int* process_pid, CError* error) {
    int pipefd[2];
    
    if (pipe(pipefd) == -1) {
        error->code = errno;
        strncpy(error->message, "Imposible crear los canales Unix (Pipe)", 256);
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        error->code = errno;
        strncpy(error->message, "Fallo crítico al bifurcar proceso (Fork)", 256);
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    if (pid == 0) { 
        if (working_dir && strlen(working_dir) > 0) {
            if (chdir(working_dir) == -1) {
                exit(errno);
            }
        }

        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        
        close(pipefd[0]);
        close(pipefd[1]);

        char* args[] = {"/bin/sh", "-c", (char*)cmd, NULL};
        execv("/bin/sh", args);
        exit(127);
    } 
    else { 
        close(pipefd[1]);

        int flags = fcntl(pipefd[0], F_GETFL, 0);
        fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

        *out_fd = pipefd[0];
        *process_pid = pid;
        
        error->code = 0;
        return 0;
    }
}

int read_command_output(int fd, char* buffer, int max_len, CError* error) {
    ssize_t bytes_read = read(fd, buffer, max_len - 1);
    
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        error->code = 0;
        return (int)bytes_read;
    } 
    else if (bytes_read == 0) {
        error->code = 0;
        return 0; 
    } 
    else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            error->code = errno;
            return -2; 
        }
        error->code = errno;
        strncpy(error->message, strerror(errno), 256);
        return -1; 
    }
}