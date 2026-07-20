#ifndef FILE_SHELL_H
#define FILE_SHELL_H

#include "backend.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char name[256];
    char absolute_path[512];
    int is_directory;
    long long size;
    char permissions[11];
} FileInfo;

// Lista el contenido de un directorio de forma síncrona (lectura directa)
FileInfo* get_directory_contents(const char* dir_path, int* count, CError* error);

// Busca archivos de manera recursiva profunda. Devuelve un array de rutas absolutas (strings).
// Esta función será la que ejecutaremos en segundo plano desde C++.
char** search_files_recursive(const char* base_path, const char* search_term, int* count, CError* error);

#ifdef __cplusplus
}
#endif

#endif // FILE_SHELL_H