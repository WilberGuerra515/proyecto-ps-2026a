#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include "backend.h"

#ifdef __cplusplus
extern "C" {
#endif

// Descarga un archivo utilizando la herramienta nativa 'wget'
int download_file_native(const char* url, const char* dest_dir, CError* error);

#ifdef __cplusplus
}
#endif

#endif // DOWNLOAD_MANAGER_H