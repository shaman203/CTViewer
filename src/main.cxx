#include <QApplication>
#include "CTviewer.h"
 
int main( int argc, char** argv )
{
  // QT Stuff
  QApplication app( argc, argv );
 
  CTViewer ctViewer;
  ctViewer.show();
 
  return app.exec();
}
