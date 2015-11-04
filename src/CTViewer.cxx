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
#include "vtkImageMapper3D.h"
#include "vtkImageSlice.h"
#include "vtkImageProperty.h"
#include "vtkActor.h"
#include <vtkImagePlaneWidget.h>
#include "vtkRenderWindow.h"

//ITK includes
#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"

#include "itkPoint.h"



CTViewer::CTViewer()
	:_actor(vtkSmartPointer<vtkImageActor>::New()),
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

void CTViewer::SetupCamera()
{

	vtkImageData * image = _actor->GetInput();

	if (!image)
	{
		return;
	}

	vtkFloatingPointType spacing[3];
	vtkFloatingPointType origin[3];
	int   dimensions[3];

	image->GetSpacing(spacing);
	image->GetOrigin(origin);
	image->GetDimensions(dimensions);

	double focalPoint[3];
	double position[3];

	for (unsigned int cc = 0; cc < 3; cc++)
	{
		focalPoint[cc] = origin[cc] + (spacing[cc] * dimensions[cc]) / 2.0;
		position[cc] = focalPoint[cc];
	}

	int idx = 0;
	switch (_orientation)
	{
	case Saggital:
	{
		idx = 0;
		_camera->SetViewUp(0, 0, -1);
		break;
	}
	case Coronal:
	{
		idx = 1;
		_camera->SetViewUp(0, 0, -1);
		break;
	}
	case Axial:
	{
		idx = 2;
		_camera->SetViewUp(0, -1, 0);
		break;
	}
	}

	const double distanceToFocalPoint = 1000;
	position[idx] += distanceToFocalPoint;

	_camera->SetPosition(position);
	_camera->SetFocalPoint(focalPoint);

#define myMAX(x,y) (((x)>(y))?(x):(y))  

	int d1 = (idx + 1) % 3;
	int d2 = (idx + 2) % 3;

	double max = myMAX(
		spacing[d1] * dimensions[d1],
		spacing[d2] * dimensions[d2]);


	_camera->SetParallelScale(max / 2 * _zoomFactor);

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
	_actor->GetMapper()->SetInputData(connector->GetOutput());
	_actor->GetProperty()->SetInterpolationTypeToCubic();

	_renderer->AddActor(_actor);
	_renderer->ResetCamera();


	vtkSmartPointer<vtkRenderWindow> renderWindow = this->ui->qvtkWidget->GetRenderWindow();
		//vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	vtkSmartPointer<vtkImagePlaneWidget> planeWidget =
		vtkSmartPointer<vtkImagePlaneWidget>::New();


	renderWindow->AddRenderer(_renderer);

	renderWindowInteractor->SetInteractorStyle(style);

	planeWidget->SetInteractor(renderWindowInteractor);

	double origin[3] = { 1, 0, 0 };
	planeWidget->SetOrigin(origin);
	planeWidget->UpdatePlacement();

	renderWindow->Render();

	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->Initialize();

	renderWindowInteractor->Start();
	

	// VTK Renderer
	
	// VTK/Qt wedded
	//this->ui->qvtkWidget->GetRenderWindow()->RemoveRenderer(_renderer);
	//this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(_renderer);

	//SetupCamera();
	//update3d();
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

void CTViewer::SetOrientation(OrientationType orientation)
{
	_orientation = orientation;
	this->SetupCamera();
}

void CTViewer::slotExit()
{
	qApp->exit();
}