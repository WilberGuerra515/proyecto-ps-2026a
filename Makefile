# --- Directorios ---
SRC_CORE = src/core
SRC_GUI = src/gui
INC_CORE = include/core
INC_GUI = include/gui
OBJ_DIR = obj
BIN_DIR = bin

# --- Nombre del Ejecutable ---
TARGET = $(BIN_DIR)/admin_tool

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
GUI_HEADERS = $(wildcard $(INC_GUI)/*.h)
MOC_SRCS = $(patsubst $(INC_GUI)/%.h, $(SRC_GUI)/moc_%.cpp, $(GUI_HEADERS))
GUI_SRCS = $(filter-out $(SRC_GUI)/moc_%.cpp, $(wildcard $(SRC_GUI)/*.cpp))
GUI_OBJS = $(patsubst $(SRC_GUI)/%.cpp, $(OBJ_DIR)/%.o, $(GUI_SRCS)) \
           $(patsubst $(SRC_GUI)/moc_%.cpp, $(OBJ_DIR)/moc_%.o, $(MOC_SRCS))

# --- Regla Principal (Construir Ejecutable) ---
all: $(TARGET)

# --- Enlazador (Linker) ---
# Une los objetos de C y C++ junto con las librerías de Qt
$(TARGET): $(CORE_OBJS) $(GUI_OBJS)
	$(CXX) -o $@ $^ $(QT_LIBS)

# --- Reglas de Construcción de Objetos ---
$(SRC_GUI)/moc_%.cpp: $(INC_GUI)/%.h
	$(MOC) $(CXXFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(SRC_CORE)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_GUI)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regla de Limpieza ---
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET) $(SRC_GUI)/moc_*.cpp