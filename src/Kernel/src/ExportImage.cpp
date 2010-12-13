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

#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtCore/QSizeF>
#include <QtGui/QGraphicsScene>
#include <QtGui/QImage>
#include <QtGui/QPainter>

#include "../../Model/src/ScreenieScene.h"
#include "ExportImage.h"

// public

ExportImage::ExportImage(const ScreenieScene &screenieScene, QGraphicsScene &graphicsScene)
    : m_screenieScene(screenieScene),
      m_graphicsScene(graphicsScene)
{
}

void ExportImage::exportImage(const QString &filePath)
{
    /*!\todo Specify some margin etc. */
    QRectF targetRect =  m_graphicsScene.itemsBoundingRect();
    QImage image(qRound(targetRect.width()), qRound(targetRect.height()), QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    QPainter painter(&image);
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing, true);
    if (m_screenieScene.isBackgroundEnabled()) {
        painter.fillRect(0, 0, image.width(), image.height(), m_screenieScene.getBackgroundColor());
    }
    m_graphicsScene.clearSelection();
    m_graphicsScene.render(&painter, QRectF(), targetRect);
    image.save(filePath, "PNG");
}