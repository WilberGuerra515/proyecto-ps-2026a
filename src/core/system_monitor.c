#include "system_monitor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int collect_system_metrics(SystemMetrics* metrics, CError* error) {
    if (!metrics) {
        strncpy(error->message, "Puntero de métricas nulo", 256);
        return -1;
    }

    // 1. LEER MEMORIA (/proc/meminfo)
    FILE* mem_fp = fopen("/proc/meminfo", "r");
    if (!mem_fp) {
        error->code = errno;
        strncpy(error->message, "Error al abrir /proc/meminfo", 256);
        return -1;
    }

    char line[256];
    long total_kb = 0, avail_kb = 0, free_kb = 0;
    while (fgets(line, sizeof(line), mem_fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line + 9, "%ld", &total_kb);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line + 8, "%ld", &free_kb);
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line + 13, "%ld", &avail_kb);
        }
    }
    fclose(mem_fp);

    metrics->total_ram_mb = total_kb / 1024;
    metrics->free_ram_mb = free_kb / 1024;
    metrics->avail_ram_mb = avail_kb / 1024;

    // 2. LEER CARGA DE PROCESOS (/proc/loadavg)
    FILE* load_fp = fopen("/proc/loadavg", "r");
    if (load_fp) {
        fscanf(load_fp, "%lf %lf", &metrics->load_1min, &metrics->load_5min);
        fclose(load_fp);
    } else {
        metrics->load_1min = 0.0;
        metrics->load_5min = 0.0;
    }

    // 3. LEER TIEMPO DE ACTIVIDAD (/proc/uptime)
    FILE* uptime_fp = fopen("/proc/uptime", "r");
    if (uptime_fp) {
        double uptime_double;
        fscanf(uptime_fp, "%lf", &uptime_double);
        metrics->uptime_seconds = (long)uptime_double;
        fclose(uptime_fp);
    } else {
        metrics->uptime_seconds = 0;
    }

    error->code = 0;
    return 0;
}