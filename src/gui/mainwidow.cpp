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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnProcess_clicked()
{
    int num1 = ui->spinNum1->value();
    int num2 = ui->spinNum2->value();
    
    int operacionSeleccionada = ui->comboOp->currentIndex();
    
    int resultado = process_data(num1, num2, operacionSeleccionada);
    
    ui->lblResult->setText("Resultado del backend (C): " + QString::number(resultado));
    
    qDebug() << "Operación enviada al Backend (C):" << operacionSeleccionada << "| Resultado:" << resultado;
}