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
    void renderImage(const QImage *image);
    void setGamma(qreal gamma);
    void setZoom(int zoom);

private slots:
    void renderLoadedImage(const QImage &image, const QString &fileName); //from drag and drop
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
    const QImage *currentImage;
    const ConverseRec *converseRec;
    qreal gamma;
    int zoom;
    UnboxingPolicy unboxingPolicy;
    QGraphicsPixmapItem *graphicsItem;
    GraphicsScene *graphicsScene;
    QList<QImage> images;
    QStringListModel *fileListViewModel;
    RenderThread renderThread;
};

#endif // MAINWINDOW_H
