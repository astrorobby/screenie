/* This file is part of the Screenie project.
   Screenie is a fancy screenshot composer.

   Copyright (C) 2008 Ariya Hidayat <ariya.hidayat@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <QtCore/QPoint>
#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtCore/QEvent>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QImage>

#include "../../Utils/src/PaintTools.h"
#include "../../Model/src/ScreenieModelInterface.h"
#include "Clipboard/MimeHelper.h"
#include "Reflection.h"
#include "ScreenieControl.h"
#include "ScreeniePixmapItem.h"

const int ScreeniePixmapItem::ScreeniePixmapType = QGraphicsItem::UserType + 1;

class ScreeniePixmapItemPrivate
{
public:
    ScreeniePixmapItemPrivate(ScreenieModelInterface &theScreenieModel, ScreenieControl &theScreenieControl, Reflection &theReflection)
        : screenieModel(theScreenieModel),
          screenieControl(theScreenieControl),
          reflection(theReflection),
          transformPixmap(true),
          ignoreUpdates(false)
    {}

    ScreenieModelInterface &screenieModel;
    ScreenieControl &screenieControl;
    Reflection &reflection;
    bool transformPixmap;
    bool ignoreUpdates;
};

// public

ScreeniePixmapItem::ScreeniePixmapItem(ScreenieModelInterface &screenieModel, ScreenieControl &screenieControl, Reflection &reflection)
    : QGraphicsPixmapItem(),
      d(new ScreeniePixmapItemPrivate(screenieModel, screenieControl, reflection))
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setTransformationMode(Qt::SmoothTransformation);
    // we also want to be able to change the reflection also in the fully translucent areas
    // of the reflection
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    QPixmap pixmap;
    pixmap.convertFromImage(d->screenieModel.readImage());
    updatePixmap(pixmap);
    setAcceptDrops(true);
    frenchConnection();
}

ScreeniePixmapItem::~ScreeniePixmapItem()
{
#ifdef DEBUG
    qDebug("ScreeniePixmapItem::~ScreeniePixmapItem: called.");
#endif
}

ScreenieModelInterface &ScreeniePixmapItem::getScreenieModel() const
{
    return d->screenieModel;
}

// protected

int ScreeniePixmapItem::type() const
{
    return ScreeniePixmapType;
}

void ScreeniePixmapItem::contextMenuEvent (QGraphicsSceneContextMenuEvent *event)
{
#ifdef DEBUG
    qDebug("ScreeniePixmapItem::contextMenuEvent: called.");
#endif
    // for now we only allow properties of a single item to be modified at the same time
    selectExclusive();
}

void ScreeniePixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
   d->transformPixmap = isInsidePixmap(event->pos());
   event->accept();
}

void ScreeniePixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (isInsidePixmap(event->pos()) && d->transformPixmap) {
        transformPixmap(event);
    } else if (!d->transformPixmap) {
        changeReflection(event);
    }
}

void ScreeniePixmapItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    int value = event->delta() / 12;
    if (isInsidePixmap(event->pos())) {
        addDistance(value);
    } else {
        addReflectionOpacity(value);
    }
    event->accept();
}

void ScreeniePixmapItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    bool accept = MimeHelper::accept(event->mimeData(), MimeHelper::Strict);
    event->setAccepted(accept);
}

void ScreeniePixmapItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    d->screenieControl.updateModel(event->mimeData(), d->screenieModel);
    event->accept();
}

QVariant ScreeniePixmapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange) {
        QPointF position = value.toPointF();
        d->screenieModel.setPosition(position);
    } else if (change == ItemSelectedChange) {
        QGraphicsPixmapItem::itemChange(change, value);
        // See comment below in #transformPixmap
        d->ignoreUpdates = true;
        d->screenieModel.setSelected(value.toBool());
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}

void ScreeniePixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);
    QString overlayText = d->screenieModel.getOverlayText();
    if (!overlayText.isNull()) {
        QRectF rect = boundingRect();
        /*!\todo Optimise this; cache the font, re-calculate when overlay text changes (add signal) */
        if (rect.width() > 100 && rect.height() > 48) {
            rect.adjust(10.0, 10.0, -10.0, -10.0);
            int length = overlayText.length();
            QFont font("Arial");
            if (length > 10) {
                font.setPixelSize(16);
            } else {
                font.setPixelSize(24);
            }
            QFontMetrics fontMetrics(font);
            overlayText = fontMetrics.elidedText(overlayText, Qt::ElideMiddle, rect.width());
            painter->setPen(Qt::white);
            painter->setFont(font);
            painter->drawText(rect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, overlayText);
        }
    }
}

// private

void ScreeniePixmapItem::frenchConnection()
{
    connect(&d->screenieModel, SIGNAL(changed()),
            this, SLOT(updateItem()));
    connect(&d->screenieModel, SIGNAL(positionChanged()),
            this, SLOT(updatePosition()));
    connect(&d->screenieModel, SIGNAL(distanceChanged()),
            this, SLOT(updateItem()));
    connect(&d->screenieModel, SIGNAL(reflectionChanged()),
            this, SLOT(updateReflection()));
    connect(&d->screenieModel, SIGNAL(pixmapChanged(const QPixmap &)),
            this, SLOT(updatePixmap(const QPixmap &)));
    connect(&d->screenieModel, SIGNAL(filePathChanged(const QString &)),
            this, SLOT(updatePixmap()));
    connect(&d->screenieModel, SIGNAL(selectionChanged()),
            this, SLOT(updateSelection()));
}

void ScreeniePixmapItem::moveTo(QPointF scenePosition)
{
    selectExclusive();
    d->screenieModel.setPosition(scenePosition);
}

void ScreeniePixmapItem::rotate(int angle)
{
    selectExclusive();
    d->screenieControl.rotate(angle);
}

void ScreeniePixmapItem::addDistance(int distance)
{
    selectExclusive();
    d->screenieControl.addDistance(distance);
}

bool ScreeniePixmapItem::isInsidePixmap(QPointF itemPosition)
{
    bool result;
    if (d->screenieModel.isReflectionEnabled()) {
        QRectF boundingRect = this->boundingRect();
        result = itemPosition.y() < boundingRect.height() / 2.0;
    } else {
        result = true;
    }
    return result;
}

void ScreeniePixmapItem::transformPixmap(QGraphicsSceneMouseEvent *event)
{
    int deltaX, deltaY;
    switch (event->buttons()) {
    case Qt::RightButton:
        deltaX = event->lastScreenPos().x() - event->screenPos().x();
        rotate(deltaX);
        event->accept();
        break;

     case Qt::MiddleButton:
        deltaY = event->lastScreenPos().y() - event->screenPos().y();
        addDistance(deltaY);
        event->accept();
        break;

    case Qt::LeftButton:
        // Welcome to the Exception To The Rule "Modify model via Controller"!
        // moving items requires quite some logic, and we re-use that logic
        // (the Qt implementation of handling move events, to be specific)
        // So first we update the View, and THEN we update the Model.
        // (see #itemChange above)
        // As we get a signal by the model nevertheless we ignore it my raising
        // a flag, because we already updated the View ourselves
        d->ignoreUpdates = true;
        selectExclusive();
        QGraphicsPixmapItem::mouseMoveEvent(event);
        break;

     default:
        event->ignore();
        break;
    }
}

void ScreeniePixmapItem::changeReflection(QGraphicsSceneMouseEvent *event)
{
    qreal height_2 = boundingRect().height() / 2.0;
    qreal factor  = (event->pos().y() - height_2) / height_2;
    int percent = qRound(factor * 100.0);
    switch (event->buttons()) {
    case Qt::LeftButton:
        if (percent < 1) {
            percent = 1;
        } else if (percent > 100) {
            percent = 100;
        }
        selectExclusive();
        d->screenieControl.setReflectionOffset(percent);
        event->accept();
        break;

    case Qt::RightButton:
        if (percent < 0) {
            percent = 0;
        } else if (percent > 100) {
            percent = 100;
        }
        selectExclusive();
        d->screenieControl.setReflectionOpacity(percent);
        event->accept();
        break;

    default:
        QGraphicsPixmapItem::mouseMoveEvent(event);
        break;
    }
}

void ScreeniePixmapItem::addReflectionOpacity(int reflectionOpacity)
{
    selectExclusive();
    d->screenieControl.addReflectionOpacity(reflectionOpacity);
}

void ScreeniePixmapItem::selectExclusive()
{
    if (!isSelected()) {
        scene()->clearSelection();
        setSelected(true);
    }
}

// private slots

void ScreeniePixmapItem::updateReflection()
{
    QPixmap pixmap = this->pixmap();
    if (d->screenieModel.isReflectionEnabled()) {
        if (d->screenieModel.getSize() != pixmap.size()) {
            // the pixmap already has a reflection (height must be 2x original height),
            // so just take the upper half (the original pixmap)
            pixmap = PaintTools::upperHalf(pixmap);
        }
        pixmap = d->reflection.addReflection(pixmap, d->screenieModel.getReflectionOpacity(), d->screenieModel.getReflectionOffset());
    } else {
        if (d->screenieModel.getSize() != pixmap.size()) {
            pixmap = PaintTools::upperHalf(pixmap);
        }
    }
    setPixmap(pixmap);
}

void ScreeniePixmapItem::updatePixmap(const QPixmap &pixmap)
{
    setPixmap(pixmap);
    updateReflection();
    updateItem();
}

void ScreeniePixmapItem::updatePixmap()
{
    QPixmap pixmap;
    pixmap.convertFromImage(d->screenieModel.readImage());
    updatePixmap(pixmap);
}

void ScreeniePixmapItem::updateItem()
{
    QTransform transform;
    QTransform scale;
    QTransform translateBack;

    qreal centerScale = 1.0 - 0.9 * d->screenieModel.getDistance() / ScreenieModelInterface::MaxDistance;
    scale = QTransform().scale(centerScale, centerScale);

    QPixmap pixmap = this->pixmap();
    qreal dx = pixmap.width() / 2.0;
    qreal dy;
    if (d->screenieModel.isReflectionEnabled()) {
        dy =  pixmap.height() / 4.0;
    } else {
        dy = pixmap.height() / 2.0;
    }
    transform.translate(dx, dy);
    transform.rotate(d->screenieModel.getRotation(), Qt::YAxis);
    translateBack.translate(-dx, -dy);
    transform = translateBack * scale * transform;
    setTransform(transform, false);
}

void ScreeniePixmapItem::updatePosition()
{
    // see comment in #transformPixmap
    if (!d->ignoreUpdates) {
        setPos(d->screenieModel.getPosition());
    }
    d->ignoreUpdates = false;
}

void ScreeniePixmapItem::updateSelection()
{
    // see comment in #transformPixmap
    if (!d->ignoreUpdates) {
        setSelected(d->screenieModel.isSelected());
    }
    d->ignoreUpdates = false;
}
