#include "SliceInteractorStyle.h"

#include "vtkRenderWindowInteractor.h"

void SliceInteractorStyle::setRenderer(vtkSmartPointer<vtkRenderWindow> renderer)
{
	this->renderer = renderer;
}

void SliceInteractorStyle::setActivePlaneWidged(vtkSmartPointer<vtkImagePlaneWidget> planeWidget, int minSlice, int maxSlice)
{
	this->activePlaneWidget = planeWidget;
	this->maxSlice = maxSlice;
	this->minSlice = minSlice;
}

void SliceInteractorStyle::MoveSliceForward() {
	if (activePlaneWidget != NULL || activePlaneWidget->GetSliceIndex() < maxSlice)
	{
		std::cout << activePlaneWidget->GetSliceIndex() << "-> ";
		activePlaneWidget->SetSliceIndex(activePlaneWidget->GetSliceIndex() + 1);
		std::cout << activePlaneWidget->GetSliceIndex() << std::endl;
		activePlaneWidget->UpdatePlacement();
		renderer->Render();
	}
}

void SliceInteractorStyle::MoveSliceBackward() {
	if (activePlaneWidget != NULL || activePlaneWidget->GetSliceIndex() > minSlice)
	{
		activePlaneWidget->SetSliceIndex(activePlaneWidget->GetSliceIndex() - 1);
		std::cout << activePlaneWidget->GetSliceIndex() << std::endl;
		activePlaneWidget->UpdatePlacement();
		renderer->Render();
	}
}


void SliceInteractorStyle::OnKeyDown() {
	std::string key = this->GetInteractor()->GetKeySym();
	if (key.compare("Up") == 0) {
		//cout << "Up arrow key was pressed." << endl;
		MoveSliceForward();
	}
	else if (key.compare("Down") == 0) {
		//cout << "Down arrow key was pressed." << endl;
		MoveSliceBackward();
	}
	// forward event
	vtkInteractorStyleTrackballCamera::OnKeyDown();
}


