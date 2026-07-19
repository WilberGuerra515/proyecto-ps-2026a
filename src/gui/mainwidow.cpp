#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "backend.h"
#include <QDebug>

// --- Constructor y Destructor ---
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "Conexión exitosa. Mensaje:" << get_backend_message();
    qDebug() << "Prueba de procesamiento (15 + 25):" << process_data(15, 25);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnProcess_clicked()
{
    int resultado = process_data(100, 50);
    ui->lblResult->setText("Resultado del backend (C): " + QString::number(resultado));
}