# --- Directorios ---
SRC_CORE = src/core
SRC_GUI = src/gui
INC_CORE = include/core
INC_GUI = include/gui
OBJ_DIR = obj
BIN_DIR = bin

# --- Configuración de Qt5 ---
QT_CFLAGS = $(shell pkg-config --cflags Qt5Widgets)
QT_LIBS = $(shell pkg-config --libs Qt5Widgets)
MOC = moc

# --- Compiladores y Banderas ---
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -I$(INC_CORE) -g
CXXFLAGS = -Wall -Wextra -I$(INC_GUI) -I$(INC_CORE) $(QT_CFLAGS) -fPIC -g

# --- Archivos Fuente y Objetos (Backend C) ---
CORE_SRCS = $(wildcard $(SRC_CORE)/*.c)
CORE_OBJS = $(patsubst $(SRC_CORE)/%.c, $(OBJ_DIR)/%.o, $(CORE_SRCS))

# --- Archivos Fuente y Objetos (Frontend C++ y MOC) ---
# 1. Detectar cabeceras GUI para generar código MOC
GUI_HEADERS = $(wildcard $(INC_GUI)/*.h)
MOC_SRCS = $(patsubst $(INC_GUI)/%.h, $(SRC_GUI)/moc_%.cpp, $(GUI_HEADERS))

# 2. Archivos C++ originales (filtramos para no duplicar los moc_ generados previamente)
GUI_SRCS = $(filter-out $(SRC_GUI)/moc_%.cpp, $(wildcard $(SRC_GUI)/*.cpp))

# 3. Lista final de objetos GUI (Originales + MOC)
GUI_OBJS = $(patsubst $(SRC_GUI)/%.cpp, $(OBJ_DIR)/%.o, $(GUI_SRCS)) \
           $(patsubst $(SRC_GUI)/moc_%.cpp, $(OBJ_DIR)/moc_%.o, $(MOC_SRCS))

# --- Regla Principal (Temporal) ---
all: core_objs gui_objs

# --- Reglas de Construcción de Objetos ---
core_objs: $(CORE_OBJS)
gui_objs: $(GUI_OBJS)

# Regla para MOC de Qt (Genera moc_*.cpp a partir de .h)
$(SRC_GUI)/moc_%.cpp: $(INC_GUI)/%.h
	$(MOC) $(CXXFLAGS) $< -o $@

# Patrón para compilar archivos .c a .o (Backend C)
$(OBJ_DIR)/%.o: $(SRC_CORE)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Patrón para compilar archivos .cpp a .o (Frontend C++ y MOC)
$(OBJ_DIR)/%.o: $(SRC_GUI)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regla de Limpieza ---
clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/* $(SRC_GUI)/moc_*.cpp