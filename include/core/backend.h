#ifndef BACKEND_H
#define BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

// Estructura genérica para manejo de errores estructurados
typedef struct {
    int code;
    char message[256];
} CError;

// CONTRATO DE MEMORIA: Libera cualquier puntero alojado con malloc/calloc en C
void free_c_pointer(void* ptr);

// Prototipos base de prueba o inicialización del sistema
int init_core_system(CError* error);

#ifdef __cplusplus
}
#endif

#endif // BACKEND_H

