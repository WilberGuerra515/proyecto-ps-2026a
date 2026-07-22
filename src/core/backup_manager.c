#include "backup_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int generate_tar_backup(const char* src_dir, const char* dest_dir, const char* name, CError* error) {
    char full_dest_path[512];
    snprintf(full_dest_path, sizeof(full_dest_path), "%s/%s.tar.gz", dest_dir, name);

    pid_t pid = fork();
    if (pid == -1) {
        error->code = errno;
        strncpy(error->message, "Fallo al crear el proceso bifurcado para el respaldo", 256);
        return -1;
    }

    if (pid == 0) { 
        char* args[] = {"tar", "-czf", full_dest_path, "-C", (char*)src_dir, ".", NULL};
        execvp("tar", args);
        
        exit(errno); 
    } 
    else { 
        int status;
        waitpid(pid, &status, 0); 
        
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code == 0) {
                error->code = 0;
                return 0; 
            } else {
                error->code = exit_code;
                snprintf(error->message, 256, "El comando 'tar' falló con código de salida: %d", exit_code);
                return -1;
            }
        } else {
            error->code = -1;
            strncpy(error->message, "El proceso de empaquetado fue interrumpido de forma anormal", 256);
            return -1;
        }
    }
}