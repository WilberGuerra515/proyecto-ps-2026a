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


// LÓGICA MÓDULO 1

void MainWindow::refreshProcesses()
{
    ui->processTreeWidget->clear();
    int count = 0;
    CError error;
    
    ProcessInfo* list = get_process_list(&count, &error);
    if (!list) {
        QMessageBox::warning(this, "Error de Lectura", QString::fromUtf8(error.message));
        return;
    }

    for (int i = 0; i < count; ++i) {
        QStringList rowData;
        rowData << QString::number(list[i].pid)
                << QString::number(list[i].ppid)
                << QString::fromUtf8(list[i].name)
                << QString("%1 KB").arg(list[i].memory_rss);

        new QTreeWidgetItem(ui->processTreeWidget, rowData);
    }
    free_c_pointer(list);
}

void MainWindow::handleProcessSignal()
{
    QTreeWidgetItem *selectedItem = ui->processTreeWidget->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "Control", "Selecciona un proceso de la lista.");
        return;
    }

    int pid = selectedItem->text(0).toInt();
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    
    int target_signal = SIGKILL;
    if (clickedButton == ui->btnStop) target_signal = SIGSTOP;
    else if (clickedButton == ui->btnCont) target_signal = SIGCONT;

    CError error;
    if (send_signal_to_process(pid, target_signal, &error) == 0) {
        QMessageBox::information(this, "Éxito", QString("Señal enviada al PID %1.").arg(pid));
        refreshProcesses();
    } else {
        QMessageBox::critical(this, "Error", QString::fromUtf8(error.message));
    }
}

