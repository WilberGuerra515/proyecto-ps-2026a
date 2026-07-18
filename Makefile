# --- Compiladores y Banderas ---
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude/core

# --- Compiladores y Banderas C++ ---
CXX = g++
CXXFLAGS = -Wall -Wextra -g -Iinclude/gui -Iinclude/core $(QT_CXXFLAGS)

# --- Configuración de Qt5 ---
QT_CXXFLAGS = $(shell pkg-config --cflags Qt5Widgets)
QT_LIBS = $(shell pkg-config --libs Qt5Widgets)

# --- Herramientas de Qt ---
MOC = qmake -query QT_INSTALL_BINS/moc

# Si qmake no está disponible directamente, usamos la ruta por defecto:
MOC = moc

# --- Rutas de Directorios ---
SRC_CORE_DIR = src/core
OBJ_DIR = obj
BIN_DIR = bin
SRC_GUI_DIR = src/gui

# --- Regla Principal (all) ---
all: $(TARGET)

# Evita conflictos si existen archivos llamados "all" o "clean"
.PHONY: all clean

# --- Archivos Fuente y Objetos (Core C) ---
C_SOURCES = $(wildcard $(SRC_CORE_DIR)/*.c)
C_OBJECTS = $(patsubst $(SRC_CORE_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES))

# --- Archivos Fuente y Objetos (GUI C++) ---
CXX_SOURCES = $(wildcard $(SRC_GUI_DIR)/*.cpp)
CXX_OBJECTS = $(patsubst $(SRC_GUI_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CXX_SOURCES))

# --- Regla de compilación para C ---
$(OBJ_DIR)/%.o: $(SRC_CORE_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Regla de compilación para C++ ---
$(OBJ_DIR)/%.o: $(SRC_GUI_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Archivos MOC Autogenerados ---
# Buscamos todas las cabeceras en include/gui/
GUI_HEADERS = $(wildcard include/gui/*.h)
# Filtramos o mapeamos cuáles generarán un archivo moc_*.cpp en obj/
MOC_SOURCES = $(patsubst include/gui/%.h, $(OBJ_DIR)/moc_%.cpp, $(GUI_HEADERS))
MOC_OBJECTS = $(patsubst %.cpp, %.o, $(MOC_SOURCES))

# --- Regla para generar archivos moc_*.cpp desde las cabeceras ---
$(OBJ_DIR)/moc_%.cpp: include/gui/%.h
	@mkdir -p $(OBJ_DIR)
	$(MOC) $< -o $@

# --- Regla para compilar los moc_*.cpp generados ---
$(OBJ_DIR)/moc_%.o: $(OBJ_DIR)/moc_%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Nombre del ejecutable ---
TARGET = $(BIN_DIR)/app_control

# --- Regla del Enlazador (Linker) ---
# Usamos $(CXX) (g++) para enlazar, ya que C++ necesita su biblioteca estándar.
# $^ representa todos los requisitos (los .o) y $@ el objetivo (TARGET).
$(TARGET): $(C_OBJECTS) $(CXX_OBJECTS) $(MOC_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@ $(QT_LIBS)

# --- Regla de limpieza (clean) ---
clean:
	@rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/moc_*.cpp $(BIN_DIR)/app_control
