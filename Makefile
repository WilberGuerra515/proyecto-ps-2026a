# --- Directorios ---
SRC_CORE = src/core
INC_CORE = include/core
OBJ_DIR = obj
BIN_DIR = bin

# --- Compilador C y Banderas ---
CC = gcc
CFLAGS = -Wall -Wextra -I$(INC_CORE) -g

# --- Archivos Fuente y Objetos (Backend C) ---
CORE_SRCS = $(wildcard $(SRC_CORE)/*.c)
CORE_OBJS = $(patsubst $(SRC_CORE)/%.c, $(OBJ_DIR)/%.o, $(CORE_SRCS))

# --- Regla Principal (Temporal) ---
all: core_objs

# --- Reglas de Compilación ---
# Construir solo los objetos de C
core_objs: $(CORE_OBJS)

# Patrón para compilar archivos .c a .o
$(OBJ_DIR)/%.o: $(SRC_CORE)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Regla de Limpieza ---
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*