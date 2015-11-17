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
	connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));

	connect(this->ui->btnAxialActive, SIGNAL(toggled(bool)), this, SLOT(slotActivatePlane()));
	connect(this->ui->btnCoronalActive, SIGNAL(toggled(bool)), this, SLOT(slotActivatePlane()));
	connect(this->ui->btnSagittalActive, SIGNAL(toggled(bool)), this, SLOT(slotActivatePlane()));

	connect(this->ui->showTransvers, SIGNAL(stateChanged(int)), this, SLOT(slotShowHidePlane()));
	connect(this->ui->showCoronal, SIGNAL(stateChanged(int)), this, SLOT(slotShowHidePlane()));
	connect(this->ui->showSagital, SIGNAL(stateChanged(int)), this, SLOT(slotShowHidePlane()));

}




void CTViewer::loadDicom(QString const &filePath)
{

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

	//addPlane();
	//addPlane();

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
	//    axes-> SetLabelFormat "%6.4g" 
	//    axes-> SetFlyModeToOuterEdges(); 
	axes->SetFontFactor(0.8);
	axes->SetAxisTitleTextProperty(tprop);
	axes->SetAxisLabelTextProperty(tprop);
	//	axes->GetProperty()->SetDiffuseColor(1, 0, 0.25); 
	axes->SetFontFactor(6);

	renderer->AddActor(axes);

	//  ren1->SetViewport(0,0,0.58333,1); 
	renderer->GetActiveCamera()->Azimuth(45);
	renderer->GetActiveCamera()->Dolly(1.0);
	renderer->ResetCameraClippingRange();
	renderer->ResetCamera();
	// Set the actors' postions 
	// 
	renderWindow->Render();


	iren->SetInteractorStyle(style);

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
	//planeWidget->SetPlaneOrientationToZAxes();
	//planeWidget->SetOrigin(oX, oY, oZ);
	//planeWidget->SetPoint1(p1X, p1Y, p1Z);
	//planeWidget->SetPoint2(p2X, p2Y, p2Z);
	//std::cout << planeWidget->GetPoint1()[0] << " " << planeWidget->GetPoint1()[1] << " " << planeWidget->GetPoint1()[2] << std::endl;
	//std::cout << planeWidget->GetPoint2()[0] << " " << planeWidget->GetPoint2()[1] << " " << planeWidget->GetPoint2()[2] << std::endl;

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
	//std::cout << dummyPlaneWidget->GetOrigin()[0] << " " << dummyPlaneWidget->GetOrigin()[1] << " " << dummyPlaneWidget->GetOrigin()[2];

	dummyPlaneWidget->SetPlaneOrientationToYAxes();
	minX = dummyPlaneWidget->GetPoint1()[0];
	maxX = dummyPlaneWidget->GetPoint2()[0];
	//std::cout << dummyPlaneWidget->GetOrigin()[0] << " " << dummyPlaneWidget->GetOrigin()[1] << " " << dummyPlaneWidget->GetOrigin()[2];

	vtkSmartPointer<vtkImagePlaneWidget> widget = addPlane(); //X plane
	widget->SetPlaneOrientationToXAxes();
	style->setActivePlaneWidged(widget, 0, sliceCount[0]);
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

	if (this->ui->showTransvers->isChecked())
	{
		planes[AxialIndex]->On();
	}
	else
	{
		planes[AxialIndex]->Off();
	}
}

void CTViewer::slotActivatePlane()
{

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

}