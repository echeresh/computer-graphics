#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QGraphicsPixmapItem>
#include <QMainWindow>
#include "graphicsscene.h"
#include "converserec.h"
#include "renderthread.h"

namespace Ui{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void renderImage();
    void renderImage(int index);
    void renderImage(const QImage &image);
    void setGamma(qreal gamma, bool autoRendering = true);
    void setZoom(int zoom, bool autoRendering = true);

private slots:
    void renderLoadedImage(const QImage &image); //from drag and drop
    void updateImage(const QImage &image);
    void on_fileListView_doubleClicked(const QModelIndex &index);
    void on_bt601RadioButton_clicked();
    void on_bt709RadioButton_clicked();
    void on_gammaSlider_valueChanged(int value);
    void on_zoomSlider_valueChanged(int value);
    void on_dubRadioButton_clicked();
    void on_averageRadioButton_clicked();

private:
    Ui::MainWindow *ui;
    QImage currentImage;
    const ConverseRec *converseRec;
    qreal gamma;
    int zoom;
    UnboxingPolicy unboxingPolicy;
    QGraphicsPixmapItem *graphicsItem;
    GraphicsScene *graphicsScene;
    const QStringList imageFiles;
    QList<QImage> images;
    QStringListModel *fileListViewModel;
    RenderThread renderThread;

    static const qreal GAMMA_DISPLAY;
    static const qreal GAMMA_DEFAULT;
    static const qreal GAMMA_SLIDER_STEP;
    static const int ZOOM_DEFAULT;
    static const QString IMAGES_PATH;
};

#endif // MAINWINDOW_H
