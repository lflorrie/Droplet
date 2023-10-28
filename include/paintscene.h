#ifndef PAINTSCENE_H
#define PAINTSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

class PaintScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PaintScene(QObject *parent = nullptr);
    PaintScene(const PaintScene &scene);
    PaintScene &operator=(const PaintScene &scene);
    // QGraphicsScene interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	void startDraw(QGraphicsSceneMouseEvent *event);
	void continueDraw(QGraphicsSceneMouseEvent *event);
	void endDraw(QGraphicsSceneMouseEvent *event);

signals:
    void rectangleHasBeenDrawn(QRectF rect);
    void rectangleHasBeenDrawn2(QRectF rect);

public slots:
    void drawRectangle(QRectF rect);
    void drawRectangleNoSignal(QRectF rect);
private:
    QPainter painter;
    QPointF start;
    QPointF destination;

//	// QGraphicsScene interface
//protected:
//	void wheelEvent(QGraphicsSceneWheelEvent *event);
};

#endif // PAINTSCENE_H
