#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include "backend.h"

typedef struct {
    long total_ram_mb;
    long free_ram_mb;
    long avail_ram_mb;
    double load_1min;
    double load_5min;
    long uptime_seconds;
} SystemMetrics;

#ifdef __cplusplus
extern "C" {
#endif

// Parsea las estructuras del Kernel en /proc y rellena la estructura de métricas
int collect_system_metrics(SystemMetrics* metrics, CError* error);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_MONITOR_H