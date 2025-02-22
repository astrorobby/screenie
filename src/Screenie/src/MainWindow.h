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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QString>
#include <QWidget>
#include <QMainWindow>

#include "RecentFiles.h"

class QWidget;
class QCloseEvent;
class QEvent;
class QAction;

class ScreenieModelInterface;
class ScreenieScene;
class ScreeniePixmapItem;
class ScreenieControl;
class ScreenieGraphicsScene;
class Clipboard;
class PlatformManager;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    bool read(const QString &filePath);

public slots:
    virtual void showFullScreen();
    virtual void showNormal();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    Q_DISABLE_COPY(MainWindow)

    Ui::MainWindow *ui;
    PlatformManager *m_platformManager;
    ScreenieGraphicsScene *m_screenieGraphicsScene;
    ScreenieScene *m_screenieScene;
    ScreenieControl *m_screenieControl;
    bool m_ignoreUpdateSignals;
    Clipboard *m_clipboard;
    QString m_documentFilePath;
    RecentFiles m_recentFiles;
    QAction *m_minimizeWindowsAction;
    QAction *m_maximizeWindowsAction;

    void frenchConnection();

    void newScene(ScreenieScene &screenieScene);
    bool writeScene(const QString &filePath);
    bool writeTemplate(const QString &filePath);

    void initializeUi();
    void updateTransformationUi();
    void updateReflectionUi();
    void updateColorUi();
    void updateEditActions();
    void updateTitle();

    void createScene();
    void updateScene(ScreenieScene &screenieScene);

    void askBeforeClose();
    void handleMultipleModifiedBeforeQuit();
    void saveBeforeClose();
    void saveAsBeforeClose();

    void restoreWindowGeometry();

    void updateDocumentManager();
    MainWindow *createMainWindow();

    inline bool isFilePathRequired() const;
    void showReadError(const QString &filePath);
    void showWriteError(const QString &documentName, const QString &filePath);
    void showError(const QString &message);

private slots:
    // File
    void on_newAction_triggered();
    void on_openAction_triggered();
    void on_saveAction_triggered();
    void on_saveAsAction_triggered();
    void on_saveAsTemplateAction_triggered();
    void on_exportAction_triggered();
    void on_closeAction_triggered();
    void on_quitAction_triggered();

    // Edit
    void on_cutAction_triggered();
    void on_copyAction_triggered();
    void on_pasteAction_triggered();
    void on_deleteAction_triggered();
    void on_selectAllAction_triggered();

    // Insert
    void on_addImageAction_triggered();
    void on_addTemplateAction_triggered();

    // View
    void on_toggleFullScreenAction_triggered();

    // About
    void on_aboutAction_triggered();
    void on_aboutQtAction_triggered();

    void on_rotationSlider_valueChanged(int value);
    void on_distanceSlider_valueChanged(int value);
    void on_reflectionGroupBox_toggled(bool enable);
    void on_reflectionOffsetSlider_valueChanged(int value);
    void on_reflectionOpacitySlider_valueChanged(int value);

    void on_backgroundColorGroupBox_toggled(bool enable);
    void on_redSlider_valueChanged(int value);
    void on_greenSlider_valueChanged(int value);
    void on_blueSlider_valueChanged(int value);
    void on_redSpinBox_valueChanged(int value);
    void on_greenSpinBox_valueChanged(int value);
    void on_blueSpinBox_valueChanged(int value);
    void on_htmlBGColorLineEdit_editingFinished();

    void updateUi();
    void updateDefaultValues();
    void handleRecentFile(const QString &filePath);
    void updateWindowMenu();

    void handleFileSaveAsSelected(const QString &filePath);
    void handleFileSaveAsTemplateSelected(const QString &filePath);
    void handleFileSaveAsBeforeCloseSelected(const QString &filePath);

    void handleAskBeforeClose(int answer);
};

#endif // MAINWINDOW_H
