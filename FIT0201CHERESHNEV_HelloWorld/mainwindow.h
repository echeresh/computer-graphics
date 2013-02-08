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
    const int unitLength;
    const int maxAreaWidth;
    const int maxAreaHeight;
    QImage areaImage;

private slots:
    void drawSinus();
    void drawAxis();
    void clearArea();
};

#endif // MAINWINDOW_H
