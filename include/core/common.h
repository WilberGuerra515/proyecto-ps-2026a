#ifndef COMMON_H
#define COMMON_H

/* Dependencias estándar de C compartidas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* 
 * Bloque para integración C/C++ 
 * Si este archivo es incluido desde el compilador de C++ (g++), 
 * envuelve las declaraciones en extern "C".
 */
#ifdef __cplusplus
extern "C" {
#endif

/* --- Contratos de Propiedad de Memoria (Memory Ownership) --- */
/* 
 * REGLA ARQUITECTÓNICA: 
 * Toda estructura dinámica (malloc/calloc) creada en el backend (C) y devuelta 
 * al frontend (C++), DEBE ser liberada invocando explícitamente su respectiva 
 * función de limpieza definida aquí, una vez que Qt haya pintado los datos.
 * C++ NUNCA debe llamar a free() directamente sobre punteros del backend.
 */

/* Prototipos base de limpieza (las estructuras se definirán en cada módulo) */
// void free_process_list(struct ProcessNode* head);
// void free_string_array(char** array, int count);

#ifdef __cplusplus
}
#endif

#endif // COMMON_H