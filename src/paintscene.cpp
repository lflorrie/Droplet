#include "paintscene.h"
#include <QDebug>
#include <QApplication>

PaintScene::PaintScene(QObject *parent)
    : QGraphicsScene{parent}
{

}

PaintScene::PaintScene(const PaintScene &scene)
{
    for (auto &i : scene.items()) {
        this->addItem(i);
    }
}

PaintScene &PaintScene::operator=(const PaintScene &scene)
{
    for (auto &i : scene.items()) {
        this->addItem(i);
	}
	return *this;
}

void PaintScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
	{
		this->startDraw(event);
    }
}

void PaintScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
	{
		this->continueDraw(event);
    }
}

void PaintScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	this->endDraw(event);
}

void PaintScene::startDraw(QGraphicsSceneMouseEvent *event)
{
	start = event->buttonDownScenePos(Qt::LeftButton);
}

void PaintScene::continueDraw(QGraphicsSceneMouseEvent *event)
{
	destination = event->scenePos();
	//        qDebug() << "Mouse moving!" << destination;
	QRectF rect;
	rect.setTopLeft(start);
	rect.setBottomRight(destination);
	if (start.x() >= destination.x())
	{
		rect.setTopRight(start);
		rect.setBottomLeft(destination);
	}
	if (start.y() >= destination.y())
	{
		rect.setBottomLeft(start);
		rect.setTopRight(destination);
	}
	if (start.x() >= destination.x() && start.y() >= destination.y())
	{
		rect.setBottomRight(start);
		rect.setTopLeft(destination);
	}
	drawRectangle(rect);
}

void PaintScene::endDraw(QGraphicsSceneMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		destination = event->scenePos();
	}
	qDebug() << "Mouse released!" << destination;
	QRectF rect;
	rect.setTopLeft(start);
	rect.setBottomRight(destination);
	if (start.x() >= destination.x())
	{
		rect.setTopRight(start);
		rect.setBottomLeft(destination);
	}
	if (start.y() >= destination.y())
	{
		rect.setBottomLeft(start);
		rect.setTopRight(destination);
	}
	if (start.x() >= destination.x() && start.y() >= destination.y())
	{
		rect.setBottomRight(start);
		rect.setTopLeft(destination);
	}
	drawRectangle(rect);
	emit rectangleHasBeenDrawn(rect);
}

void PaintScene::drawRectangle(QRectF rect)
{
    if (this->items().size() > 1) {
//        this->items().removeFirst();
        this->removeItem(this->items().front());
    }

    this->addRect(rect, QPen(Qt::red, 2), QBrush());
//    emit rectangleHasBeenDrawn(rect);
//    emit rectangleHasBeenDrawn(rect);
}

void PaintScene::drawRectangleNoSignal(QRectF rect)
{
    if (this->items().size() > 1) {
        //        this->items().removeFirst();
        this->removeItem(this->items().front());
    }
	this->addRect(rect, QPen(Qt::red, 2), QBrush());
	emit rectangleHasBeenDrawn2(rect);
}

//void PaintScene::wheelEvent(QGraphicsSceneWheelEvent *event)
//{

////	if (QApplication::keyboardModifiers() == Qt::CTRL)
////	{
////		qDebug() << "Ctrl";
////		return;
////	}
//}

