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

#include <QString>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QSlider>
#include <QLineEdit>

#include "../../../Model/src/ScreenieModelInterface.h"
#include "../../../Model/src/SceneLimits.h"
#include "../ScreenieControl.h"
#include "GeometryPropertiesWidget.h"
#include "PropertyValidatorWidget.h"
#include "ui_GeometryPropertiesWidget.h"

class GeometryPropertiesWidgetPrivate
{
public:
    GeometryPropertiesWidgetPrivate(ScreenieModelInterface &theScreenieModel, ScreenieControl &theScreenieControl)
        : screenieModel(theScreenieModel),
          screenieControl(theScreenieControl)
    {}

    ScreenieModelInterface &screenieModel;
    ScreenieControl &screenieControl;
};

// public

GeometryPropertiesWidget::GeometryPropertiesWidget(ScreenieModelInterface &screenieModel, ScreenieControl &screenieControl, QWidget *parent) :
    PropertyValidatorWidget(parent),
    ui(new Ui::GeometryPropertiesWidget),
    d(new GeometryPropertiesWidgetPrivate(screenieModel, screenieControl))
{
    ui->setupUi(this);
    initializeUi();
    updateUi();
    frenchConnection();
}

GeometryPropertiesWidget::~GeometryPropertiesWidget()
{
#ifdef DEBUG
    qDebug("GeometryPropertiesWidget::~GeometryPropertiesWidget(): called.");
#endif
    delete ui;
    delete d;
}

// private

void GeometryPropertiesWidget::initializeUi()
{
    ui->distanceSlider->setMaximum(SceneLimits::MinDistance);
    ui->distanceSlider->setMaximum(SceneLimits::MaxDistance);
    QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    QIntValidator *integerValidator = new QIntValidator(this);
    ui->positionXLineEdit->setValidator(doubleValidator);
    ui->positionYLineEdit->setValidator(doubleValidator);
    ui->rotationLineEdit->setValidator(integerValidator);
    ui->distanceLineEdit->setValidator(doubleValidator);
}

void GeometryPropertiesWidget::frenchConnection()
{
    connect(&d->screenieModel, SIGNAL(positionChanged()),
            this, SLOT(updateUi()));
    connect(&d->screenieModel, SIGNAL(rotationChanged()),
            this, SLOT(updateUi()));
    connect(&d->screenieModel, SIGNAL(distanceChanged()),
            this, SLOT(updateUi()));
}

// private slots

void GeometryPropertiesWidget::updateUi()
{
    qreal x = d->screenieModel.getPosition().x();
    qreal y = d->screenieModel.getPosition().y();
    qreal z = d->screenieModel.getDistance();
    ui->positionXLineEdit->setText(QString::number(x));
    ui->positionYLineEdit->setText(QString::number(y));
    ui->distanceLineEdit->setText(QString::number(z));
    validate(*ui->distanceLineEdit, true);
    ui->distanceSlider->setValue(z);
    int rotation = d->screenieModel.getRotation();
    ui->rotationLineEdit->setText(QString::number(rotation));
    ui->rotationSlider->setValue(rotation);
}

/*!\todo Use ScreenieControl to manipulate the model values! */
void GeometryPropertiesWidget::on_positionXLineEdit_editingFinished()
{
    bool ok;
    qreal x = ui->positionXLineEdit->text().toFloat(&ok);
    if (ok) {
        d->screenieControl.setPositionX(x, &d->screenieModel);
    }
}

void GeometryPropertiesWidget::on_positionYLineEdit_editingFinished()
{
    bool ok;
    qreal y = ui->positionYLineEdit->text().toFloat(&ok);
    if (ok) {
       d->screenieControl.setPositionY(y, &d->screenieModel);
    }
}

void GeometryPropertiesWidget::on_rotationLineEdit_editingFinished()
{
    bool ok;
    int angle = ui->rotationLineEdit->text().toInt(&ok);
    if (ok) {
        d->screenieControl.setRotation(angle, &d->screenieModel);
    }
}

void GeometryPropertiesWidget::on_rotationSlider_valueChanged(int value)
{
    d->screenieControl.setRotation(value, &d->screenieModel);
}

void GeometryPropertiesWidget::on_distanceLineEdit_editingFinished()
{
    bool ok;
    qreal distance = ui->distanceLineEdit->text().toFloat(&ok);
    if (ok && distance >= 0.0 && distance <= SceneLimits::MaxDistance) {
        d->screenieControl.setDistance(distance, &d->screenieModel);
        validate(*ui->distanceLineEdit, true);
    } else {
        validate(*ui->distanceLineEdit, false);
    }
}

void GeometryPropertiesWidget::on_distanceSlider_valueChanged(int value)
{
    d->screenieControl.setDistance(value, &d->screenieModel);
}
