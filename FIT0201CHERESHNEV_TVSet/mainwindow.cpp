#include <QDir>
#include "mainwindow.h"
#include "ui_mainwindow.h"

static const qreal GAMMA_SLIDER_STEP = .01;
static const qreal GAMMA_DEFAULT = 1.;
static const qreal GAMMA_DISPLAY = 2.2;
static const int ZOOM_DEFAULT = 1;
static const QString IMAGES_PATH = ":\\tvset_yuv\\";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    converseRec(&ConverseRec::BT601),
    unboxingPolicy(Dub),
    currentImage(NULL)
{
    ui->setupUi(this);
    connect(&renderThread, SIGNAL(renderedImage(QImage)), SLOT(updateImage(QImage)));

    QStringList imageFiles = QDir(IMAGES_PATH).entryList();
    foreach (const QString &str, imageFiles)
    {
        images.append(QImage(IMAGES_PATH + str));
    }
    fileListViewModel = new QStringListModel(imageFiles);
    ui->fileListView->setModel(fileListViewModel);
    ui->fileListView->selectionModel()->select(fileListViewModel->index(0), QItemSelectionModel::Select);
    graphicsItem = new QGraphicsPixmapItem();

    ui->zoomSlider->setValue(ZOOM_DEFAULT);
    ui->gammaSlider->setValue(static_cast<int>(GAMMA_DEFAULT / GAMMA_SLIDER_STEP + .5));
    renderImage(0);

    graphicsScene = new GraphicsScene();
    connect(graphicsScene, SIGNAL(loadedImage(QImage, QString)), SLOT(renderLoadedImage(QImage, QString)));
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
    if (index < 0 || index >= images.count())
    {
        return;
    }
    renderImage(&images[index]);
}

void MainWindow::renderImage(const QImage *image)
{
    currentImage = image;
    renderThread.render(image, gamma / GAMMA_DISPLAY, converseRec, zoom, unboxingPolicy);
}

void MainWindow::updateImage(const QImage &image)
{
    graphicsItem->setPixmap(QPixmap::fromImage(image));
    graphicsScene->setSceneRect(0, 0, image.width(), image.height());
    ui->graphicsView->updateScrollBars();
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

void MainWindow::setGamma(qreal gamma)
{
    if (gamma < ui->gammaSlider->minimum() * GAMMA_SLIDER_STEP || gamma > ui->gammaSlider->maximum() * GAMMA_SLIDER_STEP)
    {
        return;
    }
    this->gamma = gamma;
    ui->gammaSliderLabel->setText(QString::number(gamma));
    if (currentImage != NULL)
    {
        renderImage();
    }
}

void MainWindow::setZoom(int zoom)
{
    this->zoom = zoom;
    ui->zoomLabel->setText(QString::number(zoom));
    if (currentImage != NULL)
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

void MainWindow::renderLoadedImage(const QImage &image, const QString &fileName)
{
    int rowCount = fileListViewModel->rowCount();
    fileListViewModel->insertRows(rowCount, 1);
    fileListViewModel->setData(fileListViewModel->index(rowCount), fileName);
    ui->fileListView->selectionModel()->clearSelection();
    ui->fileListView->selectionModel()->select(fileListViewModel->index(rowCount), QItemSelectionModel::SelectCurrent);
    images.append(image);
    renderImage(rowCount);
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
