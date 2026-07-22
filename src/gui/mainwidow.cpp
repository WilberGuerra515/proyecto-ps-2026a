#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "process_manager.h"
#include "file_shell.h"
#include "filesearchworker.h"
#include "command_console.h"

#include <QMessageBox>
#include <QThread>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

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

    // Preparar las propiedades visuales y las conexiones
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

    // --- Configuración Módulo 2 (Archivos) ---
    ui->fileTreeWidget->setColumnCount(3);
    ui->fileTreeWidget->setHeaderLabels({"Nombre", "Tamaño", "Permisos"});
    ui->fileTreeWidget->setColumnWidth(0, 220);

    connect(ui->txtCurrentPath, &QLineEdit::returnPressed, this, [this](){ browseToPath(ui->txtCurrentPath->text()); });
    connect(ui->fileTreeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::onFileItemDoubleClicked);
    connect(ui->btnSearchFile, &QPushButton::clicked, this, &MainWindow::startAsyncSearch);

    // --- Configuración Módulo 3 (Consola) ---
    connect(ui->btnSendCommand, &QPushButton::clicked, this, &MainWindow::startCommandExecution);
    connect(ui->txtCommandInput, &QLineEdit::returnPressed, this, &MainWindow::startCommandExecution);
    connect(ui->btnTerminateCommand, &QPushButton::clicked, this, &MainWindow::terminateActiveCommand);
    ui->btnTerminateCommand->setEnabled(false);
    
    // Cargas iniciales
    refreshProcesses();
    browseToPath("/");
}

// =============================================================================
// LÓGICA MÓDULO 1
// =============================================================================
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

// =============================================================================
// LÓGICA MÓDULO 2
// =============================================================================
void MainWindow::browseToPath(const QString &path)
{
    int count = 0;
    CError error;
    FileInfo* list = get_directory_contents(path.toUtf8().constData(), &count, &error);

    if (error.code != 0) {
        QMessageBox::warning(this, "Error", "No se puede acceder a la ruta.");
        return;
    }

    ui->fileTreeWidget->clear();
    currentExploredPath = path;
    ui->txtCurrentPath->setText(path);

    for (int i = 0; i < count; ++i) {
        QStringList rowData;
        rowData << QString::fromUtf8(list[i].name);
        
        if (list[i].is_directory) {
            rowData << "[DIR]" << QString::fromUtf8(list[i].permissions);
        } else {
            rowData << QString("%1 Bytes").arg(list[i].size) << QString::fromUtf8(list[i].permissions);
        }

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->fileTreeWidget, rowData);
        item->setData(0, Qt::UserRole, QString::fromUtf8(list[i].absolute_path));
    }
    free_c_pointer(list);
}

void MainWindow::onFileItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    QString fullPath = item->data(0, Qt::UserRole).toString();
    if (item->text(1) == "[DIR]") {
        browseToPath(fullPath);
    }
}

void MainWindow::startAsyncSearch()
{
    QString term = ui->txtSearchFile->text().trimmed();
    if (term.isEmpty()) return;

    ui->btnSearchFile->setEnabled(false);
    ui->searchProgressBar->setVisible(true);
    ui->lstSearchResults->clear();

    QThread *searchThread = new QThread(this);
    FileSearchWorker *worker = new FileSearchWorker(currentExploredPath, term);
    
    worker->moveToThread(searchThread);

    connect(searchThread, &QThread::started, worker, &FileSearchWorker::processSearch);
    connect(worker, &FileSearchWorker::searchFinished, this, &MainWindow::handleSearchFinished);
    connect(worker, &FileSearchWorker::searchFinished, searchThread, &QThread::quit);
    connect(worker, &FileSearchWorker::searchFinished, worker, &QObject::deleteLater);
    connect(searchThread, &QThread::finished, searchThread, &QObject::deleteLater);

    searchThread->start();
}

void MainWindow::handleSearchFinished(const QStringList &results, int errorCode, const QString &errorMsg)
{
    Q_UNUSED(errorCode);
    Q_UNUSED(errorMsg);

    if (results.isEmpty()) {
        ui->lstSearchResults->addItem("No se encontraron coincidencias.");
    } else {
        for (const QString &match : results) {
            ui->lstSearchResults->addItem(match);
        }
    }

    ui->searchProgressBar->setVisible(false);
    ui->btnSearchFile->setEnabled(true);
}


// =============================================================================
// LÓGICA MÓDULO 3
// =============================================================================

void MainWindow::startCommandExecution()
{
    QString cmd = ui->txtCommandInput->text().trimmed();
    if (cmd.isEmpty()) return;

    if (commandFd != -1) {
        terminateActiveCommand();
    }

    CError error;
    QString targetDirectory = "/home/Dieguito";

    if (execute_command_async(cmd.toUtf8().constData(), 
                              targetDirectory.toUtf8().constData(), 
                              &commandFd, &commandPid, &error) == 0) 
    {
        ui->txtConsoleOutput->appendPlainText(QString("[/home/Dieguito]$ %1").arg(cmd));
        ui->txtCommandInput->clear();
        ui->btnTerminateCommand->setEnabled(true);
        ui->btnSendCommand->setEnabled(false);

        commandNotifier = new QSocketNotifier(commandFd, QSocketNotifier::Read, this);
        connect(commandNotifier, &QSocketNotifier::activated, this, &MainWindow::readCommandLiveOutput);
    } else {
        ui->txtConsoleOutput->appendPlainText(QString("[ERROR]: %1").arg(QString::fromUtf8(error.message)));
    }
}

void MainWindow::readCommandLiveOutput()
{
    char buffer[512];
    CError error;
    int bytesRead = read_command_output(commandFd, buffer, sizeof(buffer), &error);

    if (bytesRead > 0) {
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
        ui->txtConsoleOutput->insertPlainText(QString::fromUtf8(buffer));
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
    } 
    else if (bytesRead == 0 || bytesRead == -1) {
        terminateActiveCommand();
        if (bytesRead == -1) {
            ui->txtConsoleOutput->appendPlainText(QString("\n[Proceso abortado: %1]\n").arg(QString::fromUtf8(error.message)));
        } else {
            ui->txtConsoleOutput->appendPlainText("\n[Proceso terminado exitosamente]\n");
        }
    }
}

void MainWindow::terminateActiveCommand()
{
    bool wasRunning = (commandPid != -1);

    if (commandNotifier) {
        commandNotifier->setEnabled(false);
        delete commandNotifier;
        commandNotifier = nullptr;
    }

    if (commandFd != -1) {
        ::close(commandFd);
        commandFd = -1;
    }

    if (commandPid != -1) {
        CError error;
        send_signal_to_process(commandPid, SIGKILL, &error);
        ::waitpid(commandPid, nullptr, WNOHANG); 
        commandPid = -1;
    }

    if (wasRunning) {
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
        ui->txtConsoleOutput->appendPlainText("\n[Proceso terminado por el usuario (SIGKILL)]\n");
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
    }

    ui->btnTerminateCommand->setEnabled(false);
    ui->btnSendCommand->setEnabled(true);
}