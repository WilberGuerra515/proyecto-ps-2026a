#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    
    setWindowTitle("Administrador de Sistemas C/C++");
    resize(1024, 768);

    // Instanciar el QTabWidget
    tabWidget = new QTabWidget(this);
    
    // Establecer el QTabWidget como el núcleo de la ventana
    setCentralWidget(tabWidget);
}

MainWindow::~MainWindow() {
}