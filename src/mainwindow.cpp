#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileSystemModel>
#include <QWheelEvent>

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
	auto scene = (PaintScene)ui->backgroundGraphicsView->scene();
	ui->backgroundGraphicsView->setInteractive(false);

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
    for (int i = 0; i < backgroundScenes.size(); ++i)
    {
        connect(&holeScenes[i], SIGNAL(rectangleHasBeenDrawn(QRectF)), &backgroundScenes[i], SLOT(drawRectangleNoSignal(QRectF)));
        connect(&backgroundScenes[i], SIGNAL(rectangleHasBeenDrawn2(QRectF)), this, SLOT(on_rectangleIsReady(QRectF)));
    }
    refractionsData.fill(0, num);
}

double MainWindow::getRefractionFromImage(QImage &cropBackground, QImage &cropHole)
{
//    float avgBackround = meanImage(cropBackground);
//    float avgHole = meanImage(cropHole);

    // make gray
    QImage grayImage = cropHole.convertToFormat(QImage::Format_Grayscale8);
    QImage grayImageBack = cropBackground.convertToFormat(QImage::Format_Grayscale8);
    // convert to cv mat
    cv::Mat mat = qimage_to_mat_ref(grayImage, CV_8UC1);
    cv::Mat matBack = qimage_to_mat_ref(grayImageBack, CV_8UC1);

    cv::Mat srcMatDroplet = mat.clone();
    cv::Mat srcMatBackground = matBack.clone();


	cv::Mat mat3;
	cv::cvtColor(mat, mat3, cv::COLOR_GRAY2RGB);

    //subtraction
	mat_ = mat.clone();
    imageSubtraction(mat_, matBack, ui->thresholdMask->value(), ui->blurNum->value());

	mat_ = getContours(mat_, this->threshold, this->threshold2Offset, ui->blurNum->value(), mat3);

#if 0
    return getMeanByMask(mat, mat_) / getMeanByMask(matBack, mat_);
#endif
    return getRefractionByMask(srcMatDroplet, srcMatBackground, mat_);

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
//    getContours(mat_, threshold, threshold2Offset);
}


void MainWindow::on_horizontalSlider_2_valueChanged(int value)
{
    threshold2Offset = value;
//    getContours(mat_, threshold, threshold2Offset);
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
        disconnect(&i);
        for (int i = 0; i < items.count(); ++i)
        {
            delete items[i];
        }
    }
    backgroundScenes.clear();
    for (auto &i : holeScenes)
    {
        disconnect(&i);
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
//    disconnect(&holeScenes[selectedScene]);
//    disconnect(&backgroundScenes[selectedScene]);
//    connect(&holeScenes[selectedScene], SIGNAL(rectangleHasBeenDrawn(QRectF)), &backgroundScenes[selectedScene], SLOT(drawRectangleNoSignal(QRectF)));
//    connect(&backgroundScenes[selectedScene], SIGNAL(rectangleHasBeenDrawn2(QRectF)), this, SLOT(on_rectangleIsReady(QRectF)));
}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "create report";
	QString filename = QFileDialog::getSaveFileName(this, tr("Save as txt"), "");
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	for (int i = 0; i < refractionsData.size(); ++i)
	{
		QTextStream out(&file);
		out << ui->startAngle->value() + ui->stepAngle->value() * i << "\t" << refractionsData[i] << "\n";
        qDebug() << refractionsData[i];
    }
}

void MainWindow::updateLanguage(int lang)
{
    language = lang;
//    QString translations = QString("MyApp%1.qm").arg(QLocale().name());
    QApplication::instance()->removeTranslator(&mTranslator);
//    const QString baseName = "HoleProcessor_" + QLocale(locale).name();
    QString baseName;
    if (lang == 1)
        baseName = "HoleProcessor_ru_RU.ts";
    else
        return;
    if (mTranslator.load(":/i18n/" + baseName)) {
        qDebug() << "LOAD FINISHED";
        QApplication::instance()->installTranslator(&mTranslator);
    } else {
        qDebug() << "COULD NOT INSTALL TRANSLATIONS ";
    }
}

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

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Tab) {
		if (QApplication::keyboardModifiers () == Qt::CTRL)
		{
			qDebug() << "tab + ctrl";
			if (ui->treeWidget->topLevelItemCount())
			{
                if (ui->treeWidget->selectedItems().size() == 0)
				{
					ui->treeWidget->topLevelItem(0)->setSelected(true);
					this->selectedScene = 0;
					ui->treeWidget->currentItemChanged(ui->treeWidget->topLevelItem(0), NULL);
				}
				else
				{
					ui->treeWidget->topLevelItem(this->selectedScene)->setSelected(false);
					this->selectedScene++;
					if (selectedScene == ui->treeWidget->topLevelItemCount())
                    {
                        this->selectedScene = 0;
                        ui->treeWidget->topLevelItem(0)->setSelected(true);
                        ui->treeWidget->currentItemChanged(ui->treeWidget->topLevelItem(0), NULL);
                        return;
					}
					ui->treeWidget->topLevelItem(this->selectedScene)->setSelected(true);
					ui->treeWidget->currentItemChanged(ui->treeWidget->topLevelItem(this->selectedScene), ui->treeWidget->topLevelItem(this->selectedScene - 1));
				}
			}

		}
	}
}


void MainWindow::changeEvent(QEvent *event)
{
    //QDialog::changeEvent(event);
    if( QEvent::LanguageChange == event->type())
    {
        ui->retranslateUi(this);
    }
}

void MainWindow::on_actionLanguage_triggered()
{
    if (language != 1)
        language = 1;
    else
        language = 0;
    updateLanguage(language);
}

