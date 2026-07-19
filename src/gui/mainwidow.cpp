#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "process_manager.h"

#include "backend.h"
#include <QDebug>

// --- Constructor y Destructor ---
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnProcess_clicked()
{
}