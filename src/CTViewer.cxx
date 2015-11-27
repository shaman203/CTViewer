// hopfer.benjamin@gmail.com
// http://benjaminhopfer.com
// 
// Phonger example application for Qt / vtk
// 32bit/64bit cross compile

#include "CTviewer.h"
#include "ui_CTviewer.h"

//QT includes
#include <QColorDialog>
#include <QFileDialog>
#include <QStandardPaths>
#include <qevent.h>
#include <qmessagebox.h>

//VTK includes
#include "vtkOutlineFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"
#include "vtkCamera.h"
#include "vtkTextProperty.h"
#include "vtkCubeAxesActor2D.h"
#include "vtkInteractorEventRecorder.h" 


vtkStandardNewMacro(SliceInteractorStyle);

CTViewer::CTViewer() :
reader(vtkDICOMImageReader::New()),
actor(vtkActor::New()),
renderer(vtkRenderer::New()),
iren(vtkRenderWindowInteractor::New()),
picker(vtkCellPicker::New()),
ipwProp(vtkProperty::New()),
style(SliceInteractorStyle::New())
{
	this->ui = new Ui_CTViewer;
	this->ui->setupUi(this);

	picker->SetTolerance(0.005);

	connect(this->ui->pbOpenDicom, SIGNAL(clicked()), this, SLOT(slotOpenDicom()));
	connect(this->ui->btnRefreshCustom, SIGNAL(clicked()), this, SLOT(slotRefreshCustom()));

	connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));

	connect(this->ui->btnAxialActive, SIGNAL(toggled(bool)), this, SLOT(slotActivatePlane()));
	connect(this->ui->btnCoronalActive, SIGNAL(toggled(bool)), this, SLOT(slotActivatePlane()));
	connect(this->ui->btnSagittalActive, SIGNAL(toggled(bool)), this, SLOT(slotActivatePlane()));
	connect(this->ui->btnCustomActive, SIGNAL(toggled(bool)), this, SLOT(slotActivatePlane()));

	connect(this->ui->showAxial, SIGNAL(stateChanged(int)), this, SLOT(slotShowHidePlane()));
	connect(this->ui->showCoronal, SIGNAL(stateChanged(int)), this, SLOT(slotShowHidePlane()));
	connect(this->ui->showSagital, SIGNAL(stateChanged(int)), this, SLOT(slotShowHidePlane()));
	connect(this->ui->showCustom, SIGNAL(stateChanged(int)), this, SLOT(slotShowHidePlane()));

}




void CTViewer::loadDicom(QString const &filePath)
{
	this->ui->tbrowseLog->clear();

	reader->SetDirectoryName(filePath.toStdString().c_str());
	reader->Update();

	vtkSmartPointer<vtkOutlineFilter> outline = vtkOutlineFilter::New();
	outline->SetInputConnection(reader->GetOutputPort());

	vtkSmartPointer<vtkPolyDataMapper> outlineMapper = vtkPolyDataMapper::New();
	outlineMapper->SetInputConnection(outline->GetOutputPort());

	actor->SetMapper(outlineMapper);
	actor->GetProperty()->SetOpacity(0);


	vtkSmartPointer<vtkRenderWindow> renderWindow = this->ui->qvtkWidget->GetRenderWindow();
	renderWindow->AddRenderer(renderer);

	style->setRenderer(renderWindow);

	iren->SetRenderWindow(renderWindow);

	addDefaultPlanesAndInit();

	renderer->AddActor(actor);

	renderer->SetBackground(0.6, 0.6, 0.6);


	renderer->GetActiveCamera()->Elevation(110);
	renderer->GetActiveCamera()->SetViewUp(0, 0, -1);

	vtkSmartPointer<vtkTextProperty> tprop = vtkTextProperty::New();
	tprop->SetColor(0, 0, 1);
	tprop->ShadowOn();

	vtkSmartPointer<vtkCubeAxesActor2D> axes = vtkCubeAxesActor2D::New();
	axes->SetInputData(outline->GetOutput());
	axes->SetCamera(renderer->GetActiveCamera());
	axes->SetFontFactor(0.8);
	axes->SetAxisTitleTextProperty(tprop);
	axes->SetAxisLabelTextProperty(tprop);
	axes->SetFontFactor(6);

	renderer->AddActor(axes);

	renderer->GetActiveCamera()->Azimuth(45);
	renderer->GetActiveCamera()->Dolly(1.0);
	renderer->ResetCameraClippingRange();
	renderer->ResetCamera();

	renderWindow->Render();

	iren->SetInteractorStyle(style);

	imageLoaded = true;
	imageLoaded = true;

	this->ui->tbrowseLog->append("Load succesful\n");

	this->ui->tbrowseLog->append("Max X: " + QString::number(maxX, 'f', 2) + "\n");
	this->ui->tbrowseLog->append("Max Y: " + QString::number(maxY, 'f', 2) + "\n");
	this->ui->tbrowseLog->append("Max Z: " + QString::number(maxZ, 'f', 2) + "\n");
	
	iren->Initialize();
	iren->Start();
	renderWindow->Render();

	
}

vtkSmartPointer<vtkImagePlaneWidget> CTViewer::addPlane()
{
	vtkSmartPointer<vtkImagePlaneWidget> planeWidget = vtkImagePlaneWidget::New();
	planeWidget->SetInteractor(iren);
	planeWidget->SetPicker(picker);
	planeWidget->GetPlaneProperty()->SetColor(0, 1, 0);
	planeWidget->SetTexturePlaneProperty(ipwProp);
	planeWidget->TextureInterpolateOn();
	planeWidget->SetResliceInterpolateToLinear();
	planeWidget->SetInputData(reader->GetOutput());
	planeWidget->DisplayTextOn();
	planeWidget->On();
	return planeWidget;
}

void CTViewer::addDefaultPlanesAndInit()
{
	int sliceCount[3];
	reader->GetOutput()->GetDimensions(sliceCount);


	vtkSmartPointer<vtkImagePlaneWidget> dummyPlaneWidget = vtkImagePlaneWidget::New();
	dummyPlaneWidget->TextureInterpolateOn();
	dummyPlaneWidget->SetResliceInterpolateToLinear();
	dummyPlaneWidget->SetInputData(reader->GetOutput());

	dummyPlaneWidget->SetPlaneOrientationToXAxes();

	minZ = dummyPlaneWidget->GetPoint1()[2];
	maxY = dummyPlaneWidget->GetPoint1()[1];
	minY = dummyPlaneWidget->GetPoint2()[1];
	maxZ = dummyPlaneWidget->GetPoint2()[2];


	dummyPlaneWidget->SetPlaneOrientationToYAxes();
	minX = dummyPlaneWidget->GetPoint1()[0];
	maxX = dummyPlaneWidget->GetPoint2()[0];

	vtkSmartPointer<vtkImagePlaneWidget> widget = addPlane(); //X plane
	widget->SetPlaneOrientationToXAxes();
	style->setActivePlaneWidged(widget, 0, sliceCount[0], false);
	widget->SetSliceIndex(sliceCount[0] / 2);
	planes[CoronalIndex] = widget;

	widget = addPlane(); //Y plane
	widget->SetPlaneOrientationToYAxes();
	widget->SetSliceIndex(sliceCount[1] / 2);
	planes[SagitalIndex] = widget;

	widget = addPlane(); //Z plane
	widget->SetPlaneOrientationToZAxes();
	widget->SetSliceIndex(sliceCount[2] / 2);
	planes[AxialIndex] = widget;

	planes[CustomIndex] = NULL;
	resetCustomCoords();

}

bool CTViewer::withinZeroAndMax(double *coords)
{
	if (coords[0] < 0.0 || coords[0] > maxX)
		return false;
	if (coords[1] < 0.0 || coords[1] > maxY)
		return false;
	if (coords[2] < 0.0 || coords[2] > maxZ)
		return false;
	return true;
}

void CTViewer::resetCustomCoords()
{
	if (planes[CustomIndex] == NULL)
	{
		this->ui->tbOx->setText("");
		this->ui->tbOy->setText("");
		this->ui->tbOz->setText("");

		this->ui->tbP1x->setText("");
		this->ui->tbP1y->setText("");
		this->ui->tbP1z->setText("");

		this->ui->tbP2x->setText("");
		this->ui->tbP2y->setText("");
		this->ui->tbP2z->setText("");

	}
	else
	{
		double origin[3], p1[3], p2[3];

		planes[CustomIndex]->GetOrigin(origin);
		planes[CustomIndex]->GetPoint1(p1);
		planes[CustomIndex]->GetPoint2(p2);

		this->ui->tbOx->setText(std::to_string(origin[0]).c_str());
		this->ui->tbOy->setText(std::to_string(origin[1]).c_str());
		this->ui->tbOz->setText(std::to_string(origin[2]).c_str());

		this->ui->tbP1x->setText(std::to_string(p1[0]).c_str());
		this->ui->tbP1y->setText(std::to_string(p1[1]).c_str());
		this->ui->tbP1z->setText(std::to_string(p1[2]).c_str());

		this->ui->tbP2x->setText(std::to_string(p2[0]).c_str());
		this->ui->tbP2y->setText(std::to_string(p2[1]).c_str());
		this->ui->tbP2z->setText(std::to_string(p2[2]).c_str());
	}
}

void CTViewer::slotOpenDicom()
{
	QStringList folders;
	folders.push_back(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)[0]);
	folders.push_back(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]);
	folders.push_back(QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0]);

	QFileDialog fileDialog(this, tr("Choose Dicom folder"));
	fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
	fileDialog.setFileMode(QFileDialog::DirectoryOnly);
	fileDialog.setDirectory(_lastOpenedPath);
	fileDialog.setHistory(folders);

	//if (fileDialog.exec())
	//{
	QString firstPath("E:/Egyetem/workspace/dicomExamples/examples/sample_data/DICOM/digest_article");// = fileDialog.selectedFiles()[0];
	_lastOpenedPath = firstPath;
	loadDicom(firstPath);
	//}
}

void CTViewer::slotExit()
{
	qApp->exit();
}

void CTViewer::slotShowHidePlane()
{
	if (!imageLoaded)
	{
		QMessageBox::information(this, "Error", "Please load an image first!");
		return;
	}
	if (this->ui->showSagital->isChecked())
	{
		planes[SagitalIndex]->On();
	}
	else
	{
		planes[SagitalIndex]->Off();
	}

	if (this->ui->showCoronal->isChecked())
	{
		planes[CoronalIndex]->On();
	}
	else
	{
		planes[CoronalIndex]->Off();
	}

	if (this->ui->showAxial->isChecked())
	{
		planes[AxialIndex]->On();
	}
	else
	{
		planes[AxialIndex]->Off();
	}

	if (planes[CustomIndex] != NULL)
	{
		if (this->ui->showCustom->isChecked())
		{
			planes[CustomIndex]->On();
		}
		else
		{
			planes[CustomIndex]->Off();
		}
	}
}

void CTViewer::slotActivatePlane()
{
	if (!imageLoaded)
	{
		QMessageBox::information(this, "Error", "Please load an image first!");
		return;
	}
	if (this->ui->btnSagittalActive->isChecked())
	{
		style->setActivePlaneWidged(planes[SagitalIndex], 0, 1000);
	}

	if (this->ui->btnCoronalActive->isChecked())
	{
		style->setActivePlaneWidged(planes[CoronalIndex], 0, 1000);
	}

	if (this->ui->btnAxialActive->isChecked())
	{
		style->setActivePlaneWidged(planes[AxialIndex], 0, 1000);
	}

	if (this->ui->btnCustomActive->isChecked() && planes[CustomIndex] != NULL)
	{
		style->setActivePlaneWidged(planes[CustomIndex], 0, 1000, false);
	}
}

void CTViewer::slotRefreshCustom()
{

	if (!imageLoaded)
	{
		QMessageBox::information(this, "Error", "Please load an image first!");
		return;
	}

	double origin[3], p1[3], p2[3];
	bool ok;
	std::string errorMessage = "";

	origin[0] = this->ui->tbOx->text().toDouble(&ok);
	if (!ok)
	{
		errorMessage += "Origin X coord. is not a double!\n";
	}
	origin[1] = this->ui->tbOy->text().toDouble(&ok);
	if (!ok)
	{
		errorMessage += "Origin Y coord. is not a double!\n";
	}
	origin[2] = this->ui->tbOz->text().toDouble(&ok);
	if (!ok)
	{
		errorMessage += "Origin Z coord. is not a double!\n";
	}

	p1[0] = this->ui->tbP1x->text().toDouble(&ok);
	if (!ok)
	{
		errorMessage += "P1 X coord. is not a double!\n";
	}
	p1[1] = this->ui->tbP1y->text().toDouble(&ok);
	if (!ok)
	{
		errorMessage += "P1 Y coord. is not a double!\n";
	}
	p1[2] = this->ui->tbP1z->text().toDouble(&ok);
	if (!ok)
	{
		errorMessage += "P1 Z coord. is not a double!\n";
	}

	p2[0] = this->ui->tbP2x->text().toDouble(&ok);
	if (!ok)
	{
		errorMessage += "P2 X coord. is not a double!\n";
	}
	p2[1] = this->ui->tbP2y->text().toDouble(&ok);
	if (!ok)
	{
		errorMessage += "P2 Y coord. is not a double!\n";
	}
	p2[2] = this->ui->tbP2z->text().toDouble(&ok);
	if (!ok)
	{
		errorMessage += "P2 Z coord. is not a double!\n";
	}

	if (errorMessage == "")
	{
		if (!withinZeroAndMax(origin))
		{
			errorMessage += "Origin coords not within image bounds!\n";
		}
		if (!withinZeroAndMax(p1))
		{
			errorMessage += "P1 coords not within image bounds!\n";
		}
		if (!withinZeroAndMax(p2))
		{
			errorMessage += "P2 coords not within image bounds!\n";
		}
	}
	if (errorMessage != "")
	{
		QMessageBox::information(this, "Conversion Error", errorMessage.c_str());
		return;
	}

	if (planes[CustomIndex] == NULL)
	{
		vtkSmartPointer<vtkImagePlaneWidget> widget = addPlane();
		widget->SetOrigin(origin);
		widget->SetPoint1(p1);
		widget->SetPoint2(p2);
		widget->UpdatePlacement();
		planes[CustomIndex] = widget;

		renderer->Render();

	}

}