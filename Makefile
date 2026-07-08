# --- Directorios ---
SRC_CORE = src/core
SRC_GUI = src/gui
INC_CORE = include/core
INC_GUI = include/gui
OBJ_DIR = obj
BIN_DIR = bin

# --- Compiladores y Banderas ---
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -I$(INC_CORE) -g
CXXFLAGS = -Wall -Wextra -I$(INC_GUI) -I$(INC_CORE) -g

# --- Archivos Fuente y Objetos (Backend C) ---
CORE_SRCS = $(wildcard $(SRC_CORE)/*.c)
CORE_OBJS = $(patsubst $(SRC_CORE)/%.c, $(OBJ_DIR)/%.o, $(CORE_SRCS))

# --- Archivos Fuente y Objetos (Frontend C++) ---
GUI_SRCS = $(wildcard $(SRC_GUI)/*.cpp)
GUI_OBJS = $(patsubst $(SRC_GUI)/%.cpp, $(OBJ_DIR)/%.o, $(GUI_SRCS))

# --- Regla Principal (Temporal) ---
all: core_objs gui_objs

# --- Reglas de Construcción de Objetos ---
core_objs: $(CORE_OBJS)
gui_objs: $(GUI_OBJS)

# Patrón para compilar archivos .c a .o (Backend)
$(OBJ_DIR)/%.o: $(SRC_CORE)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Patrón para compilar archivos .cpp a .o (Frontend)
$(OBJ_DIR)/%.o: $(SRC_GUI)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regla de Limpieza ---
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*