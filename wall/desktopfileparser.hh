#ifndef DESKTOPFILEPARSER_H
#define DESKTOPFILEPARSER_H

#include <QMap>
#include <QString>

namespace qtablet{

    //! DesktopFileParser is a class for parsing freedesktop.org standard desktop files.
    //!
    //! Each application should define an own app_name.desktop file which should provide
    //! at least the following information:
    //!
    //! Type=Application
    //! Name=Foo Viewer
    //! Comment=The best viewer for Foo objects available!
    //! TryExec=fooview
    //! Exec=fooview %F
    //! Icon=fooview
    //! MimeType=image/x-foo;
    class DesktopFileParser
    {
    public:
        DesktopFileParser();

        //! \return The content of the parsed desktop file.
        static QMap <QString, QString> parse( QString const & desktopFileName );

        // Desktop file keys
        static QString const Encoding;
        static QString const Version;
        static QString const Type;
        static QString const Name;
        static QString const Exec;
        static QString const Icon;
        static QString const MimeType;
        static QString const Comment;
        static QString const Categories;

    };

}
#endif // DESKTOPFILEPARSER_H
