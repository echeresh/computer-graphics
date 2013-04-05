#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::updateFillMode(bool isFilling)
{
	ui->lightCheckBox->setEnabled(isFilling);
}

void MainWindow::setPolygonMode()
{
	if (ui->fillCheckBox->isChecked())
	{
		ui->lightCheckBox->setEnabled(true);
	}
	ui->fillCheckBox->setEnabled(true);
	ui->bezierGroupBox->setEnabled(false);

}

void MainWindow::setBezierMode()
{
	ui->lightCheckBox->setEnabled(false);
	ui->fillCheckBox->setEnabled(false);
	ui->bezierGroupBox->setEnabled(true);
}
