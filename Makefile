# --- Directorios ---
SRC_CORE = src/core
SRC_GUI = src/gui
INC_CORE = include/core
INC_GUI = include/gui
OBJ_DIR = obj
BIN_DIR = bin

# --- Configuración de Qt5 ---
# pkg-config extrae automáticamente las rutas de los headers y librerías instaladas
QT_CFLAGS = $(shell pkg-config --cflags Qt5Widgets)
QT_LIBS = $(shell pkg-config --libs Qt5Widgets)

# --- Compiladores y Banderas ---
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -I$(INC_CORE) -g
# Integramos las banderas de Qt exclusivamente en C++
CXXFLAGS = -Wall -Wextra -I$(INC_GUI) -I$(INC_CORE) $(QT_CFLAGS) -fPIC -g

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

# Patrón para compilar archivos .c a .o (Backend C)
$(OBJ_DIR)/%.o: $(SRC_CORE)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Patrón para compilar archivos .cpp a .o (Frontend C++)
$(OBJ_DIR)/%.o: $(SRC_GUI)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regla de Limpieza ---
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*