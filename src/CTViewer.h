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

	CTViewer();

	~CTViewer(){};

protected:
	enum Axe
	{
		SagitalIndex = 1,
		CoronalIndex = 2,
		AxialIndex = 3
	};

private slots:

  void slotOpenDicom();

  void slotExit();
  
  void slotShowHidePlane();

  void slotActivatePlane();

private:
 
  // Designer form
  Ui_CTViewer *ui;

  QString _lastOpenedPath;
  

  vtkSmartPointer<vtkDICOMImageReader> reader;
  vtkSmartPointer<vtkActor> actor;
  vtkSmartPointer<vtkRenderer> renderer;
  vtkSmartPointer<vtkRenderWindowInteractor> iren;
  vtkSmartPointer<vtkCellPicker> picker;
  vtkSmartPointer<vtkProperty> ipwProp;
  vtkSmartPointer<SliceInteractorStyle> style;

  

  std::map<Axe, vtkSmartPointer<vtkImagePlaneWidget>> planes;
  double minX, maxX, minY, maxY, minZ, maxZ;

  void loadDicom(QString const &dirPath);
  vtkSmartPointer<vtkImagePlaneWidget> addPlane();
  void addDefaultPlanesAndInit();
};
 

#endif