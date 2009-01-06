#include "desktopfileparser.hh"
#include <QSettings>
#include <QtDebug>
#include <QStringList>

namespace qtablet{

//Keys
QString const DesktopFileParser::Encoding    = "Encoding";
QString const DesktopFileParser::Version     = "Version";
QString const DesktopFileParser::Type        = "Type";
QString const DesktopFileParser::Name        = "Name";
QString const DesktopFileParser::Exec        = "Exec";
QString const DesktopFileParser::Icon        = "Icon";
QString const DesktopFileParser::MimeType    = "MimeType";
QString const DesktopFileParser::Comment     = "Comment";
QString const DesktopFileParser::Categories  = "Categories";

DesktopFileParser::DesktopFileParser()
{
}

QMap <QString, QString> DesktopFileParser::parse( QString const & desktopFileName ){
    // Let's read the desktop file using QSettings
    QSettings desktopFile( "desktop:" + desktopFileName, QSettings::IniFormat );

    // Make sure that everything went ok
    QMap <QString, QString> result;
    if ( desktopFile.status() != QSettings::NoError ){
        qCritical() << "DesktopFileParser: Failed to parse file: " << desktopFileName;
        return result;
    }

    // Get the group
    QStringList group = desktopFile.childGroups();
    if ( !group.contains("Desktop Entry") ){
        qCritical() << "File: " << desktopFileName << "contains wrong syntax!" << group;
        return result;
    }

    // Define keys that we are interested in.
    QStringList keys;
    keys    << Encoding
            << Version
            << Type
            << Name
            << Exec
            << Icon
            << MimeType
            << Comment
            << Categories;


    desktopFile.beginGroup( "Desktop Entry" );
    foreach( QString key, keys ){
        result.insert(key, desktopFile.value( key, "").toString() );
        //qDebug() << key << desktopFile.value( key, "").toString();
    }
    desktopFile.endGroup();

    return result;
}

}
