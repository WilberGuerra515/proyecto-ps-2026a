#include <QApplication>

// Aquí incluiremos nuestra ventana principal

int main(int argc, char *argv[]) {
    // QApplication gestiona los recursos de la GUI y el flujo de control principal
    QApplication app(argc, argv);

        /* * app.exec() inicia el bucle de eventos (Event Loop) de Qt.
     * Mantiene el programa en ejecución esperando interacciones del usuario.
     * Como aún no hemos instanciado ninguna ventana, si ejecutas esto ahora,
     * el proceso se quedará en segundo plano (puedes detenerlo con Ctrl+C).
     */
    return app.exec();
}