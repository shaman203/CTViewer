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
	void setActivePlaneWidged(vtkSmartPointer<vtkImagePlaneWidget> planeWidget, int minSlice, int maxSlice);

protected:

	vtkSmartPointer<vtkImagePlaneWidget> activePlaneWidget = NULL;
	vtkSmartPointer<vtkRenderWindow> renderer = NULL;
	int minSlice;
	int maxSlice;

	SliceInteractorStyle(){};
	~SliceInteractorStyle(){};

	void MoveSliceForward();
	void MoveSliceBackward();

	virtual void OnKeyDown();

};

