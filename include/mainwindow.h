#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include "paintscene.h"
#include <opencv_utils.h>
#include <QCollator>
#include <QTreeWidgetItem>

#include <QDropEvent>
#include <QMimeData>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void fillDataFromFolder(const QString &folder);
    float getRefractionFromImage(QImage &cropBackground, QImage &cropHole);
private slots:
    void on_rectangleIsReady(QRectF rect);
    float meanImage(QImage image);
    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);
    void on_actionOpenTriggered();
    void clearAll();
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QString     backgroundFileName;
    QString     holeFileName;

    int         threshold;
    int         threshold2Offset;

    QVector<PaintScene> backgroundScenes;
    QVector<PaintScene>  holeScenes;
    QVector<float>  refractionsData;
    int selectedScene;

    QString     folderName;

    //temp
    cv::Mat     mat_;

    // QWidget interface
protected:
    void dropEvent(QDropEvent *event);

    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event);

    // QWidget interface
protected:
    void wheelEvent(QWheelEvent *event) override;
};
#endif // MAINWINDOW_H
