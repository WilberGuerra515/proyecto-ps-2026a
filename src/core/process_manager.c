#include "process_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

static int is_numeric_string(const char* str) {
    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

ProcessInfo* get_process_list(int* count, CError* error) {
    DIR* dir = opendir("/proc");
    if (!dir) {
        error->code = errno;
        strncpy(error->message, "Error al abrir /proc", 256);
        *count = 0;
        return NULL;
    }

    int capacity = 32;
    *count = 0;
    ProcessInfo* list = malloc(capacity * sizeof(ProcessInfo));

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && is_numeric_string(entry->d_name)) {
            int pid = atoi(entry->d_name);
            char path[256];
            
            snprintf(path, sizeof(path), "/proc/%d/stat", pid);
            FILE* stat_file = fopen(path, "r");
            if (stat_file) {
                ProcessInfo p;
                p.pid = pid;
                char dummy_state;
                char raw_name[256];

                if (fscanf(stat_file, "%d %s %c %d", &p.pid, raw_name, &dummy_state, &p.ppid) == 4) {
                    size_t name_len = strlen(raw_name);
                    if (name_len > 2) {
                        strncpy(p.name, raw_name + 1, name_len - 2);
                        p.name[name_len - 2] = '\0';
                    } else {
                        strncpy(p.name, raw_name, sizeof(p.name));
                    }

                    snprintf(path, sizeof(path), "/proc/%d/statm", pid);
                    FILE* statm_file = fopen(path, "r");
                    if (statm_file) {
                        long size, resident;
                        if (fscanf(statm_file, "%ld %ld", &size, &resident) == 2) {
                            p.memory_rss = resident * (sysconf(_SC_PAGESIZE) / 1024);
                        } else {
                            p.memory_rss = 0;
                        }
                        fclose(statm_file);
                    } else {
                        p.memory_rss = 0;
                    }

                    if (*count >= capacity) {
                        capacity *= 2;
                        list = realloc(list, capacity * sizeof(ProcessInfo));
                    }
                    list[*count] = p;
                    (*count)++;
                }
                fclose(stat_file);
            }
        }
    }
    closedir(dir);
    
    error->code = 0;
    strcpy(error->message, "Procesos cargados con éxito.");
    return list;
}

int send_signal_to_process(int pid, int signal_num, CError* error) {
    if (kill(pid, signal_num) == 0) {
        error->code = 0;
        strcpy(error->message, "Señal enviada exitosamente.");
        return 0;
    } else {
        error->code = errno;
        strncpy(error->message, strerror(errno), 256);
        return -1;
    }
}
