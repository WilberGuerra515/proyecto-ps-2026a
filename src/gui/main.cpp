#include <QApplication>
#include "MainWindow.h" // Esta clase la crearemos en el siguiente paso

int main(int argc, char *argv[]) {
    // QApplication inicializa el sistema de ventanas y recursos de Qt
    QApplication app(argc, argv);

    // Instanciamos y mostramos la ventana principal
    MainWindow window;
    window.show();

    // exec() inicia el loop de eventos, manteniendo la aplicación viva y responsiva
    return app.exec();
}