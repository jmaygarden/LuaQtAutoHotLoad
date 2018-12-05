#pragma once

// Qt
#include <QMainWindow>
#include <QSettings>

class QCloseEvent;
class QMoveEvent;
class QResizeEvent;

class SettingsWindow : public QMainWindow
{
public:
  SettingsWindow(QString const &name, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

  void loadWindowGeometry();
  void saveWindowGeometry();

protected:
  void changeEvent(QEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
  void moveEvent(QMoveEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private:
  QSettings settings;
};
