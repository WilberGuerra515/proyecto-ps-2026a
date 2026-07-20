#ifndef FILESEARCHWORKER_H
#define FILESEARCHWORKER_H

#include <QObject>
#include <QStringList>
#include "backend.h"
#include "file_shell.h" 

class FileSearchWorker : public QObject
{
    Q_OBJECT
public:
    FileSearchWorker(const QString &basePath, const QString &term)
        : m_basePath(basePath), m_term(term) {}

signals:
    void searchFinished(const QStringList &results, int errorCode, const QString &errorMsg);

public slots:
    void processSearch() {
        int count = 0;
        CError error;
        
        // Llamada pesada en segundo plano al backend C
        char** c_results = search_files_recursive(m_basePath.toUtf8().constData(), 
                                                  m_term.toUtf8().constData(), 
                                                  &count, &error);
        
        QStringList finalResults;
        if (c_results) {
            for (int i = 0; i < count; ++i) {
                finalResults.append(QString::fromUtf8(c_results[i]));
                free(c_results[i]); // Liberamos cada strdup hecho en C
            }
            free_c_pointer(c_results); // Cumplimos el contrato de memoria
        }

        emit searchFinished(finalResults, error.code, QString::fromUtf8(error.message));
    }

private:
    QString m_basePath;
    QString m_term;
};

#endif // FILESEARCHWORKER_H
