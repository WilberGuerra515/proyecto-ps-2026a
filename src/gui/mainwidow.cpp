#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "process_manager.h"
#include <QMessageBox>
#include <QThread>
#include <signal.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    CError error;
    if (init_core_system(&error) != 0) {
        QMessageBox::critical(this, "Error de Core", QString::fromUtf8(error.message));
        exit(1);
    }

    setupComponentsView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupComponentsView()
{
    // --- Configuración Módulo 1 (Tareas) ---
    ui->processTreeWidget->setColumnCount(4);
    ui->processTreeWidget->setHeaderLabels({"PID", "PPID", "Nombre del Proceso", "Memoria RSS"});
    ui->processTreeWidget->setColumnWidth(0, 80);
    ui->processTreeWidget->setColumnWidth(1, 80);
    ui->processTreeWidget->setColumnWidth(2, 350);

    connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshProcesses);
    connect(ui->btnKill, &QPushButton::clicked, this, &MainWindow::handleProcessSignal);
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::handleProcessSignal);
    connect(ui->btnCont, &QPushButton::clicked, this, &MainWindow::handleProcessSignal);

}
