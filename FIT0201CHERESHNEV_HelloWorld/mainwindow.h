#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    static const int UNIT_LENGTH = 40;
    static const int MARK_HALF_LENGTH = 40;
    static const int MAX_AREA_WIDTH = 710;
    static const int MAX_AREA_HEIGHT = 410;
    QImage areaImage;
    void clearArea();

private slots:
    void on_drawSinButton_clicked();
    void on_drawAxisButton_clicked();
    void on_clearButton_clicked();
};

#endif // MAINWINDOW_H
