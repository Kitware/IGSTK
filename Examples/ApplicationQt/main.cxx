#include <qapplication.h>
#include "application.h"


int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    Application gui;

    a.setMainWidget( &gui );
    gui.SetApplication( &a );
    gui.show();
    return a.exec();
}
