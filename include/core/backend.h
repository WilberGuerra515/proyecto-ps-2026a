#ifndef BACKEND_H
#define BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int code;
    char message[256];
} CError;

const char* get_backend_message(void);
int process_data(int a, int b);

#ifdef __cplusplus
}
#endif

#endif // BACKEND_H