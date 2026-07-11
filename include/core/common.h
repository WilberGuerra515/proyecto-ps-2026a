#ifndef COMMON_H
#define COMMON_H

/* * Bloque 'extern "C"': 
 * Le indica al compilador de C++ que no aplique "Name Mangling" a las funciones 
 * que se declaren dentro de este bloque, permitiendo que el Linker encuentre 
 * las referencias exactas a las funciones compiladas en C puro.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

/* Aquí agregaremos macros, estructuras globales o definiciones de errores más adelante */

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */