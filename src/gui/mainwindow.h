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

private:
    Ui::MainWindow *ui;
    
    QString currentExploredPath;
    void setupComponentsView(); 
};

#endif // MAINWINDOW_H

