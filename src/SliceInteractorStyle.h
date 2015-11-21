#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkObjectFactory.h"
#include "vtkImagePlaneWidget.h"
#include "vtkSmartPointer.h"
#include "vtkRenderWindow.h"

class SliceInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
	static SliceInteractorStyle* New();
	vtkTypeMacro(SliceInteractorStyle, vtkInteractorStyleTrackballCamera);
	void setRenderer(vtkSmartPointer<vtkRenderWindow> renderer);
	void setActivePlaneWidged(vtkSmartPointer<vtkImagePlaneWidget> planeWidget, int minSlice, int maxSlice, bool useMoveSlice = true);

protected:

	vtkSmartPointer<vtkImagePlaneWidget> activePlaneWidget = NULL;
	vtkSmartPointer<vtkRenderWindow> renderer = NULL;
	int minSlice;
	int maxSlice;
	bool useMoveSlice = true;

	SliceInteractorStyle(){};
	~SliceInteractorStyle(){};

	void MoveSliceForward();
	void MoveSliceBackward();

	void getNormalVector(vtkSmartPointer<vtkImagePlaneWidget> activePlaneWidget, double* dX, double* dY, double* dZ);

	virtual void OnKeyDown();

};

