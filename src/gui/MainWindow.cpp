#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    
    // Configuración básica de la ventana
    setWindowTitle("Administrador de Sistemas C/C++");
    resize(1024, 768);
}

MainWindow::~MainWindow() {
}