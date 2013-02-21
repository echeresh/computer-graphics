#include <QDir>
#include "mainwindow.h"
#include "ui_mainwindow.h"

const qreal MainWindow::GAMMA_SLIDER_STEP = .01;
const qreal MainWindow::GAMMA_DEFAULT = 1.;
const qreal MainWindow::GAMMA_DISPLAY = 2.2;
const int MainWindow::ZOOM_DEFAULT = 1;
const QString MainWindow::IMAGES_PATH = ":\\tvset_yuv\\";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    converseRec(&ConverseRec::BT601),
    unboxingPolicy(Dub),
    imageFiles(QDir(IMAGES_PATH).entryList())
{
    ui->setupUi(this);
    connect(&renderThread, SIGNAL(renderedImage(QImage)), SLOT(updateImage(QImage)));

    foreach (const QString &str, imageFiles)
    {
        images.append(QImage(IMAGES_PATH + str));
    }
    fileListViewModel = new QStringListModel(imageFiles);
    ui->fileListView->setModel(fileListViewModel);
    ui->fileListView->selectionModel()->select(fileListViewModel->index(0, 0), QItemSelectionModel::Select);
    graphicsItem = new QGraphicsPixmapItem();

    setZoom(ZOOM_DEFAULT, false);
    setGamma(GAMMA_DEFAULT, false);
    renderImage(0);

    graphicsScene = new GraphicsScene();
    connect(graphicsScene, SIGNAL(loadedImage(QImage)), SLOT(renderLoadedImage(QImage)));
    graphicsScene->addItem(graphicsItem);
    ui->graphicsView->setScene(graphicsScene);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete graphicsScene;
    delete fileListViewModel;

}

void MainWindow::on_fileListView_doubleClicked(const QModelIndex &index)
{
    renderImage(index.row());
}

void MainWindow::renderImage()
{
    renderImage(currentImage);
}

void MainWindow::renderImage(int index)
{
    if (index < 0 || index >= imageFiles.count())
    {
        return;
    }
    renderImage(images[index]);
}

void MainWindow::renderImage(const QImage &image)
{
    currentImage = image;
    renderThread.render(image, gamma / GAMMA_DISPLAY, converseRec, zoom, unboxingPolicy);
}

void MainWindow::updateImage(const QImage &image)
{
    graphicsItem->setPixmap(QPixmap::fromImage(image));
    ui->graphicsView->setSceneRect(0, 0, image.width(), image.height());
}

void MainWindow::on_bt601RadioButton_clicked()
{
    converseRec = &ConverseRec::BT601;
    renderImage();
}

void MainWindow::on_bt709RadioButton_clicked()
{
    converseRec = &ConverseRec::BT709;
    renderImage();
}

void MainWindow::setGamma(qreal gamma, bool autoRendering)
{
    if (gamma < ui->gammaSlider->minimum() * GAMMA_SLIDER_STEP || gamma > ui->gammaSlider->maximum() * GAMMA_SLIDER_STEP)
        return;
    this->gamma = gamma;
    ui->gammaSlider->setValue(static_cast<int>(gamma / GAMMA_SLIDER_STEP + .5));
    ui->gammaSliderLabel->setText(QString::number(gamma));
    if (autoRendering)
    {
        renderImage();
    }
}

void MainWindow::setZoom(int zoom, bool autoRendering)
{
    this->zoom = zoom;
    ui->zoomSlider->setValue(zoom);
    ui->zoomLabel->setText(QString::number(zoom));
    if (autoRendering)
    {
        renderImage();
    }
}

void MainWindow::on_gammaSlider_valueChanged(int value)
{
    setGamma(value * GAMMA_SLIDER_STEP);
}
void MainWindow::on_zoomSlider_valueChanged(int value)
{
    setZoom(value);
}

void MainWindow::renderLoadedImage(const QImage &image)
{
    renderImage(image);
}

void MainWindow::on_dubRadioButton_clicked()
{
    unboxingPolicy = Dub;
    renderImage();
}

void MainWindow::on_averageRadioButton_clicked()
{
    unboxingPolicy = Average;
    renderImage();
}
