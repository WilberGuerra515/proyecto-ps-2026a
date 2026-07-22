#ifndef BACKUPWORKER_H
#define BACKUPWORKER_H

#include <QObject>
#include <QString>
#include "backend.h"
#include "backup_manager.h"

class BackupWorker : public QObject
{
    Q_OBJECT
public:
    BackupWorker(const QString &src, const QString &dest, const QString &name)
        : m_src(src), m_dest(dest), m_name(name) {}

signals:
    void backupFinished(bool success, const QString &message);

public slots:
    void processBackup() {
        CError error;
        int result = generate_tar_backup(m_src.toUtf8().constData(),
                                         m_dest.toUtf8().constData(),
                                         m_name.toUtf8().constData(),
                                         &error);
        if (result == 0) {
            emit backupFinished(true, QString("Respaldo '%1.tar.gz' creado exitosamente.").arg(m_name));
        } else {
            emit backupFinished(false, QString::fromUtf8(error.message));
        }
    }

private:
    QString m_src;
    QString m_dest;
    QString m_name;
};

#endif // BACKUPWORKER_H