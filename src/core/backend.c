#include "backend.h"

const char* get_backend_message(void) 
{
    return "Sistema backend en C inicializado correctamente.";
}

int process_data(int a, int b, int operacion) {
    switch (operacion) {
        case OP_SUMAR:
            return a + b;
        case OP_RESTAR:
            return a - b;
        case OP_MULTIPLICAR:
            return a * b;
        default:
            return 0; 
    }
}
