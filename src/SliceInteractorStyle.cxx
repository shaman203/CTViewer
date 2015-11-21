#include "SliceInteractorStyle.h"

#include "vtkRenderWindowInteractor.h"

void SliceInteractorStyle::setRenderer(vtkSmartPointer<vtkRenderWindow> renderer)
{
	this->renderer = renderer;
}

void SliceInteractorStyle::setActivePlaneWidged(vtkSmartPointer<vtkImagePlaneWidget> planeWidget, int minSlice, int maxSlice, bool useMoveSlice)
{
	this->activePlaneWidget = planeWidget;
	this->maxSlice = maxSlice;
	this->minSlice = minSlice;
	this->useMoveSlice = useMoveSlice;
}

void SliceInteractorStyle::MoveSliceForward() {
	if (activePlaneWidget != NULL || activePlaneWidget->GetSliceIndex() < maxSlice)
	{
		if (useMoveSlice)
		{
			activePlaneWidget->SetSliceIndex(activePlaneWidget->GetSliceIndex() + 1);
		}
		else
		{
			double dX, dY, dZ;
			getNormalVector(activePlaneWidget, &dX, &dY, &dZ);
			double oldCoords[3];

			activePlaneWidget->GetOrigin(oldCoords);
			oldCoords[0] += dX;
			oldCoords[1] += dY;
			oldCoords[2] += dZ;
			activePlaneWidget->SetOrigin(oldCoords);

			activePlaneWidget->GetPoint1(oldCoords);
			oldCoords[0] += dX;
			oldCoords[1] += dY;
			oldCoords[2] += dZ;
			activePlaneWidget->SetPoint1(oldCoords);

			activePlaneWidget->GetPoint2(oldCoords);
			oldCoords[0] += dX;
			oldCoords[1] += dY;
			oldCoords[2] += dZ;
			activePlaneWidget->SetPoint2(oldCoords);

		}
		activePlaneWidget->UpdatePlacement();
		renderer->Render();
	}
}

void SliceInteractorStyle::MoveSliceBackward() {
	if (activePlaneWidget != NULL || activePlaneWidget->GetSliceIndex() > minSlice)
	{
		if (useMoveSlice)
		{
			activePlaneWidget->SetSliceIndex(activePlaneWidget->GetSliceIndex() - 1);
			//std::cout << activePlaneWidget->GetSliceIndex() << std::endl;
		}
		else
		{
			double dX, dY, dZ;
			getNormalVector(activePlaneWidget, &dX, &dY, &dZ);
			double oldCoords[3];

			activePlaneWidget->GetOrigin(oldCoords);
			oldCoords[0] -= dX;
			oldCoords[1] -= dY;
			oldCoords[2] -= dZ;
			activePlaneWidget->SetOrigin(oldCoords);

			activePlaneWidget->GetPoint1(oldCoords);
			oldCoords[0] -= dX;
			oldCoords[1] -= dY;
			oldCoords[2] -= dZ;
			activePlaneWidget->SetPoint1(oldCoords);

			activePlaneWidget->GetPoint2(oldCoords);
			oldCoords[0] -= dX;
			oldCoords[1] -= dY;
			oldCoords[2] -= dZ;
			activePlaneWidget->SetPoint2(oldCoords);

		}
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

void SliceInteractorStyle::getNormalVector(vtkSmartPointer<vtkImagePlaneWidget> activePlaneWidget, double* dX, double* dY, double* dZ)
{
	double v1[3], v2[3], o[3], n[3];

	activePlaneWidget->GetOrigin(o);
	activePlaneWidget->GetPoint1(v1);
	activePlaneWidget->GetPoint2(v2);

	v1[0] = v1[0] - o[0];
	v1[1] = v1[1] - o[1];
	v1[2] = v1[2] - o[2];

	v2[0] = v2[0] - o[0];
	v2[1] = v2[1] - o[1];
	v2[2] = v2[2] - o[2];

	n[0] = v1[1] * v2[2] - v1[2] * v2[1];
	n[1] = -1*(v1[0] * v2[2] - v1[2] * v2[0]);
	n[2] = v1[0] * v2[1] - v1[1] * v2[0];

	double length = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	n[0]  = n[0] / length;
	n[1]  = n[1] / length;
	n[2]  = n[2] / length;

	*dX = n[0];
	*dY = n[1];
	*dZ = n[2];
}
