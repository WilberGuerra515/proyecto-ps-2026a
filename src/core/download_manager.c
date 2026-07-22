#include "download_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int download_file_native(const char* url, const char* dest_dir, CError* error) {
    pid_t pid = fork();
    if (pid == -1) {
        error->code = errno;
        strncpy(error->message, "Error al crear el proceso para la descarga", 256);
        return -1;
    }

    if (pid == 0) { // --- PROCESO HIJO ---
        // Parámetro '-P' le indica a wget en qué directorio guardar el archivo descagado
        char* args[] = {"wget", "-P", (char*)dest_dir, (char*)url, NULL};
        execvp("wget", args);
        
        // Si execvp falla (por ejemplo, wget no está instalado), salimos con el error
        exit(errno);
    } 
    else { // --- PROCESO PADRE ---
        int status;
        waitpid(pid, &status, 0); // El hilo del Worker esperará aquí pacientemente
        
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code == 0) {
                error->code = 0;
                return 0; // Descarga exitosa
            } else {
                error->code = exit_code;
                snprintf(error->message, 256, "wget falló. Código de salida: %d (Verifica la URL)", exit_code);
                return -1;
            }
        } else {
            error->code = -1;
            strncpy(error->message, "El proceso de descarga fue interrumpido abruptamente", 256);
            return -1;
        }
    }
}