#ifndef BACKUP_MANAGER_H
#define BACKUP_MANAGER_H

#include "backend.h"

#ifdef __cplusplus
extern "C" {
#endif

int generate_tar_backup(const char* src_dir, const char* dest_dir, const char* name, CError* error);

#ifdef __cplusplus
}
#endif

#endif // BACKUP_MANAGER_H