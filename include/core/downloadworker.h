#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QObject>
#include <QString>
#include "backend.h"
#include "download_manager.h"

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    DownloadWorker(const QString &url, const QString &dest)
        : m_url(url), m_dest(dest) {}

signals:
    void downloadFinished(bool success, const QString &message);

public slots:
    void processDownload() {
        CError error;
        int result = download_file_native(m_url.toUtf8().constData(),
                                          m_dest.toUtf8().constData(),
                                          &error);
        if (result == 0) {
            emit downloadFinished(true, "Archivo descargado correctamente.");
        } else {
            emit downloadFinished(false, QString::fromUtf8(error.message));
        }
    }

private:
    QString m_url;
    QString m_dest;
};

#endif // DOWNLOADWORKER_H