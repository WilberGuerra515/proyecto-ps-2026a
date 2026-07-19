#ifndef BACKEND_H
#define BACKEND_H

typedef enum {
    OP_SUMAR = 0,
    OP_RESTAR,
    OP_MULTIPLICAR
} TipoOperacion;

#ifdef __cplusplus
extern "C" {
#endif

int process_data(int a, int b, int operacion);

#ifdef __cplusplus
}
#endif

#endif // BACKEND_H