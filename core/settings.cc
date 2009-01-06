#include "settings.hh"

#include <QSettings>
#include <QDir>
#include <QtDebug>

#ifdef Q_WS_QWS
// Nothing here yet. Add if needed
#endif

#ifdef Q_WS_X11
#include <QDesktopWidget>
#include <QDesktopServices>
#endif

namespace qtablet{

class SettingsPrivate{
    public:
    SettingsPrivate(){

        QSettings settings( "config:qtablet.conf", QSettings::IniFormat );

        if ( settings.status() != QSettings::NoError ){
            qWarning() << "Settings: Failed to parse configuration file!";
            return;
        }

        // Parse group contents
        parseGroup( "device", settings );
        parseGroup( "paths",  settings );

    }

    void parseGroup( QString const & group, QSettings & settings ){
        settings.beginGroup( group );
        QStringList keys = settings.childKeys();


        foreach ( QString key, keys ){
            m_values.insert( key, settings.value( key, "" ).toString() );            
        }

        settings.endGroup();
    }



    QHash <QString, QString> m_values;
};


////// BEGIN Settings CLASS ///////
Settings * Settings::instance(){
    static Settings instance;
    return &instance;
}

Settings::~Settings(){
    delete d_ptr;
}

QSize Settings::screenResolution() const{
    // If using X11 just get the resolution from the desktop widget.
#ifdef Q_WS_X11
    QDesktopWidget widget;
    return widget.screenGeometry().size();
#endif

    // Read resolition from the configuration file.
    QString resolution = d_ptr->m_values.value( resolution );
    QStringList values = resolution.split("x");
    QSize size;

    if ( values.size() != 2 ){
        qWarning() << "Check configuration file. Resolution wrong!";
        return size;
    }

    size.setWidth ( values.at(0).toInt() );
    size.setHeight( values.at(1).toInt() );
    return size;
}


QStringList Settings::desktopFilePaths() const{
    return d_ptr->m_values.value( "desktopfiles" ).split(":");
}

QStringList Settings::desktopFiles() const{
    // Get the paths to desktop files
    QStringList paths = desktopFilePaths();

    // Get the actual files from the paths
    QStringList files;
    foreach( QString path, paths ){
        QDir dir( path );
        files << dir.entryList( QStringList("*.desktop"), QDir::Files );
    }

    return  files;
}


QStringList Settings::iconPaths() const{
    return d_ptr->m_values.value( "icons" ).split(":");
}

QStringList Settings::imagePaths() const{
    return d_ptr->m_values.value( "images" ).split(":");
}


Settings::Settings():
    d_ptr( new SettingsPrivate )
{
     // Set global search paths for the framework level.
     QDir::setSearchPaths( "desktop", desktopFilePaths() );
     QDir::setSearchPaths( "icon",    iconPaths()        );
     QDir::setSearchPaths( "image",   imagePaths()       );    
}

}
