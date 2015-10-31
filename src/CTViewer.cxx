// hopfer.benjamin@gmail.com
// http://benjaminhopfer.com
// 
// Phonger example application for Qt / vtk
// 32bit/64bit cross compile

#include "CTviewer.h"

#include "ui_CTviewer.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QStandardPaths>

#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>



static double const GRADIENT_BACKGROUND_TOP[3] = { 0.4, 0.4, 0.4 };
static double const GRADIENT_BACKGROUND_BOT[3] = { 0.6, 0.6, 0.6 };


CTViewer::CTViewer()
	: _actor(vtkSmartPointer<vtkActor>::New()),
	_renderer(vtkSmartPointer<vtkRenderer>::New())
{
	this->ui = new Ui_CTViewer;
	this->ui->setupUi(this);

	connect(this->ui->pbOpenDicom, SIGNAL(clicked()), this, SLOT(slotOpenDicom()));
	connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}


void CTViewer::loadDicom(QString const &filePath)
{

	// VTK/Qt wedded
	this->ui->qvtkWidget->GetRenderWindow()->RemoveRenderer(_renderer);
	this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(_renderer);

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

	if (fileDialog.exec())
	{
		QString firstPath = fileDialog.selectedFiles()[0];
		_lastOpenedPath = firstPath;
		loadDicom(firstPath);
	}

	_renderer->RemoveActor(_actor);
	_renderer->AddActor(_actor);

	_renderer->ResetCameraClippingRange();
	_renderer->ResetCamera();
}

void CTViewer::slotExit()
{
	qApp->exit();
}