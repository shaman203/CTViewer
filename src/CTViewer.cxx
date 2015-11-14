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
#include <vtkCamera.h>
#include <vtkProperty.h>
#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageActor.h"
#include "vtkImageProperty.h"
#include "vtkActor.h"
#include <vtkImagePlaneWidget.h>
#include "vtkRenderWindow.h"
#include "vtkOutlineFilter.h"
#include "vtkCellPicker.h"
#include "vtkTextProperty.h"
#include "vtkCubeAxesActor2D.h"

//ITK includes
#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"

#include "itkPoint.h"



CTViewer::CTViewer()
	:_actor(vtkSmartPointer<vtkActor>::New()),
	 _renderer(vtkSmartPointer<vtkRenderer>::New())
{
	this->ui = new Ui_CTViewer;
	this->ui->setupUi(this);

	_camera = _renderer->GetActiveCamera();
	_camera->ParallelProjectionOn();

	//this->SetOrientation(Saggital);

	_renderer->SetBackground(0.5, 0.5, 0.5);
	
	connect(this->ui->pbOpenDicom, SIGNAL(clicked()), this, SLOT(slotOpenDicom()));
	connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}




void CTViewer::loadDicom(QString const &filePath)
{
	const unsigned int InputDimension = 3;
	typedef signed short PixelType;
	typedef itk::Image< PixelType, InputDimension >
		InputImageType;
	typedef itk::ImageSeriesReader< InputImageType >
		ReaderType;
	typedef itk::GDCMImageIO
		ImageIOType;
	typedef itk::GDCMSeriesFileNames
		InputNamesGeneratorType;

	typedef itk::ImageToVTKImageFilter< InputImageType > ConnectorType;

	ImageIOType::Pointer gdcmIO = ImageIOType::New();
	InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
	inputNames->SetInputDirectory(filePath.toStdString());
	const ReaderType::FileNamesContainer & filenames =
		inputNames->GetInputFileNames();
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetImageIO(gdcmIO);
	reader->SetFileNames(filenames);
	try
	{
		reader->Update();
	}
	catch (itk::ExceptionObject &excp)
	{
		std::cerr << "Exception thrown while reading the series" << std::endl;
		std::cerr << excp << std::endl;
		return;
	}
	std::cout << "No error on load :D";
	
	ConnectorType::Pointer connector = ConnectorType::New();
	connector->DebugOn();
	connector->SetInput(reader->GetOutput());

	connector->Update();
	
	vtkOutlineFilter* outline = vtkOutlineFilter::New();
	outline->SetInputData(connector->GetOutput());

	vtkPolyDataMapper* outlineMapper = vtkPolyDataMapper::New();
	outlineMapper->SetInputConnection(outline->GetOutputPort());


	_actor->SetMapper(outlineMapper);
	_actor->GetProperty()->SetOpacity(0);

	
	vtkSmartPointer<vtkRenderWindow> renderWindow = this->ui->qvtkWidget->GetRenderWindow();
	renderWindow->AddRenderer(_renderer);


	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renderWindow);

	vtkCellPicker* picker = vtkCellPicker::New();
	picker->SetTolerance(0.005);

	vtkProperty* ipwProp = vtkProperty::New();
	//assign default props to the ipw's texture plane actor 


	vtkImagePlaneWidget* planeWidgetZ = vtkImagePlaneWidget::New();
	planeWidgetZ->SetInteractor(iren);
	planeWidgetZ->SetPicker(picker);
	planeWidgetZ->GetPlaneProperty()->SetColor(0, 1, 0);
	planeWidgetZ->SetTexturePlaneProperty(ipwProp);
	planeWidgetZ->TextureInterpolateOn();
	planeWidgetZ->SetResliceInterpolateToCubic();
	planeWidgetZ->SetInputData(connector->GetOutput());
	planeWidgetZ->SetPlaneOrientationToZAxes();
	planeWidgetZ->SetSliceIndex(9);
	//    planeWidgetZ->SetLookupTable( planeWidgetX->GetLookupTable()); 
	planeWidgetZ->DisplayTextOn();
	planeWidgetZ->On();


	vtkImagePlaneWidget* planeWidgetZ1 = vtkImagePlaneWidget::New();
	planeWidgetZ1->SetInteractor(iren);
	planeWidgetZ1->SetPicker(picker);
	planeWidgetZ1->GetPlaneProperty()->SetColor(0, 1, 0);
	planeWidgetZ1->SetTexturePlaneProperty(ipwProp);
	planeWidgetZ1->TextureInterpolateOn();
	planeWidgetZ1->SetResliceInterpolateToLinear();
	planeWidgetZ1->SetInputData(connector->GetOutput());
	planeWidgetZ1->SetPlaneOrientationToYAxes();
	planeWidgetZ1->SetSliceIndex(106);
	planeWidgetZ1->DisplayTextOn();
	planeWidgetZ1->On();

	vtkImagePlaneWidget* planeWidgetZ2 = vtkImagePlaneWidget::New();
	planeWidgetZ2->SetInteractor(iren);
	planeWidgetZ2->SetPicker(picker);
	planeWidgetZ2->GetPlaneProperty()->SetColor(0, 1, 0);
	planeWidgetZ2->SetTexturePlaneProperty(ipwProp);
	planeWidgetZ2->TextureInterpolateOn();
	planeWidgetZ2->SetResliceInterpolateToLinear();
	planeWidgetZ2->SetInputData(connector->GetOutput());
	planeWidgetZ2->SetPlaneOrientationToXAxes();
	planeWidgetZ2->SetSliceIndex(106);
	planeWidgetZ2->DisplayTextOn();
	planeWidgetZ2->On();

	_renderer->AddActor(_actor);

	_renderer->SetBackground(0.6, 0.6, 0.6);


	_renderer->GetActiveCamera()->Elevation(110);
	_renderer->GetActiveCamera()->SetViewUp(0, 0, -1);

	vtkTextProperty *tprop = vtkTextProperty::New();
	tprop->SetColor(0, 0, 1);
	tprop->ShadowOn();

	vtkCubeAxesActor2D *axes = vtkCubeAxesActor2D::New();
	axes->SetInputData(outline->GetOutput());
	axes->SetCamera(_renderer->GetActiveCamera());
	//    axes-> SetLabelFormat "%6.4g" 
	//    axes-> SetFlyModeToOuterEdges(); 
	axes->SetFontFactor(0.8);
	axes->SetAxisTitleTextProperty(tprop);
	axes->SetAxisLabelTextProperty(tprop);
	//	axes->GetProperty()->SetDiffuseColor(1, 0, 0.25); 
	axes->SetFontFactor(6);

	_renderer->AddActor(axes);

	//  ren1->SetViewport(0,0,0.58333,1); 
	_renderer->GetActiveCamera()->Azimuth(45);
	_renderer->GetActiveCamera()->Dolly(1.0);
	_renderer->ResetCameraClippingRange();
	_renderer->ResetCamera();
	// Set the actors' postions 
	// 
	renderWindow->Render();

	iren->Initialize();
	iren->Start();
	renderWindow->Render();
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

void CTViewer::update3d()
{
	_renderer->RemoveActor(_actor);
	_renderer->AddActor(_actor);
	_renderer->ResetCamera();
	ui->qvtkWidget->update();
}

void CTViewer::slotExit()
{
	qApp->exit();
}