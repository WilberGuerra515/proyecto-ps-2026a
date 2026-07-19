#include "backend.h"

const char* get_backend_message(void) 
{
    return "Sistema backend en C inicializado correctamente.";
}

int process_data(int a, int b) 
{
    return a + b;
}