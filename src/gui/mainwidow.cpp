#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "process_manager.h"
#include "file_shell.h"
#include "filesearchworker.h"
#include "command_console.h"
#include "backup_manager.h"
#include "backupworker.h"

#include <QMessageBox>
#include <QThread>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <QDateTime>
#include "system_monitor.h"

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

    // --- Configuración Módulo 4 (Respaldos) ---
    connect(ui->btnCreateBackup, &QPushButton::clicked, this, &MainWindow::startBackupGeneration);

    // --- Configuración Módulo 5 (Monitor) ---
    monitorTimer = new QTimer(this);
    connect(monitorTimer, &QTimer::timeout, this, &MainWindow::refreshSystemMetrics);
    monitorTimer->start(1000);
    refreshSystemMetrics();

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
        cleanupActiveCommand();
    }

    CError error;
    QString targetDirectory = "/home/Dieguito";

    if (execute_command_async(cmd.toUtf8().constData(), 
                              targetDirectory.toUtf8().constData(), 
                              &commandFd, &commandPid, &error) == 0) 
    {
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
        if (!ui->txtConsoleOutput->toPlainText().isEmpty()) {
            ui->txtConsoleOutput->insertPlainText("\n");
        }
        
        ui->txtConsoleOutput->insertPlainText(QString("[/home/Dieguito]$ %1\n").arg(cmd));
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);

        ui->txtCommandInput->clear();
        ui->btnTerminateCommand->setEnabled(true);
        ui->btnSendCommand->setEnabled(false);

        commandNotifier = new QSocketNotifier(commandFd, QSocketNotifier::Read, this);
        connect(commandNotifier, &QSocketNotifier::activated, this, &MainWindow::readCommandLiveOutput);
    } else {
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
        ui->txtConsoleOutput->insertPlainText(QString("\n[ERROR]: %1\n").arg(QString::fromUtf8(error.message)));
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
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
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
        if (bytesRead == -1) {
            ui->txtConsoleOutput->insertPlainText(QString("\n[Proceso abortado: %1]\n").arg(QString::fromUtf8(error.message)));
        } else {
            ui->txtConsoleOutput->insertPlainText("\n[Proceso terminado exitosamente]\n");
        }
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
        
        cleanupActiveCommand();
    }
}

void MainWindow::terminateActiveCommand()
{
    if (commandPid != -1) {
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
        ui->txtConsoleOutput->insertPlainText("\n[Proceso terminado por el usuario (SIGKILL)]\n");
        ui->txtConsoleOutput->moveCursor(QTextCursor::End);
    }
    cleanupActiveCommand();
}

void MainWindow::cleanupActiveCommand()
{
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

    ui->btnTerminateCommand->setEnabled(false);
    ui->btnSendCommand->setEnabled(true);
}

// =============================================================================
// LÓGICA MÓDULO 4: GESTOR DE RESPALDOS
// =============================================================================
void MainWindow::startBackupGeneration()
{
    QString src = ui->txtSrcDir->text().trimmed();
    QString dest = ui->txtDestDir->text().trimmed();
    QString name = ui->txtBackupName->text().trimmed();

    if (src.isEmpty() || dest.isEmpty() || name.isEmpty()) {
        QMessageBox::warning(this, "Validación", "Por favor completa todos los campos del formulario.");
        return;
    }

    ui->btnCreateBackup->setEnabled(false);
    ui->backupProgressBar->setVisible(true);
    ui->backupProgressBar->setRange(0, 0); 
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->txtBackupLog->append(QString("[%1] Iniciando compresión de: %2 ...").arg(timestamp, src));

    QThread *backupThread = new QThread(this);
    BackupWorker *worker = new BackupWorker(src, dest, name);
    worker->moveToThread(backupThread);

    connect(backupThread, &QThread::started, worker, &BackupWorker::processBackup);
    connect(worker, &BackupWorker::backupFinished, this, &MainWindow::handleBackupFinished);
    connect(worker, &BackupWorker::backupFinished, backupThread, &QThread::quit);
    connect(worker, &BackupWorker::backupFinished, worker, &QObject::deleteLater);
    connect(backupThread, &QThread::finished, backupThread, &QObject::deleteLater);

    backupThread->start();
}

void MainWindow::handleBackupFinished(bool success, const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    
    if (success) {
        ui->txtBackupLog->append(QString("[%1] [ÉXITO]: %2").arg(timestamp, message));
        QMessageBox::information(this, "Respaldos", "¡Copia de seguridad generada con éxito!");
    } else {
        ui->txtBackupLog->append(QString("[%1] [ERROR]: %2").arg(timestamp, message));
        QMessageBox::critical(this, "Respaldos", QString("Fallo al crear el respaldo:\n%1").arg(message));
    }

    ui->backupProgressBar->setVisible(false);
    ui->btnCreateBackup->setEnabled(true);
}

// =============================================================================
// LÓGICA MÓDULO 5
// =============================================================================
void MainWindow::refreshSystemMetrics()
{
    SystemMetrics metrics;
    CError error;

    if (collect_system_metrics(&metrics, &error) == 0) {
        // 1. Actualizar RAM
        long usedRam = metrics.total_ram_mb - metrics.avail_ram_mb;
        ui->barRAM->setMaximum(metrics.total_ram_mb);
        ui->barRAM->setValue(usedRam);
        
        ui->lblRAMDetails->setText(QString("Memoria Libre Real: %1 MB  |  Disponible para Apps: %2 MB")
                                   .arg(metrics.free_ram_mb)
                                   .arg(metrics.avail_ram_mb));

        // 2. Actualizar Carga de Sistema
        ui->valLoad1->setText(QString::number(metrics.load_1min, 'f', 2));
        ui->valLoad5->setText(QString::number(metrics.load_5min, 'f', 2));

        // 3. Formatear y Actualizar Uptime (Segundos a HH:MM:SS)
        long secs = metrics.uptime_seconds;
        long hours = secs / 3600;
        long mins = (secs % 3600) / 60;
        long remainingSecs = secs % 60;

        ui->valUptime->setText(QString("%1h %2m %3s")
                               .arg(hours, 2, 10, QChar('0'))
                               .arg(mins, 2, 10, QChar('0'))
                               .arg(remainingSecs, 2, 10, QChar('0')));
    } else {
        ui->lblRAMDetails->setText(QString("[Error de Telemetría]: %1").arg(QString::fromUtf8(error.message)));
    }
}