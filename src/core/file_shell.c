#include "file_shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

// Helper para formatear permisos al estilo linux (drwxrwxrwx)
static void get_permissions_string(mode_t mode, char* out_str, int is_dir) {
    out_str[0] = is_dir ? 'd' : '-';
    out_str[1] = (mode & S_IRUSR) ? 'r' : '-';
    out_str[2] = (mode & S_IWUSR) ? 'w' : '-';
    out_str[3] = (mode & S_IXUSR) ? 'x' : '-';
    out_str[4] = (mode & S_IRGRP) ? 'r' : '-';
    out_str[5] = (mode & S_IWGRP) ? 'w' : '-';
    out_str[6] = (mode & S_IXGRP) ? 'x' : '-';
    out_str[7] = (mode & S_IROTH) ? 'r' : '-';
    out_str[8] = (mode & S_IWOTH) ? 'w' : '-';
    out_str[9] = (mode & S_IXOTH) ? 'x' : '-';
    out_str[10] = '\0';
}

FileInfo* get_directory_contents(const char* dir_path, int* count, CError* error) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        error->code = errno;
        strncpy(error->message, "No se pudo abrir el directorio", 256);
        *count = 0;
        return NULL;
    }

    int capacity = 64;
    *count = 0;
    FileInfo* list = malloc(capacity * sizeof(FileInfo));

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar los accesos relativos "." y ".." para evitar bucles visuales
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        FileInfo item;
        strncpy(item.name, entry->d_name, sizeof(item.name));
        snprintf(item.absolute_path, sizeof(item.absolute_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(item.absolute_path, &st) == 0) {
            item.is_directory = S_ISDIR(st.st_mode);
            item.size = item.is_directory ? 0 : (long long)st.st_size;
            get_permissions_string(st.st_mode, item.permissions, item.is_directory);
        } else {
            item.is_directory = (entry->d_type == DT_DIR);
            item.size = 0;
            strcpy(item.permissions, "----------");
        }

        if (*count >= capacity) {
            capacity *= 2;
            list = realloc(list, capacity * sizeof(FileInfo));
        }
        list[*count] = item;
        (*count)++;
    }
    closedir(dir);
    error->code = 0;
    return list;
}
