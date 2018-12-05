#include "SettingsWindow.h"

// Qt
#include <QEvent>

namespace
{
  constexpr char * GEOMETRY_TAG = "geometry";
  constexpr char * STATE_TAG = "state";
  constexpr char * MAXIMIZED_TAG = "maximized";
}

SettingsWindow::SettingsWindow(QString const & name, QWidget *parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
  , settings("BeastCode", name, this)
{
  setObjectName(name);
}

void SettingsWindow::loadWindowGeometry()
{
  settings.beginGroup(objectName());
  {
    restoreGeometry(settings.value(GEOMETRY_TAG, saveGeometry()).toByteArray());
    restoreState(settings.value(STATE_TAG, saveState()).toByteArray());
  }
  settings.endGroup();
}

void SettingsWindow::saveWindowGeometry()
{
  settings.beginGroup(objectName());
  {
    settings.setValue(GEOMETRY_TAG, saveGeometry());
    settings.setValue(STATE_TAG, saveState());
  }
  settings.endGroup();
}

void SettingsWindow::changeEvent(QEvent *event)
{
  QMainWindow::changeEvent(event);
  
  switch(event->type())
  {
  case QEvent::WindowStateChange:
    saveWindowGeometry();
    break;

  default:
    break;
  }
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
  QMainWindow::closeEvent(event);

  saveWindowGeometry();
}

void SettingsWindow::moveEvent(QMoveEvent *event)
{
  QMainWindow::moveEvent(event);

  saveWindowGeometry();
}

void SettingsWindow::resizeEvent(QResizeEvent *event)
{
  QMainWindow::resizeEvent(event);

  saveWindowGeometry();
}
