#ifndef BACKEND_H
#define BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int code;
    char message[256];
} CError;

#ifdef __cplusplus
}
#endif

#endif // BACKEND_H