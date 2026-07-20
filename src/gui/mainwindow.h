#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "backend.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slots Módulo 1
    void refreshProcesses();
    void handleProcessSignal();

    // Slots Módulo 2
    void browseToPath(const QString &path);
    void onFileItemDoubleClicked(QTreeWidgetItem *item, int column);
    void startAsyncSearch();
    void handleSearchFinished(const QStringList &results, int errorCode, const QString &errorMsg);

private:
    Ui::MainWindow *ui;
    
    QString currentExploredPath;
    void setupComponentsView(); // Configuración inicial de cabeceras visuales
};

#endif // MAINWINDOW_H

