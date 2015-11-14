#ifndef CTViewer_H
#define CTViewer_H

#include <QMainWindow>

#ifndef vtkFloatingPointType
# define vtkFloatingPointType vtkFloatingPointType
typedef double vtkFloatingPointType;
#endif

#include "vtkSmartPointer.h"
#include "vtkDICOMImageReader.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCellPicker.h"
#include "vtkProperty.h"
#include "vtkImagePlaneWidget.h"


#include "SliceInteractorStyle.h"

class Ui_CTViewer;

class CTViewer : public QMainWindow
{
	Q_OBJECT

public:

	typedef enum
	{
		Saggital,
		Coronal,
		Axial
	}
	OrientationType;

	CTViewer();

	~CTViewer(){};
 
private slots:

  void slotOpenDicom();

  void slotExit();

private:
 
  // Designer form
  Ui_CTViewer *ui;

  QString _lastOpenedPath;
  
  OrientationType     _orientation;
  double              _zoomFactor;

  vtkSmartPointer<vtkDICOMImageReader> reader;
  vtkSmartPointer<vtkActor> actor;
  vtkSmartPointer<vtkRenderer> renderer;
  vtkSmartPointer<vtkRenderWindowInteractor> iren;
  vtkSmartPointer<vtkCellPicker> picker;
  vtkSmartPointer<vtkProperty> ipwProp;
  vtkSmartPointer<SliceInteractorStyle> style;

  std::vector<vtkSmartPointer<vtkImagePlaneWidget>> planes;

  void loadDicom(QString const &dirPath);
  vtkSmartPointer<vtkImagePlaneWidget> addPlane(double p1X, double p1Y, double p1Z, double p2X, double p2Y, double p2Z, int slice = -1);
};
 

#endif