#ifndef CTViewer_H
#define CTViewer_H


#include <vtkActor.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>



#include <QMainWindow>

class Ui_CTViewer;

class CTViewer : public QMainWindow
{
  Q_OBJECT

public:
 
	CTViewer();

	~CTViewer() { };

 
private slots:

  void slotOpenDicom();

  void slotExit();


private:
 
  // Designer form
  Ui_CTViewer *ui;

  QString _lastOpenedPath;

  vtkSmartPointer<vtkActor> _actor;
  vtkSmartPointer<vtkRenderer> _renderer;

  void loadDicom(QString const &dirPath);

};
 

#endif