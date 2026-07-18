#include "MainWindow.h"
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    
    setWindowTitle("Administrador de Sistemas C/C++");
    resize(1024, 768);

    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    // Crear widgets vacíos (lienzos en blanco) para cada módulo
    QWidget *tabTasks = new QWidget();
    QWidget *tabFiles = new QWidget();
    QWidget *tabCommands = new QWidget();
    QWidget *tabBackups = new QWidget();
    QWidget *tabBash = new QWidget();
    QWidget *tabDownloads = new QWidget();

    // Insertar las pestañas en el QTabWidget con sus respectivos títulos
    tabWidget->addTab(tabTasks, "Tareas");
    tabWidget->addTab(tabFiles, "Archivos");
    tabWidget->addTab(tabCommands, "Comandos");
    tabWidget->addTab(tabBackups, "Respaldos");
    tabWidget->addTab(tabBash, "Bash");
    tabWidget->addTab(tabDownloads, "Descargas");
}

MainWindow::~MainWindow() {
}