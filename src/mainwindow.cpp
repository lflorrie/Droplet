#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileSystemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(on_actionOpenTriggered()));
//    ui->treeWidget->setDragEnabled(1);
//    ui->treeWidget->setDragDropMode();
    this->setAcceptDrops(true);
    ui->thresholdMask->setValue(5);
    ui->blurNum->setValue(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillDataFromFolder(const QString &folder)
{
    this->folderName = folder;
    QDir directory(folderName);
    QStringList images = directory.entryList(QStringList() << "*.bmp" << "*.BMP", QDir::Files);

    // sorting
    QCollator collator;
    collator.setNumericMode(true);
    std::sort(images.begin(), images.end(), collator);


    //clear all
    clearAll();
    // create new tree
    QTreeWidgetItem *w;
    int num = 0;
    ui->treeWidget->setHeaderLabel(folderName);

    for(int i = 0; i < images.count(); i += 2) {
        w = new QTreeWidgetItem;
        w->setText(0, QString::number(num + 1));
        w->addChild(new QTreeWidgetItem(QStringList{images[i]}));
        w->addChild(new QTreeWidgetItem(QStringList{images[i + 1]}));
        ui->treeWidget->addTopLevelItem(w);
        {
            QPixmap image(folderName + "/" + images[i]);
            QGraphicsPixmapItem* item = new QGraphicsPixmapItem(image);
            PaintScene scene;
            scene.addItem(item);
            backgroundScenes.push_back(scene);
        }
        {
            QPixmap image(folderName + "/" + images[i + 1]);
            QGraphicsPixmapItem* item = new QGraphicsPixmapItem(image);
            PaintScene scene;
            scene.addItem(item);
            holeScenes.push_back(scene);
        }
        ++num;
    }
    refractionsData.fill(0, num);
}

float MainWindow::getRefractionFromImage(QImage &cropBackground, QImage &cropHole)
{
//    float avgBackround = meanImage(cropBackground);
//    float avgHole = meanImage(cropHole);

    // make gray
    QImage grayImage = cropHole.convertToFormat(QImage::Format_Grayscale8);
    QImage grayImageBack = cropBackground.convertToFormat(QImage::Format_Grayscale8);
    // convert to cv mat
    cv::Mat mat = qimage_to_mat_ref(grayImage, CV_8UC1);
    cv::Mat matBack = qimage_to_mat_ref(grayImageBack, CV_8UC1);

    //subtraction
    mat_ = mat.clone();
    imageSubtraction(mat_, matBack, ui->thresholdMask->value(), ui->blurNum->value());

    getContours(mat_, threshold, threshold2Offset);
#if 0
    return getMeanByMask(mat, mat_) / getMeanByMask(matBack, mat_);
#endif
    return getRefractionByMask(mat, matBack, mat_);

//    qDebug() << avgBackround << " " << avgHole;
//    return avgHole / (float)avgBackround;
}

void MainWindow::on_rectangleIsReady(QRectF rect)
{
    qDebug() << "Is ready";
    QPixmap backgroundImage(folderName + "/" + backgroundFileName);
    QPixmap holeImage(folderName + "/" + holeFileName);

    QImage cropBackground = backgroundImage.copy(rect.toRect()).toImage();
    QImage cropHole = holeImage.copy(rect.toRect()).toImage();



//    grayImage.save("C:/Projects/1.png");
//    image.save("C:/Projects/2.png");

    refractionsData[selectedScene] = getRefractionFromImage(cropBackground, cropHole);
    ui->refractionText->setText(QString::number(getRefractionFromImage(cropBackground, cropHole)));

//    pixMap.save("C:/Projects/background.png");
//    pixMap2.save("C:/Projects/hole.png");

}

float MainWindow::meanImage(QImage image)
{
    int res = 0;
    for (int i = 0; i < image.size().width(); ++i)
    {
        for (int j = 0; j < image.size().height(); ++j)
        {
            int pixel = qGray(image.pixel(i,j));
            res += pixel;
        }
    }
    return res / (float)(image.size().height() * image.size().width());
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    threshold = value;
    getContours(mat_, threshold, threshold2Offset);
}


void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    threshold2Offset = value;
    getContours(mat_, threshold, threshold2Offset);
}

void MainWindow::on_actionOpenTriggered()
{
    folderName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "",
                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qDebug() << "Triggered" << folderName;
    fillDataFromFolder(folderName);
}

void MainWindow::clearAll()
{
    // Scenes
    for (auto &i : backgroundScenes)
    {
        auto items = i.items();
        for (int i = 0; i < items.count(); ++i)
        {
            delete items[i];
        }
    }
    backgroundScenes.clear();
    for (auto &i : holeScenes)
    {
        auto items = i.items();
        for (int i = 0; i < items.count(); ++i)
        {
            delete items[i];
        }
    }
    holeScenes.clear();

    // ui tree widget
    ui->treeWidget->clear();
    for( int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);

        for (int j = 0; j < item->childCount(); ++j)
        {
            delete item->child(j);
        }
        delete item;
    }
    // refractionsData
    refractionsData.clear();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    foreach (const QUrl &url, event->mimeData()->urls()) {
        QString path = url.toLocalFile();
        QFileInfo fi(path);
        if (fi.exists() && fi.isDir())
        {
            qDebug() << "Dropped file:" << path;
            fillDataFromFolder(path);
        }
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *item, QTreeWidgetItem *previous)
{
	Q_UNUSED(previous);

    if (item->childCount() < 2)
        return;
    this->selectedScene = item->text(0).toInt() - 1;

    ui->holeGraphicsView->setScene(&holeScenes[selectedScene]);
    ui->backgroundGraphicsView->setScene(&backgroundScenes[selectedScene]);

    this->backgroundFileName = item->child(0)->text(0);
    this->holeFileName = item->child(1)->text(0);

    connect(&holeScenes[selectedScene], SIGNAL(rectangleHasBeenDrawn(QRectF)), &backgroundScenes[selectedScene], SLOT(drawRectangleNoSignal(QRectF)));
    connect(&backgroundScenes[selectedScene], SIGNAL(rectangleHasBeenDrawn2(QRectF)), this, SLOT(on_rectangleIsReady(QRectF)));

}


void MainWindow::on_pushButton_clicked()
{
    qDebug() << "create report";
    QFile file("Report.txt");
    file.open(QIODevice::WriteOnly);
    for (int i = 0; i < refractionsData.size(); ++i)
    {
        QTextStream out(&file);
        out << ui->startAngle->value() + ui->stepAngle->value() * i << "\t" << refractionsData[i] << "\n";
        qDebug() << refractionsData[i];
    }

}


#include <QWheelEvent>
void MainWindow::wheelEvent(QWheelEvent *event)
{
    if ( QApplication::keyboardModifiers () == Qt::CTRL) {
    ui->backgroundGraphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->holeGraphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    if(event->angleDelta().y() > 0) {
        // Zoom in
        ui->backgroundGraphicsView-> scale(scaleFactor, scaleFactor);
        ui->holeGraphicsView->scale(scaleFactor, scaleFactor);

    } else {
        // Zooming out
        ui->backgroundGraphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        ui->holeGraphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
    }
}
