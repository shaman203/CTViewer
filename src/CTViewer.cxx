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

//VTK includes
#include "vtkOutlineFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkImagePlaneWidget.h"
#include "vtkImageData.h"
#include "vtkCamera.h"
#include "vtkTextProperty.h"
#include "vtkCubeAxesActor2D.h"
#include "vtkInteractorEventRecorder.h" 
#include "vtkInteractorStyleTrackballCamera.h"

CTViewer::CTViewer():
reader(vtkDICOMImageReader::New()),
actor(vtkActor::New()),
renderer(vtkRenderer::New()),
iren(vtkRenderWindowInteractor::New()),
picker(vtkCellPicker::New()),
ipwProp(vtkProperty::New())
{
	this->ui = new Ui_CTViewer;
	this->ui->setupUi(this);
	
	picker->SetTolerance(0.005);

	connect(this->ui->pbOpenDicom, SIGNAL(clicked()), this, SLOT(slotOpenDicom()));
	connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
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

	iren->SetRenderWindow(renderWindow);

	addPlane(100,0,0,0,100,0); //X plane
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

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

	iren->SetInteractorStyle(style);

	iren->Initialize();
	iren->Start();
	renderWindow->Render();
}

void CTViewer::addPlane(double p1X, double p1Y, double p1Z, double p2X, double p2Y, double p2Z, int slice)
{
	vtkSmartPointer<vtkImagePlaneWidget> planeWidget = vtkImagePlaneWidget::New();
	planeWidget->SetInteractor(iren);
	planeWidget->SetPicker(picker);
	planeWidget->GetPlaneProperty()->SetColor(0, 1, 0);
	planeWidget->SetTexturePlaneProperty(ipwProp);
	planeWidget->TextureInterpolateOn();
	planeWidget->SetResliceInterpolateToLinear();
	planeWidget->SetInputData(reader->GetOutput());
	std::cout << reader->GetOutput()->GetDimensions()[0] << " " << reader->GetOutput()->GetDimensions()[1] << " " << reader->GetOutput()->GetDimensions()[2] << std::endl;
	planeWidget->SetPlaneOrientationToZAxes();
	//planeWidget->SetPoint1(p1X, p1Y, p1Z);
	//planeWidget->SetPoint2(p2X, p2Y, p2Z);
	//std::cout << planeWidget->GetPoint1()[0] << " " << planeWidget->GetPoint1()[1] << " " << planeWidget->GetPoint1()[2] << std::endl;
	//std::cout << planeWidget->GetPoint2()[0] << " " << planeWidget->GetPoint2()[1] << " " << planeWidget->GetPoint2()[2] << std::endl;

	if (slice < 0)
	{
		planeWidget->SetSliceIndex(100);
		std::cout << *planeWidget->GetResliceOutput()->GetExtent() << std::endl;
	}
	else
	{
		planeWidget->SetSliceIndex(slice);
	}	
	planeWidget->DisplayTextOn();
	planeWidget->On();
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