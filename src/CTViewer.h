#ifndef CTViewer_H
#define CTViewer_H


#include <vtkImageActor.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include "vtkSphereSource.h" 

#include <QMainWindow>

#ifndef vtkFloatingPointType
# define vtkFloatingPointType vtkFloatingPointType
typedef double vtkFloatingPointType;
#endif

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

	void SetOrientation(OrientationType orientation);
 
private slots:

  void slotOpenDicom();

  void slotExit();

  void update3d();

private:
 
  // Designer form
  Ui_CTViewer *ui;

  QString _lastOpenedPath;

  vtkSmartPointer<vtkImageActor> _actor;
  vtkSmartPointer<vtkRenderer> _renderer;
  vtkSmartPointer<vtkCamera>   _camera;

  
  OrientationType     _orientation;
  double              _zoomFactor;

  void loadDicom(QString const &dirPath);

  void SetupCamera();
};
 

#endif