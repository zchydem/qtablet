#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSize>
#include <QString>
#include <QtGlobal>

namespace qtablet{

class SettingsPrivate;

class Settings
{
public:
    static Settings * instance();

    ~Settings();

    QSize screenResolution() const;

    QStringList desktopFilePaths() const;

    QStringList desktopFiles() const;

    QStringList iconPaths() const;

    QStringList imagePaths() const;

private:
    Settings();
    SettingsPrivate * d_ptr;
    Q_DISABLE_COPY( Settings );
};

}
#endif // SETTINGS_H
