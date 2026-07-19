cmake_minimum_required(VERSION 3.19)

# Nombre del proyecto y lenguajes (C para backend, C++ para frontend)
project(AppControl VERSION 1.0 LANGUAGES C CXX)

# Estándares de programación
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Habilitamos la magia automática de Qt (MOC, UIC, RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Buscamos los módulos base de Qt6
find_package(Qt6 6 REQUIRED COMPONENTS Core Widgets)

# Le decimos al compilador dónde buscar los archivos .h
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
)

# Automatizamos la búsqueda de todos nuestros archivos fuente y de interfaz
file(GLOB_RECURSE CORE_SOURCES "src/core/*.c")
file(GLOB_RECURSE GUI_SOURCES "src/gui/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "include/core/*.h")
file(GLOB_RECURSE GUI_HEADERS "src/gui/*.h")
file(GLOB_RECURSE GUI_FORMS "src/gui/*.ui")

# Unimos todo en nuestro ejecutable
add_executable(${PROJECT_NAME}
    src/main.cpp
    ${CORE_SOURCES} 
    ${GUI_SOURCES}
    ${CORE_HEADERS}
    ${GUI_HEADERS}
    ${GUI_FORMS}
)

# Enlazamos las librerías gráficas de Qt6
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core)