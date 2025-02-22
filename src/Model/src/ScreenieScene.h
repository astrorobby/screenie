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

#ifndef SCREENIESCENE_H
#define SCREENIESCENE_H

#include <QObject>
#include <QList>
#include <QColor>

#include "ModelLib.h"

class ScreenieModelInterface;
class ScreenieScenePrivate;

/*!
 * The ScreenieScene model which contains all the instances of the ScreenieModelInterface.
 */
class ScreenieScene : public QObject
{
    Q_OBJECT
public:
    MODEL_API explicit ScreenieScene(QObject *parent = 0);
    MODEL_API virtual ~ScreenieScene();

    /*!
     * Adds the \p screenieModel to this ScreenieScene. This ScreenieScene takes ownership and
     * the \p screenieModel must have been allocated on the heap.
     *
     * \param screenieModel
     *        the ScreenieModel to be added
     * \sa #modelAdded(ScreenieModel &)
     * \sa #changed()
     */
    MODEL_API void addModel(ScreenieModelInterface *screenieModel);

    /*!
     * \sa #modelRemoved(const ScreenieModelInterface &)
     * \sa #changed()
     */
    MODEL_API void removeModel(ScreenieModelInterface *screenieModel);

    /*!
     * \sa #modelRemoved(const ScreenieModelInterface &)
     * \sa #changed()
     */
    MODEL_API void removeModel(int index);

    /*!
     * \return the ScreenieModelInterface identified by \p index; may be 0
     * \sa #count()
     */
    MODEL_API ScreenieModelInterface *getModel(int index) const;
    MODEL_API const QList<ScreenieModelInterface *> &getModels() const;
    MODEL_API QList<ScreenieModelInterface *> getSelectedModels() const;
    MODEL_API int count() const;

    MODEL_API bool isBackgroundEnabled() const;

    /*!
     * \sa #backgroundChanged()
     * \sa #changed()
     */
    MODEL_API void setBackgroundEnabled(bool enable);

    MODEL_API QColor getBackgroundColor() const;

    /*!
     * \sa #backgroundChanged()
     * \sa #changed()
     */
    MODEL_API void setBackgroundColor(QColor colour);

    /*!
     * Returns whether this ScreenieScene has at least one template model. Note
     * that when \c true is returned this does \em not imply that this ScreenieScene
     * is a \em template scene.
     *
     * \sa ScreenieModelInterface#isTemplate()
     * \sa #hasTemplatesExclusively()
     * \sa #isTemplate()
     */
    MODEL_API bool hasTemplates() const;

    /*!
     * Returns whether this ScreenieScene has \em only template models. Note
     * that when \c true is returned this does \em not imply that this ScreenieScene
     * is a \em template scene.
     *
     * \sa ScreenieModelInterface#isTemplate()
     * \sa #hasTemplates()
     * \sa #isTemplate()
     */
    MODEL_API bool hasTemplatesExclusively() const;

    /*!
     * Returns whether this ScreenieScene is a \em template scene, that is it
     * has been explicitly saved as such. Note that when \c true is returned this does
     * \em not imply that this ScreenieScene has any \em template models. This is typically the case
     * when all \em template items have already been replaced by ScreenieFilePathModel
     * or ScreenieImageModel items.
     *
     * \sa ScreenieModelInterface#isTemplate()
     * \sa #hasTemplatesExclusively()
     * \sa #hasTemplates()
     */
    MODEL_API bool isTemplate() const;

    MODEL_API void setTemplate(bool enable);

    /*!
     * Returns whether this ScreenieScene has been modified since creation or the last save.
     *
     * \return \c true if this ScreenieScene has unsaved changes; \c false else
     * \sa #isDefault()
     */
    MODEL_API bool isModified() const;

    /*!
     * Sets this ScreenieScene as \p modified.
     *
     * \param modified
     *        set to \c true if modified; \c false else
     */
    MODEL_API void setModified(bool modified);

    /*!
     * Returns whether this Scene is a \em default Scene, that is whether it has
     * just been created. A \em default Scene contains no items and the Scene
     * properties are all set to default values.
     *
     * \return \c true if this Scene is a \em default Scene; \c false else
     * \sa #isModified()
     */
    MODEL_API bool isDefault() const;

signals:
    /*!
     * Emitted whenever this ScreenieScene or one of the instances of the ScreenieModelInterface has changed.
     * This signal is emitted for <em>every</em> kind of change. Receivers should only do very cheap operations
     * when receiving this signal, such as updating widget states.
     *
     * For more costly operations connect to one of the specialised signals, either on this ScreenieScene, or
     * on the ScreenieModelInterface instance directly
     *
     * \sa ScreenieModelInterface#reflectionChanged()
     * \sa ScreenieModelInterface#changed()
     * \sa #modelAdded(ScreenieModelInterface &)
     * \sa #modelRemoved(ScreenieModelInterface &)
     * \sa #backgroundChanged()
     * \sa #distanceChanged
     */
    void changed();

    void modelAdded(ScreenieModelInterface &screenieModel);

    /*!
     * Emitted whenever the \p screenieModel has been removed from the scene. The \p screenieModel instance will be
     * \c deleted right after the signal has been emitted, so don't store and re-use the reference later on.
     *
     * This signal is emitted <em>in addition</em> to the #changed() signal.
     *
     * Connect to this signal in order to remove the corresponding view items from the view, which refer to this
     * \p screenieModel, for example.
     */
    void modelRemoved(ScreenieModelInterface &screenieModel);
    void backgroundChanged();
    void distanceChanged();
    void selectionChanged();

private:
    Q_DISABLE_COPY(ScreenieScene)
    ScreenieScenePrivate *d;

    void frenchConnection();

private slots:
    void handleChanged();
};

#endif // SCREENIESCENE_H
