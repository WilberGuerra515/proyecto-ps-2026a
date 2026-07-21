#include "backend.h"
#include <stdlib.h>
#include <string.h>

void free_c_pointer(void* ptr) {
    if (ptr != NULL) {
        free(ptr);
    }
}

int init_core_system(CError* error) {
    if (error == NULL) return -1;
    
    // Inicialización limpia simulada
    error->code = 0;
    strcpy(error->message, "Core C inicializado correctamente.");
    return 0;
}
